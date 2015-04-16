#include "parameterwidget.h"
#include "ui_parameterwidget.h"

#include <QDebug>

ParameterWidget::ParameterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParameterWidget)
{
    ui->setupUi(this);

    parameter_ctr = 0;
    startTimer(1000);
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

void ParameterWidget::timerEvent(QTimerEvent *event)
{
    QString str = "";
    FFT::tstFFTParams FFTPar;
    RedpitayaInterface::tsRPStreamParams RPPar;

    // Only continue if all objects are passed
    if(parameter_ctr < 2) return;
    FFTPar = fft->getParams();

    /*! FFT parameters */
    str.sprintf("%d", FFTPar.visiblePoints);
    ui->valVisiblePoints->setText(str);
    str.sprintf("%f", FFTPar.resolution);
    ui->valFrequencyResolution->setText(str);
    //str.sprintf("%s", pFFTPar.window.toStdString().c_str());
    ui->valWindow->setText("None (Flat Top)");
    str.sprintf("%d", FFTPar.nSamples);
    ui->valRealSamples->setText(str);
    str.sprintf("%d", FFTPar.numZeroes);
    ui->valZeroes->setText(str);
    str.sprintf("%d", FFTPar.elapsedTimeUs);
    ui->valCalculationTime->setText(str);


    /*! Acquisition parameters */
    RPPar = rpif->getStreamParams();
    str.sprintf("%d kS/s", 125000000.0/(RPPar.decimation));
    ui->valSamplerate->setText(str);
    str.sprintf("%d", RPPar.decimation);
    ui->valDecimation->setText(str);
    str.sprintf("%s", RPPar.channel ? "A" : "B");
    ui->valChannel->setText(str);
    str.sprintf("%s", RPPar.noShaping ? "ON" : "OFF");
    ui->valShapingFilter->setText(str);
    str.sprintf("%s", RPPar.noEQ ? "ON" : "OFF");
    ui->valEqualization->setText(str);
}
