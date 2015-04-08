#include "fftwidget.h"
#include "ui_fftwidget.h"

FFTWidget::FFTWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FFTWidget)
{
    ui->setupUi(this);
}

FFTWidget::~FFTWidget()
{
    delete ui;
}


void FFTWidget::calcEffectiveRate()
{
    int inVal;
    //calculate next near sample rate
    // TODO: calculate next near sample rate
    inVal = ui->SampleRateBox->value();
    if(ui->SamplekButton->isChecked()) inVal *= 1000;
    if(ui->SampleMButton->isChecked()) inVal *= 1000000;

    ui->effectiveBox->setText(QString::number(inVal));
}

void FFTWidget::on_SampleMButton_toggled(bool checked)
{
    if(checked)
    {
        ui->SamplekButton->setChecked(false);
        ui->SampleSButton->setChecked(false);
    }
    this->calcEffectiveRate();
}

void FFTWidget::on_SamplekButton_toggled(bool checked)
{
    if(checked)
    {
        ui->SampleMButton->setChecked(false);
        ui->SampleSButton->setChecked(false);
    }
    this->calcEffectiveRate();
}

void FFTWidget::on_SampleSButton_toggled(bool checked)
{
    if(checked)
    {
        ui->SamplekButton->setChecked(false);
        ui->SampleMButton->setChecked(false);
    }
    this->calcEffectiveRate();
}

void FFTWidget::on_SampleRateBox_valueChanged(int arg1)
{
    this->calcEffectiveRate();
}
