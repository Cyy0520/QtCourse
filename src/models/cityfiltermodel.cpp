/**
 * @file cityfiltermodel.cpp
 * @brief 城市搜索过滤模型类实现
 */

#include "cityfiltermodel.h"
#include "citymodel.h"

CityFilterModel::CityFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_favoritesOnly(false)
{
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

void CityFilterModel::setSearchKeyword(const QString &keyword)
{
    if (m_searchKeyword != keyword) {
        m_searchKeyword = keyword.trimmed();
        invalidateFilter();
    }
}

QString CityFilterModel::searchKeyword() const
{
    return m_searchKeyword;
}

void CityFilterModel::setFavoritesOnly(bool favoritesOnly)
{
    if (m_favoritesOnly != favoritesOnly) {
        m_favoritesOnly = favoritesOnly;
        invalidateFilter();
    }
}

bool CityFilterModel::favoritesOnly() const
{
    return m_favoritesOnly;
}

bool CityFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex nameIndex = sourceModel()->index(sourceRow, CityModel::ColName, sourceParent);
    QModelIndex provinceIndex = sourceModel()->index(sourceRow, CityModel::ColProvince, sourceParent);
    QModelIndex favoriteIndex = sourceModel()->index(sourceRow, CityModel::ColIsFavorite, sourceParent);
    
    // 检查收藏过滤
    if (m_favoritesOnly) {
        bool isFavorite = sourceModel()->data(favoriteIndex).toBool();
        if (!isFavorite) {
            return false;
        }
    }
    
    // 如果没有搜索关键词，显示所有（或所有收藏）
    if (m_searchKeyword.isEmpty()) {
        return true;
    }
    
    // 获取城市名和省份
    QString cityName = sourceModel()->data(nameIndex).toString();
    QString province = sourceModel()->data(provinceIndex).toString();
    
    // 检查是否匹配
    if (matchesKeyword(cityName, m_searchKeyword)) {
        return true;
    }
    if (matchesKeyword(province, m_searchKeyword)) {
        return true;
    }
    
    // 检查拼音首字母匹配
    QString pinyinInitials = toPinyinInitials(cityName);
    if (pinyinInitials.contains(m_searchKeyword, Qt::CaseInsensitive)) {
        return true;
    }
    
    return false;
}

bool CityFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    // 收藏的城市排在前面
    QModelIndex leftFav = sourceModel()->index(left.row(), CityModel::ColIsFavorite);
    QModelIndex rightFav = sourceModel()->index(right.row(), CityModel::ColIsFavorite);
    
    bool leftIsFavorite = sourceModel()->data(leftFav).toBool();
    bool rightIsFavorite = sourceModel()->data(rightFav).toBool();
    
    if (leftIsFavorite != rightIsFavorite) {
        return leftIsFavorite; // 收藏的排前面
    }
    
    // 如果都是收藏，按收藏顺序排序
    if (leftIsFavorite && rightIsFavorite) {
        QModelIndex leftOrder = sourceModel()->index(left.row(), CityModel::ColFavoriteOrder);
        QModelIndex rightOrder = sourceModel()->index(right.row(), CityModel::ColFavoriteOrder);
        return sourceModel()->data(leftOrder).toInt() < sourceModel()->data(rightOrder).toInt();
    }
    
    // 否则按名称排序
    return QSortFilterProxyModel::lessThan(left, right);
}

bool CityFilterModel::matchesKeyword(const QString &text, const QString &keyword) const
{
    return text.contains(keyword, Qt::CaseInsensitive);
}

QString CityFilterModel::toPinyinInitials(const QString &chinese) const
{
    // 简化版拼音首字母转换
    // 实际项目中可以使用完整的拼音库
    static const QMap<QChar, QChar> pinyinMap = {
        // 常用城市首字母映射
        {QChar(0x5317), 'B'}, // 北
        {QChar(0x4E0A), 'S'}, // 上
        {QChar(0x5E7F), 'G'}, // 广
        {QChar(0x6DF1), 'S'}, // 深
        {QChar(0x5929), 'T'}, // 天
        {QChar(0x91CD), 'C'}, // 重
        {QChar(0x6210), 'C'}, // 成
        {QChar(0x6B66), 'W'}, // 武
        {QChar(0x897F), 'X'}, // 西
        {QChar(0x5357), 'N'}, // 南
        {QChar(0x4E1C), 'D'}, // 东
        {QChar(0x957F), 'C'}, // 长
        {QChar(0x676D), 'H'}, // 杭
        {QChar(0x82CF), 'S'}, // 苏
        {QChar(0x65E0), 'W'}, // 无
        {QChar(0x5408), 'H'}, // 合
        {QChar(0x6D4E), 'J'}, // 济
        {QChar(0x9752), 'Q'}, // 青
        {QChar(0x5927), 'D'}, // 大
        {QChar(0x6C88), 'S'}, // 沈
        {QChar(0x54C8), 'H'}, // 哈
        {QChar(0x5170), 'L'}, // 兰
        {QChar(0x6606), 'K'}, // 昆
        {QChar(0x8D35), 'G'}, // 贵
        {QChar(0x592A), 'T'}, // 太
        {QChar(0x77F3), 'S'}, // 石
        {QChar(0x90D1), 'Z'}, // 郑
        {QChar(0x5357), 'N'}, // 南
        {QChar(0x798F), 'F'}, // 福
        {QChar(0x53A6), 'X'}, // 厦
        {QChar(0x6D77), 'H'}, // 海
        {QChar(0x4E91), 'Y'}, // 云
        {QChar(0x5B81), 'N'}, // 宁
        {QChar(0x6E29), 'W'}, // 温
        {QChar(0x73E0), 'Z'}, // 珠
        {QChar(0x4F5B), 'F'}, // 佛
        {QChar(0x4E2D), 'Z'}, // 中
        {QChar(0x5C71), 'S'}, // 山
        {QChar(0x6CB3), 'H'}, // 河
        {QChar(0x6E56), 'H'}, // 湖
        {QChar(0x6C5F), 'J'}, // 江
        {QChar(0x5DDE), 'Z'}, // 州
        {QChar(0x5E02), 'S'}, // 市
        {QChar(0x533A), 'Q'}, // 区
        {QChar(0x53BF), 'X'}, // 县
        {QChar(0x9547), 'Z'}, // 镇
        {QChar(0x4EAC), 'J'}, // 京
        {QChar(0x6D25), 'J'}, // 津
        {QChar(0x5E86), 'Q'}, // 庆
        {QChar(0x90FD), 'D'}, // 都
        {QChar(0x6C49), 'H'}, // 汉
        {QChar(0x5B89), 'A'}, // 安
        {QChar(0x5DDE), 'Z'}, // 州
    };
    
    QString result;
    for (const QChar &ch : chinese) {
        if (ch.isLetter() && ch.unicode() < 128) {
            // ASCII字母直接使用
            result += ch.toUpper();
        } else if (pinyinMap.contains(ch)) {
            result += pinyinMap.value(ch);
        }
    }
    return result;
}
