#include <memory>
struct [[clang::annotate("debug")]] Example2StructA
{
    [[clang::annotate("debug")]]
    int* m_p;
    int m_value;
};
namespace Sike
{
struct [[clang::annotate("debug")]] Example2StructB
{
    int* m_p;
    [[clang::annotate("debug")]]
    int m_value;
    struct Impl;
    [[clang::annotate("debug")]]
    std::unique_ptr<Impl> m_impl;
    ~Example2StructB();
};
}