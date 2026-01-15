/**
 * @file dataexporter.h
 * @brief 数据导出工具类
 */

#ifndef DATAEXPORTER_H
#define DATAEXPORTER_H

#include <QString>
#include <QList>
#include "../models/weatherdata.h"

/**
 * @class DataExporter
 * @brief 天气数据导出工具
 * 
 * 支持导出为 JSON 和 CSV 格式
 */
class DataExporter
{
public:
    /**
     * @brief 导出当前天气为 JSON
     */
    static bool exportCurrentWeatherToJson(const CurrentWeather &weather, const QString &filePath);
    
    /**
     * @brief 导出预报数据为 JSON
     */
    static bool exportForecastToJson(const QList<DailyForecast> &forecast, const QString &filePath);
    
    /**
     * @brief 导出预报数据为 CSV
     */
    static bool exportForecastToCsv(const QList<DailyForecast> &forecast, const QString &filePath);
    
    /**
     * @brief 导出逐小时预报为 CSV
     */
    static bool exportHourlyToCsv(const QList<HourlyForecast> &forecast, const QString &filePath);

private:
    DataExporter() = default;
};

#endif // DATAEXPORTER_H
