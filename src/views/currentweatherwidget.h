/**
 * @file currentweatherwidget.h
 * @brief 实时天气展示组件
 */

#ifndef CURRENTWEATHERWIDGET_H
#define CURRENTWEATHERWIDGET_H

#include <QWidget>
#include "../models/weatherdata.h"

namespace Ui {
class CurrentWeatherWidget;
}

/**
 * @class CurrentWeatherWidget
 * @brief 实时天气展示界面
 * 
 * 显示当前天气信息，包括温度、天气状况、空气质量等
 */
class CurrentWeatherWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CurrentWeatherWidget(QWidget *parent = nullptr);
    ~CurrentWeatherWidget();
    
    /**
     * @brief 设置当前城市
     * @param cityId 城市ID
     * @param cityName 城市名称
     */
    void setCity(const QString &cityId, const QString &cityName);
    
    /**
     * @brief 更新天气数据
     * @param weather 天气数据
     */
    void updateWeather(const CurrentWeather &weather);
    
    /**
     * @brief 清空显示
     */
    void clear();

signals:
    /**
     * @brief 请求刷新天气数据
     * @param cityId 城市ID
     */
    void refreshRequested(const QString &cityId);

private slots:
    void onRefreshClicked();

private:
    void setupConnections();
    QString getWeatherEmoji(const QString &code);
    QString getAqiColor(int aqi);
    QString getAqiLevel(int aqi);

private:
    Ui::CurrentWeatherWidget *ui;
    QString m_currentCityId;
    QString m_currentCityName;
};

#endif // CURRENTWEATHERWIDGET_H
