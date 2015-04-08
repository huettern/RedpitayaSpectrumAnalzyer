#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "spectrumplot.h"
#include "redpitayainterface.h"

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

private:
    Ui::MainWindow *m_ui;
    SpectrumPlot *m_plot;
    RedpitayaInterface *m_rpif;

};

#endif // MAINWINDOW_H
