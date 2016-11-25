#include <stdio.h>
#include <stdlib.h>
#include "udp.h"

#define ack2float(str) atof((str)+8)
#define REF_U 1
#define PERIOD 0.004
#define K_P 10
#define K_I 800

void get_wakeup_time(struct timespec *wu_time){
	clock_gettime(CLOCK_REALTIME, wu_time);

	wu_time->tv_nsec += (PERIOD-0.002) * 1000000000;

	if (wu_time->tv_nsec > 1000000000){
		++(wu_time->tv_sec);
		wu_time->tv_nsec = wu_time->tv_nsec - 1000000000;
	}
}

int half_sec_diff(struct timespec curr, struct timespec start){
	int sec_diff = curr.tv_sec - start.tv_sec;
	int nsec_diff = curr.tv_nsec - start.tv_nsec;

	return (sec_diff > 1) ||
			(sec_diff == 1 && curr.tv_nsec + (1000000000 - start.tv_nsec) > 500000000) ||
			(nsec_diff > 500000000);
	//return sec_diff > 5;
}

int main()
{
	udp_conn data;

	int res = udp_init_client(&data, 9999, "192.168.0.1");
	if (res==-1)
		printf("Hostname not found\n");
	else if (res == 0)
		printf("Initialized successfully\n");
	else
		printf("Unable to open socket\n");

	char start_cmd[] = "START";
	char get_cmd[] = "GET";
	char set_cmd[] = "SET:123.456       ";
	char stop_cmd[] = "STOP";
	char reply[32];

	if (-1 == udp_send(&data, start_cmd, sizeof(start_cmd)))
		printf("Cannot send msg\n");

	float integral = 0, error;
	float u, y;
	struct timespec curr_time, start_time, wakeup_time;
	clock_gettime(CLOCK_REALTIME, &curr_time);
	clock_gettime(CLOCK_REALTIME, &start_time);

	int iter_count = 0;

	while(!half_sec_diff(curr_time, start_time)){
		get_wakeup_time(&wakeup_time);
		if (-1 == udp_send(&data, get_cmd, sizeof(get_cmd)))
			printf("Cannot send msg\n");

		if (-1 == udp_receive(&data, reply, sizeof(reply)))
			printf("Error on receivefrom\n");

		y = ack2float(reply);
		error = REF_U - y;
		integral += (error * PERIOD);
		u = K_P * error + K_I * integral;

		sprintf(set_cmd, "SET:%f", u);
		if (-1 == udp_send(&data, set_cmd, sizeof(set_cmd)))
			printf("Cannot send msg\n");

		++iter_count;
		clock_nanosleep(&wakeup_time);

		clock_gettime(CLOCK_REALTIME, &curr_time);
	}

	if (-1 == udp_send(&data, stop_cmd, sizeof(stop_cmd)))
		printf("Cannot send msg\n");

	udp_close(&data);

	printf("Iter_count = %d\n", iter_count);

	return 0;
}

