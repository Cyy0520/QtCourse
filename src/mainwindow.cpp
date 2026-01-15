/**
 * @file mainwindow.cpp
 * @brief 主窗口类实现
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "views/citywidget.h"
#include "views/currentweatherwidget.h"
#include "workers/weatherworker.h"
#include "models/citymodel.h"
#include <QDateTime>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_cityWidget(nullptr)
    , m_currentWeatherWidget(nullptr)
{
    ui->setupUi(this);
    
    // 初始化数据库
    if (!initDatabase()) {
        QMessageBox::critical(this, tr("错误"), tr("数据库初始化失败，程序可能无法正常工作。"));
    }
    
    // 初始化页面
    setupPages();
    
    // 连接天气数据信号
    connect(&WeatherThreadController::instance(), &WeatherThreadController::currentWeatherReady,
            this, [this](const CurrentWeather &weather) {
        if (m_currentWeatherWidget) {
            m_currentWeatherWidget->updateWeather(weather);
        }
        updateStatusBar();
    });
    
    // 启动缓存清理定时器
    WeatherThreadController::instance().startCacheCleanTimer();
    
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
    
    // 创建城市管理页面
    m_cityWidget = new CityWidget(this);
    
    // 替换占位页面（索引4是城市管理）
    QWidget *oldWidget = ui->stackedWidget->widget(4);
    ui->stackedWidget->removeWidget(oldWidget);
    ui->stackedWidget->insertWidget(4, m_cityWidget);
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
        
        // 请求天气数据
        WeatherThreadController::instance().requestCurrentWeather(cityId);
        
        // 切换到实时天气页面
        ui->navListWidget->setCurrentRow(0);
        updateStatusBar();
    });
}
