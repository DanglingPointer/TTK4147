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

#ifndef EOK
#define EOK 0
#endif

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

int main(int argc, char *argv[]) {


	set_priority(30);

	int fid = shm_open("/sharedpid", O_RDWR | O_CREAT, S_IRWXU);

	ftruncate(fid, sizeof(pid_data));
	pid_data *ptr = reinterpret_cast<pid_data *>(mmap(0, sizeof(pid_data), PROT_READ | PROT_WRITE, MAP_SHARED, fid, 0));
	ptr->pid = getpid();

	pthread_mutexattr_t myattr;
	pthread_mutexattr_init(&myattr);
	pthread_mutexattr_setpshared(&myattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&ptr->pid_mutex, &myattr);


	std::cout << "Creator process: pid_mutex = " << ptr->pid << std::endl;

	int chid = ChannelCreate(_NTO_CHF_FIXED_PRIORITY);

	for(;;){
		_msg_info msginfo;
		char databuffer[128];
		std::cout << "Prio before receive = " << get_priority() << std::endl;
		int msgid = MsgReceive(chid, databuffer, sizeof(databuffer), &msginfo);
		std::cout << "Prio after receive = " << get_priority() << std::endl;

		std::cout << "Received from client: " << databuffer << std::endl
				<< "Client pid = " << msginfo.pid << ", client tid = " << msginfo.tid << "\n\n";
		std::cout.flush();

		char reply[] = "Hello client!";
		int replid = MsgReply(msgid, EOK, reply, sizeof(reply));
	}

	return EXIT_SUCCESS;
}
