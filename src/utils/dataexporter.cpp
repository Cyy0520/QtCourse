/**
 * @file dataexporter.cpp
 * @brief 数据导出工具类实现
 */

#include "dataexporter.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>

bool DataExporter::exportCurrentWeatherToJson(const CurrentWeather &weather, const QString &filePath)
{
    QJsonObject obj;
    obj["cityId"] = weather.cityId;
    obj["cityName"] = weather.cityName;
    obj["temperature"] = weather.temperature;
    obj["feelsLike"] = weather.feelsLike;
    obj["humidity"] = weather.humidity;
    obj["pressure"] = weather.pressure;
    obj["visibility"] = weather.visibility;
    obj["windSpeed"] = weather.windSpeed;
    obj["windDirection"] = weather.windDirection;
    obj["weatherDesc"] = weather.weatherDesc;
    obj["aqi"] = weather.aqi;
    obj["aqiLevel"] = weather.aqiLevel;
    obj["sunriseTime"] = weather.sunriseTime;
    obj["sunsetTime"] = weather.sunsetTime;
    obj["updateTime"] = weather.updateTime.toString(Qt::ISODate);
    
    QJsonDocument doc(obj);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool DataExporter::exportForecastToJson(const QList<DailyForecast> &forecast, const QString &filePath)
{
    QJsonArray arr;
    
    for (const DailyForecast &day : forecast) {
        QJsonObject obj;
        obj["date"] = day.date.toString("yyyy-MM-dd");
        obj["highTemp"] = day.highTemp;
        obj["lowTemp"] = day.lowTemp;
        obj["humidity"] = day.humidity;
        obj["weatherDay"] = day.weatherDescDay;
        obj["weatherNight"] = day.weatherDescNight;
        obj["windSpeed"] = day.windSpeed;
        obj["windDirection"] = day.windDirection;
        obj["precipitationProb"] = day.precipitationProb;
        obj["uvIndex"] = day.uvIndex;
        arr.append(obj);
    }
    
    QJsonDocument doc(arr);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool DataExporter::exportForecastToCsv(const QList<DailyForecast> &forecast, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // CSV 头
    out << "日期,最高温,最低温,湿度,白天天气,夜间天气,风速,风向,降水概率,紫外线指数\n";
    
    for (const DailyForecast &day : forecast) {
        out << day.date.toString("yyyy-MM-dd") << ","
            << day.highTemp << ","
            << day.lowTemp << ","
            << day.humidity << ","
            << day.weatherDescDay << ","
            << day.weatherDescNight << ","
            << day.windSpeed << ","
            << day.windDirection << ","
            << day.precipitationProb << ","
            << day.uvIndex << "\n";
    }
    
    file.close();
    return true;
}

bool DataExporter::exportHourlyToCsv(const QList<HourlyForecast> &forecast, const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // CSV 头
    out << "时间,温度,湿度,天气,风速,风向,降水概率\n";
    
    for (const HourlyForecast &hour : forecast) {
        out << hour.time.toString("yyyy-MM-dd HH:mm") << ","
            << hour.temperature << ","
            << hour.humidity << ","
            << hour.weatherDesc << ","
            << hour.windSpeed << ","
            << hour.windDirection << ","
            << hour.precipitationProb << "\n";
    }
    
    file.close();
    return true;
}
