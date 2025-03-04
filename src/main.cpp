#include "clang/Frontend/ASTUnit.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/CommandLine.h"

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <fstream>
// Represents a debug structure storing annotated elements
class DebugEntry
{
public:
    std::string _name;
    std::vector<DebugEntry> _entries;

    DebugEntry(const std::string& name) : _name(name) {}
    DebugEntry() : _name() {}

    void addEntry(const DebugEntry& entry)
    {
        _entries.push_back(entry);
    }
    std::string getSimpleName() const
    {
      size_t pos = _name.rfind("::");
      return (pos == std::string::npos) ? _name : _name.substr(pos + 2);
    }

    void print(std::ofstream& of) const
    {
      of << "class "<< this->getSimpleName()<<"Printer:"<<std::endl;
      of << "\tdef __init__(self, val):"<<std::endl;
      of << "\t\tself.val = val"<<std::endl;
      of << "\tdef to_string(self):"<<std::endl;
      std::string params;
      for (const auto &entry : _entries)
      {
        params += entry._name + "={self.val[\""+entry._name+"\"]},";
      }
      params.pop_back();
      of << "\t\t return f'"<<this->getSimpleName()<<"("<< params <<")'"<<std::endl;
      of << std::endl;
      of << "def lookup"<<this->getSimpleName()<<"(val):"<<std::endl;
      of << "\tif str(val.type) == \""<< this->_name<<"\":"<<std::endl;
      of << "\t\treturn "<<this->getSimpleName()<<"Printer(val)"<<std::endl;
      of << "\treturn None"<<std::endl;
      of << "gdb.pretty_printers.append(lookup"+this->getSimpleName()+")"<<std::endl;
      of << std::endl;
    }
};
class DebugASTVisitor : public clang::RecursiveASTVisitor<DebugASTVisitor> {
  public:
      explicit DebugASTVisitor(clang::ASTContext &Context) : Context(Context) {}

      std::vector<DebugEntry> Entries; // Stores the final debug hierarchy

      bool VisitCXXRecordDecl(clang::CXXRecordDecl *Decl) {
          if (Decl->isAnonymousStructOrUnion()) return true; // Ignore anonymous structs/unions

          std::string ClassName = getQualifiedName(Decl);
          DebugEntry *ParentEntry = nullptr;

          // Find parent class if nested
          clang::CXXRecordDecl *Parent = llvm::dyn_cast<clang::CXXRecordDecl>(Decl->getParent());
          while (Parent) {
              auto it = std::find_if(Entries.begin(), Entries.end(), [&](const DebugEntry &entry) {
                  return entry._name == getQualifiedName(Parent);
              });

              if (it != Entries.end()) {
                  ParentEntry = &(*it);
                  break;
              }
              Parent = llvm::dyn_cast<clang::CXXRecordDecl>(Parent->getParent());
          }

          // Check if this class is annotated or has fields annotated
          bool HasAnnotation = Decl->hasAttrs() && hasAnnotateAttr(Decl);
          bool HasAnnotatedFields = hasAnnotatedFields(Decl);

          if (HasAnnotation || HasAnnotatedFields) {
              DebugEntry NewEntry(ClassName);

              if (ParentEntry) {
                  //ParentEntry->addEntry(NewEntry);
              } else {
                  Entries.push_back(NewEntry);
              }
          }

          return true;
      }

      bool VisitFieldDecl(clang::FieldDecl *Decl) {
          if (!Decl->hasAttrs() || !hasAnnotateAttr(Decl)) return true; // Only process annotated fields

          std::string FieldName = Decl->getNameAsString();
          std::string ParentType = getQualifiedName(Decl->getParent());

          // Find the parent class in the hierarchy (or create one)
          auto it = std::find_if(Entries.begin(), Entries.end(), [&](const DebugEntry &entry) {
              return entry._name == ParentType;
          });

          if (it == Entries.end()) {
              Entries.emplace_back(ParentType);
              it = Entries.end() - 1;
          }
          it->addEntry(DebugEntry(FieldName));
          return true;
      }

      void print(std::ofstream& os) {
          for (auto &entry : Entries) {
              entry.print(os);
          }
      }

  private:
      clang::ASTContext &Context;

      bool hasAnnotateAttr(const clang::Decl *Decl) {
          for (const auto *Attr : Decl->attrs()) {
              if (llvm::isa<clang::AnnotateAttr>(Attr)) return true;
          }
          return false;
      }

      bool hasAnnotatedFields(const clang::CXXRecordDecl *Decl) {
          for (const auto *Field : Decl->fields()) {
              if (hasAnnotateAttr(Field)) return true;
          }
          return false;
      }

      // Returns the fully qualified name, including namespaces
      std::string getQualifiedName(const clang::NamedDecl *Decl) {
          if (!Decl) return "";
          std::string Name;
          llvm::raw_string_ostream OS(Name);
          Decl->printQualifiedName(OS);
          return OS.str();
      }
  };


int main(int argc, char **argv)
{
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input AST file> <output .py file>"<<std::endl;
        return 1;
    }

    std::string astFile = argv[1];
    std::string outputFileName = argv[2];

    clang::CompilerInstance CI;
    CI.createDiagnostics();

    auto AST = clang::ASTUnit::LoadFromASTFile(
      astFile,
      CI.getPCHContainerReader(),
      clang::ASTUnit::LoadEverything,
      &CI.getDiagnostics(),
      CI.getFileSystemOpts(),CI.getHeaderSearchOptsPtr());

    if (!AST)
    {
        std::cerr << "Failed to load AST file: " << astFile << "\n";
        return 1;
    }

    DebugASTVisitor visitor(AST->getASTContext());
    visitor.TraverseDecl(AST->getASTContext().getTranslationUnitDecl());

    std::ofstream file(outputFileName);
    if (file)
    {
      visitor.print(file);
      file.close();
    }
    return 0;
}
