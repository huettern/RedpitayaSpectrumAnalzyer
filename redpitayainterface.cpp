#include "redpitayainterface.h"

#include "connectwrapper.h"

#include <QDebug>

#include <QTextLayout>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <resolv.h>
#include <arpa/inet.h>


/****************************************************************************
 * PUBLIC SECTION
 -----------------------------------------------------------------------*//**
 * @publicsection
 ****************************************************************************/
RedpitayaInterface::RedpitayaInterface(QObject *parent) : QObject(parent)
{
    rpState = DISCONNECTED;
    serial = new QSerialPort(this);
}

RedpitayaInterface::~RedpitayaInterface()
{

}

/**
 * @brief RedpitayaInterface::Connect
 * @param ipadr IP address of the redpitaya
 * @param port  Destination port
 *
 * Connects to the redpitaya through SSH and opens TCP socket for RX
 */
int RedpitayaInterface::Connect(const char* ipadr, int port,
                                QString COM, ConsoleWindow *console)
{
    qDebug() << "Connecting with ip:" << ipadr << "port:" << port;

    // Open TCP socket for fast data reception
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0) //TCP
    //if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))< 0) //UDP
    {
       qDebug() << "\n Error : Could not create socket \n";
       disconnect();
       return errno;
    }

    // Open console connection for commands and status
    if(COM == NULL) return -1;
    serial->setPortName(COM);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    // try to open port
    int i = 0;
    for(i = 0; i < 5; i++)
    {
        qDebug() << "\nAttemt " << i;
        if (serial->open(QIODevice::ReadWrite) != 0)
        {
            qDebug() << "\n Error : Could not open Serial port:";
            qDebug() << " " << errno << strerror(errno);
            qDebug() << " COM" << COM;
        }
        else
            break;
    }
    if(i >= 5)
    {
        this->Disconnect();
        return errno;
    }

    writeData("\n\nmonitor 0x40000030 0xff\n");
    // connect serial output to console
    m_console = console;
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_console, SIGNAL(getData(QString)), this, SLOT(writeData(QString)));

    qDebug() << "\nConnected\n";
    emit setStatusMsg("Redpitaya Connected!", 0);

    rpState = CONNECTED;
    return 0;
}

/**
 * @brief RedpitayaInterface::Disconnect
 *
 * Closes all connections to the Redpitaya
 */
void RedpitayaInterface::Disconnect()
{
    // Close TCP socket
    close(sockfd);

    // Close console connection for commands and status
    if (serial->isOpen())
    {
        writeData("\n\nmonitor 0x40000030 0x00\n");
        serial->waitForBytesWritten(1000);
        serial->close();
    }
    qDebug() << "Disconnected";
    emit setStatusMsg("Redpitaya Disconnected!", 0);
    rpState = DISCONNECTED;
}

/**
 * @brief RedpitayaInterface::startStream
 * @return Status
 *
 * Starts the data stream from Redpitaya to PC
 */
int RedpitayaInterface::startStream ()
{
    startServer();

    return 0;
}

/**
 * @brief RedpitayaInterface::stopStream
 * @return status
 *
 * Stops the data stream from Redpitaya to PC
 */
int RedpitayaInterface::stopStream ()
{
    stopServer();

    return 0;
}


/****************************************************************************
 * PRIVATE SECTION
 -----------------------------------------------------------------------*//**
 * @privatesection
 ****************************************************************************/
/**
 * @brief RedpitayaInterface::readData
 *
 * Is called when Serial data is received
 */
void RedpitayaInterface::readData()
{
    QByteArray data = serial->readAll();
    m_console->putData(data);
}

/**
 * @brief RedpitayaInterface::writeData
 * @param data
 *
 * Slot gets called to write data to the redpitaya
 */
void RedpitayaInterface::writeData(QString str)
{
    serial->write(str.toLocal8Bit());
}

/**
 * @brief RedpitayaInterface::startServer
 *
 * Starts the sampling and transmission server
 */
void RedpitayaInterface::startServer()
{
    writeData("\n\nmonitor 0x40000030 0xF0\n");
    writeData("/opt/ddrdump/preview/preview_rp_remote_acquire -p 1234 -m 2  -r -c 0 -d 8 -k 4096");

    rpState = RUNNING;
}

/**
 * @brief RedpitayaInterface::stopServer
 *
 * Stops the sampling and transmission server
 */
void RedpitayaInterface::stopServer()
{
    writeData("\n\nmonitor 0x40000030 0x0f\n");
    rpState = CONNECTED;
}

/**
 * @brief getData
 *
 * Opens the network socket and receives n bytes
 */
int RedpitayaInterface::getData ()
{
    // allocate memory to hold the converted short values
    data_buf = (int16_t*) malloc (2*numbytes);
    if (data_buf == NULL) {fputs ("Memory error buf",stderr); exit (2);}

    // open network socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0) //TCP
    //if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))< 0) //UDP
    {
       printf("\n Error : Could not create socket \n");
       return 1;
    }

    //connect to server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = inet_addr(hostName.toStdString().c_str());

    if(socket_connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    }

    // read from socket buffer
    n_packets = recv(sockfd, data_buf, 2*numbytes, MSG_WAITALL);
    printf("n=%d numbytes=%d\n", n_packets, numbytes);

    // done
    //DataReady(data_buf);

    return 0;
}
