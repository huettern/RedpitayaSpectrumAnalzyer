#ifndef REDPITAYAINTERFACE_H
#define REDPITAYAINTERFACE_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QMutex>

#include <QtSerialPort/QtSerialPort>

#include "consolewindow.h"

#include <arpa/inet.h>

class RedpitayaInterface : public QObject
{
    Q_OBJECT
public:
    explicit RedpitayaInterface(QObject *parent = 0);
    ~RedpitayaInterface();

    int Connect(const char* ipadr, unsigned int port,
                QString COM, ConsoleWindow* console);
    void Disconnect();

    /*! Red pitaya states */
    enum enRpState
    {
        DISCONNECTED, ///< Rp is not connected
        CONNECTED,    ///< Rp is connected via Console
        RUNNING,      ///< The sampling and trasmission module is running
        TCP_CONNECTED ///< The TCP Socket is connected to ther RP
    };

    /*! Red pitaya streaming parameters: Which Channel */
    enum enRpStrParChannel
    {
        CH_A = 0,
        CH_B = 1
    };

    /*! Red pitaya streaming parameters */
    typedef struct
    {
        /*! Server Port */
        unsigned int        port;
        /*! Server IP */
        QString             IP;
        /*! number of KB per block */
        unsigned int        numKBytes;
        /*! report over console */
        bool                reportRate;
        /*! choose channel */
        enRpStrParChannel   channel;
        /*! Decimation */
        int                 decimation;
        /*! Disable Equalization */
        bool                noEQ;
        /*! Disable Shaping */
        bool                noShaping;
        /*! Time to sample the number of given samples at the given rate */
        double              acquireDuration;
    } tsRPStreamParams;

    int startStream ();
    int stopStream ();
    int rcvData ();
    int singleAcquisition();

    double getSamplerate(void);

    // Data handling
    size_t getDataArray (void* dest, size_t n);
    size_t getDataArraySize ();

    tsRPStreamParams getStreamParams ();

    // Settings Handling
    void setDecimation(unsigned int dec);
    void setBlockSize(unsigned int numkbytes);

    // status
    bool getsingleAcquisitionRunning();
    bool getrcvError();

signals:
    void setStatusMsg (QString, int);
    void dataReady();

public slots:
    void on_SingleAcquisitionRequest();

private slots:
    void readData();
    void writeData(QString str);

private:
    //Output Buffer
    int16_t* data_buf;
    size_t numkbytes, numbytes;

    // Published Data
    tsRPStreamParams publish_data_buf_params;
    int16_t* publish_data_buf;

    // Data for ethernet socket
    int sockfd;
    struct sockaddr_in serv_addr;
    unsigned long n_packets;

    bool running;
    bool singleAcquisitionRunning;
    bool rcvError;

    // Data for serial port
    QSerialPort* serial;

    // Threading
    QMutex mutex;

    // Console window
    ConsoleWindow* m_console;

    // Private methods
    int     checkCOM();
    int     startServer();
    int     stopServer();

    // Data
    enRpState           rpState;
    tsRPStreamParams    rpStreamParams;
};

#endif // REDPITAYAINTERFACE_H
