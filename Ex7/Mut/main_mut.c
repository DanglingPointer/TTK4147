#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/sem.h>
#include <native/mutex.h>
#include <rtdk.h>

typedef struct data_t {
    RT_TASK *ptask;
    RT_SEM *psem;
    RT_MUTEX *pmut;
} data_t;

void print_pri(RT_TASK *task, char *s){
    struct rt_task_info temp;
    rt_task_inquire(task, &temp);
    rt_printf("b:%i c:%i ", temp.bprio, temp.cprio);
    rt_printf(s);
}

void busy_wait_ms(unsigned long delay){
    unsigned long count = 0;
    while(count <= delay*10){
	rt_timer_spin(1000*100);
	count++;
    }
}

void ass_b1(void *arg){
    data_t *pdata = (data_t *)arg;
    if (rt_sem_p(pdata->psem, TM_INFINITE)){
	rt_printf("Cannot aquire semaphore\n");
	return;
    }

    print_pri(pdata->ptask, "Waiting on mut\n");

    if (rt_mutex_acquire(pdata->pmut, TM_INFINITE)){
	rt_printf("Cannot aquire mutex\n");
	return;
    }

    print_pri(pdata->ptask, "Busy waiting\n");
    busy_wait_ms(300);

    if (rt_mutex_release(pdata->pmut))
	rt_printf("Cannot release mutex\n");

    print_pri(pdata->ptask, "Released mut and finished\n");
}
void ass_b2(void *arg){
    data_t *pdata = (data_t *)arg;
    if (rt_sem_p(pdata->psem, TM_INFINITE)){
	rt_printf("Cannot aquire semaphore\n");
	return;
    }

    print_pri(pdata->ptask, "Sleeping\n");
    rt_task_sleep(100000000);

    print_pri(pdata->ptask, "Busy waiting\n");
    busy_wait_ms(500);

    print_pri(pdata->ptask, "Finished\n");
}
void ass_b3(void *arg){
    data_t *pdata = (data_t *)arg;
    if (rt_sem_p(pdata->psem, TM_INFINITE)){
	rt_printf("Cannot aquire semaphore\n");
	return;
    }

    print_pri(pdata->ptask, "Sleeping\n");
    rt_task_sleep(200000000);
    
    print_pri(pdata->ptask, "Waiting on mut\n");
     if (rt_mutex_acquire(pdata->pmut, TM_INFINITE)){
	rt_printf("Cannot aquire mutex\n");
	return;
    }

    print_pri(pdata->ptask, "Busy waiting\n");
    busy_wait_ms(200);

    if (rt_mutex_release(pdata->pmut))
	rt_printf("Cannot release mutex\n");

    print_pri(pdata->ptask, "Released mut and finished\n");
}
    


int main() 
{
    rt_print_auto_init(1);
    mlockall(MCL_CURRENT|MCL_FUTURE);


    RT_SEM my_sem;
    RT_MUTEX my_mut;

    if (rt_sem_create(&my_sem, "sem0", 0, S_PRIO)){
	rt_printf("Cannot create sem\n");
	return 1;
    }
    if (rt_mutex_create(&my_mut, "mut0")){
	rt_printf("Cannot create mut\n");
	return 1;
    }

    RT_TASK h1, h2, h3;

    data_t d1, d2, d3;

    d1.ptask = &h1;
    d2.ptask = &h2;
    d3.ptask = &h3;

    d1.pmut = d2.pmut = d3.pmut = &my_mut;
    d1.psem = d2.psem = d3.psem = &my_sem;

    if (rt_task_create(&h1, "mytask1", 0, 30, T_CPU(1) | T_JOINABLE)) {
        rt_printf("Create error 1");
        return 1;
    }
    if (rt_task_create(&h2, "mytask2", 0, 60, T_CPU(1) | T_JOINABLE)) {
        rt_printf("Create error 2");
        return 1;
    }
    if (rt_task_create(&h3, "mytask3", 0, 90, T_CPU(1) | T_JOINABLE)) {
        rt_printf("Create error 3");
        return 1;
    }


    if (rt_task_start(&h1, ass_b1, &d1)) {
        rt_printf("Start error 1");
        return 3;
    }
    if (rt_task_start(&h2, ass_b2, &d2)) {
        rt_printf("Start error 2");
        return 3;
    }
    if (rt_task_start(&h3, ass_b3, &d3)) {
        rt_printf("Start error 3");
        return 3;
    }


    usleep(1000000);


    if (rt_sem_broadcast(&my_sem)){
	rt_printf("Cannot broadcast semaphore\n");
	return 4;
    } else {
	rt_printf("Semaphore broadcasted\n");
    }




    rt_task_join(&h1);
    rt_task_join(&h2);
    rt_task_join(&h3);

    if (rt_sem_delete(&my_sem))
	rt_printf("Cannot delete semaphore\n");

    if (rt_mutex_delete(&my_mut))
	rt_printf("Cannot delete mutex\n");

    return 0;
}
