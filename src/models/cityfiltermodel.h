/**
 * @file cityfiltermodel.h
 * @brief 城市搜索过滤模型类声明
 */

#ifndef CITYFILTERMODEL_H
#define CITYFILTERMODEL_H

#include <QSortFilterProxyModel>

/**
 * @class CityFilterModel
 * @brief 城市搜索过滤代理模型
 * 
 * 支持按城市名称、拼音、省份进行模糊搜索
 */
class CityFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit CityFilterModel(QObject *parent = nullptr);
    
    /**
     * @brief 设置搜索关键词
     * @param keyword 搜索关键词
     */
    void setSearchKeyword(const QString &keyword);
    
    /**
     * @brief 获取当前搜索关键词
     * @return 搜索关键词
     */
    QString searchKeyword() const;
    
    /**
     * @brief 设置是否只显示收藏城市
     * @param favoritesOnly 是否只显示收藏
     */
    void setFavoritesOnly(bool favoritesOnly);
    
    /**
     * @brief 是否只显示收藏城市
     * @return 是否只显示收藏
     */
    bool favoritesOnly() const;

protected:
    /**
     * @brief 过滤行
     * @param sourceRow 源行号
     * @param sourceParent 源父索引
     * @return 是否显示该行
     */
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    
    /**
     * @brief 比较排序
     * @param left 左索引
     * @param right 右索引
     * @return 比较结果
     */
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    /**
     * @brief 检查字符串是否匹配关键词
     * @param text 待检查文本
     * @param keyword 关键词
     * @return 是否匹配
     */
    bool matchesKeyword(const QString &text, const QString &keyword) const;
    
    /**
     * @brief 将中文转换为拼音首字母（简化版）
     * @param chinese 中文字符串
     * @return 拼音首字母
     */
    QString toPinyinInitials(const QString &chinese) const;

private:
    QString m_searchKeyword;
    bool m_favoritesOnly;
};

#endif // CITYFILTERMODEL_H
