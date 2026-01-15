/**
 * @file chartwidget.h
 * @brief 数据可视化图表组件
 */

#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QBarCategoryAxis>
#include "../models/weatherdata.h"

namespace Ui {
class ChartWidget;
}

/**
 * @class ChartWidget
 * @brief 天气数据可视化图表
 * 
 * 使用Qt Charts展示温度趋势、湿度变化等
 */
class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    enum ChartType {
        Temperature,
        Humidity,
        WindSpeed,
        Pressure
    };

    explicit ChartWidget(QWidget *parent = nullptr);
    ~ChartWidget();
    
    void setCity(const QString &cityId, const QString &cityName);
    void updateHourlyData(const QList<HourlyForecast> &forecast);
    void updateDailyData(const QList<DailyForecast> &forecast);
    void clear();

signals:
    void refreshRequested(const QString &cityId);

private slots:
    void onRefreshClicked();
    void onChartTypeChanged(int index);

private:
    void setupConnections();
    void setupCharts();
    void updateHourlyChart();
    void updateDailyChart();
    
    // 图表创建方法
    void createTemperatureChart(QChart *chart, bool isHourly);
    void createHumidityChart(QChart *chart, bool isHourly);
    void createWindSpeedChart(QChart *chart, bool isHourly);
    void createPressureChart(QChart *chart, bool isHourly);

private:
    Ui::ChartWidget *ui;
    QString m_currentCityId;
    QString m_currentCityName;
    
    QChartView *m_hourlyChartView;
    QChartView *m_dailyChartView;
    QChart *m_hourlyChart;
    QChart *m_dailyChart;
    
    QList<HourlyForecast> m_hourlyData;
    QList<DailyForecast> m_dailyData;
    
    ChartType m_currentChartType;
};

#endif // CHARTWIDGET_H
