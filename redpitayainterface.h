#ifndef REDPITAYAINTERFACE_H
#define REDPITAYAINTERFACE_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QMutex>

#include <QtSerialPort/QtSerialPort>

#include <arpa/inet.h>

class RedpitayaInterface : public QObject
{
    Q_OBJECT
public:
    explicit RedpitayaInterface(QObject *parent = 0);
    ~RedpitayaInterface();

    int Connect(const char* ipadr, int port, QString COM);
    void Disconnect();

signals:

public slots:

private:
    //Output Buffer
    int16_t* data_buf;
    size_t numkbytes, numbytes;

    // Data for ethernet socket
    int sockfd;
    struct sockaddr_in serv_addr;
    unsigned long n_packets;

    bool running;

    // Data for serial port
    QSerialPort* serial;

    // Threading
    QString hostName;
    quint16 port;
    QMutex mutex;

    // Private methods
    int getData ();

};

#endif // REDPITAYAINTERFACE_H
