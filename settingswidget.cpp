#include "settingswidget.h"
#include "ui_settingswidget.h"

#include <QtSerialPort/QSerialPortInfo>

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    // list serial ports
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->COMPortsComboBox->addItem(info.portName());
    }
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::setRpifHandler (RedpitayaInterface* rpif)
{
    m_rpif = rpif;
}

void SettingsWidget::on_connectButton_clicked()
{
    int retval;
    retval = m_rpif->Connect(
                ui->IPLineEdit->text().toStdString().c_str(),
                ui->portSpinBox->value(),
                ui->COMPortsComboBox->currentText());
    if(retval == 0)
    {
        ui->statusLabel->setText("Connected");
        ui->statusLabel->setStyleSheet
                ("QLabel { color : green; }");
        this->lockInputs();
    }
    else
    {
        ui->statusLabel->setText("Disconnected");
        ui->statusLabel->setStyleSheet
                ("QLabel { color : red; }");
        this->unlockInputs();
    }
}

void SettingsWidget::on_disconnectButton_clicked()
{
    m_rpif->Disconnect();
    ui->statusLabel->setText("Disconnected");
    ui->statusLabel->setStyleSheet
            ("QLabel { color : red; }");
    this->unlockInputs();
}

void SettingsWidget::lockInputs()
{
    ui->IPLineEdit->setEnabled(false);
    ui->COMPortsComboBox->setEnabled(false);
    ui->portSpinBox->setEnabled(false);
}


void SettingsWidget::unlockInputs()
{
    ui->IPLineEdit->setEnabled(true);
    ui->COMPortsComboBox->setEnabled(true);
    ui->portSpinBox->setEnabled(true);
}

