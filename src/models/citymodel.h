/**
 * @file citymodel.h
 * @brief 城市数据模型类声明
 */

#ifndef CITYMODEL_H
#define CITYMODEL_H

#include <QAbstractTableModel>
#include <QList>

/**
 * @struct CityInfo
 * @brief 城市信息结构体
 */
struct CityInfo {
    int id = 0;
    QString cityId;
    QString name;
    QString province;
    QString country = "CN";
    double latitude = 0.0;
    double longitude = 0.0;
    bool isFavorite = false;
    int favoriteOrder = 0;
};

/**
 * @class CityModel
 * @brief 城市数据模型
 * 
 * 基于QAbstractTableModel实现，用于城市列表的显示和管理
 */
class CityModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        ColId = 0,
        ColCityId,
        ColName,
        ColProvince,
        ColCountry,
        ColLatitude,
        ColLongitude,
        ColIsFavorite,
        ColFavoriteOrder,
        ColCount
    };

    enum Role {
        CityIdRole = Qt::UserRole + 1,
        CityInfoRole
    };

    explicit CityModel(QObject *parent = nullptr);
    
    // QAbstractTableModel 接口实现
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    // 数据操作
    void setCities(const QList<CityInfo> &cities);
    void addCity(const CityInfo &city);
    void updateCity(int row, const CityInfo &city);
    void removeCity(int row);
    void clear();
    
    // 数据获取
    CityInfo cityAt(int row) const;
    CityInfo cityById(const QString &cityId) const;
    int findRowByCityId(const QString &cityId) const;
    QList<CityInfo> allCities() const;
    QList<CityInfo> favoriteCities() const;
    
    // 收藏操作
    void setFavorite(int row, bool favorite);
    void setFavorite(const QString &cityId, bool favorite);
    void moveFavorite(int fromRow, int toRow);
    
    // 从数据库加载
    void loadFromDatabase();
    void loadFavoritesFromDatabase();

signals:
    void cityAdded(const CityInfo &city);
    void cityRemoved(const QString &cityId);
    void favoriteChanged(const QString &cityId, bool isFavorite);

private:
    QList<CityInfo> m_cities;
};

#endif // CITYMODEL_H
