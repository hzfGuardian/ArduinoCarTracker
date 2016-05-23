#include "TimerB.h"

extern int send_flag;

TimerB::TimerB()
{

}

void TimerB::run()
{
    while (1) {
        send_flag = 1;
        msleep(1000);
    }
}
