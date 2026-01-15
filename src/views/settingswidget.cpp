/**
 * @file settingswidget.cpp
 * @brief 设置页面组件实现
 */

#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "../config/configmanager.h"
#include <QMessageBox>

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
    setupConnections();
    loadSettings();
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::setupConnections()
{
    connect(ui->saveBtn, &QPushButton::clicked,
            this, &SettingsWidget::onSaveClicked);
    connect(ui->resetBtn, &QPushButton::clicked,
            this, &SettingsWidget::onResetClicked);
}

void SettingsWidget::loadSettings()
{
    ConfigManager &config = ConfigManager::instance();
    
    // 温度单位
    ui->tempUnitCombo->setCurrentIndex(
        config.temperatureUnit() == TemperatureUnit::Celsius ? 0 : 1
    );
    
    // 风速单位
    switch (config.windSpeedUnit()) {
        case WindSpeedUnit::KmPerHour:
            ui->windUnitCombo->setCurrentIndex(0);
            break;
        case WindSpeedUnit::MPerSecond:
            ui->windUnitCombo->setCurrentIndex(1);
            break;
        case WindSpeedUnit::MilesPerHour:
            ui->windUnitCombo->setCurrentIndex(2);
            break;
    }
    
    // 气压单位
    ui->pressureUnitCombo->setCurrentIndex(
        config.pressureUnit() == PressureUnit::HPa ? 0 : 1
    );
    
    // 主题
    switch (config.themeMode()) {
        case ThemeMode::Light:
            ui->lightThemeRadio->setChecked(true);
            break;
        case ThemeMode::Dark:
            ui->darkThemeRadio->setChecked(true);
            break;
    }
}

void SettingsWidget::saveSettings()
{
    ConfigManager &config = ConfigManager::instance();
    
    // 温度单位
    config.setTemperatureUnit(
        ui->tempUnitCombo->currentIndex() == 0 
            ? TemperatureUnit::Celsius 
            : TemperatureUnit::Fahrenheit
    );
    
    // 风速单位
    switch (ui->windUnitCombo->currentIndex()) {
        case 0:
            config.setWindSpeedUnit(WindSpeedUnit::KmPerHour);
            break;
        case 1:
            config.setWindSpeedUnit(WindSpeedUnit::MPerSecond);
            break;
        case 2:
            config.setWindSpeedUnit(WindSpeedUnit::MilesPerHour);
            break;
    }
    
    // 气压单位
    config.setPressureUnit(
        ui->pressureUnitCombo->currentIndex() == 0 
            ? PressureUnit::HPa 
            : PressureUnit::MmHg
    );
    
    // 主题
    ThemeMode theme = ThemeMode::Light;
    if (ui->darkThemeRadio->isChecked()) {
        theme = ThemeMode::Dark;
    }
    config.setThemeMode(theme);
    
    emit settingsChanged();
    emit themeChanged(static_cast<int>(theme));
}

void SettingsWidget::onSaveClicked()
{
    saveSettings();
    QMessageBox::information(this, tr("设置"), tr("设置已保存"));
}

void SettingsWidget::onResetClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("确认"), tr("确定要恢复默认设置吗？"),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        ConfigManager &config = ConfigManager::instance();
        config.setTemperatureUnit(TemperatureUnit::Celsius);
        config.setWindSpeedUnit(WindSpeedUnit::KmPerHour);
        config.setPressureUnit(PressureUnit::HPa);
        config.setThemeMode(ThemeMode::Light);
        
        loadSettings();
        emit settingsChanged();
        emit themeChanged(static_cast<int>(ThemeMode::Light));
        
        QMessageBox::information(this, tr("设置"), tr("已恢复默认设置"));
    }
}
