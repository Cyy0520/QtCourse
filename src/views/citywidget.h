/**
 * @file citywidget.h
 * @brief 城市管理页面类声明
 */

#ifndef CITYWIDGET_H
#define CITYWIDGET_H

#include <QWidget>
#include "../models/citymodel.h"
#include "../models/cityfiltermodel.h"

namespace Ui {
class CityWidget;
}

/**
 * @class CityWidget
 * @brief 城市管理页面
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
    void onFavoriteClicked();
    void onRefreshClicked();

private:
    void setupConnections();
    void loadCities();
    void addDefaultCities();

private:
    Ui::CityWidget *ui;
    CityModel *m_cityModel;
    CityFilterModel *m_filterModel;
};

#endif // CITYWIDGET_H
