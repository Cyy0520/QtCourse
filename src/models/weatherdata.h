/**
 * @file weatherdata.h
 * @brief 天气数据结构定义
 */

#ifndef WEATHERDATA_H
#define WEATHERDATA_H

#include <QString>
#include <QDateTime>
#include <QList>

/**
 * @struct CurrentWeather
 * @brief 当前天气数据
 */
struct CurrentWeather {
    QString cityId;
    QString cityName;
    double temperature = 0;      // 温度(℃)
    double feelsLike = 0;        // 体感温度
    int humidity = 0;            // 湿度(%)
    int pressure = 0;            // 气压(hPa)
    int visibility = 0;          // 能见度(km)
    double windSpeed = 0;        // 风速(km/h)
    QString windDirection;       // 风向
    int windDegree = 0;          // 风向角度
    QString weatherCode;         // 天气代码
    QString weatherDesc;         // 天气描述
    QString weatherIcon;         // 天气图标
    int cloudCover = 0;          // 云量(%)
    double uvIndex = 0;          // 紫外线指数
    int aqi = 0;                 // 空气质量指数
    QString aqiLevel;            // 空气质量等级
    double pm25 = 0;
    double pm10 = 0;
    double o3 = 0;
    QString sunriseTime;         // 日出时间
    QString sunsetTime;          // 日落时间
    QDateTime observationTime;   // 观测时间
    QDateTime updateTime;        // 更新时间
    
    bool isValid() const { return !cityId.isEmpty(); }
};

/**
 * @struct HourlyForecast
 * @brief 逐小时预报
 */
struct HourlyForecast {
    QDateTime time;
    double temperature = 0;
    int humidity = 0;
    QString weatherCode;
    QString weatherDesc;
    QString weatherIcon;
    double windSpeed = 0;
    QString windDirection;
    int precipitationProb = 0;   // 降水概率(%)
    double precipitation = 0;    // 降水量(mm)
};

/**
 * @struct DailyForecast
 * @brief 每日预报
 */
struct DailyForecast {
    QDate date;
    double highTemp = 0;         // 最高温度
    double lowTemp = 0;          // 最低温度
    int humidity = 0;
    QString weatherCodeDay;      // 白天天气代码
    QString weatherDescDay;      // 白天天气描述
    QString weatherIconDay;
    QString weatherCodeNight;    // 夜间天气代码
    QString weatherDescNight;    // 夜间天气描述
    QString weatherIconNight;
    double windSpeed = 0;
    QString windDirection;
    int precipitationProb = 0;
    double precipitation = 0;
    double uvIndex = 0;
    QString sunriseTime;
    QString sunsetTime;
};

/**
 * @struct WeatherHistory
 * @brief 历史天气数据
 */
struct WeatherHistory {
    QDate date;
    double avgTemp = 0;
    double maxTemp = 0;
    double minTemp = 0;
    int humidity = 0;
    int pressure = 0;
    double windSpeed = 0;
    QString weatherCode;
    QString weatherDesc;
    double precipitation = 0;
};

/**
 * @struct LifeIndex
 * @brief 生活指数
 */
struct LifeIndex {
    QString type;           // 指数类型
    QString name;           // 指数名称
    QString level;          // 等级
    QString category;       // 类别
    QString description;    // 描述
};

/**
 * @struct WeatherAlert
 * @brief 天气预警
 */
struct WeatherAlert {
    QString id;
    QString sender;         // 发布单位
    QString pubTime;        // 发布时间
    QString title;          // 标题
    QString status;         // 状态
    QString level;          // 预警等级
    QString type;           // 预警类型
    QString typeName;       // 预警类型名称
    QString text;           // 预警详情
};

#endif // WEATHERDATA_H
