#ifndef TIMER_H
#define TIMER_H

#include <QThread>

class Timer : public QThread
{
public:
    Timer();
protected:
    void run();
};

#endif // TIMER_H
