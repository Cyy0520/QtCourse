/**
 * @file alertwidget.h
 * @brief 天气预警展示组件
 */

#ifndef ALERTWIDGET_H
#define ALERTWIDGET_H

#include <QWidget>
#include <QFrame>
#include "../models/weatherdata.h"

namespace Ui {
class AlertWidget;
}

class AlertWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AlertWidget(QWidget *parent = nullptr);
    ~AlertWidget();
    
    void updateAlerts(const QList<WeatherAlert> &alerts);
    void clear();

private:
    void clearAlertCards();
    QFrame* createAlertCard(const WeatherAlert &alert);
    QString getLevelColor(const QString &level);
    QString getLevelIcon(const QString &level);

private:
    Ui::AlertWidget *ui;
    QList<QFrame*> m_alertCards;
};

#endif // ALERTWIDGET_H
