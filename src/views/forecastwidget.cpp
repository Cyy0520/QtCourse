/**
 * @file forecastwidget.cpp
 * @brief 天气预报展示组件实现
 */

#include "forecastwidget.h"
#include "ui_forecastwidget.h"
#include "../config/configmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

ForecastWidget::ForecastWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ForecastWidget)
{
    ui->setupUi(this);
    setupConnections();
}

ForecastWidget::~ForecastWidget()
{
    delete ui;
}

void ForecastWidget::setupConnections()
{
    connect(ui->refreshBtn, &QPushButton::clicked,
            this, &ForecastWidget::onRefreshClicked);
}

void ForecastWidget::setCity(const QString &cityId, const QString &cityName)
{
    m_currentCityId = cityId;
    m_currentCityName = cityName;
    ui->cityLabel->setText(cityName);
    clear();
}

void ForecastWidget::updateHourlyForecast(const QList<HourlyForecast> &forecast)
{
    clearHourlyItems();
    
    ConfigManager &config = ConfigManager::instance();
    
    for (const HourlyForecast &hour : forecast) {
        QFrame *item = createHourlyItem(hour);
        ui->hourlyLayout->addWidget(item);
        m_hourlyItems.append(item);
    }
    
    // 添加弹性空间
    ui->hourlyLayout->addStretch();
}

void ForecastWidget::updateDailyForecast(const QList<DailyForecast> &forecast)
{
    clearDailyItems();
    
    for (const DailyForecast &day : forecast) {
        QFrame *item = createDailyItem(day);
        ui->dailyLayout->addWidget(item);
        m_dailyItems.append(item);
    }
    
    // 添加弹性空间
    ui->dailyLayout->addStretch();
}

void ForecastWidget::clear()
{
    clearHourlyItems();
    clearDailyItems();
}

void ForecastWidget::clearHourlyItems()
{
    for (QFrame *item : m_hourlyItems) {
        ui->hourlyLayout->removeWidget(item);
        delete item;
    }
    m_hourlyItems.clear();
    
    // 移除弹性空间
    QLayoutItem *child;
    while ((child = ui->hourlyLayout->takeAt(0)) != nullptr) {
        delete child;
    }
}

void ForecastWidget::clearDailyItems()
{
    for (QFrame *item : m_dailyItems) {
        ui->dailyLayout->removeWidget(item);
        delete item;
    }
    m_dailyItems.clear();
    
    // 移除弹性空间
    QLayoutItem *child;
    while ((child = ui->dailyLayout->takeAt(0)) != nullptr) {
        delete child;
    }
}

QFrame* ForecastWidget::createHourlyItem(const HourlyForecast &forecast)
{
    ConfigManager &config = ConfigManager::instance();
    
    QFrame *frame = new QFrame();
    frame->setObjectName("hourItem");
    frame->setStyleSheet(
        "QFrame#hourItem { background-color: #f5f7fa; border-radius: 8px; }"
    );
    frame->setMinimumWidth(70);
    frame->setMaximumWidth(80);
    
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setSpacing(4);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setAlignment(Qt::AlignCenter);
    
    // 时间
    QLabel *timeLabel = new QLabel(forecast.time.toString("HH:mm"));
    timeLabel->setStyleSheet("font-size: 12px; color: #909399;");
    timeLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(timeLabel);
    
    // 天气图标
    QLabel *iconLabel = new QLabel(getWeatherEmoji(forecast.weatherCode));
    iconLabel->setStyleSheet("font-size: 24px;");
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);
    
    // 温度
    QString tempStr = config.formatTemperature(forecast.temperature);
    QLabel *tempLabel = new QLabel(tempStr);
    tempLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #303133;");
    tempLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(tempLabel);
    
    // 降水概率
    if (forecast.precipitationProb > 0) {
        QLabel *precipLabel = new QLabel(QString("💧%1%").arg(forecast.precipitationProb));
        precipLabel->setStyleSheet("font-size: 10px; color: #409EFF;");
        precipLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(precipLabel);
    }
    
    return frame;
}

QFrame* ForecastWidget::createDailyItem(const DailyForecast &forecast)
{
    ConfigManager &config = ConfigManager::instance();
    
    QFrame *frame = new QFrame();
    frame->setObjectName("dailyItem");
    frame->setStyleSheet(
        "QFrame#dailyItem { background-color: #f5f7fa; border-radius: 8px; }"
    );
    
    QHBoxLayout *layout = new QHBoxLayout(frame);
    layout->setSpacing(16);
    layout->setContentsMargins(16, 12, 16, 12);
    
    // 日期和星期
    QVBoxLayout *dateLayout = new QVBoxLayout();
    dateLayout->setSpacing(2);
    
    QLabel *dateLabel = new QLabel(forecast.date.toString("MM/dd"));
    dateLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #303133;");
    dateLayout->addWidget(dateLabel);
    
    QLabel *weekdayLabel = new QLabel(getWeekdayName(forecast.date));
    weekdayLabel->setStyleSheet("font-size: 12px; color: #909399;");
    dateLayout->addWidget(weekdayLabel);
    
    layout->addLayout(dateLayout);
    
    // 白天天气
    QVBoxLayout *dayLayout = new QVBoxLayout();
    dayLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *dayIconLabel = new QLabel(getWeatherEmoji(forecast.weatherCodeDay));
    dayIconLabel->setStyleSheet("font-size: 24px;");
    dayIconLabel->setAlignment(Qt::AlignCenter);
    dayLayout->addWidget(dayIconLabel);
    
    QLabel *dayDescLabel = new QLabel(forecast.weatherDescDay);
    dayDescLabel->setStyleSheet("font-size: 12px; color: #606266;");
    dayDescLabel->setAlignment(Qt::AlignCenter);
    dayLayout->addWidget(dayDescLabel);
    
    layout->addLayout(dayLayout);
    
    // 温度范围
    layout->addStretch();
    
    QString highStr = config.formatTemperature(forecast.highTemp);
    QLabel *highLabel = new QLabel(highStr);
    highLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #F56C6C;");
    layout->addWidget(highLabel);
    
    QLabel *sepLabel = new QLabel("/");
    sepLabel->setStyleSheet("font-size: 16px; color: #909399;");
    layout->addWidget(sepLabel);
    
    QString lowStr = config.formatTemperature(forecast.lowTemp);
    QLabel *lowLabel = new QLabel(lowStr);
    lowLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #409EFF;");
    layout->addWidget(lowLabel);
    
    layout->addStretch();
    
    // 夜间天气
    QVBoxLayout *nightLayout = new QVBoxLayout();
    nightLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *nightIconLabel = new QLabel(getWeatherEmoji(forecast.weatherCodeNight));
    nightIconLabel->setStyleSheet("font-size: 24px;");
    nightIconLabel->setAlignment(Qt::AlignCenter);
    nightLayout->addWidget(nightIconLabel);
    
    QLabel *nightDescLabel = new QLabel(forecast.weatherDescNight);
    nightDescLabel->setStyleSheet("font-size: 12px; color: #606266;");
    nightDescLabel->setAlignment(Qt::AlignCenter);
    nightLayout->addWidget(nightDescLabel);
    
    layout->addLayout(nightLayout);
    
    // 降水概率
    if (forecast.precipitationProb > 0) {
        QLabel *precipLabel = new QLabel(QString("💧%1%").arg(forecast.precipitationProb));
        precipLabel->setStyleSheet("font-size: 12px; color: #409EFF;");
        precipLabel->setMinimumWidth(50);
        layout->addWidget(precipLabel);
    } else {
        QWidget *spacer = new QWidget();
        spacer->setMinimumWidth(50);
        layout->addWidget(spacer);
    }
    
    return frame;
}

void ForecastWidget::onRefreshClicked()
{
    if (!m_currentCityId.isEmpty()) {
        emit refreshRequested(m_currentCityId);
    }
}

QString ForecastWidget::getWeatherEmoji(const QString &code)
{
    static QMap<QString, QString> emojiMap = {
        {"100", "☀️"}, {"101", "⛅"}, {"102", "🌤️"}, {"103", "⛅"}, {"104", "☁️"},
        {"150", "🌙"}, {"151", "🌙"}, {"300", "🌧️"}, {"301", "🌧️"}, {"302", "⛈️"},
        {"303", "⛈️"}, {"304", "⛈️"}, {"305", "🌧️"}, {"306", "🌧️"}, {"307", "🌧️"},
        {"308", "🌧️"}, {"309", "🌧️"}, {"310", "🌧️"}, {"311", "🌧️"}, {"312", "🌧️"},
        {"400", "🌨️"}, {"401", "🌨️"}, {"402", "❄️"}, {"403", "❄️"}, {"404", "🌨️"},
        {"500", "🌫️"}, {"501", "🌫️"}, {"502", "🌫️"}, {"503", "🌫️"}, {"504", "🌫️"}
    };
    return emojiMap.value(code, "🌡️");
}

QString ForecastWidget::getWeekdayName(const QDate &date)
{
    static QStringList weekdays = {
        tr("周日"), tr("周一"), tr("周二"), tr("周三"),
        tr("周四"), tr("周五"), tr("周六")
    };
    
    if (date == QDate::currentDate()) {
        return tr("今天");
    } else if (date == QDate::currentDate().addDays(1)) {
        return tr("明天");
    }
    
    return weekdays.at(date.dayOfWeek() % 7);
}
