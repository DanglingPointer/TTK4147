#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/dispatch.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <pthread.h>

struct pid_data{
	pthread_mutex_t pid_mutex;
	pid_t pid;
};

int set_priority(int prio){
	int policy;
	sched_param param;
	if (prio < 1 || prio > 63)
		return -1;

	pthread_getschedparam(pthread_self(), &policy, &param);
	param.sched_priority = prio;
	return pthread_setschedparam(pthread_self(), policy, &param);
}

int get_priority()
{
	 int policy;
	 struct sched_param param;
	 // get priority
	 pthread_getschedparam(pthread_self(), &policy, &param);
	 return param.sched_curpriority;
}

void *foo(void *arg){
	int prio = *((int *)arg);

	set_priority(prio);

	int fid = shm_open("/sharedpid", O_RDWR, S_IRWXU);
	pid_data *ptr = reinterpret_cast<pid_data *>(mmap(0, sizeof(pid_data), PROT_READ | PROT_WRITE, MAP_SHARED, fid, 0));

	std::cout << "Accessor process: old pid = " << ptr->pid << std::endl;

	int chid = ConnectAttach(NULL, ptr->pid, 1, NULL, NULL);

	char senddata[] = "Hello server!";
	char reply[128];
	int status = MsgSend(chid, senddata, sizeof(senddata), reply, sizeof(reply));
	std::cout << "Reply from server: " << reply << std::endl;
	std::cout << "Prio = " << get_priority() << std::endl << std::endl;

	ConnectDetach(chid);
}

int main(int argc, char *argv[]) {

	set_priority(62);

	pthread_t threads[4];

	int prio1 = 20, prio2 = 15, prio3 = 45, prio4 = 50;

	if(pthread_create(&threads[0], NULL, foo, &prio1))
		std::cout << "Thread creating error\n";
	if(pthread_create(&threads[1], NULL, foo, &prio2))
		std::cout << "Thread creating error\n";
	if(pthread_create(&threads[2], NULL, foo, &prio3))
		std::cout << "Thread creating error\n";
	if(pthread_create(&threads[3], NULL, foo, &prio4))
		std::cout << "Thread creating error\n";

	for(int i = 0; i < 4; ++i){
		pthread_join(threads[i], NULL);
	}

	/*
	int fid = shm_open("/sharedpid", O_RDWR, S_IRWXU);
	pid_data *ptr = reinterpret_cast<pid_data *>(mmap(0, sizeof(pid_data), PROT_READ | PROT_WRITE, MAP_SHARED, fid, 0));

	std::cout << "Accessor process: old pid = " << ptr->pid << std::endl;

	int chid = ConnectAttach(NULL, ptr->pid, 1, NULL, NULL);

	char senddata[] = "Hello server!";
	char reply[128];
	int status = MsgSend(chid, senddata, sizeof(senddata), reply, sizeof(reply));
	std::cout << "Reply from server: " << reply << std::endl;

	ConnectDetach(chid);
	*/

	return EXIT_SUCCESS;
}
