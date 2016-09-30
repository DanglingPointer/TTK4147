#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <sched.h>
#include <time.h>
#include "io.h"

void timespec_add_us(struct timespec *t, long us)
{
// add microseconds to timespecs nanosecond counter
    t->tv_nsec += us*1000;
// if wrapping nanosecond counter, increment second counter
    if (t->tv_nsec > 1000000000)
    {
        t->tv_nsec = t->tv_nsec - 1000000000;
        t->tv_sec += 1;
    }
}

int set_cpu(int cpu_number)
{
// setting cpu set to the selected cpu
    cpu_set_t cpu;
    CPU_ZERO(&cpu);
    CPU_SET(cpu_number, &cpu);
// set cpu set to current thread and return
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpu);
}

void TaskA2(int channel)
{
    set_cpu(1);
    timespec next;
    for(;;){
        if (!io_read(channel)){
            io_write(channel, 1);
            std::this_thread::sleep_for(std::chrono::microseconds(5));
            io_write(channel, 0);
        }
        clock_gettime(CLOCK_REALTIME, &next);
        timespec_add_us(&next, 100);
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next, NULL);
    }
}

void TaskA1(int channel)
{
    for(;;){
        if (!io_read(channel)){
            io_write(channel, 1);
            std::this_thread::sleep_for(std::chrono::microseconds(5));
            io_write(channel, 0);
        }
    }
}

long long TaskBDisturbance(){
    set_cpu(1);
    long long i = 0;
    for(;;){
        ++i;
    }
    return i;
}



int main() {
    std::vector<std::thread> dist;

    io_init();

    std::thread t1(TaskA2, 1);
    std::thread t2(TaskA2, 2);
    std::thread t3(TaskA2, 3);


    for(int i = 0; i < 10; ++i)
        dist.emplace_back(TaskBDisturbance);

    t1.join();
    t2.join();
    t3.join();

    for(auto& t : dist)
        t.join();

    return 0;
}