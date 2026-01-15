/**
 * @file weatherworker.h
 * @brief 天气数据后台工作线程
 */

#ifndef WEATHERWORKER_H
#define WEATHERWORKER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QTimer>
#include "../models/weatherdata.h"

/**
 * @struct WeatherTask
 * @brief 天气任务结构
 */
struct WeatherTask {
    enum Type {
        FetchCurrent,
        FetchHourly,
        FetchDaily,
        FetchLifeIndex,
        FetchAlert,
        CleanCache
    };
    
    Type type;
    QString cityId;
    int param = 0;  // hours/days
};

/**
 * @class WeatherWorker
 * @brief 天气数据后台工作类
 * 
 * 在独立线程中处理天气数据请求，避免阻塞UI
 */
class WeatherWorker : public QObject
{
    Q_OBJECT

public:
    explicit WeatherWorker(QObject *parent = nullptr);
    ~WeatherWorker();
    
    /**
     * @brief 添加任务到队列
     */
    void addTask(const WeatherTask &task);
    
    /**
     * @brief 清空任务队列
     */
    void clearTasks();
    
    /**
     * @brief 获取队列中的任务数量
     */
    int pendingTaskCount() const;

public slots:
    /**
     * @brief 处理任务队列
     */
    void processQueue();
    
    /**
     * @brief 清理过期缓存
     */
    void cleanExpiredCache();

signals:
    void currentWeatherReady(const CurrentWeather &weather);
    void hourlyForecastReady(const QList<HourlyForecast> &forecast);
    void dailyForecastReady(const QList<DailyForecast> &forecast);
    void lifeIndexReady(const QList<LifeIndex> &indices);
    void weatherAlertReady(const QList<WeatherAlert> &alerts);
    void taskStarted(const QString &cityId, WeatherTask::Type type);
    void taskFinished(const QString &cityId, WeatherTask::Type type);
    void errorOccurred(const QString &error);
    void cacheCleanFinished(int removedCount);

private:
    void processTask(const WeatherTask &task);
    
    QQueue<WeatherTask> m_taskQueue;
    mutable QMutex m_mutex;
    bool m_processing = false;
};

/**
 * @class WeatherThreadController
 * @brief 天气线程控制器
 * 
 * 管理后台工作线程的生命周期
 */
class WeatherThreadController : public QObject
{
    Q_OBJECT

public:
    static WeatherThreadController& instance();
    
    /**
     * @brief 请求当前天气
     */
    void requestCurrentWeather(const QString &cityId);
    
    /**
     * @brief 请求逐小时预报
     */
    void requestHourlyForecast(const QString &cityId, int hours = 24);
    
    /**
     * @brief 请求每日预报
     */
    void requestDailyForecast(const QString &cityId, int days = 7);
    
    /**
     * @brief 请求生活指数
     */
    void requestLifeIndex(const QString &cityId);
    
    /**
     * @brief 请求天气预警
     */
    void requestWeatherAlert(const QString &cityId);
    
    /**
     * @brief 请求所有天气数据
     */
    void requestAllWeatherData(const QString &cityId);
    
    /**
     * @brief 启动定时缓存清理
     */
    void startCacheCleanTimer(int intervalMs = 300000);  // 默认5分钟
    
    /**
     * @brief 停止定时缓存清理
     */
    void stopCacheCleanTimer();
    
    /**
     * @brief 获取待处理任务数
     */
    int pendingTaskCount() const;

signals:
    void currentWeatherReady(const CurrentWeather &weather);
    void hourlyForecastReady(const QList<HourlyForecast> &forecast);
    void dailyForecastReady(const QList<DailyForecast> &forecast);
    void lifeIndexReady(const QList<LifeIndex> &indices);
    void weatherAlertReady(const QList<WeatherAlert> &alerts);
    void taskStarted(const QString &cityId, int type);
    void taskFinished(const QString &cityId, int type);
    void errorOccurred(const QString &error);
    void allDataReady(const QString &cityId);

private slots:
    void onTaskFinished(const QString &cityId, WeatherTask::Type type);

private:
    explicit WeatherThreadController(QObject *parent = nullptr);
    ~WeatherThreadController();
    
    WeatherThreadController(const WeatherThreadController&) = delete;
    WeatherThreadController& operator=(const WeatherThreadController&) = delete;
    
    QThread *m_workerThread;
    WeatherWorker *m_worker;
    QTimer *m_cacheCleanTimer;
    
    // 跟踪批量请求
    QString m_batchCityId;
    int m_pendingBatchTasks = 0;
};

#endif // WEATHERWORKER_H
