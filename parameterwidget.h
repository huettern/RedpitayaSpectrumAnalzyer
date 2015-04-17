#ifndef PARAMETERWIDGET_H
#define PARAMETERWIDGET_H

#include <QWidget>

#include "redpitayainterface.h"
#include "fft.h"

class RedpitayaInterface;
class FFT;

namespace Ui {
class ParameterWidget;
}

class ParameterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ParameterWidget(QWidget *parent = 0);
    ~ParameterWidget();

    void setRPif(RedpitayaInterface* ifc);
    void setFFTObj(FFT* obj);
    void setRunning(bool state);


protected:
    void timerEvent(QTimerEvent *event);

private:
    Ui::ParameterWidget *ui;
    RedpitayaInterface *rpif;
    FFT *fft;

    int parameter_ctr;
    int timerID;
    bool timerRunning;

};

#endif // PARAMETERWIDGET_H
