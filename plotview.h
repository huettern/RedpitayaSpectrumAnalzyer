#ifndef PLOTVIEW_H
#define PLOTVIEW_H

#include <QMainWindow>

#include "redpitayainterface.h"

class RedpitayaInterface;


namespace Ui {
class PlotView;
}

class PlotView : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotView(QWidget *parent = 0);
    ~PlotView();
    void closeEvent (QCloseEvent *event);
    void setRPif(RedpitayaInterface* ifc);

private slots:
    void on_quitButton_clicked();
    void on_autoScaleButton_clicked();

public slots:
    void on_dataChanged();

signals:
    void plotViewDestroyed();

private:
    Ui::PlotView *ui;
    RedpitayaInterface *rpif;

    QWidget *m_parent;

    // Data buffer
    int16_t* data_buf;
    QVector<double> x_vector;
    QVector<double> y_vector;


};

#endif // PLOTVIEW_H
