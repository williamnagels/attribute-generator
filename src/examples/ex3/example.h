#include <iostream>
#include <mutex>
#include <thread>

struct SafeData
{
    int m_withLock{0};
    mutable std::mutex m_mutex;

    void setWithLock(int value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_withLock = value;
    }
    void setWithoutLock(int value)
    {
        m_withLock = value;
    }
};