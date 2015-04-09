#include "consolewindow.h"
#include "ui_consolewindow.h"

#include <QScrollBar>
#include <QDebug>

ConsoleWindow::ConsoleWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConsoleWindow)
{
    ui->setupUi(this);

    ui->consoleBox->document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::gray);
    setPalette(p);
    ui->consoleBox->setPalette(p);
    ui->commandLine->setPalette(p);
}

ConsoleWindow::~ConsoleWindow()
{
    delete ui;
}

void ConsoleWindow::putData(const QByteArray &data)
{
    qDebug() << "Put";

//    for(int i = 0; i < data.size(); i++)
//    {
//        // if we already are in a esc seq, clear byte if not end
//        if(inEcapeSeq)
//        {
//            if(data[i] == )
//        }

//        //check if escpae begin char
//        if(data[i] == 0x1B)
//    }

    ui->consoleBox->insertPlainText(QString(data).toLatin1());

    QScrollBar *bar = ui->consoleBox->verticalScrollBar();
    bar->setValue(bar->maximum());
}



void ConsoleWindow::on_sendButton_clicked()
{
    QString str = ui->commandLine->text() + '\n';
    ui->commandLine->selectAll();
    emit getData(str);
}
