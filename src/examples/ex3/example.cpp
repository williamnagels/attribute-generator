#include "example.h"
#include <chrono>

//check_lock data.m_withLock data.m_mutex
// tui new-layout example_1 Window 1 cmd 1 src 1
// layout example_1
std::unique_lock<std::mutex>* global_lock = nullptr;

int main()
{
    SafeData data;
    while(true)
    {
        data.setWithLock(99);
        data.setWithoutLock(100);
        data.setWithLock(101);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //std::thread t1([&data]() {
        //    global_lock = new std::unique_lock<std::mutex>(data.m_mutex);
        //});
        //data.setWithoutLock(102);
    }

    return 0;
}
