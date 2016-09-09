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
void foo1(int *pvar1, int *pvar2)
{
    std::cout << "t1 started\n";
    while (running) {
	{
	std::lock_guard<std::mutex> lk(m);
        ++(*pvar1);
        *pvar2 = *pvar1;
	}
    }
}
void foo2(int *pvar1, int *pvar2)
{
    std::cout << "t2 started\n";
    for (auto i = 1; i <= 20; ++i) {
	{
	std::lock_guard<std::mutex> lk(m);
        std::cout << "var1 = " << *pvar1 << ", var2 = " << *pvar2 << std::endl;
	}
        std::this_thread::sleep_for(std::chrono::microseconds(100000));
    }
    running = false;
}
int main(int argc, char** argv)
{
    std::thread t1(foo1, &var1, &var2);
    std::thread t2(foo2, &var1, &var2);
    t1.join();
    t2.join();
    return 0;
}

