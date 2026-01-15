/**
 * @file aboutwidget.h
 * @brief 关于页面组件
 */

#ifndef ABOUTWIDGET_H
#define ABOUTWIDGET_H

#include <QWidget>

namespace Ui {
class AboutWidget;
}

/**
 * @class AboutWidget
 * @brief 显示应用程序信息的关于页面
 */
class AboutWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AboutWidget(QWidget *parent = nullptr);
    ~AboutWidget();

private:
    Ui::AboutWidget *ui;
};

#endif // ABOUTWIDGET_H
