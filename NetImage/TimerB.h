#ifndef TIMERB_H
#define TIMERB_H

#include <QThread>

class TimerB : public QThread
{
public:
    TimerB();
protected:
    void run();
};

#endif // TIMERB_H
