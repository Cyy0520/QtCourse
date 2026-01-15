/**
 * @file cityservice.h
 * @brief 城市服务类声明
 */

#ifndef CITYSERVICE_H
#define CITYSERVICE_H

#include <QObject>
#include "../models/citymodel.h"

/**
 * @class CityService
 * @brief 城市数据服务类
 * 
 * 负责城市数据的数据库CRUD操作
 */
class CityService : public QObject
{
    Q_OBJECT

public:
    static CityService& instance();
    
    // 城市CRUD操作
    bool addCity(const CityInfo &city);
    bool updateCity(const CityInfo &city);
    bool deleteCity(const QString &cityId);
    bool cityExists(const QString &cityId);
    CityInfo getCity(const QString &cityId);
    QList<CityInfo> getAllCities();
    QList<CityInfo> getFavoriteCities();
    
    // 收藏操作
    bool setFavorite(const QString &cityId, bool favorite);
    bool updateFavoriteOrder(const QString &cityId, int order);
    bool swapFavoriteOrder(const QString &cityId1, const QString &cityId2);
    int getFavoriteCount();
    
    // 批量操作
    bool addCities(const QList<CityInfo> &cities);
    bool clearAllCities();
    
    // 搜索
    QList<CityInfo> searchCities(const QString &keyword, int limit = 50);

signals:
    void cityAdded(const CityInfo &city);
    void cityUpdated(const CityInfo &city);
    void cityDeleted(const QString &cityId);
    void favoriteChanged(const QString &cityId, bool isFavorite);
    void errorOccurred(const QString &error);

private:
    explicit CityService(QObject *parent = nullptr);
    ~CityService() = default;
    
    CityService(const CityService&) = delete;
    CityService& operator=(const CityService&) = delete;
};

#endif // CITYSERVICE_H
