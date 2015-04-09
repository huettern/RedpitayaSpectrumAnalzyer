#ifndef FFT_H
#define FFT_H

#include <QObject>

class FFT : public QObject
{
    Q_OBJECT
public:
    explicit FFT(QObject *parent = 0);
    ~FFT();

signals:

public slots:
};

#endif // FFT_H
