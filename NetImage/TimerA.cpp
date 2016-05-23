#include "TimerA.h"

extern int read_flag;

TimerA::TimerA()
{

}

void TimerA::run()
{
    while (1) {
        read_flag = 1;
        msleep(20);
    }
}
