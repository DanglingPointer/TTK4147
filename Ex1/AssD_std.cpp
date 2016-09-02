//
// Created by student on 9/2/16.
//
// compile with:
// g++-4.9 -std=gnu++14 -pthread AssD_std.cpp -o AssD_std
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

std::mutex mymutex;

void foo(int i)
{
    using namespace std::chrono_literals;
    {
        std::lock_guard<std::mutex> lg(mymutex);
        std::cout << "Thread " << i << " started\n";
    }
    std::this_thread::sleep_for(5s);
    std::cout << "Thread " << i << " finished\n";
}

int main()
{
    std::thread t1(foo, 1);
    std::thread t2(foo, 2);

    t1.join();
    t2.join();
}