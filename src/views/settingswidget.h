/**
 * @file settingswidget.h
 * @brief 设置页面组件
 */

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>

namespace Ui {
class SettingsWidget;
}

/**
 * @class SettingsWidget
 * @brief 应用设置界面
 * 
 * 提供单位设置、主题切换等功能
 */
class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

signals:
    void settingsChanged();
    void themeChanged(int themeMode);

private slots:
    void onSaveClicked();
    void onResetClicked();

private:
    void setupConnections();
    void loadSettings();
    void saveSettings();

private:
    Ui::SettingsWidget *ui;
};

#endif // SETTINGSWIDGET_H
