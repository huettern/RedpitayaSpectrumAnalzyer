#include "fftwidget.h"
#include "ui_fftwidget.h"

#include <math.h>

#include <qdebug.h>

FFTWidget::FFTWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FFTWidget)
{
    tempaltesFilled = false;

    tstParams par;
    ui->setupUi(this);
    params = new QVector<tstParams>;

    // Setup Templates
    ui->templateComboBox->addItem("Maximum Frequency");
    par.blockSize = 16;
    par.sampleRate = 15625000;
    par.effectiveRate = 15625000;
    par.refreshRate = 5;
    par.zeroPads = 0;
    params->append(par);

    ui->templateComboBox->addItem("Audio Spectrum");
    par.blockSize = 16;
    par.sampleRate = 125000000/2048;
    par.effectiveRate = 125000000/2048;
    par.refreshRate = 5;
    par.zeroPads = 0;
    params->append(par);

    ui->templateComboBox->addItem("Testing");
    par.blockSize = 16;
    par.sampleRate = 125000000/8;
    par.effectiveRate = 125000000/8;
    par.refreshRate = 20;
    par.zeroPads = 0;
    params->append(par);

    ui->templateComboBox->setCurrentIndex(2);
    tempaltesFilled = true;
}

FFTWidget::~FFTWidget()
{
    delete ui;
}

void FFTWidget::setRPif(RedpitayaInterface* ifc)
{
    rpif = ifc;
}


void FFTWidget::setFFTObj(FFT* obj)
{
    fft = obj;
}



void FFTWidget::calcEffectiveRate()
{
    int inVal;
    //calculate next near sample rate
    inVal = ui->SampleRateBox->value();
    if(ui->SamplekButton->isChecked()) inVal *= 1000;
    if(ui->SampleMButton->isChecked()) inVal *= 1000000;

    /*! calculates the nearest decimation value */
    int d = 0;
    int32_t best = 0xefffffff;
    int exp = 0;
    for(int i = 0; i < 17; i++)
    {
        d = (125000000/pow(2, i)) - inVal;
        if(abs(d) < abs(best))
        {
            best = abs(d);
            exp = i;
        }
    }

    ui->effectiveBox->setText(
                QString::number((125000000/pow(2, exp)), 'f', 2));
    ui->decimationBox->setText(QString::number(pow(2, exp)));
}

void FFTWidget::on_SampleMButton_toggled(bool checked)
{
    if(checked)
    {
        ui->SamplekButton->setChecked(false);
        ui->SampleSButton->setChecked(false);
    }
    calcEffectiveRate();
}

void FFTWidget::on_SamplekButton_toggled(bool checked)
{
    if(checked)
    {
        ui->SampleMButton->setChecked(false);
        ui->SampleSButton->setChecked(false);
    }
    calcEffectiveRate();
}

void FFTWidget::on_SampleSButton_toggled(bool checked)
{
    if(checked)
    {
        ui->SamplekButton->setChecked(false);
        ui->SampleMButton->setChecked(false);
    }
    calcEffectiveRate();
}

void FFTWidget::on_SampleRateBox_valueChanged(int arg1)
{
    this->calcEffectiveRate();
}

void FFTWidget::on_templateComboBox_currentIndexChanged(int index)
{
    if(tempaltesFilled != true) return;

    int idx = ui->templateComboBox->currentIndex();
    tstParams par = params->at(idx);

    ui->SamplekButton->setChecked(false);
    ui->SampleMButton->setChecked(false);
    ui->SampleSButton->setChecked(true);

    ui->effectiveBox->setText(QString::number(par.effectiveRate));
    ui->SampleRateBox->setValue(par.sampleRate);
    ui->RefreshRateBox->setValue(par.refreshRate);
    ui->BlockSizeBox->setValue(par.blockSize);
    ui->zeroPadBox->setValue(par.zeroPads);
}

void FFTWidget::on_applyBtn_clicked()
{
    rpif->setBlockSize(ui->BlockSizeBox->value());
    rpif->setDecimation(ui->decimationBox->text().toInt());
    fft->setRefreshInterval(ui->RefreshRateBox->value());
    fft->setNumZeroes(ui->zeroPadBox->value());
}
