/*
 * Compile with:
 * g++-4.9 -std=gnu++14 -pthread main_nomutex.cpp -o NoMutex
 * 
 * Run with:
 * ./NoMutex
 * 
 */
#include<iostream>
#include<thread>
#include<chrono>

bool running = true;
int var1 = 0;
int var2 = 0;
void foo1()
{
    while (running) {
        ++var1;
        var2 = var1;
    }
}
void foo2()
{
    for (auto i = 1; i <= 20; ++i) {
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

