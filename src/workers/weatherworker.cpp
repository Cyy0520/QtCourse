/**
 * @file weatherworker.cpp
 * @brief 天气数据后台工作线程实现
 */

#include "weatherworker.h"
#include "../services/weatherservice.h"
#include "../network/networkmanager.h"
#include <QDebug>
#include <memory>

// ==================== WeatherWorker ====================

WeatherWorker::WeatherWorker(QObject *parent)
    : QObject(parent)
{
}

WeatherWorker::~WeatherWorker()
{
}

void WeatherWorker::addTask(const WeatherTask &task)
{
    QMutexLocker locker(&m_mutex);
    m_taskQueue.enqueue(task);
    
    if (!m_processing) {
        QMetaObject::invokeMethod(this, "processQueue", Qt::QueuedConnection);
    }
}

void WeatherWorker::clearTasks()
{
    QMutexLocker locker(&m_mutex);
    m_taskQueue.clear();
}

int WeatherWorker::pendingTaskCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_taskQueue.size();
}

void WeatherWorker::processQueue()
{
    WeatherTask task;
    
    {
        QMutexLocker locker(&m_mutex);
        if (m_taskQueue.isEmpty()) {
            m_processing = false;
            return;
        }
        m_processing = true;
        task = m_taskQueue.dequeue();
    }
    
    processTask(task);
    
    // 继续处理下一个任务
    QMetaObject::invokeMethod(this, "processQueue", Qt::QueuedConnection);
}

void WeatherWorker::processTask(const WeatherTask &task)
{
    emit taskStarted(task.cityId, task.type);
    
    WeatherService &service = WeatherService::instance();
    
    switch (task.type) {
        case WeatherTask::FetchCurrent: {
            auto conn = std::make_shared<QMetaObject::Connection>();
            *conn = connect(&service, &WeatherService::currentWeatherReady,
                          this, [this, task, conn](const CurrentWeather &weather) {
                emit currentWeatherReady(weather);
                emit taskFinished(task.cityId, task.type);
                disconnect(*conn);
            });
            service.fetchCurrentWeather(task.cityId);
            break;
        }
        case WeatherTask::FetchHourly: {
            auto conn = std::make_shared<QMetaObject::Connection>();
            *conn = connect(&service, &WeatherService::hourlyForecastReady,
                          this, [this, task, conn](const QList<HourlyForecast> &forecast) {
                emit hourlyForecastReady(forecast);
                emit taskFinished(task.cityId, task.type);
                disconnect(*conn);
            });
            service.fetchHourlyForecast(task.cityId, task.param > 0 ? task.param : 24);
            break;
        }
        case WeatherTask::FetchDaily: {
            auto conn = std::make_shared<QMetaObject::Connection>();
            *conn = connect(&service, &WeatherService::dailyForecastReady,
                          this, [this, task, conn](const QList<DailyForecast> &forecast) {
                emit dailyForecastReady(forecast);
                emit taskFinished(task.cityId, task.type);
                disconnect(*conn);
            });
            service.fetchDailyForecast(task.cityId, task.param > 0 ? task.param : 7);
            break;
        }
        case WeatherTask::FetchLifeIndex: {
            auto conn = std::make_shared<QMetaObject::Connection>();
            *conn = connect(&service, &WeatherService::lifeIndexReady,
                          this, [this, task, conn](const QList<LifeIndex> &indices) {
                emit lifeIndexReady(indices);
                emit taskFinished(task.cityId, task.type);
                disconnect(*conn);
            });
            service.fetchLifeIndex(task.cityId);
            break;
        }
        case WeatherTask::FetchAlert: {
            auto conn = std::make_shared<QMetaObject::Connection>();
            *conn = connect(&service, &WeatherService::weatherAlertReady,
                          this, [this, task, conn](const QList<WeatherAlert> &alerts) {
                emit weatherAlertReady(alerts);
                emit taskFinished(task.cityId, task.type);
                disconnect(*conn);
            });
            service.fetchWeatherAlert(task.cityId);
            break;
        }
        case WeatherTask::CleanCache: {
            cleanExpiredCache();
            emit taskFinished(QString(), task.type);
            break;
        }
    }
}

void WeatherWorker::cleanExpiredCache()
{
    int removed = NetworkManager::instance().cleanExpiredCache();
    emit cacheCleanFinished(removed);
    qDebug() << "Cache cleaned, removed" << removed << "entries";
}

// ==================== WeatherThreadController ====================

WeatherThreadController::WeatherThreadController(QObject *parent)
    : QObject(parent)
    , m_workerThread(new QThread(this))
    , m_worker(new WeatherWorker())
    , m_cacheCleanTimer(new QTimer(this))
{
    m_worker->moveToThread(m_workerThread);
    
    // 连接信号
    connect(m_worker, &WeatherWorker::currentWeatherReady,
            this, &WeatherThreadController::currentWeatherReady);
    connect(m_worker, &WeatherWorker::hourlyForecastReady,
            this, &WeatherThreadController::hourlyForecastReady);
    connect(m_worker, &WeatherWorker::dailyForecastReady,
            this, &WeatherThreadController::dailyForecastReady);
    connect(m_worker, &WeatherWorker::lifeIndexReady,
            this, &WeatherThreadController::lifeIndexReady);
    connect(m_worker, &WeatherWorker::weatherAlertReady,
            this, &WeatherThreadController::weatherAlertReady);
    connect(m_worker, &WeatherWorker::errorOccurred,
            this, &WeatherThreadController::errorOccurred);
    
    connect(m_worker, &WeatherWorker::taskStarted,
            this, [this](const QString &cityId, WeatherTask::Type type) {
        emit taskStarted(cityId, static_cast<int>(type));
    });
    
    connect(m_worker, &WeatherWorker::taskFinished,
            this, &WeatherThreadController::onTaskFinished);
    
    // 缓存清理定时器
    connect(m_cacheCleanTimer, &QTimer::timeout, this, [this]() {
        WeatherTask task;
        task.type = WeatherTask::CleanCache;
        m_worker->addTask(task);
    });
    
    // 线程清理
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    
    m_workerThread->start();
}

WeatherThreadController::~WeatherThreadController()
{
    m_cacheCleanTimer->stop();
    m_workerThread->quit();
    m_workerThread->wait();
}

WeatherThreadController& WeatherThreadController::instance()
{
    static WeatherThreadController instance;
    return instance;
}

void WeatherThreadController::requestCurrentWeather(const QString &cityId)
{
    WeatherTask task;
    task.type = WeatherTask::FetchCurrent;
    task.cityId = cityId;
    m_worker->addTask(task);
}

void WeatherThreadController::requestHourlyForecast(const QString &cityId, int hours)
{
    WeatherTask task;
    task.type = WeatherTask::FetchHourly;
    task.cityId = cityId;
    task.param = hours;
    m_worker->addTask(task);
}

void WeatherThreadController::requestDailyForecast(const QString &cityId, int days)
{
    WeatherTask task;
    task.type = WeatherTask::FetchDaily;
    task.cityId = cityId;
    task.param = days;
    m_worker->addTask(task);
}

void WeatherThreadController::requestLifeIndex(const QString &cityId)
{
    WeatherTask task;
    task.type = WeatherTask::FetchLifeIndex;
    task.cityId = cityId;
    m_worker->addTask(task);
}

void WeatherThreadController::requestWeatherAlert(const QString &cityId)
{
    WeatherTask task;
    task.type = WeatherTask::FetchAlert;
    task.cityId = cityId;
    m_worker->addTask(task);
}

void WeatherThreadController::requestAllWeatherData(const QString &cityId)
{
    m_batchCityId = cityId;
    m_pendingBatchTasks = 5;  // current, hourly, daily, lifeIndex, alert
    
    requestCurrentWeather(cityId);
    requestHourlyForecast(cityId);
    requestDailyForecast(cityId);
    requestLifeIndex(cityId);
    requestWeatherAlert(cityId);
}

void WeatherThreadController::startCacheCleanTimer(int intervalMs)
{
    m_cacheCleanTimer->start(intervalMs);
}

void WeatherThreadController::stopCacheCleanTimer()
{
    m_cacheCleanTimer->stop();
}

int WeatherThreadController::pendingTaskCount() const
{
    return m_worker->pendingTaskCount();
}

void WeatherThreadController::onTaskFinished(const QString &cityId, WeatherTask::Type type)
{
    emit taskFinished(cityId, static_cast<int>(type));
    
    // 检查批量请求是否完成
    if (!m_batchCityId.isEmpty() && cityId == m_batchCityId) {
        m_pendingBatchTasks--;
        if (m_pendingBatchTasks <= 0) {
            emit allDataReady(m_batchCityId);
            m_batchCityId.clear();
            m_pendingBatchTasks = 0;
        }
    }
}
