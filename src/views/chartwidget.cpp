/**
 * @file chartwidget.cpp
 * @brief 数据可视化图表组件实现
 */

#include "chartwidget.h"
#include "ui_chartwidget.h"
#include "../config/configmanager.h"
#include <QtCharts/QBarSet>
#include <QDateTime>

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChartWidget)
    , m_hourlyChartView(nullptr)
    , m_dailyChartView(nullptr)
    , m_hourlyChart(nullptr)
    , m_dailyChart(nullptr)
    , m_currentChartType(Temperature)
{
    ui->setupUi(this);
    setupCharts();
    setupConnections();
}

ChartWidget::~ChartWidget()
{
    delete ui;
}

void ChartWidget::setupConnections()
{
    connect(ui->refreshBtn, &QPushButton::clicked,
            this, &ChartWidget::onRefreshClicked);
    connect(ui->chartTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ChartWidget::onChartTypeChanged);
}

void ChartWidget::setupCharts()
{
    // 创建小时图表
    m_hourlyChart = new QChart();
    m_hourlyChart->setAnimationOptions(QChart::SeriesAnimations);
    m_hourlyChart->legend()->setVisible(true);
    m_hourlyChart->legend()->setAlignment(Qt::AlignBottom);
    
    m_hourlyChartView = new QChartView(m_hourlyChart);
    m_hourlyChartView->setRenderHint(QPainter::Antialiasing);
    ui->hourlyChartLayout->addWidget(m_hourlyChartView);
    
    // 创建日图表
    m_dailyChart = new QChart();
    m_dailyChart->setAnimationOptions(QChart::SeriesAnimations);
    m_dailyChart->legend()->setVisible(true);
    m_dailyChart->legend()->setAlignment(Qt::AlignBottom);
    
    m_dailyChartView = new QChartView(m_dailyChart);
    m_dailyChartView->setRenderHint(QPainter::Antialiasing);
    ui->dailyChartLayout->addWidget(m_dailyChartView);
}

void ChartWidget::setCity(const QString &cityId, const QString &cityName)
{
    m_currentCityId = cityId;
    m_currentCityName = cityName;
    ui->cityLabel->setText(cityName);
    clear();
}

void ChartWidget::updateHourlyData(const QList<HourlyForecast> &forecast)
{
    m_hourlyData = forecast;
    updateHourlyChart();
}

void ChartWidget::updateDailyData(const QList<DailyForecast> &forecast)
{
    m_dailyData = forecast;
    updateDailyChart();
}

void ChartWidget::clear()
{
    m_hourlyData.clear();
    m_dailyData.clear();
    m_hourlyChart->removeAllSeries();
    m_dailyChart->removeAllSeries();
    
    // 清除坐标轴
    for (QAbstractAxis *axis : m_hourlyChart->axes()) {
        m_hourlyChart->removeAxis(axis);
    }
    for (QAbstractAxis *axis : m_dailyChart->axes()) {
        m_dailyChart->removeAxis(axis);
    }
}

void ChartWidget::onRefreshClicked()
{
    if (!m_currentCityId.isEmpty()) {
        emit refreshRequested(m_currentCityId);
    }
}

void ChartWidget::onChartTypeChanged(int index)
{
    m_currentChartType = static_cast<ChartType>(index);
    updateHourlyChart();
    updateDailyChart();
}

void ChartWidget::updateHourlyChart()
{
    m_hourlyChart->removeAllSeries();
    for (QAbstractAxis *axis : m_hourlyChart->axes()) {
        m_hourlyChart->removeAxis(axis);
    }
    
    if (m_hourlyData.isEmpty()) {
        m_hourlyChart->setTitle(tr("暂无数据"));
        return;
    }
    
    switch (m_currentChartType) {
        case Temperature:
            createTemperatureChart(m_hourlyChart, true);
            break;
        case Humidity:
            createHumidityChart(m_hourlyChart, true);
            break;
        case WindSpeed:
            createWindSpeedChart(m_hourlyChart, true);
            break;
        case Pressure:
            createPressureChart(m_hourlyChart, true);
            break;
    }
}

void ChartWidget::updateDailyChart()
{
    m_dailyChart->removeAllSeries();
    for (QAbstractAxis *axis : m_dailyChart->axes()) {
        m_dailyChart->removeAxis(axis);
    }
    
    if (m_dailyData.isEmpty()) {
        m_dailyChart->setTitle(tr("暂无数据"));
        return;
    }
    
    switch (m_currentChartType) {
        case Temperature:
            createTemperatureChart(m_dailyChart, false);
            break;
        case Humidity:
            createHumidityChart(m_dailyChart, false);
            break;
        case WindSpeed:
            createWindSpeedChart(m_dailyChart, false);
            break;
        case Pressure:
            createPressureChart(m_dailyChart, false);
            break;
    }
}

void ChartWidget::createTemperatureChart(QChart *chart, bool isHourly)
{
    chart->setTitle(isHourly ? tr("24小时温度趋势") : tr("7日温度趋势"));
    
    if (isHourly) {
        QLineSeries *series = new QLineSeries();
        series->setName(tr("温度"));
        
        qreal minTemp = 100, maxTemp = -100;
        for (const HourlyForecast &h : m_hourlyData) {
            series->append(h.time.toMSecsSinceEpoch(), h.temperature);
            minTemp = qMin(minTemp, h.temperature);
            maxTemp = qMax(maxTemp, h.temperature);
        }
        
        chart->addSeries(series);
        
        // X轴 - 时间
        QDateTimeAxis *axisX = new QDateTimeAxis();
        axisX->setFormat("HH:mm");
        axisX->setTitleText(tr("时间"));
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        
        // Y轴 - 温度
        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(minTemp - 2, maxTemp + 2);
        axisY->setTitleText(tr("温度 (°C)"));
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
        
    } else {
        // 日数据 - 使用区域图显示高低温范围
        QLineSeries *highSeries = new QLineSeries();
        QLineSeries *lowSeries = new QLineSeries();
        highSeries->setName(tr("最高温"));
        lowSeries->setName(tr("最低温"));
        
        qreal minTemp = 100, maxTemp = -100;
        QStringList categories;
        
        for (int i = 0; i < m_dailyData.size(); ++i) {
            const DailyForecast &d = m_dailyData[i];
            highSeries->append(i, d.highTemp);
            lowSeries->append(i, d.lowTemp);
            minTemp = qMin(minTemp, d.lowTemp);
            maxTemp = qMax(maxTemp, d.highTemp);
            categories << d.date.toString("MM/dd");
        }
        
        // 创建区域图
        QAreaSeries *areaSeries = new QAreaSeries(highSeries, lowSeries);
        areaSeries->setName(tr("温度范围"));
        areaSeries->setOpacity(0.3);
        
        chart->addSeries(areaSeries);
        chart->addSeries(highSeries);
        chart->addSeries(lowSeries);
        
        // X轴
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        areaSeries->attachAxis(axisX);
        highSeries->attachAxis(axisX);
        lowSeries->attachAxis(axisX);
        
        // Y轴
        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(minTemp - 3, maxTemp + 3);
        axisY->setTitleText(tr("温度 (°C)"));
        chart->addAxis(axisY, Qt::AlignLeft);
        areaSeries->attachAxis(axisY);
        highSeries->attachAxis(axisY);
        lowSeries->attachAxis(axisY);
    }
}

void ChartWidget::createHumidityChart(QChart *chart, bool isHourly)
{
    chart->setTitle(isHourly ? tr("24小时湿度变化") : tr("7日湿度变化"));
    
    QLineSeries *series = new QLineSeries();
    series->setName(tr("湿度"));
    
    if (isHourly) {
        for (const HourlyForecast &h : m_hourlyData) {
            series->append(h.time.toMSecsSinceEpoch(), h.humidity);
        }
        
        chart->addSeries(series);
        
        QDateTimeAxis *axisX = new QDateTimeAxis();
        axisX->setFormat("HH:mm");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        
    } else {
        QStringList categories;
        for (int i = 0; i < m_dailyData.size(); ++i) {
            series->append(i, m_dailyData[i].humidity);
            categories << m_dailyData[i].date.toString("MM/dd");
        }
        
        chart->addSeries(series);
        
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
    }
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText(tr("湿度 (%)"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void ChartWidget::createWindSpeedChart(QChart *chart, bool isHourly)
{
    chart->setTitle(isHourly ? tr("24小时风速变化") : tr("7日风速变化"));
    
    QLineSeries *series = new QLineSeries();
    series->setName(tr("风速"));
    
    qreal maxWind = 0;
    
    if (isHourly) {
        for (const HourlyForecast &h : m_hourlyData) {
            series->append(h.time.toMSecsSinceEpoch(), h.windSpeed);
            maxWind = qMax(maxWind, h.windSpeed);
        }
        
        chart->addSeries(series);
        
        QDateTimeAxis *axisX = new QDateTimeAxis();
        axisX->setFormat("HH:mm");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        
    } else {
        QStringList categories;
        for (int i = 0; i < m_dailyData.size(); ++i) {
            series->append(i, m_dailyData[i].windSpeed);
            maxWind = qMax(maxWind, m_dailyData[i].windSpeed);
            categories << m_dailyData[i].date.toString("MM/dd");
        }
        
        chart->addSeries(series);
        
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
    }
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, maxWind + 5);
    axisY->setTitleText(tr("风速 (km/h)"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void ChartWidget::createPressureChart(QChart *chart, bool isHourly)
{
    chart->setTitle(isHourly ? tr("24小时气压变化") : tr("7日气压变化"));
    
    QLineSeries *series = new QLineSeries();
    series->setName(tr("气压"));
    
    // 气压数据只在小时预报中有
    if (isHourly) {
        for (const HourlyForecast &h : m_hourlyData) {
            // 假设有气压数据，这里用湿度代替演示
            series->append(h.time.toMSecsSinceEpoch(), 1013 + (h.humidity - 50) * 0.5);
        }
        
        chart->addSeries(series);
        
        QDateTimeAxis *axisX = new QDateTimeAxis();
        axisX->setFormat("HH:mm");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        
    } else {
        QStringList categories;
        for (int i = 0; i < m_dailyData.size(); ++i) {
            series->append(i, 1013 + (m_dailyData[i].humidity - 50) * 0.5);
            categories << m_dailyData[i].date.toString("MM/dd");
        }
        
        chart->addSeries(series);
        
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
    }
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(990, 1030);
    axisY->setTitleText(tr("气压 (hPa)"));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}
