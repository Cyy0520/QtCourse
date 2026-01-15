/**
 * @file mainwindow.cpp
 * @brief 主窗口类实现
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "views/citywidget.h"
#include "views/currentweatherwidget.h"
#include "views/forecastwidget.h"
#include "views/chartwidget.h"
#include "views/lifeindexwidget.h"
#include "views/settingswidget.h"
#include "views/aboutwidget.h"
#include "views/historywidget.h"
#include "views/alertwidget.h"
#include "workers/weatherworker.h"
#include "models/citymodel.h"
#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_cityWidget(nullptr)
    , m_currentWeatherWidget(nullptr)
    , m_forecastWidget(nullptr)
    , m_chartWidget(nullptr)
    , m_lifeIndexWidget(nullptr)
    , m_settingsWidget(nullptr)
    , m_aboutWidget(nullptr)
    , m_historyWidget(nullptr)
    , m_alertWidget(nullptr)
{
    ui->setupUi(this);
    
    // 初始化数据库
    if (!initDatabase()) {
        QMessageBox::critical(this, tr("错误"), tr("数据库初始化失败，程序可能无法正常工作。"));
    }
    
    // 初始化页面
    setupPages();
    
    // 连接天气数据信号
    WeatherThreadController &controller = WeatherThreadController::instance();
    
    connect(&controller, &WeatherThreadController::currentWeatherReady,
            this, [this](const CurrentWeather &weather) {
        if (m_currentWeatherWidget) {
            m_currentWeatherWidget->updateWeather(weather);
        }
        updateStatusBar();
    });
    
    connect(&controller, &WeatherThreadController::hourlyForecastReady,
            this, [this](const QList<HourlyForecast> &forecast) {
        if (m_forecastWidget) {
            m_forecastWidget->updateHourlyForecast(forecast);
        }
        if (m_chartWidget) {
            m_chartWidget->updateHourlyData(forecast);
        }
    });
    
    connect(&controller, &WeatherThreadController::dailyForecastReady,
            this, [this](const QList<DailyForecast> &forecast) {
        if (m_forecastWidget) {
            m_forecastWidget->updateDailyForecast(forecast);
        }
        if (m_chartWidget) {
            m_chartWidget->updateDailyData(forecast);
        }
    });
    
    connect(&controller, &WeatherThreadController::lifeIndexReady,
            this, [this](const QList<LifeIndex> &indices) {
        if (m_lifeIndexWidget) {
            m_lifeIndexWidget->updateLifeIndex(indices);
        }
    });
    
    connect(&controller, &WeatherThreadController::weatherAlertReady,
            this, [this](const QList<WeatherAlert> &alerts) {
        if (m_alertWidget) {
            m_alertWidget->updateAlerts(alerts);
        }
    });
    
    // 启动缓存清理定时器
    controller.startCacheCleanTimer();
    
    // 默认选中第一项
    ui->navListWidget->setCurrentRow(0);
    
    // 更新状态栏
    updateStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::initDatabase()
{
    DatabaseManager &dbManager = DatabaseManager::instance();
    
    connect(&dbManager, &DatabaseManager::errorOccurred, this, [](const QString &error) {
        qCritical() << "Database error:" << error;
    });
    
    return dbManager.initialize();
}

void MainWindow::updateStatusBar()
{
    QString updateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->statusbar->showMessage(tr("就绪 | 最后更新: %1").arg(updateTime));
}


void MainWindow::setupPages()
{
    // 创建实时天气页面
    m_currentWeatherWidget = new CurrentWeatherWidget(this);
    
    // 替换占位页面（索引0是实时天气）
    QWidget *oldWeatherWidget = ui->stackedWidget->widget(0);
    ui->stackedWidget->removeWidget(oldWeatherWidget);
    ui->stackedWidget->insertWidget(0, m_currentWeatherWidget);
    delete oldWeatherWidget;
    
    // 连接刷新信号
    connect(m_currentWeatherWidget, &CurrentWeatherWidget::refreshRequested,
            this, [this](const QString &cityId) {
        WeatherThreadController::instance().requestCurrentWeather(cityId);
    });
    
    // 创建天气预报页面
    m_forecastWidget = new ForecastWidget(this);
    
    // 替换占位页面（索引1是天气预报）
    QWidget *oldForecastWidget = ui->stackedWidget->widget(1);
    ui->stackedWidget->removeWidget(oldForecastWidget);
    ui->stackedWidget->insertWidget(1, m_forecastWidget);
    delete oldForecastWidget;
    
    // 连接预报刷新信号
    connect(m_forecastWidget, &ForecastWidget::refreshRequested,
            this, [this](const QString &cityId) {
        WeatherThreadController::instance().requestHourlyForecast(cityId);
        WeatherThreadController::instance().requestDailyForecast(cityId);
    });
    
    // 创建数据分析页面
    m_chartWidget = new ChartWidget(this);
    
    // 替换占位页面（索引2是数据分析）
    QWidget *oldChartWidget = ui->stackedWidget->widget(2);
    ui->stackedWidget->removeWidget(oldChartWidget);
    ui->stackedWidget->insertWidget(2, m_chartWidget);
    delete oldChartWidget;
    
    // 连接图表刷新信号
    connect(m_chartWidget, &ChartWidget::refreshRequested,
            this, [this](const QString &cityId) {
        WeatherThreadController::instance().requestHourlyForecast(cityId);
        WeatherThreadController::instance().requestDailyForecast(cityId);
    });
    
    // 创建生活指数页面
    m_lifeIndexWidget = new LifeIndexWidget(this);
    
    // 替换占位页面（索引3是生活指数）
    QWidget *oldLifeWidget = ui->stackedWidget->widget(3);
    ui->stackedWidget->removeWidget(oldLifeWidget);
    ui->stackedWidget->insertWidget(3, m_lifeIndexWidget);
    delete oldLifeWidget;
    
    // 连接生活指数刷新信号
    connect(m_lifeIndexWidget, &LifeIndexWidget::refreshRequested,
            this, [this](const QString &cityId) {
        WeatherThreadController::instance().requestLifeIndex(cityId);
    });
    
    // 创建历史记录页面
    m_historyWidget = new HistoryWidget(this);
    
    // 替换占位页面（索引4是历史记录）
    QWidget *oldHistoryWidget = ui->stackedWidget->widget(4);
    ui->stackedWidget->removeWidget(oldHistoryWidget);
    ui->stackedWidget->insertWidget(4, m_historyWidget);
    delete oldHistoryWidget;
    
    // 创建天气预警页面
    m_alertWidget = new AlertWidget(this);
    
    // 替换占位页面（索引5是天气预警）
    QWidget *oldAlertWidget = ui->stackedWidget->widget(5);
    ui->stackedWidget->removeWidget(oldAlertWidget);
    ui->stackedWidget->insertWidget(5, m_alertWidget);
    delete oldAlertWidget;
    
    // 创建城市管理页面
    m_cityWidget = new CityWidget(this);
    
    // 替换占位页面（索引6是城市管理）
    QWidget *oldWidget = ui->stackedWidget->widget(6);
    ui->stackedWidget->removeWidget(oldWidget);
    ui->stackedWidget->insertWidget(6, m_cityWidget);
    delete oldWidget;
    
    // 连接城市选择信号
    connect(m_cityWidget, &CityWidget::citySelected, this, [this](const QString &cityId) {
        m_currentCityId = cityId;
        // 获取城市名称
        CityModel model;
        model.loadFromDatabase();
        CityInfo city = model.cityById(cityId);
        m_currentCityName = city.name;
        
        // 更新实时天气页面
        if (m_currentWeatherWidget) {
            m_currentWeatherWidget->setCity(cityId, m_currentCityName);
        }
        
        // 更新预报页面
        if (m_forecastWidget) {
            m_forecastWidget->setCity(cityId, m_currentCityName);
        }
        
        // 更新图表页面
        if (m_chartWidget) {
            m_chartWidget->setCity(cityId, m_currentCityName);
        }
        
        // 更新生活指数页面
        if (m_lifeIndexWidget) {
            m_lifeIndexWidget->setCity(cityId, m_currentCityName);
        }
        
        // 更新历史记录页面
        if (m_historyWidget) {
            m_historyWidget->setCity(cityId, m_currentCityName);
        }
        
        // 请求所有天气数据
        WeatherThreadController::instance().requestAllWeatherData(cityId);
        
        // 切换到实时天气页面
        ui->navListWidget->setCurrentRow(0);
        updateStatusBar();
    });
    
    // 创建设置页面
    m_settingsWidget = new SettingsWidget(this);
    
    // 替换占位页面（索引7是设置）
    QWidget *oldSettingsWidget = ui->stackedWidget->widget(7);
    ui->stackedWidget->removeWidget(oldSettingsWidget);
    ui->stackedWidget->insertWidget(7, m_settingsWidget);
    delete oldSettingsWidget;
    
    // 连接设置变更信号
    connect(m_settingsWidget, &SettingsWidget::settingsChanged,
            this, [this]() {
        // 刷新当前天气显示以应用新单位
        if (!m_currentCityId.isEmpty()) {
            WeatherThreadController::instance().requestCurrentWeather(m_currentCityId);
        }
    });
    
    // 创建关于页面
    m_aboutWidget = new AboutWidget(this);
    
    // 替换占位页面（索引8是关于）
    QWidget *oldAboutWidget = ui->stackedWidget->widget(8);
    ui->stackedWidget->removeWidget(oldAboutWidget);
    ui->stackedWidget->insertWidget(8, m_aboutWidget);
    delete oldAboutWidget;
}
