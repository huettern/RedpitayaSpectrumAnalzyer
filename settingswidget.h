#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

#include "redpitayainterface.h"
#include "consolewindow.h"

namespace Ui {
class SettingsWidget;
}

class RedpitayaInterface;

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = 0);
    ~SettingsWidget();

    void setRpifHandler (RedpitayaInterface* rpif);

private slots:
    void on_connectButton_clicked();

    void on_disconnectButton_clicked();

    void on_openConsoleButton_clicked();

private:
    Ui::SettingsWidget *ui;
    RedpitayaInterface* m_rpif;
    ConsoleWindow* m_consolewindow;


    void lockInputs();
    void unlockInputs();
};

#endif // SETTINGSWIDGET_H
