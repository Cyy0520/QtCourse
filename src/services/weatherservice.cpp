/**
 * @file weatherservice.cpp
 * @brief 天气服务类实现
 */

#include "weatherservice.h"
#include "cityservice.h"
#include <QJsonArray>
#include <QUrlQuery>
#include <QDebug>
#include <QRandomGenerator>

WeatherService::WeatherService(QObject *parent)
    : QObject(parent)
    , m_baseUrl("https://api.open-meteo.com/v1")  // Open-Meteo 免费 API，无需 Key
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
    Q_UNUSED(cityId)
    Q_UNUSED(params)
    // Open-Meteo 使用经纬度，这里先返回基础 URL
    return m_baseUrl + endpoint;
}

void WeatherService::fetchCurrentWeather(const QString &cityId)
{
    // 获取城市经纬度
    double lat = 39.9042, lon = 116.4074;  // 默认北京
    getCityCoordinates(cityId, lat, lon);
    
    QString url = QString("%1/forecast?latitude=%2&longitude=%3"
                         "&current=temperature_2m,relative_humidity_2m,apparent_temperature,"
                         "weather_code,surface_pressure,wind_speed_10m,wind_direction_10m"
                         "&timezone=auto")
                  .arg(m_baseUrl).arg(lat).arg(lon);
    
    qDebug() << "Fetching current weather:" << url;
    m_pendingRequests[url] = CurrentWeatherRequest;
    m_pendingCityId = cityId;
    NetworkManager::instance().get(url, true, 300);
}

void WeatherService::fetchHourlyForecast(const QString &cityId, int hours)
{
    double lat = 39.9042, lon = 116.4074;
    getCityCoordinates(cityId, lat, lon);
    
    QString url = QString("%1/forecast?latitude=%2&longitude=%3"
                         "&hourly=temperature_2m,relative_humidity_2m,weather_code,"
                         "wind_speed_10m,wind_direction_10m,precipitation_probability"
                         "&forecast_hours=%4&timezone=auto")
                  .arg(m_baseUrl).arg(lat).arg(lon).arg(hours);
    
    m_pendingRequests[url] = HourlyForecastRequest;
    m_pendingCityId = cityId;
    NetworkManager::instance().get(url, true, 600);
}

void WeatherService::fetchDailyForecast(const QString &cityId, int days)
{
    double lat = 39.9042, lon = 116.4074;
    getCityCoordinates(cityId, lat, lon);
    
    QString url = QString("%1/forecast?latitude=%2&longitude=%3"
                         "&daily=temperature_2m_max,temperature_2m_min,weather_code,"
                         "wind_speed_10m_max,wind_direction_10m_dominant,"
                         "precipitation_probability_max,uv_index_max,sunrise,sunset"
                         "&forecast_days=%4&timezone=auto")
                  .arg(m_baseUrl).arg(lat).arg(lon).arg(days);
    
    m_pendingRequests[url] = DailyForecastRequest;
    m_pendingCityId = cityId;
    NetworkManager::instance().get(url, true, 1800);
}

void WeatherService::fetchLifeIndex(const QString &cityId)
{
    // Open-Meteo 没有生活指数，使用模拟数据
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
    
    LifeIndex idx4;
    idx4.type = "6"; idx4.name = "洗车指数";
    idx4.level = "1"; idx4.category = "适宜";
    idx4.description = "天气较好，适合洗车";
    indices.append(idx4);
    
    LifeIndex idx5;
    idx5.type = "8"; idx5.name = "舒适度指数";
    idx5.level = "2"; idx5.category = "舒适";
    idx5.description = "白天温度适宜，风力不大";
    indices.append(idx5);
    
    LifeIndex idx6;
    idx6.type = "9"; idx6.name = "感冒指数";
    idx6.level = "2"; idx6.category = "较易发";
    idx6.description = "注意添加衣物，预防感冒";
    indices.append(idx6);
    
    emit lifeIndexReady(indices);
}

void WeatherService::fetchWeatherAlert(const QString &cityId)
{
    Q_UNUSED(cityId)
    // Open-Meteo 免费版没有预警，返回空
    emit weatherAlertReady(QList<WeatherAlert>());
}

void WeatherService::fetchAirQuality(const QString &cityId)
{
    Q_UNUSED(cityId)
    // 暂不实现
}

void WeatherService::onRequestFinished(const QString &url, const NetworkResponse &response)
{
    if (!m_pendingRequests.contains(url)) {
        return;
    }
    
    RequestType type = m_pendingRequests.take(url);
    
    if (!response.success) {
        qWarning() << "API request failed:" << response.errorString;
        emit errorOccurred(tr("网络请求失败: %1").arg(response.errorString));
        return;
    }
    
    QJsonObject json = response.data;
    
    // Open-Meteo 返回格式检查
    if (json.contains("error") && json["error"].toBool()) {
        QString reason = json["reason"].toString();
        qWarning() << "API error:" << reason;
        emit errorOccurred(tr("API错误: %1").arg(reason));
        return;
    }
    
    switch (type) {
        case CurrentWeatherRequest: {
            CurrentWeather weather = parseOpenMeteoCurrentWeather(json, m_pendingCityId);
            emit currentWeatherReady(weather);
            break;
        }
        case HourlyForecastRequest: {
            QList<HourlyForecast> forecast = parseOpenMeteoHourlyForecast(json);
            emit hourlyForecastReady(forecast);
            break;
        }
        case DailyForecastRequest: {
            QList<DailyForecast> forecast = parseOpenMeteoDailyForecast(json);
            emit dailyForecastReady(forecast);
            break;
        }
        default:
            break;
    }
}

void WeatherService::getCityCoordinates(const QString &cityId, double &lat, double &lon)
{
    // 优先从数据库获取城市经纬度
    CityInfo city = CityService::instance().getCity(cityId);
    if (!city.cityId.isEmpty() && city.latitude != 0 && city.longitude != 0) {
        lat = city.latitude;
        lon = city.longitude;
        qDebug() << "Using DB coordinates for" << city.name << ":" << lat << lon;
        return;
    }
    
    // 备用：城市ID到经纬度的映射
    static QMap<QString, QPair<double, double>> cityCoords = {
        // 直辖市
        {"101010100", {39.9042, 116.4074}},   // 北京
        {"101020100", {31.2304, 121.4737}},   // 上海
        {"101030100", {39.0842, 117.2009}},   // 天津
        {"101040100", {29.4316, 106.9123}},   // 重庆
        // 广东省
        {"101280101", {23.1291, 113.2644}},   // 广州
        {"101280601", {22.5431, 114.0579}},   // 深圳
        {"101281601", {23.0489, 113.7447}},   // 东莞
        {"101280301", {22.2006, 113.5461}},   // 珠海
        {"101280501", {23.0292, 113.1056}},   // 佛山
        {"101280701", {22.7789, 113.9213}},   // 惠州
        {"101280201", {24.2998, 116.6822}},   // 梅州
        {"101280401", {23.3535, 116.6819}},   // 汕头
        {"101281501", {22.5177, 113.3926}},   // 中山
        {"101281701", {21.8577, 111.9822}},   // 江门
        {"101280801", {23.6618, 116.6229}},   // 揭阳
        // 浙江省
        {"101210101", {30.2741, 120.1551}},   // 杭州
        {"101210401", {29.8683, 121.5440}},   // 宁波
        {"101210301", {28.0001, 120.6722}},   // 温州
        // 江苏省
        {"101190101", {32.0603, 118.7969}},   // 南京
        {"101190401", {31.2989, 120.5853}},   // 苏州
        {"101190201", {31.4906, 120.3119}},   // 无锡
        // 湖北省
        {"101200101", {30.5928, 114.3055}},   // 武汉
        // 四川省
        {"101270101", {30.5728, 104.0668}},   // 成都
        // 湖南省
        {"101250101", {28.2282, 112.9388}},   // 长沙
        // 陕西省
        {"101110101", {34.3416, 108.9398}},   // 西安
        // 福建省
        {"101230101", {26.0745, 119.2965}},   // 福州
        {"101230201", {24.4798, 118.0894}},   // 厦门
        // 山东省
        {"101120101", {36.6512, 117.1201}},   // 济南
        {"101120201", {36.0671, 120.3826}},   // 青岛
        // 辽宁省
        {"101070101", {41.8057, 123.4315}},   // 沈阳
        {"101070201", {38.9140, 121.6147}},   // 大连
        // 河南省
        {"101180101", {34.7466, 113.6253}},   // 郑州
        // 河北省
        {"101090101", {38.0428, 114.5149}},   // 石家庄
        // 安徽省
        {"101220101", {31.8206, 117.2272}},   // 合肥
        // 江西省
        {"101240101", {28.6820, 115.8579}},   // 南昌
        // 云南省
        {"101290101", {25.0389, 102.7183}},   // 昆明
        // 贵州省
        {"101260101", {26.6470, 106.6302}},   // 贵阳
        // 广西
        {"101300101", {22.8150, 108.3275}},   // 南宁
        // 海南省
        {"101310101", {20.0200, 110.3486}},   // 海口
        {"101310201", {18.2533, 109.5117}},   // 三亚
        // 黑龙江省
        {"101050101", {45.8038, 126.5340}},   // 哈尔滨
        // 吉林省
        {"101060101", {43.8171, 125.3235}},   // 长春
        // 内蒙古
        {"101080101", {40.8424, 111.7490}},   // 呼和浩特
        // 新疆
        {"101130101", {43.7930, 87.6271}},    // 乌鲁木齐
        // 甘肃省
        {"101160101", {36.0611, 103.8343}},   // 兰州
        // 宁夏
        {"101170101", {38.4872, 106.2309}},   // 银川
        // 青海省
        {"101150101", {36.6171, 101.7782}},   // 西宁
        // 西藏
        {"101140101", {29.6500, 91.1000}},    // 拉萨
        // 山西省
        {"101100101", {37.8706, 112.5489}},   // 太原
    };
    
    if (cityCoords.contains(cityId)) {
        lat = cityCoords[cityId].first;
        lon = cityCoords[cityId].second;
        qDebug() << "Using hardcoded coordinates for" << cityId << ":" << lat << lon;
    } else {
        qDebug() << "No coordinates found for" << cityId << ", using default Beijing";
    }
}

CurrentWeather WeatherService::parseOpenMeteoCurrentWeather(const QJsonObject &json, const QString &cityId)
{
    CurrentWeather weather;
    weather.cityId = cityId;
    
    QJsonObject current = json["current"].toObject();
    
    weather.temperature = current["temperature_2m"].toDouble();
    weather.feelsLike = current["apparent_temperature"].toDouble();
    weather.humidity = current["relative_humidity_2m"].toInt();
    weather.pressure = qRound(current["surface_pressure"].toDouble());
    weather.windSpeed = current["wind_speed_10m"].toDouble();
    weather.windDegree = current["wind_direction_10m"].toInt();
    weather.windDirection = getWindDirectionName(weather.windDegree);
    
    int wmoCode = current["weather_code"].toInt();
    weather.weatherCode = QString::number(wmoCode);
    weather.weatherDesc = getWmoWeatherDesc(wmoCode);
    weather.weatherIcon = weather.weatherCode;
    
    weather.visibility = 10;  // Open-Meteo 免费版没有能见度
    weather.aqi = 50;         // 默认值
    weather.aqiLevel = "良";
    weather.sunriseTime = "06:30";
    weather.sunsetTime = "18:30";
    weather.updateTime = QDateTime::currentDateTime();
    
    return weather;
}

QList<HourlyForecast> WeatherService::parseOpenMeteoHourlyForecast(const QJsonObject &json)
{
    QList<HourlyForecast> forecast;
    
    QJsonObject hourly = json["hourly"].toObject();
    QJsonArray times = hourly["time"].toArray();
    QJsonArray temps = hourly["temperature_2m"].toArray();
    QJsonArray humidity = hourly["relative_humidity_2m"].toArray();
    QJsonArray weatherCodes = hourly["weather_code"].toArray();
    QJsonArray windSpeed = hourly["wind_speed_10m"].toArray();
    QJsonArray windDir = hourly["wind_direction_10m"].toArray();
    QJsonArray precip = hourly["precipitation_probability"].toArray();
    
    int count = qMin(24, times.size());
    for (int i = 0; i < count; ++i) {
        HourlyForecast h;
        h.time = QDateTime::fromString(times[i].toString(), Qt::ISODate);
        h.temperature = temps[i].toDouble();
        h.humidity = humidity[i].toInt();
        
        int wmoCode = weatherCodes[i].toInt();
        h.weatherCode = QString::number(wmoCode);
        h.weatherDesc = getWmoWeatherDesc(wmoCode);
        h.weatherIcon = h.weatherCode;
        
        h.windSpeed = windSpeed[i].toDouble();
        h.windDirection = getWindDirectionName(windDir[i].toInt());
        h.precipitationProb = precip.size() > i ? precip[i].toInt() : 0;
        
        forecast.append(h);
    }
    
    return forecast;
}

QList<DailyForecast> WeatherService::parseOpenMeteoDailyForecast(const QJsonObject &json)
{
    QList<DailyForecast> forecast;
    
    QJsonObject daily = json["daily"].toObject();
    QJsonArray dates = daily["time"].toArray();
    QJsonArray maxTemps = daily["temperature_2m_max"].toArray();
    QJsonArray minTemps = daily["temperature_2m_min"].toArray();
    QJsonArray weatherCodes = daily["weather_code"].toArray();
    QJsonArray windSpeed = daily["wind_speed_10m_max"].toArray();
    QJsonArray windDir = daily["wind_direction_10m_dominant"].toArray();
    QJsonArray precip = daily["precipitation_probability_max"].toArray();
    QJsonArray uvIndex = daily["uv_index_max"].toArray();
    QJsonArray sunrise = daily["sunrise"].toArray();
    QJsonArray sunset = daily["sunset"].toArray();
    
    for (int i = 0; i < dates.size() && i < 7; ++i) {
        DailyForecast d;
        d.date = QDate::fromString(dates[i].toString(), "yyyy-MM-dd");
        d.highTemp = maxTemps[i].toDouble();
        d.lowTemp = minTemps[i].toDouble();
        d.humidity = 60;  // Open-Meteo daily 没有湿度
        
        int wmoCode = weatherCodes[i].toInt();
        d.weatherCodeDay = QString::number(wmoCode);
        d.weatherDescDay = getWmoWeatherDesc(wmoCode);
        d.weatherCodeNight = d.weatherCodeDay;
        d.weatherDescNight = d.weatherDescDay;
        
        d.windSpeed = windSpeed.size() > i ? windSpeed[i].toDouble() : 10;
        d.windDirection = windDir.size() > i ? getWindDirectionName(windDir[i].toInt()) : "东风";
        d.precipitationProb = precip.size() > i ? precip[i].toInt() : 0;
        d.uvIndex = uvIndex.size() > i ? uvIndex[i].toDouble() : 5;
        
        if (sunrise.size() > i) {
            QDateTime sr = QDateTime::fromString(sunrise[i].toString(), Qt::ISODate);
            d.sunriseTime = sr.time().toString("HH:mm");
        }
        if (sunset.size() > i) {
            QDateTime ss = QDateTime::fromString(sunset[i].toString(), Qt::ISODate);
            d.sunsetTime = ss.time().toString("HH:mm");
        }
        
        forecast.append(d);
    }
    
    return forecast;
}

QString WeatherService::getWindDirectionName(int degree)
{
    if (degree >= 337.5 || degree < 22.5) return "北风";
    if (degree < 67.5) return "东北风";
    if (degree < 112.5) return "东风";
    if (degree < 157.5) return "东南风";
    if (degree < 202.5) return "南风";
    if (degree < 247.5) return "西南风";
    if (degree < 292.5) return "西风";
    return "西北风";
}

QString WeatherService::getWmoWeatherDesc(int code)
{
    // WMO 天气代码映射
    static QMap<int, QString> wmoDesc = {
        {0, "晴"},
        {1, "晴"}, {2, "多云"}, {3, "阴"},
        {45, "雾"}, {48, "雾凇"},
        {51, "小雨"}, {53, "中雨"}, {55, "大雨"},
        {61, "小雨"}, {63, "中雨"}, {65, "大雨"},
        {71, "小雪"}, {73, "中雪"}, {75, "大雪"},
        {80, "阵雨"}, {81, "阵雨"}, {82, "暴雨"},
        {85, "小雪"}, {86, "大雪"},
        {95, "雷阵雨"}, {96, "雷阵雨"}, {99, "雷阵雨"},
    };
    return wmoDesc.value(code, "未知");
}
