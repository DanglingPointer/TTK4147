//
// Created by student on 9/2/16.
//
#include <iostream>
#include <unistd.h>
#include <sys/times.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>

void busy_wait_delay(int seconds)
{
    int i, dummy;
    int tps = sysconf(_SC_CLK_TCK);
    clock_t start;
    struct tms exec_time;
    times(&exec_time);
    start = exec_time.tms_utime;
    while( (exec_time.tms_utime - start) < (seconds * tps))
    {
        for(i=0; i<1000; i++)
        {
            dummy = i;
        }
        times(&exec_time);
    }
}


void *foo(void *ptr)
{
    std::cout << "Thread started\n";

    //sleep(5);
    busy_wait_delay(5);

    std::cout << "Thread finished\n";
}

int main()
{
    pthread_t t1, t2;

    if (int iret = pthread_create(&t1, NULL, foo, NULL))
    {
        std::cerr << "Error code: " << iret << std::endl;
        exit(EXIT_FAILURE);
    }
    if (int iret = pthread_create(&t2, NULL, foo, NULL))
    {
        std::cerr << "Error code: " << iret << std::endl;
        exit(EXIT_FAILURE);
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
}
