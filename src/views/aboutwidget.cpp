/**
 * @file aboutwidget.cpp
 * @brief 关于页面组件实现
 */

#include "aboutwidget.h"
#include "ui_aboutwidget.h"

AboutWidget::AboutWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AboutWidget)
{
    ui->setupUi(this);
}

AboutWidget::~AboutWidget()
{
    delete ui;
}
