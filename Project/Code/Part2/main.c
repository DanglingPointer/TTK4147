#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "udp.h"

#define ack2float(str) atof((str)+8)
#define REF_U 1
#define PERIOD 0.003
#define K_P 10
#define K_I 800

sem_t signal_recvd_sem, y_recvd_sem, u_found_sem;
pthread_mutex_t y_mut, u_mut, send_mut;
float y = 0, u = 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sets wu_time to PERIOD seconds from now
void get_wakeup_time(struct timespec *wu_time){
	clock_gettime(CLOCK_REALTIME, wu_time);

	wu_time->tv_nsec += (PERIOD-0.001) * 1000000000;

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
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void *udp_listener(void *arg)
{
	udp_conn udp_data = *((udp_conn*)arg);
	char reply[32];
	int output_recvd;

	printf("Listener started\n");

	for(;;){
		if (-1 == udp_receive(&udp_data, reply, sizeof(reply)))
			printf("Error on receivefrom\n");

		output_recvd = strcmp(reply, "SIGNAL");

		if (output_recvd){
			pthread_mutex_lock(&y_mut);
			y = ack2float(reply);
			pthread_mutex_unlock(&y_mut);
			sem_post(&y_recvd_sem);
		}
		else {
			sem_post(&signal_recvd_sem);
		}
	}
	return NULL;
}
// Calculates new input when output received
void *pi_controller(void *arg)
{
	float integral = 0, error;

	printf("Controller started\n");

	for(;;){
		sem_wait(&y_recvd_sem);

		pthread_mutex_lock(&u_mut);
		pthread_mutex_lock(&y_mut);
		error = REF_U - y;
		integral += (error * PERIOD);
		u = K_P * error + K_I * integral;
		pthread_mutex_unlock(&u_mut);
		pthread_mutex_unlock(&y_mut);

		sem_post(&u_found_sem);

	}
	return NULL;
}
// Requests output measurement and sends new input
void *input_sender(void *arg)
{
	udp_conn udp_data = *((udp_conn*)arg);
	char get_cmd[] = "GET";
	char set_cmd[] = "SET:123.456       ";

	struct timespec curr_time, start_time, wakeup_time;
	clock_gettime(CLOCK_REALTIME, &curr_time);
	clock_gettime(CLOCK_REALTIME, &start_time);

	while(!half_sec_diff(curr_time, start_time)){
		get_wakeup_time(&wakeup_time);

		pthread_mutex_lock(&send_mut);
		if (-1 == udp_send(&udp_data, get_cmd, sizeof(get_cmd)))
			printf("Signal response: Cannot send msg\n");
		pthread_mutex_unlock(&send_mut);

		sem_wait(&u_found_sem);

		pthread_mutex_lock(&u_mut);
		sprintf(set_cmd, "SET:%f", u);
		pthread_mutex_unlock(&u_mut);

		pthread_mutex_lock(&send_mut);
		if (-1 == udp_send(&udp_data, set_cmd, sizeof(set_cmd)))
			printf("Cannot send msg\n");
		pthread_mutex_unlock(&send_mut);


		clock_nanosleep(&wakeup_time);
		clock_gettime(CLOCK_REALTIME, &curr_time);
	}

	// udp stop
	char stop_cmd[] = "STOP";
	if (-1 == udp_send(&udp_data, stop_cmd, sizeof(stop_cmd)))
		printf("Cannot send stop-msg\n");

	return NULL;
}
// Responds to signals
void* signal_response(void *arg)
{
	udp_conn udp_data = *((udp_conn*)arg);
	char signal_ack[] = "SIGNAL_ACK";

	for(;;){
		sem_wait(&signal_recvd_sem);

		pthread_mutex_lock(&send_mut);
		if (-1 == udp_send(&udp_data, signal_ack, sizeof(signal_ack)))
			printf("Cannot send msg\n");
		pthread_mutex_unlock(&send_mut);

	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main()
{
	// init sems and mutexes
	if (-1 == sem_init(&signal_recvd_sem, 0, 0))
		printf("Init error\n");
	if (-1 == sem_init(&u_found_sem, 0, 0))
		printf("Init error\n");
	if (-1 == sem_init(&y_recvd_sem, 0, 0))
		printf("Init error\n");
	if (-1 == pthread_mutex_init(&y_mut, NULL))
		printf("Init error\n");
	if (-1 == pthread_mutex_init(&u_mut, NULL))
		printf("Init error\n");
	if (-1 == pthread_mutex_init(&send_mut, NULL))
		printf("Init error\n");

	// init udp
	udp_conn data;
	int res = udp_init_client(&data, 9999, "192.168.0.1");
	if (res==-1)
		printf("Hostname not found\n");
	else if (res == 0)
		printf("Initialized successfully\n");
	else
		printf("Unable to open socket\n");
	char start_cmd[] = "START";
	if (-1 == udp_send(&data, start_cmd, sizeof(start_cmd)))
		printf("Cannot send start-msg\n");


	// threads init
	pthread_t listener_thread, response_thread, controller_thread, input_thread;
	if (pthread_create(&listener_thread, NULL, udp_listener, &data))
		printf("Cannot create listener\n");
	if (pthread_create(&input_thread, NULL, input_sender, &data))
		printf("Cannot create listener\n");
	if (pthread_create(&response_thread, NULL, signal_response, &data))
		printf("Cannot create response thread\n");
	if (pthread_create(&controller_thread, NULL, pi_controller, NULL))
		printf("Cannot create controller\n");


	// threads join
	pthread_join(listener_thread, NULL);
	pthread_join(controller_thread, NULL);
	pthread_join(response_thread, NULL);
	pthread_join(input_thread, NULL);


	udp_close(&data);

	// destroy mutexes
	sem_destroy(&signal_recvd_sem);
	sem_destroy(&y_recvd_sem);
	sem_destroy(&u_found_sem);
	pthread_mutex_destroy(&y_mut);
	pthread_mutex_destroy(&u_mut);
	pthread_mutex_destroy(&send_mut);

	return 0;
}

