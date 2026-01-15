/**
 * @file currentweatherwidget.cpp
 * @brief 实时天气展示组件实现
 */

#include "currentweatherwidget.h"
#include "ui_currentweatherwidget.h"
#include "../config/configmanager.h"

CurrentWeatherWidget::CurrentWeatherWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CurrentWeatherWidget)
{
    ui->setupUi(this);
    setupConnections();
}

CurrentWeatherWidget::~CurrentWeatherWidget()
{
    delete ui;
}

void CurrentWeatherWidget::setupConnections()
{
    connect(ui->refreshBtn, &QPushButton::clicked,
            this, &CurrentWeatherWidget::onRefreshClicked);
}

void CurrentWeatherWidget::setCity(const QString &cityId, const QString &cityName)
{
    m_currentCityId = cityId;
    m_currentCityName = cityName;
    ui->cityNameLabel->setText(cityName);
    clear();
}

void CurrentWeatherWidget::updateWeather(const CurrentWeather &weather)
{
    if (!weather.isValid()) {
        return;
    }
    
    ConfigManager &config = ConfigManager::instance();
    
    // 城市名称
    if (!weather.cityName.isEmpty()) {
        ui->cityNameLabel->setText(weather.cityName);
    }
    
    // 温度
    QString tempStr = config.formatTemperature(weather.temperature);
    ui->temperatureLabel->setText(tempStr);
    
    // 体感温度
    QString feelsLikeStr = config.formatTemperature(weather.feelsLike);
    ui->feelsLikeLabel->setText(tr("体感温度 %1").arg(feelsLikeStr));
    
    // 天气图标和描述
    ui->weatherIconLabel->setText(getWeatherEmoji(weather.weatherCode));
    ui->weatherDescLabel->setText(weather.weatherDesc);
    
    // 空气质量
    ui->aqiValueLabel->setText(tr("AQI %1").arg(weather.aqi));
    ui->aqiLabel->setText(getAqiLevel(weather.aqi));
    ui->aqiLabel->setStyleSheet(QString(
        "font-size: 14px; font-weight: bold; padding: 4px 12px; "
        "border-radius: 4px; background-color: %1; color: white;"
    ).arg(getAqiColor(weather.aqi)));
    
    // 湿度
    ui->humidityLabel->setText(QString("%1%").arg(weather.humidity));
    
    // 风速和风向
    QString windStr = config.formatWindSpeed(weather.windSpeed);
    ui->windLabel->setText(windStr);
    ui->windDirLabel->setText(weather.windDirection);
    
    // 气压
    QString pressureStr = config.formatPressure(weather.pressure);
    ui->pressureLabel->setText(pressureStr);
    
    // 能见度
    ui->visibilityLabel->setText(QString("%1 km").arg(weather.visibility));
    
    // 日出日落
    ui->sunriseLabel->setText(weather.sunriseTime.isEmpty() ? "--:--" : weather.sunriseTime);
    ui->sunsetLabel->setText(weather.sunsetTime.isEmpty() ? "--:--" : weather.sunsetTime);
    
    // 更新时间
    QString updateStr = weather.updateTime.isValid() 
        ? weather.updateTime.toString("yyyy-MM-dd HH:mm")
        : "--";
    ui->updateTimeLabel->setText(tr("更新时间: %1").arg(updateStr));
}

void CurrentWeatherWidget::clear()
{
    ui->temperatureLabel->setText("--°");
    ui->feelsLikeLabel->setText(tr("体感温度 --°"));
    ui->weatherIconLabel->setText("☀");
    ui->weatherDescLabel->setText("--");
    ui->aqiLabel->setText("--");
    ui->aqiValueLabel->setText("AQI --");
    ui->humidityLabel->setText("--%");
    ui->windLabel->setText("-- km/h");
    ui->windDirLabel->setText("--");
    ui->pressureLabel->setText("-- hPa");
    ui->visibilityLabel->setText("-- km");
    ui->sunriseLabel->setText("--:--");
    ui->sunsetLabel->setText("--:--");
    ui->updateTimeLabel->setText(tr("更新时间: --"));
}

void CurrentWeatherWidget::onRefreshClicked()
{
    if (!m_currentCityId.isEmpty()) {
        emit refreshRequested(m_currentCityId);
    }
}

QString CurrentWeatherWidget::getWeatherEmoji(const QString &code)
{
    // 和风天气图标代码映射到 emoji
    static QMap<QString, QString> emojiMap = {
        {"100", "☀️"},   // 晴
        {"101", "⛅"},   // 多云
        {"102", "🌤️"},  // 少云
        {"103", "⛅"},   // 晴间多云
        {"104", "☁️"},   // 阴
        {"150", "🌙"},   // 晴(夜)
        {"151", "🌙"},   // 多云(夜)
        {"300", "🌧️"},  // 阵雨
        {"301", "🌧️"},  // 强阵雨
        {"302", "⛈️"},  // 雷阵雨
        {"303", "⛈️"},  // 强雷阵雨
        {"304", "⛈️"},  // 雷阵雨伴有冰雹
        {"305", "🌧️"},  // 小雨
        {"306", "🌧️"},  // 中雨
        {"307", "🌧️"},  // 大雨
        {"308", "🌧️"},  // 极端降雨
        {"309", "🌧️"},  // 毛毛雨
        {"310", "🌧️"},  // 暴雨
        {"311", "🌧️"},  // 大暴雨
        {"312", "🌧️"},  // 特大暴雨
        {"313", "🌧️"},  // 冻雨
        {"314", "🌧️"},  // 小到中雨
        {"315", "🌧️"},  // 中到大雨
        {"316", "🌧️"},  // 大到暴雨
        {"400", "🌨️"},  // 小雪
        {"401", "🌨️"},  // 中雪
        {"402", "❄️"},   // 大雪
        {"403", "❄️"},   // 暴雪
        {"404", "🌨️"},  // 雨夹雪
        {"405", "🌨️"},  // 雨雪天气
        {"406", "🌨️"},  // 阵雨夹雪
        {"407", "🌨️"},  // 阵雪
        {"500", "🌫️"},  // 薄雾
        {"501", "🌫️"},  // 雾
        {"502", "🌫️"},  // 霾
        {"503", "🌫️"},  // 扬沙
        {"504", "🌫️"},  // 浮尘
        {"507", "🌫️"},  // 沙尘暴
        {"508", "🌫️"},  // 强沙尘暴
        {"900", "🔥"},   // 热
        {"901", "❄️"},   // 冷
        {"999", "❓"}    // 未知
    };
    
    return emojiMap.value(code, "🌡️");
}

QString CurrentWeatherWidget::getAqiColor(int aqi)
{
    if (aqi <= 50) return "#67C23A";       // 优 - 绿色
    if (aqi <= 100) return "#E6A23C";      // 良 - 黄色
    if (aqi <= 150) return "#F56C6C";      // 轻度污染 - 橙色
    if (aqi <= 200) return "#E6A23C";      // 中度污染 - 红色
    if (aqi <= 300) return "#909399";      // 重度污染 - 紫色
    return "#303133";                       // 严重污染 - 褐红色
}

QString CurrentWeatherWidget::getAqiLevel(int aqi)
{
    if (aqi <= 50) return tr("优");
    if (aqi <= 100) return tr("良");
    if (aqi <= 150) return tr("轻度污染");
    if (aqi <= 200) return tr("中度污染");
    if (aqi <= 300) return tr("重度污染");
    return tr("严重污染");
}
