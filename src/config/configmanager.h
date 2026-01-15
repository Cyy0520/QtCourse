/**
 * @file configmanager.h
 * @brief 配置管理器类声明
 */

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>

/**
 * @enum TemperatureUnit
 * @brief 温度单位枚举
 */
enum class TemperatureUnit {
    Celsius,    // 摄氏度
    Fahrenheit  // 华氏度
};

/**
 * @enum WindSpeedUnit
 * @brief 风速单位枚举
 */
enum class WindSpeedUnit {
    KmPerHour,  // km/h
    MPerSecond, // m/s
    MilesPerHour // mph
};

/**
 * @enum PressureUnit
 * @brief 气压单位枚举
 */
enum class PressureUnit {
    HPa,    // 百帕
    MmHg    // 毫米汞柱
};

/**
 * @enum ThemeMode
 * @brief 主题模式枚举
 */
enum class ThemeMode {
    Light,
    Dark
};

/**
 * @class ConfigManager
 * @brief 配置管理单例类
 * 
 * 使用QSettings管理应用程序配置
 */
class ConfigManager : public QObject
{
    Q_OBJECT

public:
    static ConfigManager& instance();
    
    // 温度单位
    TemperatureUnit temperatureUnit() const;
    void setTemperatureUnit(TemperatureUnit unit);
    QString temperatureUnitString() const;
    
    // 风速单位
    WindSpeedUnit windSpeedUnit() const;
    void setWindSpeedUnit(WindSpeedUnit unit);
    QString windSpeedUnitString() const;
    
    // 气压单位
    PressureUnit pressureUnit() const;
    void setPressureUnit(PressureUnit unit);
    QString pressureUnitString() const;
    
    // 主题
    ThemeMode themeMode() const;
    void setThemeMode(ThemeMode mode);
    
    // 图标风格
    QString iconStyle() const;
    void setIconStyle(const QString &style);
    
    // 自动刷新间隔（分钟）
    int autoRefreshInterval() const;
    void setAutoRefreshInterval(int minutes);
    
    // 当前城市
    QString currentCityId() const;
    void setCurrentCityId(const QString &cityId);
    
    // 通用读写
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void setValue(const QString &key, const QVariant &value);
    
    // 单位转换工具
    static double celsiusToFahrenheit(double celsius);
    static double fahrenheitToCelsius(double fahrenheit);
    static double kmhToMs(double kmh);
    static double kmhToMph(double kmh);
    static double hpaToMmhg(double hpa);
    
    // 格式化显示
    QString formatTemperature(double celsius) const;
    QString formatWindSpeed(double kmh) const;
    QString formatPressure(double hpa) const;

signals:
    void temperatureUnitChanged(TemperatureUnit unit);
    void windSpeedUnitChanged(WindSpeedUnit unit);
    void pressureUnitChanged(PressureUnit unit);
    void themeModeChanged(ThemeMode mode);
    void configChanged(const QString &key);

private:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager() = default;
    
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

private:
    QSettings *m_settings;
    
    // 配置键常量
    static const QString KEY_TEMPERATURE_UNIT;
    static const QString KEY_WIND_SPEED_UNIT;
    static const QString KEY_PRESSURE_UNIT;
    static const QString KEY_THEME_MODE;
    static const QString KEY_ICON_STYLE;
    static const QString KEY_AUTO_REFRESH;
    static const QString KEY_CURRENT_CITY;
};

#endif // CONFIGMANAGER_H
