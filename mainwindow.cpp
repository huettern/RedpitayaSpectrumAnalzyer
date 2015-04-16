#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    // Setup UI
    m_ui->setupUi(this);

    //Setup Plot
    m_plot = new SpectrumPlot(this);
    m_plot->setPlot(m_ui->plot);
    m_plot->setFFT(m_fft);

    //Setup Redpitaya object
    m_rpif = new RedpitayaInterface(this);
    m_ui->settingswidget->setRpifHandler(m_rpif);
    connect(m_rpif, SIGNAL(setStatusMsg(QString,int)),
            this, SLOT(setStatusBarMsg(QString,int)));

    //Setup FFT
    m_fft = new FFT();
    m_fft->setRPif(m_rpif);
    m_fft->setPlot(m_ui->plot);
    m_fft_thread = new QThread();
    m_fft->setThread(m_fft_thread);
    m_fft->moveToThread(m_fft_thread);
    m_fft_thread->start(QThread::NormalPriority);

    /*! connect FFT signal and plot slot */
    qRegisterMetaType<QVector<double> >("QVector<double>");
    connect(m_fft, SIGNAL(dataReady(QVector<double>,QVector<double>)),
            m_plot, SLOT(onDataChanged(QVector<double>,QVector<double>)));

    /*! Setup other widgets */
    m_ui->viewWidget->setRPif(m_rpif);
    m_ui->viewWidget->setSpectrumPlot(m_ui->plot);

    m_ui->fftwidget->setRPif(m_rpif);
    m_ui->fftwidget->setFFTObj(m_fft);

    m_ui->parameterwidget->setRPif(m_rpif);
    m_ui->parameterwidget->setFFTObj(m_fft);

}

MainWindow::~MainWindow()
{
    m_rpif->Disconnect();

    m_fft->abortThread(); //Tell the thread to abort
    if(!m_fft_thread->wait(5000)) //Wait until it actually has terminated (max. 5 sec)
    {
        qWarning("Thread deadlock detected, bad things may happen !!!");
        m_fft_thread->terminate(); //Thread didn't exit in time, probably deadlocked, terminate it!
        m_fft_thread->wait(); //Note: We have to wait again here!
    }

    delete m_rpif;
    delete m_fft;
    delete m_plot;
    delete m_fft_thread;
    delete m_ui;
}

/**
 * @brief MainWindow::on_MainWindow_destroyed
 *
 * Gets called if mainwindow is closed
 */
void MainWindow::on_MainWindow_destroyed()
{
}

/**
 * @brief MainWindow::setStatusBarMsg
 * @param str Message to be desplayed
 * @param time Duration of the message. 0 = infinite
 */
void MainWindow::setStatusBarMsg (QString str, int time)
{
    m_ui->statusBar->showMessage(str, time);
}

void MainWindow::on_RUNButton_clicked()
{
    m_fft->setConConversionRunning(true);
}

void MainWindow::on_STOPButton_clicked()
{
    m_fft->setConConversionRunning(false);
}

void MainWindow::on_SINGLEButton_clicked()
{
    // TODO: do nothing if not connected
    m_rpif->singleAcquisition();
    m_fft->singleConversion();
}
