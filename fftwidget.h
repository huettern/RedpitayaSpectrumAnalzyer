#ifndef FFTWIDGET_H
#define FFTWIDGET_H

#include <QWidget>

namespace Ui {
class FFTWidget;
}

class FFTWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FFTWidget(QWidget *parent = 0);
    ~FFTWidget();

private slots:
    void on_SampleMButton_toggled(bool checked);

    void on_SamplekButton_toggled(bool checked);

    void on_SampleSButton_toggled(bool checked);

    void on_SampleRateBox_valueChanged(int arg1);

private:
    Ui::FFTWidget *ui;

    void calcEffectiveRate ();
};

#endif // FFTWIDGET_H
