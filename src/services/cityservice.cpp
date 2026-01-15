/**
 * @file cityservice.cpp
 * @brief 城市服务类实现
 */

#include "cityservice.h"
#include "../database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

CityService::CityService(QObject *parent)
    : QObject(parent)
{
}

CityService& CityService::instance()
{
    static CityService instance;
    return instance;
}

bool CityService::addCity(const CityInfo &city)
{
    if (!DatabaseManager::instance().isConnected()) {
        emit errorOccurred(tr("数据库未连接"));
        return false;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "INSERT INTO city (city_id, name, province, country, latitude, longitude, "
        "is_favorite, favorite_order, create_time, update_time) "
        "VALUES (:city_id, :name, :province, :country, :latitude, :longitude, "
        ":is_favorite, :favorite_order, :create_time, :update_time)"
    );
    
    query.bindValue(":city_id", city.cityId);
    query.bindValue(":name", city.name);
    query.bindValue(":province", city.province);
    query.bindValue(":country", city.country);
    query.bindValue(":latitude", city.latitude);
    query.bindValue(":longitude", city.longitude);
    query.bindValue(":is_favorite", city.isFavorite ? 1 : 0);
    query.bindValue(":favorite_order", city.favoriteOrder);
    query.bindValue(":create_time", QDateTime::currentDateTime().toString(Qt::ISODate));
    query.bindValue(":update_time", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    if (!query.exec()) {
        QString error = query.lastError().text();
        qWarning() << "Failed to add city:" << error;
        emit errorOccurred(error);
        return false;
    }
    
    emit cityAdded(city);
    return true;
}

bool CityService::updateCity(const CityInfo &city)
{
    if (!DatabaseManager::instance().isConnected()) {
        emit errorOccurred(tr("数据库未连接"));
        return false;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "UPDATE city SET name = :name, province = :province, country = :country, "
        "latitude = :latitude, longitude = :longitude, is_favorite = :is_favorite, "
        "favorite_order = :favorite_order, update_time = :update_time "
        "WHERE city_id = :city_id"
    );
    
    query.bindValue(":city_id", city.cityId);
    query.bindValue(":name", city.name);
    query.bindValue(":province", city.province);
    query.bindValue(":country", city.country);
    query.bindValue(":latitude", city.latitude);
    query.bindValue(":longitude", city.longitude);
    query.bindValue(":is_favorite", city.isFavorite ? 1 : 0);
    query.bindValue(":favorite_order", city.favoriteOrder);
    query.bindValue(":update_time", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    if (!query.exec()) {
        QString error = query.lastError().text();
        qWarning() << "Failed to update city:" << error;
        emit errorOccurred(error);
        return false;
    }
    
    emit cityUpdated(city);
    return true;
}

bool CityService::deleteCity(const QString &cityId)
{
    if (!DatabaseManager::instance().isConnected()) {
        emit errorOccurred(tr("数据库未连接"));
        return false;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM city WHERE city_id = :city_id");
    query.bindValue(":city_id", cityId);
    
    if (!query.exec()) {
        QString error = query.lastError().text();
        qWarning() << "Failed to delete city:" << error;
        emit errorOccurred(error);
        return false;
    }
    
    emit cityDeleted(cityId);
    return true;
}

bool CityService::cityExists(const QString &cityId)
{
    if (!DatabaseManager::instance().isConnected()) {
        return false;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT COUNT(*) FROM city WHERE city_id = :city_id");
    query.bindValue(":city_id", cityId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

CityInfo CityService::getCity(const QString &cityId)
{
    CityInfo city;
    if (!DatabaseManager::instance().isConnected()) {
        return city;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT id, city_id, name, province, country, latitude, longitude, "
        "is_favorite, favorite_order FROM city WHERE city_id = :city_id"
    );
    query.bindValue(":city_id", cityId);
    
    if (query.exec() && query.next()) {
        city.id = query.value(0).toInt();
        city.cityId = query.value(1).toString();
        city.name = query.value(2).toString();
        city.province = query.value(3).toString();
        city.country = query.value(4).toString();
        city.latitude = query.value(5).toDouble();
        city.longitude = query.value(6).toDouble();
        city.isFavorite = query.value(7).toBool();
        city.favoriteOrder = query.value(8).toInt();
    }
    return city;
}

QList<CityInfo> CityService::getAllCities()
{
    QList<CityInfo> cities;
    if (!DatabaseManager::instance().isConnected()) {
        return cities;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT id, city_id, name, province, country, latitude, longitude, "
        "is_favorite, favorite_order FROM city ORDER BY name"
    );
    
    if (query.exec()) {
        while (query.next()) {
            CityInfo city;
            city.id = query.value(0).toInt();
            city.cityId = query.value(1).toString();
            city.name = query.value(2).toString();
            city.province = query.value(3).toString();
            city.country = query.value(4).toString();
            city.latitude = query.value(5).toDouble();
            city.longitude = query.value(6).toDouble();
            city.isFavorite = query.value(7).toBool();
            city.favoriteOrder = query.value(8).toInt();
            cities.append(city);
        }
    }
    return cities;
}

QList<CityInfo> CityService::getFavoriteCities()
{
    QList<CityInfo> cities;
    if (!DatabaseManager::instance().isConnected()) {
        return cities;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT id, city_id, name, province, country, latitude, longitude, "
        "is_favorite, favorite_order FROM city WHERE is_favorite = 1 "
        "ORDER BY favorite_order"
    );
    
    if (query.exec()) {
        while (query.next()) {
            CityInfo city;
            city.id = query.value(0).toInt();
            city.cityId = query.value(1).toString();
            city.name = query.value(2).toString();
            city.province = query.value(3).toString();
            city.country = query.value(4).toString();
            city.latitude = query.value(5).toDouble();
            city.longitude = query.value(6).toDouble();
            city.isFavorite = query.value(7).toBool();
            city.favoriteOrder = query.value(8).toInt();
            cities.append(city);
        }
    }
    return cities;
}

bool CityService::setFavorite(const QString &cityId, bool favorite)
{
    if (!DatabaseManager::instance().isConnected()) {
        emit errorOccurred(tr("数据库未连接"));
        return false;
    }
    
    int favoriteOrder = 0;
    if (favorite) {
        // 获取当前最大收藏顺序
        QSqlQuery maxQuery(DatabaseManager::instance().database());
        maxQuery.exec("SELECT MAX(favorite_order) FROM city WHERE is_favorite = 1");
        if (maxQuery.next()) {
            favoriteOrder = maxQuery.value(0).toInt() + 1;
        }
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "UPDATE city SET is_favorite = :is_favorite, favorite_order = :favorite_order, "
        "update_time = :update_time WHERE city_id = :city_id"
    );
    query.bindValue(":city_id", cityId);
    query.bindValue(":is_favorite", favorite ? 1 : 0);
    query.bindValue(":favorite_order", favoriteOrder);
    query.bindValue(":update_time", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    if (!query.exec()) {
        QString error = query.lastError().text();
        qWarning() << "Failed to set favorite:" << error;
        emit errorOccurred(error);
        return false;
    }
    
    emit favoriteChanged(cityId, favorite);
    return true;
}

bool CityService::updateFavoriteOrder(const QString &cityId, int order)
{
    if (!DatabaseManager::instance().isConnected()) {
        return false;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE city SET favorite_order = :order WHERE city_id = :city_id");
    query.bindValue(":city_id", cityId);
    query.bindValue(":order", order);
    
    return query.exec();
}

bool CityService::swapFavoriteOrder(const QString &cityId1, const QString &cityId2)
{
    CityInfo city1 = getCity(cityId1);
    CityInfo city2 = getCity(cityId2);
    
    if (city1.cityId.isEmpty() || city2.cityId.isEmpty()) {
        return false;
    }
    
    int tempOrder = city1.favoriteOrder;
    updateFavoriteOrder(cityId1, city2.favoriteOrder);
    updateFavoriteOrder(cityId2, tempOrder);
    
    return true;
}

int CityService::getFavoriteCount()
{
    if (!DatabaseManager::instance().isConnected()) {
        return 0;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.exec("SELECT COUNT(*) FROM city WHERE is_favorite = 1");
    
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

bool CityService::addCities(const QList<CityInfo> &cities)
{
    if (!DatabaseManager::instance().isConnected()) {
        return false;
    }
    
    QSqlDatabase db = DatabaseManager::instance().database();
    db.transaction();
    
    for (const CityInfo &city : cities) {
        if (!addCity(city)) {
            db.rollback();
            return false;
        }
    }
    
    return db.commit();
}

bool CityService::clearAllCities()
{
    if (!DatabaseManager::instance().isConnected()) {
        return false;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    return query.exec("DELETE FROM city");
}

QList<CityInfo> CityService::searchCities(const QString &keyword, int limit)
{
    QList<CityInfo> cities;
    if (!DatabaseManager::instance().isConnected() || keyword.isEmpty()) {
        return cities;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(
        "SELECT id, city_id, name, province, country, latitude, longitude, "
        "is_favorite, favorite_order FROM city "
        "WHERE name LIKE :keyword OR province LIKE :keyword "
        "ORDER BY is_favorite DESC, name LIMIT :limit"
    );
    query.bindValue(":keyword", "%" + keyword + "%");
    query.bindValue(":limit", limit);
    
    if (query.exec()) {
        while (query.next()) {
            CityInfo city;
            city.id = query.value(0).toInt();
            city.cityId = query.value(1).toString();
            city.name = query.value(2).toString();
            city.province = query.value(3).toString();
            city.country = query.value(4).toString();
            city.latitude = query.value(5).toDouble();
            city.longitude = query.value(6).toDouble();
            city.isFavorite = query.value(7).toBool();
            city.favoriteOrder = query.value(8).toInt();
            cities.append(city);
        }
    }
    return cities;
}
