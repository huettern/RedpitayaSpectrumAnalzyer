#ifndef FFTWIDGET_H
#define FFTWIDGET_H

#include <QWidget>
#include "redpitayainterface.h"

class RedpitayaInterface;

namespace Ui {
class FFTWidget;
}

class FFTWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FFTWidget(QWidget *parent = 0);
    ~FFTWidget();

    void setRPif(RedpitayaInterface* ifc);

private slots:
    void on_SampleMButton_toggled(bool checked);

    void on_SamplekButton_toggled(bool checked);

    void on_SampleSButton_toggled(bool checked);

    void on_SampleRateBox_valueChanged(int arg1);

    void on_templateComboBox_currentIndexChanged(int index);

    void on_applyBtn_clicked();

private:
    Ui::FFTWidget *ui;
    RedpitayaInterface *rpif;

    void calcEffectiveRate ();

    bool tempaltesFilled;

    typedef struct {
        unsigned int refreshRate;
        unsigned int sampleRate;
        unsigned int effectiveRate;
        unsigned int blockSize;
    } tstParams;
    QVector<tstParams> *params;
};

#endif // FFTWIDGET_H
