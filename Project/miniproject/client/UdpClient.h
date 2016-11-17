#ifndef UDPCLIENT_H_
#define UDPCLIENT_H_

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

// typedef
typedef void (*sighandler_t)(int);

// structs that store the information needed for an udp connection
struct udp_conn
{
	int sock;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t client_len;
};

// RAII wrapper for the given functions
class UdpClient
{
public:
    // initialize the struct and connect to a udp server on the given port and ip
    UdpClient(const udp_conn& udp, int port, char *ip) :m_sockData(udp)
    {
        struct hostent *host;

        if ((host = gethostbyname(ip)) == NULL) 
            return;

        m_sockData.client_len = sizeof(m_sockData.client);
        // define servers
        memset((char *)&(m_sockData.server), 0, sizeof(m_sockData.server));
        m_sockData.server.sin_family = AF_INET;
        m_sockData.server.sin_port = htons(port);
        bcopy((char *)host->h_addr, (char *)&(m_sockData.server).sin_addr.s_addr, host->h_length);

        // open socket
        if ((m_sockData.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
            return;
    }
    // function for closing a connection
    ~UdpClient()
    {
        close(m_sockData.sock);
    }
    udp_conn get_Sockdata() const
    {
        return m_sockData;
    }
    // function for sending a string over an udp connection
    __forceinline int Send(char *buf, int len)
    {
        return sendto(m_sockData.sock, buf, len, 0, (struct sockaddr *)&(m_sockData.server), sizeof(m_sockData.server));
    }
    // function for receiving a string over an udp connection
    int Receive(char *buf, int len)
    {
        return recvfrom(m_sockData.sock, buf, len, 0, (struct sockaddr *)&(m_sockData.client), &(m_sockData.client_len));
    }
private:
    udp_conn m_sockData;
};

// function replacing clock_nanosleep
// DO NOT use for periods over 500 ms
int clock_nanosleep(struct timespec *next)
{
    struct timespec now;
    struct timespec sleep;

    // get current time
    clock_gettime(CLOCK_REALTIME, &now);

    // find the time the function should sleep
    sleep.tv_sec = next->tv_sec - now.tv_sec;
    sleep.tv_nsec = next->tv_nsec - now.tv_nsec;

    // if the nanosecon is below zero, decrement the seconds
    if (sleep.tv_nsec < 0) {
        sleep.tv_nsec += 1000000000;
        sleep.tv_sec -= 1;
    }

    // sleep
    nanosleep(&sleep, NULL);

    return 0;
}

void timespec_add_us(struct timespec *t, long us)
{
    // add microseconds to timespecs nanosecond counter
    t->tv_nsec += us * 1000;

    // if wrapping nanosecond counter, increment second counter
    if (t->tv_nsec > 1000000000) {
        t->tv_nsec -= 1000000000;
        t->tv_sec += 1;
    }
}

#endif /* UDPCLIENT_H_ */