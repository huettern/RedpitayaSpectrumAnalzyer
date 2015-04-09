#ifndef CONSOLEWINDOW_H
#define CONSOLEWINDOW_H

#include <QMainWindow>

#include <QPlainTextEdit>

namespace Ui {
class ConsoleWindow;
}

class ConsoleWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConsoleWindow(QWidget *parent = 0);
    ~ConsoleWindow();

    void putData(const QByteArray &data);

protected:

private:
    Ui::ConsoleWindow *ui;
    // is set if the received data is currently in a escape seq
    bool inEcapeSeq;

signals:
    void getData(QString str);

private slots:
    void on_sendButton_clicked();
};

#endif // CONSOLEWINDOW_H
