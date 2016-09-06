/*
 * Compile with:
 * g++-4.9 -std=gnu++14 -pthread main_mutex.cpp -o Mutex
 * 
 * Run with:
 * ./Mutex
 * 
 */
#include<iostream>
#include<thread>
#include<chrono>
#include<mutex>

bool running = true;
int var1 = 0;
int var2 = 0;
std::mutex m;
void foo1()
{
    while (running) {
        std::lock_guard<std::mutex> lk(m);
        ++var1;
        var2 = var1;
        // without the following line the numbers are fcked (at least using cygwin)
        std::this_thread::sleep_for(std::chrono::microseconds(100000));
    }
}
void foo2()
{
    for (auto i = 1; i <= 20; ++i) {
        std::lock_guard<std::mutex> lk(m);
        std::cout << "var1 = " << var1 << ", var2 = " << var2 << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(100000));
    }
    running = false;
}
int main(int argc, char** argv)
{
    std::thread t1(foo1);
    std::thread t2(foo2);
    t1.join();
    t2.join();
    return 0;
}

