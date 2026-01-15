/**
 * @file historywidget.h
 * @brief 历史记录页面组件
 */

#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QWidget>
#include <QList>
#include "../models/weatherdata.h"

namespace Ui {
class HistoryWidget;
}

/**
 * @class HistoryWidget
 * @brief 显示天气历史记录的页面
 */
class HistoryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryWidget(QWidget *parent = nullptr);
    ~HistoryWidget();
    
    /**
     * @brief 设置当前城市
     */
    void setCity(const QString &cityId, const QString &cityName);
    
    /**
     * @brief 添加历史记录
     */
    void addHistoryRecord(const CurrentWeather &weather);

signals:
    /**
     * @brief 请求导出数据
     */
    void exportRequested(const QString &filePath);

private slots:
    void onQueryClicked();
    void onExportClicked();
    void onClearClicked();

private:
    void initTable();
    void loadMockData();
    void updateRecordCount();

private:
    Ui::HistoryWidget *ui;
    QString m_cityId;
    QString m_cityName;
    QList<CurrentWeather> m_historyData;
};

#endif // HISTORYWIDGET_H
