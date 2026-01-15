/**
 * @file configmanager.cpp
 * @brief 配置管理器类实现
 */

#include "configmanager.h"
#include <QCoreApplication>

// 配置键定义
const QString ConfigManager::KEY_TEMPERATURE_UNIT = "units/temperature";
const QString ConfigManager::KEY_WIND_SPEED_UNIT = "units/windSpeed";
const QString ConfigManager::KEY_PRESSURE_UNIT = "units/pressure";
const QString ConfigManager::KEY_THEME_MODE = "appearance/theme";
const QString ConfigManager::KEY_ICON_STYLE = "appearance/iconStyle";
const QString ConfigManager::KEY_AUTO_REFRESH = "general/autoRefreshInterval";
const QString ConfigManager::KEY_CURRENT_CITY = "general/currentCityId";

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
{
    m_settings = new QSettings(
        QSettings::IniFormat,
        QSettings::UserScope,
        QCoreApplication::organizationName(),
        QCoreApplication::applicationName(),
        this
    );
}

ConfigManager& ConfigManager::instance()
{
    static ConfigManager instance;
    return instance;
}

// 温度单位
TemperatureUnit ConfigManager::temperatureUnit() const
{
    QString unit = m_settings->value(KEY_TEMPERATURE_UNIT, "celsius").toString();
    return (unit == "fahrenheit") ? TemperatureUnit::Fahrenheit : TemperatureUnit::Celsius;
}

void ConfigManager::setTemperatureUnit(TemperatureUnit unit)
{
    QString value = (unit == TemperatureUnit::Fahrenheit) ? "fahrenheit" : "celsius";
    m_settings->setValue(KEY_TEMPERATURE_UNIT, value);
    emit temperatureUnitChanged(unit);
    emit configChanged(KEY_TEMPERATURE_UNIT);
}

QString ConfigManager::temperatureUnitString() const
{
    return (temperatureUnit() == TemperatureUnit::Celsius) ? "°C" : "°F";
}

// 风速单位
WindSpeedUnit ConfigManager::windSpeedUnit() const
{
    QString unit = m_settings->value(KEY_WIND_SPEED_UNIT, "km/h").toString();
    if (unit == "m/s") return WindSpeedUnit::MPerSecond;
    if (unit == "mph") return WindSpeedUnit::MilesPerHour;
    return WindSpeedUnit::KmPerHour;
}

void ConfigManager::setWindSpeedUnit(WindSpeedUnit unit)
{
    QString value;
    switch (unit) {
        case WindSpeedUnit::MPerSecond: value = "m/s"; break;
        case WindSpeedUnit::MilesPerHour: value = "mph"; break;
        default: value = "km/h"; break;
    }
    m_settings->setValue(KEY_WIND_SPEED_UNIT, value);
    emit windSpeedUnitChanged(unit);
    emit configChanged(KEY_WIND_SPEED_UNIT);
}

QString ConfigManager::windSpeedUnitString() const
{
    switch (windSpeedUnit()) {
        case WindSpeedUnit::MPerSecond: return "m/s";
        case WindSpeedUnit::MilesPerHour: return "mph";
        default: return "km/h";
    }
}

// 气压单位
PressureUnit ConfigManager::pressureUnit() const
{
    QString unit = m_settings->value(KEY_PRESSURE_UNIT, "hPa").toString();
    return (unit == "mmHg") ? PressureUnit::MmHg : PressureUnit::HPa;
}

void ConfigManager::setPressureUnit(PressureUnit unit)
{
    QString value = (unit == PressureUnit::MmHg) ? "mmHg" : "hPa";
    m_settings->setValue(KEY_PRESSURE_UNIT, value);
    emit pressureUnitChanged(unit);
    emit configChanged(KEY_PRESSURE_UNIT);
}

QString ConfigManager::pressureUnitString() const
{
    return (pressureUnit() == PressureUnit::HPa) ? "hPa" : "mmHg";
}

// 主题
ThemeMode ConfigManager::themeMode() const
{
    QString mode = m_settings->value(KEY_THEME_MODE, "light").toString();
    return (mode == "dark") ? ThemeMode::Dark : ThemeMode::Light;
}

void ConfigManager::setThemeMode(ThemeMode mode)
{
    QString value = (mode == ThemeMode::Dark) ? "dark" : "light";
    m_settings->setValue(KEY_THEME_MODE, value);
    emit themeModeChanged(mode);
    emit configChanged(KEY_THEME_MODE);
}

// 图标风格
QString ConfigManager::iconStyle() const
{
    return m_settings->value(KEY_ICON_STYLE, "simple").toString();
}

void ConfigManager::setIconStyle(const QString &style)
{
    m_settings->setValue(KEY_ICON_STYLE, style);
    emit configChanged(KEY_ICON_STYLE);
}

// 自动刷新间隔
int ConfigManager::autoRefreshInterval() const
{
    return m_settings->value(KEY_AUTO_REFRESH, 30).toInt();
}

void ConfigManager::setAutoRefreshInterval(int minutes)
{
    m_settings->setValue(KEY_AUTO_REFRESH, minutes);
    emit configChanged(KEY_AUTO_REFRESH);
}

// 当前城市
QString ConfigManager::currentCityId() const
{
    return m_settings->value(KEY_CURRENT_CITY, "").toString();
}

void ConfigManager::setCurrentCityId(const QString &cityId)
{
    m_settings->setValue(KEY_CURRENT_CITY, cityId);
    emit configChanged(KEY_CURRENT_CITY);
}

// 通用读写
QVariant ConfigManager::value(const QString &key, const QVariant &defaultValue) const
{
    return m_settings->value(key, defaultValue);
}

void ConfigManager::setValue(const QString &key, const QVariant &value)
{
    m_settings->setValue(key, value);
    emit configChanged(key);
}

// 单位转换
double ConfigManager::celsiusToFahrenheit(double celsius)
{
    return celsius * 9.0 / 5.0 + 32.0;
}

double ConfigManager::fahrenheitToCelsius(double fahrenheit)
{
    return (fahrenheit - 32.0) * 5.0 / 9.0;
}

double ConfigManager::kmhToMs(double kmh)
{
    return kmh / 3.6;
}

double ConfigManager::kmhToMph(double kmh)
{
    return kmh * 0.621371;
}

double ConfigManager::hpaToMmhg(double hpa)
{
    return hpa * 0.750062;
}

// 格式化显示
QString ConfigManager::formatTemperature(double celsius) const
{
    double value = celsius;
    if (temperatureUnit() == TemperatureUnit::Fahrenheit) {
        value = celsiusToFahrenheit(celsius);
    }
    return QString::number(qRound(value)) + temperatureUnitString();
}

QString ConfigManager::formatWindSpeed(double kmh) const
{
    double value = kmh;
    switch (windSpeedUnit()) {
        case WindSpeedUnit::MPerSecond:
            value = kmhToMs(kmh);
            break;
        case WindSpeedUnit::MilesPerHour:
            value = kmhToMph(kmh);
            break;
        default:
            break;
    }
    return QString::number(value, 'f', 1) + " " + windSpeedUnitString();
}

QString ConfigManager::formatPressure(double hpa) const
{
    double value = hpa;
    if (pressureUnit() == PressureUnit::MmHg) {
        value = hpaToMmhg(hpa);
    }
    return QString::number(qRound(value)) + " " + pressureUnitString();
}
