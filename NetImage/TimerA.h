#ifndef TIMERA_H
#define TIMERA_H

#include <QThread>

class TimerA : public QThread
{
public:
    TimerA();
protected:
    void run();
};

#endif // TIMERA_H
