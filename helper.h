#ifndef HELPER_H
#define HELPER_H



#include <QThread>

class Helper : public QThread
{
public:
    Helper();
    ~Helper();
    static void usleep(unsigned long usecs){QThread::usleep(usecs);}
    static void msleep(unsigned long msecs){QThread::msleep(msecs);}
    static void sleep(unsigned long secs){QThread::sleep(secs);}
};

#endif // HELPER_H
