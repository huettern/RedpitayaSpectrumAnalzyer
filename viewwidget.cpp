#include "viewwidget.h"
#include "ui_viewwidget.h"

ViewWidget::ViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewWidget)
{
    ui->setupUi(this);
    rawplot = new PlotView();
}

ViewWidget::~ViewWidget()
{
    delete ui;
}

void ViewWidget::on_rawplotButton_clicked()
{
    rawplot->show();
    rawplot->raise();
}
