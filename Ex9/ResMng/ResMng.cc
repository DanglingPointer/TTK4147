#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <pthread.h>

dispatch_t              *dpp;
resmgr_attr_t           resmgr_attr;
dispatch_context_t      *ctp;
resmgr_connect_funcs_t  connect_funcs;
resmgr_io_funcs_t       io_funcs;
iofunc_attr_t           io_attr;

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb);
int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb);
void *thread_foo(void *arg);

int counter = 0;
char buf[16];
char command_buf[128] = "up";

pthread_mutex_t counter_mut;
pthread_mutex_t command_mut;


void error(char *s)
{
	perror(s);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	printf("Start resource manager\n");

	pthread_mutex_init(&counter_mut, NULL);
	pthread_mutex_init(&command_mut, NULL);

	pthread_t my_thread;


	// create dispatch.
	if (!(dpp = dispatch_create()))
		error("Create dispatch");

	// initialize resource manager attributes.
	memset(&resmgr_attr, 0, sizeof(resmgr_attr));
	resmgr_attr.nparts_max = 1;
	resmgr_attr.msg_max_size = 2048;

	// set standard connect and io functions.
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_funcs, _RESMGR_IO_NFUNCS, &io_funcs);
	iofunc_attr_init(&io_attr, S_IFNAM | 0666, 0, 0);

	// override functions
	io_funcs.read = io_read;
	io_funcs.write = io_write;

	// establish resource manager
	if (resmgr_attach(dpp, &resmgr_attr, "/dev/myresource", _FTYPE_ANY, 0, &connect_funcs, &io_funcs, &io_attr) < 0)
		error("Resmanager attach");
	ctp = dispatch_context_alloc(dpp);


	pthread_create(&my_thread, NULL, thread_foo, NULL);

	// wait forever, handling messages.
	while(1){
		if (!(ctp = dispatch_block(ctp)))
			error("Dispatch block");
		dispatch_handler(ctp);
	}

	pthread_join(my_thread, NULL);
	pthread_mutex_destroy(&counter_mut);
	pthread_mutex_destroy(&command_mut);

	exit(EXIT_SUCCESS);
}

int io_read(resmgr_context_t *ctp, io_read_t *msg, iofunc_ocb_t *ocb)
{

	if(!ocb->offset)
	{
		pthread_mutex_lock(&counter_mut);

		// set data to return
		sprintf(buf, "Counter = %d\n", counter);

		SETIOV(ctp->iov, buf, strlen(buf));
		_IO_SET_READ_NBYTES(ctp, strlen(buf));

		// increase the offset (new reads will not get the same data)
		ocb->offset = 1;

		pthread_mutex_unlock(&counter_mut);

		// return
		return (_RESMGR_NPARTS(1));
	}
	else
	{
		// set to return no data
		_IO_SET_READ_NBYTES(ctp, 0);

		// return
		return (_RESMGR_NPARTS(0));
	}
}

int io_write(resmgr_context_t *ctp, io_write_t *msg, RESMGR_OCB_T *ocb){

	printf("io_write started\n");
    int     status;

    /* set up the number of bytes (returned by client's write()) */

    _IO_SET_WRITE_NBYTES (ctp, msg->i.nbytes);


    /*
     *  Reread the data from the sender's message buffer.
     *  We're not assuming that all of the data fit into the
     *  resource manager library's receive buffer.
     */

    pthread_mutex_lock(&command_mut);
    printf("Command mutex locked\n");

    resmgr_msgread(ctp, command_buf, msg->i.nbytes, sizeof(msg->i));
    command_buf [msg->i.nbytes] = '\0'; /* just in case the text is not NULL terminated */

    pthread_mutex_unlock(&command_mut);
    printf ("Mutex unlocked, Received %d bytes = '%s'\n", msg -> i.nbytes, buf);

    if (msg->i.nbytes > 0)
        ocb->attr->flags |= IOFUNC_ATTR_MTIME | IOFUNC_ATTR_CTIME;

    return (_RESMGR_NPARTS (0));
}

void *thread_foo(void *arg){
	for(;;){
		pthread_mutex_lock(&command_mut);

		if (strstr(command_buf, "up")){
			pthread_mutex_lock(&counter_mut);
			++counter;
			pthread_mutex_unlock(&counter_mut);
		}
		if (strstr(command_buf, "down")){
			pthread_mutex_lock(&counter_mut);
			--counter;
			pthread_mutex_unlock(&counter_mut);
		}

		pthread_mutex_unlock(&command_mut);
		delay(1000);
	}
}
