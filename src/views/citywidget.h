/**
 * @file citywidget.h
 * @brief 城市管理页面类声明
 */

#ifndef CITYWIDGET_H
#define CITYWIDGET_H

#include <QWidget>
#include <QListView>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../models/citymodel.h"
#include "../models/cityfiltermodel.h"

/**
 * @class CityWidget
 * @brief 城市管理页面
 * 
 * 包含城市搜索、收藏列表、城市添加等功能
 */
class CityWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CityWidget(QWidget *parent = nullptr);
    ~CityWidget();

signals:
    void citySelected(const QString &cityId);

private slots:
    void onSearchTextChanged(const QString &text);
    void onCityClicked(const QModelIndex &index);
    void onCityDoubleClicked(const QModelIndex &index);
    void onAddCityClicked();
    void onRemoveCityClicked();
    void onRefreshClicked();

private:
    void setupUI();
    void setupConnections();
    void loadCities();
    void addDefaultCities();

private:
    // UI组件
    QLineEdit *m_searchEdit;
    QListView *m_cityListView;
    QPushButton *m_addBtn;
    QPushButton *m_removeBtn;
    QPushButton *m_refreshBtn;
    QLabel *m_statusLabel;
    
    // 数据模型
    CityModel *m_cityModel;
    CityFilterModel *m_filterModel;
};

#endif // CITYWIDGET_H
