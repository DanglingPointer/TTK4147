//
// Created by student on 9/30/16.
//

#include <iostream>
#include <thread>
#include <chrono>
#include <comedilib.h>

#define IO_DEV 3
#define DI(x) (24-x)
#define DO(x) (16-x)
#define DO_PWR 0


static comedi_t *it_g = NULL;

int io_init()
{
    int i;
    int res;

    it_g = comedi_open("/dev/comedi0");

    if (it_g == NULL) return -1;

    // init 3 inputs and 3 outputs
    for(i=1; i<4; i++)
    {
        res = comedi_dio_config(it_g, IO_DEV, DI(i), COMEDI_INPUT);
        if (res<0) return res;
        res = comedi_dio_config(it_g, IO_DEV, DO(i), COMEDI_OUTPUT);
        if (res<0) return res;
        res = comedi_dio_write(it_g, IO_DEV, DO(i), 0);
        if (res<0) return res;
    }

    // set outputs to high
    for(i=1; i<4; i++)
    {
        comedi_dio_write(it_g, IO_DEV, DO(i), 1);
    }

    return 1;
}

void io_write(int channel, int value)
{
    if(channel >= 1 && channel <= 3)
    {
        if(value == 0) comedi_dio_write(it_g, IO_DEV, DO(channel), 0);
        else comedi_dio_write(it_g, IO_DEV, DO(channel), 1);
    }
    else
    {
        printf("Incorrect io channel\n");
    }
}

int io_read(int channel)
{
    unsigned int data=0;

    if(channel >= 1 && channel <= 3)
    {
        comedi_dio_read(it_g, IO_DEV, DI(channel), &data);
        return (int)data;
    }
    else
    {
        printf("Incorrect io channel\n");
        return -1;
    }


}

void Task(int channel)
{
    for(;;){
        if (!io_read(channel)){
            io_write(channel, 1);
            std::this_thread::sleep_for(std::chrono::microseconds(5));
            io_write(channel, 0);
        }
    }
}


int main() {
    io_init();

    std::thread t1(Task, 1);
    std::thread t2(Task, 2);
    std::thread t3(Task, 3);

    t1.join();
    t2.join();
    t3.join();

    return 0;
}