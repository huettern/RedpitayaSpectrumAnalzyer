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
