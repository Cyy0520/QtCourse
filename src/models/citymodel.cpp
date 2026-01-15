/**
 * @file citymodel.cpp
 * @brief 城市数据模型类实现
 */

#include "citymodel.h"
#include "../database/databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

CityModel::CityModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int CityModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_cities.count();
}

int CityModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return ColCount;
}

QVariant CityModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_cities.count())
        return QVariant();
    
    const CityInfo &city = m_cities.at(index.row());
    
    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (index.column()) {
            case ColId: return city.id;
            case ColCityId: return city.cityId;
            case ColName: return city.name;
            case ColProvince: return city.province;
            case ColCountry: return city.country;
            case ColLatitude: return city.latitude;
            case ColLongitude: return city.longitude;
            case ColIsFavorite: return city.isFavorite;
            case ColFavoriteOrder: return city.favoriteOrder;
        }
    } else if (role == CityIdRole) {
        return city.cityId;
    } else if (role == CityInfoRole) {
        return QVariant::fromValue(city);
    }
    
    return QVariant();
}

QVariant CityModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();
    
    switch (section) {
        case ColId: return tr("ID");
        case ColCityId: return tr("城市ID");
        case ColName: return tr("城市名称");
        case ColProvince: return tr("省份");
        case ColCountry: return tr("国家");
        case ColLatitude: return tr("纬度");
        case ColLongitude: return tr("经度");
        case ColIsFavorite: return tr("收藏");
        case ColFavoriteOrder: return tr("排序");
    }
    return QVariant();
}

Qt::ItemFlags CityModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void CityModel::setCities(const QList<CityInfo> &cities)
{
    beginResetModel();
    m_cities = cities;
    endResetModel();
}

void CityModel::addCity(const CityInfo &city)
{
    beginInsertRows(QModelIndex(), m_cities.count(), m_cities.count());
    m_cities.append(city);
    endInsertRows();
    emit cityAdded(city);
}

void CityModel::updateCity(int row, const CityInfo &city)
{
    if (row < 0 || row >= m_cities.count()) return;
    
    m_cities[row] = city;
    emit dataChanged(index(row, 0), index(row, ColCount - 1));
}

void CityModel::removeCity(int row)
{
    if (row < 0 || row >= m_cities.count()) return;
    
    QString cityId = m_cities.at(row).cityId;
    beginRemoveRows(QModelIndex(), row, row);
    m_cities.removeAt(row);
    endRemoveRows();
    emit cityRemoved(cityId);
}

void CityModel::clear()
{
    beginResetModel();
    m_cities.clear();
    endResetModel();
}

CityInfo CityModel::cityAt(int row) const
{
    if (row >= 0 && row < m_cities.count())
        return m_cities.at(row);
    return CityInfo();
}

CityInfo CityModel::cityById(const QString &cityId) const
{
    for (const CityInfo &city : m_cities) {
        if (city.cityId == cityId)
            return city;
    }
    return CityInfo();
}

int CityModel::findRowByCityId(const QString &cityId) const
{
    for (int i = 0; i < m_cities.count(); ++i) {
        if (m_cities.at(i).cityId == cityId)
            return i;
    }
    return -1;
}

QList<CityInfo> CityModel::allCities() const
{
    return m_cities;
}

QList<CityInfo> CityModel::favoriteCities() const
{
    QList<CityInfo> favorites;
    for (const CityInfo &city : m_cities) {
        if (city.isFavorite)
            favorites.append(city);
    }
    // 按收藏顺序排序
    std::sort(favorites.begin(), favorites.end(), [](const CityInfo &a, const CityInfo &b) {
        return a.favoriteOrder < b.favoriteOrder;
    });
    return favorites;
}

void CityModel::setFavorite(int row, bool favorite)
{
    if (row < 0 || row >= m_cities.count()) return;
    
    m_cities[row].isFavorite = favorite;
    if (favorite) {
        // 设置收藏顺序为当前最大值+1
        int maxOrder = 0;
        for (const CityInfo &city : m_cities) {
            if (city.isFavorite && city.favoriteOrder > maxOrder)
                maxOrder = city.favoriteOrder;
        }
        m_cities[row].favoriteOrder = maxOrder + 1;
    }
    
    emit dataChanged(index(row, ColIsFavorite), index(row, ColFavoriteOrder));
    emit favoriteChanged(m_cities.at(row).cityId, favorite);
}

void CityModel::setFavorite(const QString &cityId, bool favorite)
{
    int row = findRowByCityId(cityId);
    if (row >= 0)
        setFavorite(row, favorite);
}

void CityModel::moveFavorite(int fromRow, int toRow)
{
    if (fromRow < 0 || fromRow >= m_cities.count()) return;
    if (toRow < 0 || toRow >= m_cities.count()) return;
    if (fromRow == toRow) return;
    
    // 交换收藏顺序
    int tempOrder = m_cities[fromRow].favoriteOrder;
    m_cities[fromRow].favoriteOrder = m_cities[toRow].favoriteOrder;
    m_cities[toRow].favoriteOrder = tempOrder;
    
    emit dataChanged(index(fromRow, ColFavoriteOrder), index(fromRow, ColFavoriteOrder));
    emit dataChanged(index(toRow, ColFavoriteOrder), index(toRow, ColFavoriteOrder));
}

void CityModel::loadFromDatabase()
{
    if (!DatabaseManager::instance().isConnected()) {
        qWarning() << "Database not connected";
        return;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT id, city_id, name, province, country, latitude, longitude, "
                  "is_favorite, favorite_order FROM city ORDER BY name");
    
    if (!query.exec()) {
        qWarning() << "Failed to load cities:" << query.lastError().text();
        return;
    }
    
    QList<CityInfo> cities;
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
    
    setCities(cities);
    qDebug() << "Loaded" << cities.count() << "cities from database";
}

void CityModel::loadFavoritesFromDatabase()
{
    if (!DatabaseManager::instance().isConnected()) {
        qWarning() << "Database not connected";
        return;
    }
    
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT id, city_id, name, province, country, latitude, longitude, "
                  "is_favorite, favorite_order FROM city WHERE is_favorite = 1 "
                  "ORDER BY favorite_order");
    
    if (!query.exec()) {
        qWarning() << "Failed to load favorite cities:" << query.lastError().text();
        return;
    }
    
    QList<CityInfo> cities;
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
    
    setCities(cities);
    qDebug() << "Loaded" << cities.count() << "favorite cities from database";
}
