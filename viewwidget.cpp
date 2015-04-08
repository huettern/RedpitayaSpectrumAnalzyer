#include "viewwidget.h"
#include "ui_viewwidget.h"

ViewWidget::ViewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewWidget)
{
    ui->setupUi(this);
}

ViewWidget::~ViewWidget()
{
    delete ui;
}
