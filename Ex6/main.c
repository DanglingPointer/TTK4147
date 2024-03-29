#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>

#include "io.h"

void rt_foo(void *cookie){
    for(;;){
        rt_task_wait_period(NULL);
        printf("Hello from rt-task...\n");
    }
}

void ass1(void *arg){
    int channel = *((int *)arg);

    for(;;){
        if (!io_read(channel)) {
            io_write(channel, 1);
            rt_task_sleep(5000);
            io_write(channel, 0);
        }
        rt_task_wait_period(NULL);
    }
}

void *dist_foo(void *dummy){
    int i = 0;
    for(;;)
        ++i;
}



int main() {
    io_init();
    mlockall(MCL_CURRENT|MCL_FUTURE);

    RT_TASK h1, h2, h3;

    int chan1 = 1, chan2 = 2, chan3 = 3;

    if (rt_task_create(&h1, "mytask1", 0, 99, T_CPU(1) | T_JOINABLE)) {
        printf("Create error 1");
        return 1;
    }
    if (rt_task_create(&h2, "mytask2", 0, 99, T_CPU(1) | T_JOINABLE)) {
        printf("Create error 2");
        return 1;
    }
    if (rt_task_create(&h3, "mytask3", 0, 99, T_CPU(1) | T_JOINABLE)) {
        printf("Create error");
        return 1;
    }


    if (rt_task_set_periodic(&h1, TM_NOW, 10000)) {
        printf("Periodic error");
        return 2;
    }

    if (rt_task_set_periodic(&h2, TM_NOW, 10000)) {
        printf("Periodic error");
        return 2;
    }

    if (rt_task_set_periodic(&h3, TM_NOW, 10000)) {
        printf("Periodic error");
        return 2;
    }


    if (rt_task_start(&h1, ass1, &chan1)) {
        printf("Start error");
        return 3;
    }
    if (rt_task_start(&h2, ass1, &chan2)) {
        printf("Start error");
        return 3;
    }
    if (rt_task_start(&h3, ass1, &chan3)) {
        printf("Start error");
        return 3;
    }

    pthread_t threads[10];
    int i;
    for(i = 0; i < 10; ++i)
        if (pthread_create(&threads[i], NULL, dist_foo, NULL)){
            printf("Disturbance create error");
            return 4;
        }




    rt_task_join(&h1);
    rt_task_join(&h2);
    rt_task_join(&h3);

    int j;
    for(j = 0; j < 10; ++j) {
        pthread_join(&threads[j], NULL);
    }

    return 0;
}