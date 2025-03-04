#include "example.h"
#include <chrono>
#include <iostream>
#include <thread>
//check_lock data.m_withLock data.m_mutex
// tui new-layout example_1 Window 1 cmd 1 src 1
// layout example_1

int main()
{
    int some_random_variable_x = 0;
    int some_random_variable_y = 0;
    int some_random_variable_z = 0;
    while(some_random_variable_x < 15)
    {
        some_random_variable_x+=1;
        some_random_variable_y+=2;
        some_random_variable_z++;
        some_random_variable_z %= 3;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return 0;
}
