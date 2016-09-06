/*
 * Compile with:
 * g++-4.9 -std=gnu++14 -pthread main_thread.cpp -o Posix_threads
 * 
 * Run with:
 * ./Posix_threads
 * 
 */
#include <iostream>
#include <thread>
#define ITER_COUNT 50

int global_var = 0;

void foo(int thread_id)
{
    int loc_var = 0;
    for (auto i = 0; i < ITER_COUNT; ++i) {
        ++global_var;
        ++loc_var;
        std::cout << "Thread " << thread_id << " here: " << "global_var = "
                << global_var << " local_var = " << loc_var << std::endl;
    }
}

int main(int argc, char** argv)
{
    std::thread t1(foo, 1);
    std::thread t2(foo, 2);
    
    t1.join();
    t2.join();
    return 0;
}

