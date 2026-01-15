/**
 * @file weatherservice.cpp
 * @brief 天气服务类实现
 */

#include "weatherservice.h"
#include <QJsonArray>
#include <QUrlQuery>
#include <QDebug>
#include <QRandomGenerator>

WeatherService::WeatherService(QObject *parent)
    : QObject(parent)
    , m_baseUrl("https://devapi.qweather.com/v7")
{
    connect(&NetworkManager::instance(), &NetworkManager::requestFinished,
            this, &WeatherService::onRequestFinished);
}

WeatherService& WeatherService::instance()
{
    static WeatherService instance;
    return instance;
}

void WeatherService::setApiKey(const QString &key)
{
    m_apiKey = key;
}

QString WeatherService::buildUrl(const QString &endpoint, const QString &cityId, const QMap<QString, QString> &params)
{
    QString url = m_baseUrl + endpoint + "?location=" + cityId + "&key=" + m_apiKey;
    
    for (auto it = params.begin(); it != params.end(); ++it) {
        url += "&" + it.key() + "=" + it.value();
    }
    
    return url;
}

void WeatherService::fetchCurrentWeather(const QString &cityId)
{
    if (m_apiKey.isEmpty()) {
        // 使用模拟数据
        CurrentWeather weather;
        weather.cityId = cityId;
        weather.temperature = 25;
        weather.feelsLike = 27;
        weather.humidity = 65;
        weather.pressure = 1013;
        weather.visibility = 10;
        weather.windSpeed = 12;
        weather.windDirection = "东南风";
        weather.windDegree = 135;
        weather.weatherCode = "100";
        weather.weatherDesc = "晴";
        weather.weatherIcon = "100";
        weather.cloudCover = 20;
        weather.uvIndex = 5;
        weather.aqi = 45;
        weather.aqiLevel = "优";
        weather.pm25 = 25;
        weather.pm10 = 40;
        weather.sunriseTime = "06:30";
        weather.sunsetTime = "18:45";
        weather.updateTime = QDateTime::currentDateTime();
        
        emit currentWeatherReady(weather);
        return;
    }
    
    QString url = buildUrl("/weather/now", cityId);
    m_pendingRequests[url] = CurrentWeatherRequest;
    m_pendingCityId = cityId;
    NetworkManager::instance().get(url, true, 300);
}

void WeatherService::fetchHourlyForecast(const QString &cityId, int hours)
{
    if (m_apiKey.isEmpty()) {
        // 模拟数据
        QList<HourlyForecast> forecast;
        QDateTime now = QDateTime::currentDateTime();
        
        for (int i = 0; i < hours && i < 24; ++i) {
            HourlyForecast h;
            h.time = now.addSecs(i * 3600);
            h.temperature = 20 + (QRandomGenerator::global()->bounded(15));
            h.humidity = 50 + (QRandomGenerator::global()->bounded(40));
            h.weatherCode = "100";
            h.weatherDesc = "晴";
            h.windSpeed = 5 + (QRandomGenerator::global()->bounded(20));
            h.windDirection = "东风";
            h.precipitationProb = QRandomGenerator::global()->bounded(30);
            forecast.append(h);
        }
        
        emit hourlyForecastReady(forecast);
        return;
    }
    
    QMap<QString, QString> params;
    QString endpoint = (hours <= 24) ? "/weather/24h" : "/weather/72h";
    QString url = buildUrl(endpoint, cityId, params);
    m_pendingRequests[url] = HourlyForecastRequest;
    NetworkManager::instance().get(url, true, 600);
}

void WeatherService::fetchDailyForecast(const QString &cityId, int days)
{
    if (m_apiKey.isEmpty()) {
        // 模拟数据
        QList<DailyForecast> forecast;
        QDate today = QDate::currentDate();
        
        for (int i = 0; i < days; ++i) {
            DailyForecast d;
            d.date = today.addDays(i);
            d.highTemp = 25 + (QRandomGenerator::global()->bounded(10));
            d.lowTemp = 15 + (QRandomGenerator::global()->bounded(8));
            d.humidity = 50 + (QRandomGenerator::global()->bounded(40));
            d.weatherCodeDay = "100";
            d.weatherDescDay = "晴";
            d.weatherCodeNight = "150";
            d.weatherDescNight = "晴";
            d.windSpeed = 10 + (QRandomGenerator::global()->bounded(15));
            d.windDirection = "东南风";
            d.precipitationProb = QRandomGenerator::global()->bounded(40);
            d.uvIndex = 3 + (QRandomGenerator::global()->bounded(5));
            d.sunriseTime = "06:30";
            d.sunsetTime = "18:45";
            forecast.append(d);
        }
        
        emit dailyForecastReady(forecast);
        return;
    }
    
    QString endpoint;
    if (days <= 3) endpoint = "/weather/3d";
    else if (days <= 7) endpoint = "/weather/7d";
    else endpoint = "/weather/10d";
    
    QString url = buildUrl(endpoint, cityId);
    m_pendingRequests[url] = DailyForecastRequest;
    NetworkManager::instance().get(url, true, 1800);
}

void WeatherService::fetchLifeIndex(const QString &cityId)
{
    if (m_apiKey.isEmpty()) {
        QList<LifeIndex> indices;
        
        LifeIndex idx1;
        idx1.type = "1"; idx1.name = "运动指数";
        idx1.level = "3"; idx1.category = "较适宜";
        idx1.description = "天气较好，适合户外运动";
        indices.append(idx1);
        
        LifeIndex idx2;
        idx2.type = "3"; idx2.name = "穿衣指数";
        idx2.level = "2"; idx2.category = "舒适";
        idx2.description = "建议穿薄外套或牛仔裤等服装";
        indices.append(idx2);
        
        LifeIndex idx3;
        idx3.type = "5"; idx3.name = "紫外线指数";
        idx3.level = "3"; idx3.category = "中等";
        idx3.description = "涂擦SPF大于15的防晒霜";
        indices.append(idx3);
        
        emit lifeIndexReady(indices);
        return;
    }
    
    QMap<QString, QString> params;
    params["type"] = "0";
    QString url = buildUrl("/indices/1d", cityId, params);
    m_pendingRequests[url] = LifeIndexRequest;
    NetworkManager::instance().get(url, true, 3600);
}

void WeatherService::fetchWeatherAlert(const QString &cityId)
{
    if (m_apiKey.isEmpty()) {
        emit weatherAlertReady(QList<WeatherAlert>());
        return;
    }
    
    QString url = buildUrl("/warning/now", cityId);
    m_pendingRequests[url] = AlertRequest;
    NetworkManager::instance().get(url, true, 300);
}

void WeatherService::fetchAirQuality(const QString &cityId)
{
    if (m_apiKey.isEmpty()) {
        return;
    }
    
    QString url = buildUrl("/air/now", cityId);
    m_pendingRequests[url] = AirQualityRequest;
    m_pendingCityId = cityId;
    NetworkManager::instance().get(url, true, 300);
}

void WeatherService::onRequestFinished(const QString &url, const NetworkResponse &response)
{
    if (!m_pendingRequests.contains(url)) {
        return;
    }
    
    RequestType type = m_pendingRequests.take(url);
    
    if (!response.success) {
        emit errorOccurred(response.errorString);
        return;
    }
    
    QJsonObject json = response.data;
    QString code = json["code"].toString();
    
    if (code != "200") {
        emit errorOccurred(tr("API错误: %1").arg(code));
        return;
    }
    
    switch (type) {
        case CurrentWeatherRequest: {
            CurrentWeather weather = parseCurrentWeather(json, m_pendingCityId);
            emit currentWeatherReady(weather);
            break;
        }
        case HourlyForecastRequest: {
            QList<HourlyForecast> forecast = parseHourlyForecast(json);
            emit hourlyForecastReady(forecast);
            break;
        }
        case DailyForecastRequest: {
            QList<DailyForecast> forecast = parseDailyForecast(json);
            emit dailyForecastReady(forecast);
            break;
        }
        case LifeIndexRequest: {
            QList<LifeIndex> indices = parseLifeIndex(json);
            emit lifeIndexReady(indices);
            break;
        }
        case AlertRequest: {
            QList<WeatherAlert> alerts = parseWeatherAlert(json);
            emit weatherAlertReady(alerts);
            break;
        }
        case AirQualityRequest: {
            // 空气质量数据会合并到当前天气中
            break;
        }
    }
}

CurrentWeather WeatherService::parseCurrentWeather(const QJsonObject &json, const QString &cityId)
{
    CurrentWeather weather;
    weather.cityId = cityId;
    
    QJsonObject now = json["now"].toObject();
    
    weather.temperature = now["temp"].toString().toDouble();
    weather.feelsLike = now["feelsLike"].toString().toDouble();
    weather.humidity = now["humidity"].toString().toInt();
    weather.pressure = now["pressure"].toString().toInt();
    weather.visibility = now["vis"].toString().toInt();
    weather.windSpeed = now["windSpeed"].toString().toDouble();
    weather.windDirection = now["windDir"].toString();
    weather.windDegree = now["wind360"].toString().toInt();
    weather.weatherCode = now["icon"].toString();
    weather.weatherDesc = now["text"].toString();
    weather.weatherIcon = now["icon"].toString();
    weather.cloudCover = now["cloud"].toString().toInt();
    
    weather.updateTime = QDateTime::fromString(json["updateTime"].toString(), Qt::ISODate);
    
    return weather;
}

QList<HourlyForecast> WeatherService::parseHourlyForecast(const QJsonObject &json)
{
    QList<HourlyForecast> forecast;
    
    QJsonArray hourly = json["hourly"].toArray();
    for (const QJsonValue &val : hourly) {
        QJsonObject obj = val.toObject();
        HourlyForecast h;
        
        h.time = QDateTime::fromString(obj["fxTime"].toString(), Qt::ISODate);
        h.temperature = obj["temp"].toString().toDouble();
        h.humidity = obj["humidity"].toString().toInt();
        h.weatherCode = obj["icon"].toString();
        h.weatherDesc = obj["text"].toString();
        h.weatherIcon = obj["icon"].toString();
        h.windSpeed = obj["windSpeed"].toString().toDouble();
        h.windDirection = obj["windDir"].toString();
        h.precipitationProb = obj["pop"].toString().toInt();
        h.precipitation = obj["precip"].toString().toDouble();
        
        forecast.append(h);
    }
    
    return forecast;
}

QList<DailyForecast> WeatherService::parseDailyForecast(const QJsonObject &json)
{
    QList<DailyForecast> forecast;
    
    QJsonArray daily = json["daily"].toArray();
    for (const QJsonValue &val : daily) {
        QJsonObject obj = val.toObject();
        DailyForecast d;
        
        d.date = QDate::fromString(obj["fxDate"].toString(), "yyyy-MM-dd");
        d.highTemp = obj["tempMax"].toString().toDouble();
        d.lowTemp = obj["tempMin"].toString().toDouble();
        d.humidity = obj["humidity"].toString().toInt();
        d.weatherCodeDay = obj["iconDay"].toString();
        d.weatherDescDay = obj["textDay"].toString();
        d.weatherIconDay = obj["iconDay"].toString();
        d.weatherCodeNight = obj["iconNight"].toString();
        d.weatherDescNight = obj["textNight"].toString();
        d.weatherIconNight = obj["iconNight"].toString();
        d.windSpeed = obj["windSpeedDay"].toString().toDouble();
        d.windDirection = obj["windDirDay"].toString();
        d.uvIndex = obj["uvIndex"].toString().toDouble();
        d.sunriseTime = obj["sunrise"].toString();
        d.sunsetTime = obj["sunset"].toString();
        
        forecast.append(d);
    }
    
    return forecast;
}

QList<LifeIndex> WeatherService::parseLifeIndex(const QJsonObject &json)
{
    QList<LifeIndex> indices;
    
    QJsonArray daily = json["daily"].toArray();
    for (const QJsonValue &val : daily) {
        QJsonObject obj = val.toObject();
        LifeIndex idx;
        
        idx.type = obj["type"].toString();
        idx.name = obj["name"].toString();
        idx.level = obj["level"].toString();
        idx.category = obj["category"].toString();
        idx.description = obj["text"].toString();
        
        indices.append(idx);
    }
    
    return indices;
}

QList<WeatherAlert> WeatherService::parseWeatherAlert(const QJsonObject &json)
{
    QList<WeatherAlert> alerts;
    
    QJsonArray warning = json["warning"].toArray();
    for (const QJsonValue &val : warning) {
        QJsonObject obj = val.toObject();
        WeatherAlert alert;
        
        alert.id = obj["id"].toString();
        alert.sender = obj["sender"].toString();
        alert.pubTime = obj["pubTime"].toString();
        alert.title = obj["title"].toString();
        alert.status = obj["status"].toString();
        alert.level = obj["level"].toString();
        alert.type = obj["type"].toString();
        alert.typeName = obj["typeName"].toString();
        alert.text = obj["text"].toString();
        
        alerts.append(alert);
    }
    
    return alerts;
}

void WeatherService::parseAirQuality(const QJsonObject &json, CurrentWeather &weather)
{
    QJsonObject now = json["now"].toObject();
    
    weather.aqi = now["aqi"].toString().toInt();
    weather.aqiLevel = now["category"].toString();
    weather.pm25 = now["pm2p5"].toString().toDouble();
    weather.pm10 = now["pm10"].toString().toDouble();
    weather.o3 = now["o3"].toString().toDouble();
}
