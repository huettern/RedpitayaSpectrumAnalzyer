#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    // Setup UI
    m_ui->setupUi(this);

    //Setup Plot
    m_plot = new SpectrumPlot(this, m_ui->plot);

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
    m_fft_thread->start();

    /*! Setup other widgets */
    m_ui->viewWidget->setRPif(m_rpif);
    m_ui->viewWidget->setSpectrumPlot(m_ui->plot);

    m_ui->fftwidget->setRPif(m_rpif);
    m_ui->fftwidget->setFFTObj(m_fft);
}

MainWindow::~MainWindow()
{
    m_rpif->Disconnect();

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
