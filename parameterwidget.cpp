#include "parameterwidget.h"
#include "ui_parameterwidget.h"
#include "helper.h"

#include <QDebug>

ParameterWidget::ParameterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterWidget)
{
    ui->setupUi(this);

    parameter_ctr = 0;
    timerID = startTimer(1000);
    timerRunning = true;
}

ParameterWidget::~ParameterWidget()
{
    delete ui;
}

void ParameterWidget::setRPif(RedpitayaInterface *ifc)
{
    rpif = ifc;
    parameter_ctr++;
}

void ParameterWidget::setFFTObj(FFT *obj)
{
    fft = obj;
    parameter_ctr++;
}

void ParameterWidget::setRunning(bool state)
{
    if(state && !timerRunning)
    {
        timerID = startTimer(1000);
        timerRunning = true;
    }
    else if (timerRunning)
    {
        killTimer(timerID);
        timerRunning = false;
    }
}

void ParameterWidget::timerEvent(QTimerEvent *event)
{
    QString str = "";
    char* chararr;
    FFT::tstFFTParams FFTPar;
    RedpitayaInterface::tsRPStreamParams RPPar;
    double in;

    // Only continue if all objects are passed
    if(parameter_ctr < 2) return;
    FFTPar = fft->getParams();

    /*! FFT parameters */
    double bw = (FFTPar.visiblePoints-1)*FFTPar.resolution;
    chararr = Helper::double2eng(bw, 3, false);
    str.sprintf("%sHz", chararr);
    ui->valBandwidth->setText(str);

    str.sprintf("%d", FFTPar.visiblePoints);
    ui->valVisiblePoints->setText(str);

    double res = FFTPar.resolution;
    chararr = Helper::double2eng(res, 3, false);
    str.sprintf("%sHz/step", chararr);
    ui->valFrequencyResolution->setText(str);

    //str.sprintf("%s", pFFTPar.window.toStdString().c_str());
    ui->valWindow->setText("None (Flat Top)");

    str.sprintf("%d", FFTPar.nSamples);
    ui->valRealSamples->setText(str);

    str.sprintf("%d", FFTPar.numZeroesReal);
    ui->valZeroes->setText(str);

    in = (double)FFTPar.elapsedTimeUs/1000000;
    chararr = Helper::double2eng(in, 3, false);
    str.sprintf("%ss", chararr);
    ui->valCalculationTime->setText(str);


    /*! Acquisition parameters */
    RPPar = rpif->getStreamParams();

    in = 125000000.0/(RPPar.decimation);
    chararr = Helper::double2eng(in, 3, false);
    str.sprintf("%sS/s", chararr);
    ui->valSamplerate->setText(str);

    in = 1024*RPPar.numKBytes;
    chararr = Helper::double2eng(in, 3, false);
    str.sprintf("%sS", chararr);
    ui->valNumSamples->setText(str);

    str.sprintf("%d", RPPar.decimation);
    ui->valDecimation->setText(str);

    str.sprintf("%s", (RPPar.channel == 0) ? "IN1" : "IN2");
    ui->valChannel->setText(str);

    str.sprintf("%s", (RPPar.noShaping == true) ? "OFF" : "ON");
    ui->valShapingFilter->setText(str);

    str.sprintf("%s", (RPPar.noEQ == true) ? "OFF" : "ON");
    ui->valEqualization->setText(str);

    in = RPPar.acquireDuration;
    chararr = Helper::double2eng(in, 3, false);
    str.sprintf("%ss", chararr);
    ui->valAcquireDuration->setText(str);
}
