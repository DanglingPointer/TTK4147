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
    RT_MUTEX *pmut_a;
    RT_MUTEX *pmut_b;
    RT_SEM *psem;
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

void ass_c1(void *arg)
{
    data_t *pdata = (data_t *)arg;

    if (rt_sem_p(pdata->psem, TM_INFINITE)){
	rt_printf("Cannot aquire semaphore\n");
	return;
    }


    /////////////////////////////////////////////////////////////////////
    rt_task_set_priority(pdata->ptask, 60);
    /////////////////////////////////////////////////////////////////////


    print_pri(pdata->ptask, "Waiting on mut A\n");
    if (rt_mutex_acquire(pdata->pmut_a, TM_INFINITE)){
	rt_printf("Cannot aquire mutex\n");
	return;
    }

    print_pri(pdata->ptask, "Busy waiting\n");
    busy_wait_ms(300);

    print_pri(pdata->ptask, "Waiting on mut B\n");
    if (rt_mutex_acquire(pdata->pmut_b, TM_INFINITE)){
	rt_printf("Cannot aquire mutex\n");
	return;
    }

    print_pri(pdata->ptask, "Busy waiting\n");
    busy_wait_ms(300);

    if (rt_mutex_release(pdata->pmut_b))
	rt_printf("Cannot release mutex\n");
    print_pri(pdata->ptask, "Released mut B\n");

    if (rt_mutex_release(pdata->pmut_a))
	rt_printf("Cannot release mutex\n");
    print_pri(pdata->ptask, "Released mut A\n");


    /////////////////////////////////////////////////////////////////////
    rt_task_set_priority(pdata->ptask, 30);
    /////////////////////////////////////////////////////////////////////

   
    busy_wait_ms(100);
    print_pri(pdata->ptask, "Task finished\n");
}

void ass_c2(void *arg)
{
    data_t *pdata = (data_t *)arg;

    if (rt_sem_p(pdata->psem, TM_INFINITE)){
	rt_printf("Cannot aquire semaphore\n");
	return;
    }

    print_pri(pdata->ptask, "Sleeping\n");
    rt_task_sleep(100000000);

    print_pri(pdata->ptask, "Waiting on mut B\n");
    if (rt_mutex_acquire(pdata->pmut_b, TM_INFINITE)){
	rt_printf("Cannot aquire mutex\n");
	return;
    }

    print_pri(pdata->ptask, "Busy waiting\n");
    busy_wait_ms(100);

    print_pri(pdata->ptask, "Waiting on mut A\n");
    if (rt_mutex_acquire(pdata->pmut_a, TM_INFINITE)){
	rt_printf("Cannot aquire mutex\n");
	return;
    }

    print_pri(pdata->ptask, "Busy waiting\n");
    busy_wait_ms(200);

    if (rt_mutex_release(pdata->pmut_a))
	rt_printf("Cannot release mutex\n");
    print_pri(pdata->ptask, "Released mut A\n");

    if (rt_mutex_release(pdata->pmut_b))
	rt_printf("Cannot release mutex\n");
    print_pri(pdata->ptask, "Released mut B\n");

    print_pri(pdata->ptask, "Busy waiting\n");
    busy_wait_ms(100);

    print_pri(pdata->ptask, "Finished\n");
}
    


int main() 
{
    rt_print_auto_init(1);
    mlockall(MCL_CURRENT|MCL_FUTURE);


    RT_SEM my_sem;
    if (rt_sem_create(&my_sem, "sem0", 0, S_PRIO)){
	rt_printf("Cannot create sem\n");
	return 1;
    }


    RT_MUTEX mut_a, mut_b;
    if (rt_mutex_create(&mut_a, "mut0")){
	rt_printf("Cannot create muta\n");
	return 1;
    }
    if (rt_mutex_create(&mut_b, "mut1")){
	rt_printf("Cannot create mutb\n");
	return 1;
    }

    RT_TASK h1, h2;

    data_t d1, d2;

    d1.ptask = &h1;
    d2.ptask = &h2;

    d1.pmut_a = d2.pmut_a = &mut_a;
    d1.pmut_b = d2.pmut_b = &mut_b;
    d1.psem = d2.psem = &my_sem;

    if (rt_task_create(&h1, "mytask1", 0, 30, T_CPU(1) | T_JOINABLE)) {
        rt_printf("Create error 1");
        return 1;
    }
    if (rt_task_create(&h2, "mytask2", 0, 60, T_CPU(1) | T_JOINABLE)) {
        rt_printf("Create error 2");
        return 1;
    }

    if (rt_task_start(&h1, ass_c1, &d1)) {
        rt_printf("Start error 1");
        return 3;
    }
    if (rt_task_start(&h2, ass_c2, &d2)) {
        rt_printf("Start error 2");
        return 3;
    }


    usleep(1000000);
    if (rt_sem_broadcast(&my_sem)){
	rt_printf("Cannot broadcast semaphore\n");
	return 4;
    }
    rt_printf("Semaphore is broadcasted\n");


    rt_task_join(&h1);
    rt_task_join(&h2);


    if (rt_mutex_delete(&mut_b))
	rt_printf("Cannot delete mutex a\n");
    if (rt_mutex_delete(&mut_a))
	rt_printf("Cannot delete mutex b\n");

    return 0;
}
