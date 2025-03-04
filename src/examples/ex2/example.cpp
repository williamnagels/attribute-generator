#include "example.h"
namespace Sike
{
Example2StructB::~Example2StructB()
{
}

struct Example2StructB::Impl{
    [[clang::annotate("debug")]]
    int m_echo_w_surely;
};
}

int main()
{
    Example2StructA example1;
    Sike::Example2StructB example2;
    example2.m_impl = std::make_unique<Sike::Example2StructB::Impl>();
    example2.m_impl->m_echo_w_surely = 96;
    example2.m_impl->m_echo_w_surely = 97;
    example2.m_impl->m_echo_w_surely = 98;
    example2.m_impl->m_echo_w_surely = 99;
}