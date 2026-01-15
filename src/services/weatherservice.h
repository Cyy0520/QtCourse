/**
 * @file weatherservice.h
 * @brief 天气服务类声明
 */

#ifndef WEATHERSERVICE_H
#define WEATHERSERVICE_H

#include <QObject>
#include "../models/weatherdata.h"
#include "../network/networkmanager.h"

/**
 * @class WeatherService
 * @brief 天气数据服务类
 * 
 * 负责从天气API获取数据并解析
 */
class WeatherService : public QObject
{
    Q_OBJECT

public:
    static WeatherService& instance();
    
    /**
     * @brief 设置API密钥
     * @param key API密钥
     */
    void setApiKey(const QString &key);
    
    /**
     * @brief 获取当前天气
     * @param cityId 城市ID
     */
    void fetchCurrentWeather(const QString &cityId);
    
    /**
     * @brief 获取逐小时预报
     * @param cityId 城市ID
     * @param hours 小时数(24/72/168)
     */
    void fetchHourlyForecast(const QString &cityId, int hours = 24);
    
    /**
     * @brief 获取每日预报
     * @param cityId 城市ID
     * @param days 天数(3/7/10/15)
     */
    void fetchDailyForecast(const QString &cityId, int days = 7);
    
    /**
     * @brief 获取生活指数
     * @param cityId 城市ID
     */
    void fetchLifeIndex(const QString &cityId);
    
    /**
     * @brief 获取天气预警
     * @param cityId 城市ID
     */
    void fetchWeatherAlert(const QString &cityId);
    
    /**
     * @brief 获取空气质量
     * @param cityId 城市ID
     */
    void fetchAirQuality(const QString &cityId);

signals:
    void currentWeatherReady(const CurrentWeather &weather);
    void hourlyForecastReady(const QList<HourlyForecast> &forecast);
    void dailyForecastReady(const QList<DailyForecast> &forecast);
    void lifeIndexReady(const QList<LifeIndex> &indices);
    void weatherAlertReady(const QList<WeatherAlert> &alerts);
    void errorOccurred(const QString &error);

private slots:
    void onRequestFinished(const QString &url, const NetworkResponse &response);

private:
    explicit WeatherService(QObject *parent = nullptr);
    ~WeatherService() = default;
    
    WeatherService(const WeatherService&) = delete;
    WeatherService& operator=(const WeatherService&) = delete;
    
    // JSON解析方法
    CurrentWeather parseCurrentWeather(const QJsonObject &json, const QString &cityId);
    QList<HourlyForecast> parseHourlyForecast(const QJsonObject &json);
    QList<DailyForecast> parseDailyForecast(const QJsonObject &json);
    QList<LifeIndex> parseLifeIndex(const QJsonObject &json);
    QList<WeatherAlert> parseWeatherAlert(const QJsonObject &json);
    void parseAirQuality(const QJsonObject &json, CurrentWeather &weather);
    
    QString buildUrl(const QString &endpoint, const QString &cityId, const QMap<QString, QString> &params = {});

private:
    QString m_apiKey;
    QString m_baseUrl;
    QString m_pendingCityId;
    
    // 请求类型标识
    enum RequestType {
        CurrentWeatherRequest,
        HourlyForecastRequest,
        DailyForecastRequest,
        LifeIndexRequest,
        AlertRequest,
        AirQualityRequest
    };
    QMap<QString, RequestType> m_pendingRequests;
};

#endif // WEATHERSERVICE_H
