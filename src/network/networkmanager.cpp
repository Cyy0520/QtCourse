/**
 * @file networkmanager.cpp
 * @brief 网络请求管理器类实现
 */

#include "networkmanager.h"
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QDateTime>
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
    , m_cache(DEFAULT_CACHE_SIZE)
    , m_timeout(DEFAULT_TIMEOUT)
    , m_maxRetries(DEFAULT_MAX_RETRIES)
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &NetworkManager::onReplyFinished);
}

NetworkManager::~NetworkManager()
{
    // 清理所有定时器
    for (QTimer *timer : m_timeoutTimers.values()) {
        timer->stop();
        delete timer;
    }
}

NetworkManager& NetworkManager::instance()
{
    static NetworkManager instance;
    return instance;
}

void NetworkManager::get(const QString &url, bool useCache, int cacheTtl)
{
    // 检查缓存
    if (useCache) {
        NetworkResponse cachedResponse;
        if (getFromCache(url, cachedResponse)) {
            qDebug() << "Cache hit for:" << url;
            emit requestFinished(url, cachedResponse);
            return;
        }
    }
    
    // 保存缓存TTL设置
    m_requestCacheTtl[url] = cacheTtl;
    
    // 初始化重试计数
    if (!m_retryCount.contains(url)) {
        m_retryCount[url] = 0;
    }
    
    // 创建请求
    QUrl requestUrl(url);
    QNetworkRequest request(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    
    QNetworkReply *reply = m_manager->get(request);
    
    // 设置超时定时器
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    m_timeoutTimers[reply] = timer;
    
    connect(timer, &QTimer::timeout, this, [this, reply]() {
        onRequestTimeout();
        reply->abort();
    });
    
    timer->start(m_timeout);
    
    qDebug() << "GET request sent:" << url;
}

void NetworkManager::post(const QString &url, const QJsonObject &data)
{
    QUrl requestUrl(url);
    QNetworkRequest request(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    
    QJsonDocument doc(data);
    QByteArray postData = doc.toJson(QJsonDocument::Compact);
    
    QNetworkReply *reply = m_manager->post(request, postData);
    
    // 设置超时定时器
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    m_timeoutTimers[reply] = timer;
    
    connect(timer, &QTimer::timeout, this, [this, reply]() {
        onRequestTimeout();
        reply->abort();
    });
    
    timer->start(m_timeout);
    
    qDebug() << "POST request sent:" << url;
}

void NetworkManager::onReplyFinished(QNetworkReply *reply)
{
    // 停止并清理定时器
    if (m_timeoutTimers.contains(reply)) {
        QTimer *timer = m_timeoutTimers.take(reply);
        timer->stop();
        timer->deleteLater();
    }
    
    QString url = reply->url().toString();
    NetworkResponse response;
    response.statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    if (reply->error() == QNetworkReply::NoError) {
        // 解析JSON
        QByteArray data = reply->readAll();
        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        
        if (parseError.error == QJsonParseError::NoError) {
            response.success = true;
            response.data = doc.object();
            
            // 保存到缓存
            int ttl = m_requestCacheTtl.value(url, 300);
            saveToCache(url, response.data, ttl);
            
            qDebug() << "Request successful:" << url;
        } else {
            response.success = false;
            response.errorString = tr("JSON解析错误: %1").arg(parseError.errorString());
            qWarning() << "JSON parse error:" << parseError.errorString();
        }
        
        // 清理重试计数
        m_retryCount.remove(url);
        m_requestCacheTtl.remove(url);
        
        emit requestFinished(url, response);
        
    } else {
        // 请求失败，尝试重试
        int retries = m_retryCount.value(url, 0);
        
        if (retries < m_maxRetries && reply->error() != QNetworkReply::OperationCanceledError) {
            m_retryCount[url] = retries + 1;
            qDebug() << "Retrying request:" << url << "attempt:" << (retries + 1);
            
            // 延迟重试
            QTimer::singleShot(1000 * (retries + 1), this, [this, url]() {
                doRetry(url);
            });
        } else {
            response.success = false;
            response.errorString = reply->errorString();
            
            m_retryCount.remove(url);
            m_requestCacheTtl.remove(url);
            
            qWarning() << "Request failed:" << url << reply->errorString();
            emit requestError(url, response.errorString);
            emit requestFinished(url, response);
        }
    }
    
    reply->deleteLater();
}

void NetworkManager::onRequestTimeout()
{
    qWarning() << "Request timeout";
}

void NetworkManager::doRetry(const QString &url)
{
    QUrl requestUrl(url);
    QNetworkRequest request(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    
    QNetworkReply *reply = m_manager->get(request);
    
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    m_timeoutTimers[reply] = timer;
    
    connect(timer, &QTimer::timeout, this, [this, reply]() {
        onRequestTimeout();
        reply->abort();
    });
    
    timer->start(m_timeout);
}

bool NetworkManager::getFromCache(const QString &url, NetworkResponse &response)
{
    QMutexLocker locker(&m_cacheMutex);
    
    CacheEntry *entry = m_cache.object(url);
    if (entry) {
        qint64 now = QDateTime::currentSecsSinceEpoch();
        if (now - entry->timestamp < entry->ttl) {
            response.success = true;
            response.data = entry->data;
            response.fromCache = true;
            return true;
        } else {
            // 缓存过期，移除
            m_cache.remove(url);
        }
    }
    return false;
}

void NetworkManager::saveToCache(const QString &url, const QJsonObject &data, int ttl)
{
    QMutexLocker locker(&m_cacheMutex);
    
    CacheEntry *entry = new CacheEntry();
    entry->data = data;
    entry->timestamp = QDateTime::currentSecsSinceEpoch();
    entry->ttl = ttl;
    
    m_cache.insert(url, entry);
    qDebug() << "Cached response for:" << url << "TTL:" << ttl << "s";
}

void NetworkManager::setTimeout(int msec)
{
    m_timeout = msec;
}

void NetworkManager::setMaxRetries(int count)
{
    m_maxRetries = count;
}

void NetworkManager::clearCache()
{
    QMutexLocker locker(&m_cacheMutex);
    m_cache.clear();
    qDebug() << "Cache cleared";
}

bool NetworkManager::isNetworkAvailable() const
{
    // Qt6 移除了 networkAccessible，简单返回 true
    // 实际网络状态会在请求失败时处理
    return true;
}
