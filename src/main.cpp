/**
 * @file main.cpp
 * @brief 天气数据展示与趋势分析系统 - 程序入口
 * @author Your Name
 * @date 2026-01-15
 */

#include "mainwindow.h"
#include "config/configmanager.h"
#include "services/weatherservice.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 设置应用程序信息
    QApplication::setApplicationName("WeatherAnalysis");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("YourOrganization");
    
    // 加载保存的 API Key
    QString apiKey = ConfigManager::instance().value("api/qweatherKey", "").toString();
    if (!apiKey.isEmpty()) {
        WeatherService::instance().setApiKey(apiKey);
    }
    
    MainWindow w;
    w.show();
    
    return a.exec();
}
