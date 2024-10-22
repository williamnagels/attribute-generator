#include <clang-c/Index.h>

#include <iostream>
#include <string>
#include <vector>
struct DebugStruct
{
        struct DebugEntry
        {
                std::string _name;
        };
        std::string _name;
        std::vector<DebugEntry> _entries;
};
std::vector<DebugStruct> _structs;
std::string getCursorKindName( CXCursorKind cursorKind )
{
  CXString kindName  = clang_getCursorKindSpelling( cursorKind );
  std::string result = clang_getCString( kindName );

  clang_disposeString( kindName );
  return result;
}

std::string getCursorSpelling( CXCursor cursor )
{
  CXString cursorSpelling = clang_getCursorSpelling( cursor );
  std::string result      = clang_getCString( cursorSpelling );

  clang_disposeString( cursorSpelling );
  return result;
}


CXChildVisitResult visitor( CXCursor cursor, CXCursor /* parent */, CXClientData clientData )
{
  CXSourceLocation location = clang_getCursorLocation( cursor );
  if( clang_Location_isFromMainFile( location ) == 0 )
    return CXChildVisit_Continue;

  CXCursorKind cursorKind = clang_getCursorKind( cursor );

  unsigned int curLevel  = *( reinterpret_cast<unsigned int*>( clientData ) );
  unsigned int nextLevel = curLevel + 1;

  if (curLevel == 0)
  {
        _structs.push_back(DebugStruct{});
  }
  std::cout << std::string( curLevel, '-' ) << " " << cursorKind << " "<< getCursorKindName(cursorKind) << " (" << getCursorSpelling( cursor ) << ")\n";
                                                                                                                                                                                                                                                                                                                                                                                                                                         clang_visitChildren( cursor,
                       visitor,
                       &nextLevel );

  return CXChildVisit_Continue;
}

int main( int argc, char** argv )
{
  if( argc < 2 )
    return -1;

  CXIndex index        = clang_createIndex( 0, 1 );
  CXTranslationUnit tu = clang_createTranslationUnit( index, argv[1] );

  if( !tu )
  {
     std::cout << "error " << argv[1] << std::endl;
     return -1;
  }

  CXCursor rootCursor  = clang_getTranslationUnitCursor( tu );
  unsigned int treeLevel = 0;
  clang_visitChildren( rootCursor, visitor, &treeLevel );
  clang_disposeTranslationUnit( tu );
  clang_disposeIndex( index );

  return 0;
}
         
