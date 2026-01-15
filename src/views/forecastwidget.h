/**
 * @file forecastwidget.h
 * @brief 天气预报展示组件
 */

#ifndef FORECASTWIDGET_H
#define FORECASTWIDGET_H

#include <QWidget>
#include <QFrame>
#include "../models/weatherdata.h"

namespace Ui {
class ForecastWidget;
}

/**
 * @class ForecastWidget
 * @brief 天气预报展示界面
 * 
 * 显示逐小时预报和多日预报
 */
class ForecastWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ForecastWidget(QWidget *parent = nullptr);
    ~ForecastWidget();
    
    /**
     * @brief 设置当前城市
     */
    void setCity(const QString &cityId, const QString &cityName);
    
    /**
     * @brief 更新逐小时预报
     */
    void updateHourlyForecast(const QList<HourlyForecast> &forecast);
    
    /**
     * @brief 更新每日预报
     */
    void updateDailyForecast(const QList<DailyForecast> &forecast);
    
    /**
     * @brief 清空显示
     */
    void clear();

signals:
    void refreshRequested(const QString &cityId);

private slots:
    void onRefreshClicked();

private:
    void setupConnections();
    void clearHourlyItems();
    void clearDailyItems();
    QFrame* createHourlyItem(const HourlyForecast &forecast);
    QFrame* createDailyItem(const DailyForecast &forecast);
    QString getWeatherEmoji(const QString &code);
    QString getWeekdayName(const QDate &date);

private:
    Ui::ForecastWidget *ui;
    QString m_currentCityId;
    QString m_currentCityName;
    QList<QFrame*> m_hourlyItems;
    QList<QFrame*> m_dailyItems;
};

#endif // FORECASTWIDGET_H
