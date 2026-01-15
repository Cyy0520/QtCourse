/**
 * @file lifeindexwidget.h
 * @brief 生活指数展示组件
 */

#ifndef LIFEINDEXWIDGET_H
#define LIFEINDEXWIDGET_H

#include <QWidget>
#include <QFrame>
#include "../models/weatherdata.h"

namespace Ui {
class LifeIndexWidget;
}

/**
 * @class LifeIndexWidget
 * @brief 生活指数展示界面
 * 
 * 显示穿衣、运动、洗车、紫外线等生活指数
 */
class LifeIndexWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LifeIndexWidget(QWidget *parent = nullptr);
    ~LifeIndexWidget();
    
    void setCity(const QString &cityId, const QString &cityName);
    void updateLifeIndex(const QList<LifeIndex> &indices);
    void clear();

signals:
    void refreshRequested(const QString &cityId);

private slots:
    void onRefreshClicked();

private:
    void setupConnections();
    void clearIndexCards();
    QFrame* createIndexCard(const LifeIndex &index);
    QString getIndexIcon(const QString &type);
    QString getLevelColor(const QString &level);

private:
    Ui::LifeIndexWidget *ui;
    QString m_currentCityId;
    QString m_currentCityName;
    QList<QFrame*> m_indexCards;
};

#endif // LIFEINDEXWIDGET_H
