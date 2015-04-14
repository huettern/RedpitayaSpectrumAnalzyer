#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include "spectrumplot.h"
#include "redpitayainterface.h"
#include "fft.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_MainWindow_destroyed();

    void on_RUNButton_clicked();

    void on_STOPButton_clicked();

    void on_SINGLEButton_clicked();

public slots:
    void setStatusBarMsg (QString str, int time = 0);

private:
    Ui::MainWindow       *m_ui;
    SpectrumPlot         *m_plot;
    RedpitayaInterface   *m_rpif;
    FFT                  *m_fft;
    QThread              *m_fft_thread;

};

#endif // MAINWINDOW_H
