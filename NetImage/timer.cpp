#include "timer.h"

extern int flag;

Timer::Timer()
{

}

void Timer::run()
{
    while (1) {
        flag = 1;
        msleep(150);
    }
}
