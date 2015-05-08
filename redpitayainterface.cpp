#include "redpitayainterface.h"

#include "connectwrapper.h"
#include "helper.h"

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
    rpStreamParams.channel = CH_A;
    rpStreamParams.decimation = 8;
    rpStreamParams.noEQ = false;
    rpStreamParams.noShaping = false;
    rpStreamParams.numKBytes = 16;
    rpStreamParams.reportRate = false;
    rpStreamParams.port = 1234;
    rpStreamParams.acquireDuration =
            (1024*rpStreamParams.numKBytes) /
            (125000000/(double)rpStreamParams.decimation);


    serial = new QSerialPort(this);

    publish_data_buf = NULL;
    singleAcquisitionRunning = false;
    rcvError = false;
}

RedpitayaInterface::~RedpitayaInterface()
{
    free(publish_data_buf);
    free(data_buf);

    delete serial;
}

/**
 * @brief RedpitayaInterface::Connect
 * @param ipadr IP address of the redpitaya
 * @param port  Destination port
 *
 * Connects to the redpitaya through SSH and opens TCP socket for RX
 */
int RedpitayaInterface::Connect(const char* ipadr, unsigned int port,
                                QString COM, ConsoleWindow *console)
{
    qDebug() << "Connecting with ip:" << ipadr << "port:" << port;
    rpStreamParams.port = port;
    rpStreamParams.IP   = QString(ipadr);

    // Open console connection for commands and status
    if(COM == NULL) return -1;
    serial->setPortName(COM);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->clearError();
    // try to open port
    int i = 0;
    for(i = 0; i < 5; i++)
    {
        qDebug() << "\nAttemt " << i+1;
        if (serial->open(QIODevice::ReadWrite) == true)
        {
            qDebug() << "\n Port is open :)";
            break;
        }
        else
        {
            QSerialPort::SerialPortError err = serial->error();
            qDebug() << "\n Error : Could not open Serial port:";
            qDebug() << " errno: " << errno << strerror(errno);
            qDebug() << " err: " << err << strerror(err);
            qDebug() << " COM" << COM;
        }
    }
    if(i >= 5)
    {
        qDebug() << " Err: (i >= 5)" << COM;
        this->Disconnect();
        return errno;
    }


    // send escape sequence
    writeData("\n\r\x03");
    // load kernel module
    writeData("/opt/ddrdump/enableddrdump.sh\n");
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
    // stop streaming
    if(rpState == RUNNING) stopStream();

    // send escape sequence
    writeData("\n\n\x03\n\n");

    /// unload kernel module
    writeData("\n\nrmmod rpad.ko\n");

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
 * @brief RedpitayaInterface::singleAcquisition
 * @return Status
 *
 * Runs a single acquisition and then stops.
 */
int RedpitayaInterface::singleAcquisition()
{
    if(startServer())   return -1;

    ///<! Give the server time to start
    Helper::msleep(30);
    if(rcvData())       return -1;

    //check status
    if(stopServer())    return -1;

    return 0;
}

/**
 * @brief RedpitayaInterface::startStream
 * @return StatuscheckCOM
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

/**
 * @brief RedpitayaInterface::getDataArray
 * @param dest Pointer to a int16_t array
 * @param n number of elements to copy
 * @return num real copied data
 *
 * Copies the most recent Data to the destination
 */
size_t RedpitayaInterface::getDataArray (void* dest, size_t n)
{
    // TODO: rename numbytes to more useful name
    // TODO: verify this routine
    if(n > numbytes)
    {
        memcpy(dest, publish_data_buf, numbytes);
        return numbytes;
    }
    else
    {
        memcpy(dest, publish_data_buf, n*2);
        return n;
    }
}

/**
 * @brief getDataArraySize
 * @return Size of the data array in number of int16_t
 *
 *
 */
size_t RedpitayaInterface::getDataArraySize ()
{
    return numbytes;
}

RedpitayaInterface::tsRPStreamParams RedpitayaInterface::getStreamParams()
{
    publish_data_buf_params = rpStreamParams;
    return publish_data_buf_params;
}

/**
 * @brief RedpitayaInterface::getSamplerate
 * @return the sample rate of the current settings
 */
double RedpitayaInterface::getSamplerate(void)
{
    return (125000000 / rpStreamParams.decimation);
}

/**
 * @brief RedpitayaInterface::setDecimation
 * @param dec
 * Set new decimation value
 */
void RedpitayaInterface::setDecimation(unsigned int dec)
{
    rpStreamParams.decimation = dec;
    rpStreamParams.acquireDuration =
            (1024*rpStreamParams.numKBytes) /
            (125000000/(double)rpStreamParams.decimation);
}

/**
 * @brief RedpitayaInterface::setBlockSize
 * @param numkbytes
 * set new number of samples to conduct
 */
void RedpitayaInterface::setBlockSize(unsigned int numkbytes)
{
    rpStreamParams.numKBytes = numkbytes;
    rpStreamParams.acquireDuration =
            (1024*rpStreamParams.numKBytes) /
            (125000000/(double)rpStreamParams.decimation);
}

bool RedpitayaInterface::getsingleAcquisitionRunning()
{
    return singleAcquisitionRunning;
}

bool RedpitayaInterface::getrcvError()
{
    return rcvError;
}

/****************************************************************************
 * PUBLIC SLOTS SECTION
 -----------------------------------------------------------------------*//**
 * @privatesection
 ****************************************************************************/
void RedpitayaInterface::on_SingleAcquisitionRequest()
{
    singleAcquisitionRunning = true;
    singleAcquisition();
    singleAcquisitionRunning = false;
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
 * @brief RedpitayaInterface::checkCOM
 * @return 0 if ok, -1 if not connected
 * Checks if the serial communication is still running
 */
int RedpitayaInterface::checkCOM()
{
    if( serial->isOpen() == false )
    {
        rpState = DISCONNECTED;
        qDebug() << " checkCOM failed!";
        return -1;
    }
    return 0;
}

/**
 * @brief RedpitayaInterface::startServer
 * @return 0 if ok, -1 if not
 * Starts the sampling and transmission server
 */
int RedpitayaInterface::startServer()
{
    // check if com is still up
    if(checkCOM()) return -1;

    // set leds
    writeData("\n\nmonitor 0x40000030 0xF0\n");

    // Create command from Parameter structure
    mutex.lock();
    QString cmd_base = "/opt/ddrdump/preview/preview_rp_remote_acquire ";
    QString port = QString().sprintf("-p %d ", rpStreamParams.port);
    QString mode = QString().sprintf("-m 2 ");
    QString chan = QString().sprintf("-c %d ", rpStreamParams.channel);
    QString dec  = QString().sprintf("-d %d ", rpStreamParams.decimation);
    QString num  = QString().sprintf("-k %d ", rpStreamParams.numKBytes);
    mutex.unlock();

    rpStreamParams.reportRate = true;
    rpStreamParams.acquireDuration =
            (1024*rpStreamParams.numKBytes) /
            (125000000/(double)rpStreamParams.decimation);

    QString flags = NULL;
    if(rpStreamParams.noEQ)
        flags.append("-e ");
    if(rpStreamParams.noShaping)
        flags.append("-s ");
    if(rpStreamParams.reportRate)
        flags.append("-r ");

    QString cmd = cmd_base + port + mode +
            chan + dec + num + flags + QString("\n");

    // Execute command, start server
    writeData(cmd);
    serial->flush();
    rpState = RUNNING;
    return 0;
}

/**
 * @brief RedpitayaInterface::stopServer
 *
 * Stops the sampling and transmission server
 */
int RedpitayaInterface::stopServer()
{
    // check if com is still up
    if(checkCOM()) return -1;

    // send escape sequence
    writeData("\x03");
    writeData("\n\nmonitor 0x40000030 0x0f\n");
    rpState = CONNECTED;

    return 0;
}

/**
 * @brief getData
 *
 * Opens the network socket and receives n bytes
 */
int RedpitayaInterface::rcvData ()
{
    rcvError = false;
//    QElapsedTimer timer;
//    timer.start();

    // get requested number of bytes
    numkbytes = rpStreamParams.numKBytes;
    numbytes = numkbytes*1024;

    // Open TCP socket for fast data reception
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0) //TCP
    //if((sockfd = socket(AF_INET, SOCK_DGRAM, 0))< 0) //UDP
    {
       qDebug() << "\n Error : Could not create socket \n";
       disconnect();
       return errno;
    }

    // allocate memory to hold the converted short values
    data_buf = (int16_t*) malloc (2*numbytes);
    if (data_buf == NULL) {fputs ("Memory error buf",stderr); exit (2);}


    /*! try to establish a connection over the socket if not done so already */
    if(rpState != TCP_CONNECTED)
    {
        //set params
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(rpStreamParams.port);
        serv_addr.sin_addr.s_addr = inet_addr(rpStreamParams.IP.toStdString().c_str());
        // connect
        if(socket_connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
        {
           qDebug() << "\n Error : Connect Failed";
           qDebug() << " Err: " << errno << strerror(errno);
           qDebug() << "\n Sockfd=" << sockfd;
           rcvError = true;
           return errno;
        }
        else
        {
            //qDebug() << "Connection established";
            //rpState = TCP_CONNECTED;
        }
    }

    // read from socket buffer
    n_packets = recv(sockfd, data_buf, 2*numbytes, MSG_WAITALL);
    //printf("n=%d numbytes=%d\n", n_packets, numbytes);

    //! Publish Data
    free(publish_data_buf);
    publish_data_buf = (int16_t*) malloc (2*numbytes);
    if (publish_data_buf == NULL) {fputs ("Memory error buf",stderr); exit (2);}
    memcpy(publish_data_buf, data_buf, 2*numbytes);
    publish_data_buf_params = rpStreamParams;

    // Close TCP socket
    close(sockfd);
    free(data_buf); data_buf = NULL;

    // done
    emit dataReady();
    return 0;
}
