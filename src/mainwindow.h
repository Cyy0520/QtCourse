/**
 * @file mainwindow.h
 * @brief 主窗口类声明
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "database/databasemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CityWidget;
class CurrentWeatherWidget;

/**
 * @class MainWindow
 * @brief 应用程序主窗口
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    /**
     * @brief 初始化数据库
     * @return 初始化是否成功
     */
    bool initDatabase();
    
    /**
     * @brief 更新状态栏
     */
    void updateStatusBar();
    
    /**
     * @brief 初始化页面
     */
    void setupPages();

private:
    Ui::MainWindow *ui;
    CityWidget *m_cityWidget;
    CurrentWeatherWidget *m_currentWeatherWidget;
    QString m_currentCityId;
    QString m_currentCityName;
};

#endif // MAINWINDOW_H
