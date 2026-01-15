/**
 * @file networkmanager.h
 * @brief 网络请求管理器类声明
 */

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QCache>
#include <QMutex>

/**
 * @struct NetworkResponse
 * @brief 网络响应数据结构
 */
struct NetworkResponse {
    bool success = false;
    int statusCode = 0;
    QJsonObject data;
    QString errorString;
    bool fromCache = false;
};

/**
 * @struct CacheEntry
 * @brief 缓存条目
 */
struct CacheEntry {
    QJsonObject data;
    qint64 timestamp;
    int ttl; // 生存时间(秒)
};

/**
 * @class NetworkManager
 * @brief 网络请求管理单例类
 * 
 * 功能：HTTP请求、JSON解析、请求缓存、自动重试
 */
class NetworkManager : public QObject
{
    Q_OBJECT

public:
    static NetworkManager& instance();
    
    /**
     * @brief 发送GET请求
     * @param url 请求URL
     * @param useCache 是否使用缓存
     * @param cacheTtl 缓存生存时间(秒)
     */
    void get(const QString &url, bool useCache = true, int cacheTtl = 300);
    
    /**
     * @brief 发送POST请求
     * @param url 请求URL
     * @param data POST数据
     */
    void post(const QString &url, const QJsonObject &data);
    
    /**
     * @brief 设置请求超时时间
     * @param msec 超时毫秒数
     */
    void setTimeout(int msec);
    
    /**
     * @brief 设置最大重试次数
     * @param count 重试次数
     */
    void setMaxRetries(int count);
    
    /**
     * @brief 清除所有缓存
     */
    void clearCache();
    
    /**
     * @brief 清理过期缓存
     * @return 清理的条目数量
     */
    int cleanExpiredCache();
    
    /**
     * @brief 检查网络是否可用
     * @return 网络状态
     */
    bool isNetworkAvailable() const;

signals:
    /**
     * @brief 请求完成信号
     * @param url 请求URL
     * @param response 响应数据
     */
    void requestFinished(const QString &url, const NetworkResponse &response);
    
    /**
     * @brief 请求错误信号
     * @param url 请求URL
     * @param error 错误信息
     */
    void requestError(const QString &url, const QString &error);
    
    /**
     * @brief 网络状态变化信号
     * @param available 是否可用
     */
    void networkStatusChanged(bool available);

private slots:
    void onReplyFinished(QNetworkReply *reply);
    void onRequestTimeout();

private:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();
    
    NetworkManager(const NetworkManager&) = delete;
    NetworkManager& operator=(const NetworkManager&) = delete;
    
    /**
     * @brief 从缓存获取数据
     * @param url 请求URL
     * @param response 输出响应
     * @return 是否命中缓存
     */
    bool getFromCache(const QString &url, NetworkResponse &response);
    
    /**
     * @brief 保存到缓存
     * @param url 请求URL
     * @param data 响应数据
     * @param ttl 生存时间
     */
    void saveToCache(const QString &url, const QJsonObject &data, int ttl);
    
    /**
     * @brief 执行重试
     * @param url 请求URL
     */
    void doRetry(const QString &url);

private:
    QNetworkAccessManager *m_manager;
    QCache<QString, CacheEntry> m_cache;
    QMutex m_cacheMutex;
    
    int m_timeout;
    int m_maxRetries;
    
    // 请求重试计数
    QMap<QString, int> m_retryCount;
    // 请求缓存TTL
    QMap<QString, int> m_requestCacheTtl;
    // 请求定时器
    QMap<QNetworkReply*, QTimer*> m_timeoutTimers;
    
    static const int DEFAULT_TIMEOUT = 15000;
    static const int DEFAULT_MAX_RETRIES = 3;
    static const int DEFAULT_CACHE_SIZE = 100;
};

#endif // NETWORKMANAGER_H
