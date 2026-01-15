/**
 * @file historywidget.cpp
 * @brief 历史记录页面组件实现
 */

#include "historywidget.h"
#include "ui_historywidget.h"
#include "../utils/dataexporter.h"
#include <QDate>
#include <QFileDialog>
#include <QMessageBox>
#include <QRandomGenerator>

HistoryWidget::HistoryWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HistoryWidget)
{
    ui->setupUi(this);
    
    initTable();
    
    // 设置默认日期范围（最近7天）
    ui->endDateEdit->setDate(QDate::currentDate());
    ui->startDateEdit->setDate(QDate::currentDate().addDays(-7));
    
    // 连接信号
    connect(ui->queryBtn, &QPushButton::clicked, this, &HistoryWidget::onQueryClicked);
    connect(ui->exportBtn, &QPushButton::clicked, this, &HistoryWidget::onExportClicked);
    connect(ui->clearBtn, &QPushButton::clicked, this, &HistoryWidget::onClearClicked);
}

HistoryWidget::~HistoryWidget()
{
    delete ui;
}

void HistoryWidget::setCity(const QString &cityId, const QString &cityName)
{
    m_cityId = cityId;
    m_cityName = cityName;
    ui->cityLabel->setText(QString("当前城市：%1").arg(cityName));
    
    // 加载模拟数据
    loadMockData();
}

void HistoryWidget::addHistoryRecord(const CurrentWeather &weather)
{
    m_historyData.prepend(weather);
    
    // 添加到表格
    int row = 0;
    ui->historyTable->insertRow(row);
    ui->historyTable->setItem(row, 0, new QTableWidgetItem(weather.updateTime.toString("yyyy-MM-dd HH:mm")));
    ui->historyTable->setItem(row, 1, new QTableWidgetItem("实时天气"));
    ui->historyTable->setItem(row, 2, new QTableWidgetItem(QString("%1°C").arg(weather.temperature)));
    ui->historyTable->setItem(row, 3, new QTableWidgetItem(weather.weatherDesc));
    ui->historyTable->setItem(row, 4, new QTableWidgetItem(QString("%1%").arg(weather.humidity)));
    ui->historyTable->setItem(row, 5, new QTableWidgetItem(QString("%1 km/h").arg(weather.windSpeed)));
    ui->historyTable->setItem(row, 6, new QTableWidgetItem(QString("%1 hPa").arg(weather.pressure)));
    
    updateRecordCount();
}

void HistoryWidget::initTable()
{
    ui->historyTable->setColumnCount(7);
    ui->historyTable->setHorizontalHeaderLabels({
        "日期", "类型", "温度", "天气", "湿度", "风速", "气压"
    });
    
    // 设置列宽
    ui->historyTable->setColumnWidth(0, 150);
    ui->historyTable->setColumnWidth(1, 80);
    ui->historyTable->setColumnWidth(2, 80);
    ui->historyTable->setColumnWidth(3, 100);
    ui->historyTable->setColumnWidth(4, 80);
    ui->historyTable->setColumnWidth(5, 100);
    ui->historyTable->setColumnWidth(6, 100);
    
    ui->historyTable->horizontalHeader()->setStretchLastSection(true);
}

void HistoryWidget::loadMockData()
{
    ui->historyTable->setRowCount(0);
    m_historyData.clear();
    
    QStringList weatherTypes = {"晴", "多云", "阴", "小雨", "中雨"};
    
    // 生成最近7天的模拟数据
    for (int i = 0; i < 7; ++i) {
        CurrentWeather weather;
        weather.cityId = m_cityId;
        weather.cityName = m_cityName;
        weather.updateTime = QDateTime::currentDateTime().addDays(-i);
        weather.temperature = QRandomGenerator::global()->bounded(15, 30);
        weather.humidity = QRandomGenerator::global()->bounded(40, 90);
        weather.windSpeed = QRandomGenerator::global()->bounded(5, 25);
        weather.pressure = QRandomGenerator::global()->bounded(1000, 1030);
        weather.weatherDesc = weatherTypes[QRandomGenerator::global()->bounded(weatherTypes.size())];
        
        m_historyData.append(weather);
        
        int row = ui->historyTable->rowCount();
        ui->historyTable->insertRow(row);
        ui->historyTable->setItem(row, 0, new QTableWidgetItem(weather.updateTime.toString("yyyy-MM-dd HH:mm")));
        ui->historyTable->setItem(row, 1, new QTableWidgetItem("实时天气"));
        ui->historyTable->setItem(row, 2, new QTableWidgetItem(QString("%1°C").arg(weather.temperature)));
        ui->historyTable->setItem(row, 3, new QTableWidgetItem(weather.weatherDesc));
        ui->historyTable->setItem(row, 4, new QTableWidgetItem(QString("%1%").arg(weather.humidity)));
        ui->historyTable->setItem(row, 5, new QTableWidgetItem(QString("%1 km/h").arg(weather.windSpeed)));
        ui->historyTable->setItem(row, 6, new QTableWidgetItem(QString("%1 hPa").arg(weather.pressure)));
    }
    
    updateRecordCount();
}

void HistoryWidget::onQueryClicked()
{
    // 重新加载数据（模拟查询）
    loadMockData();
    QMessageBox::information(this, "查询完成", 
        QString("已查询 %1 到 %2 的历史数据")
            .arg(ui->startDateEdit->date().toString("yyyy-MM-dd"))
            .arg(ui->endDateEdit->date().toString("yyyy-MM-dd")));
}

void HistoryWidget::onExportClicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, 
        "导出历史数据", 
        QString("%1_history.json").arg(m_cityName),
        "JSON 文件 (*.json);;CSV 文件 (*.csv)");
    
    if (filePath.isEmpty()) return;
    
    bool success = false;
    if (filePath.endsWith(".json")) {
        // 导出为 JSON（使用第一条记录作为示例）
        if (!m_historyData.isEmpty()) {
            success = DataExporter::exportCurrentWeatherToJson(m_historyData.first(), filePath);
        }
    } else {
        // 导出为 CSV
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out.setEncoding(QStringConverter::Utf8);
            out << "日期,类型,温度,天气,湿度,风速,气压\n";
            for (const CurrentWeather &w : m_historyData) {
                out << w.updateTime.toString("yyyy-MM-dd HH:mm") << ","
                    << "实时天气" << ","
                    << w.temperature << ","
                    << w.weatherDesc << ","
                    << w.humidity << ","
                    << w.windSpeed << ","
                    << w.pressure << "\n";
            }
            file.close();
            success = true;
        }
    }
    
    if (success) {
        QMessageBox::information(this, "导出成功", "历史数据已成功导出！");
    } else {
        QMessageBox::warning(this, "导出失败", "无法导出数据，请检查文件路径。");
    }
}

void HistoryWidget::onClearClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
        "确认清空", 
        "确定要清空所有历史记录吗？",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        ui->historyTable->setRowCount(0);
        m_historyData.clear();
        updateRecordCount();
    }
}

void HistoryWidget::updateRecordCount()
{
    ui->countLabel->setText(QString("共 %1 条记录").arg(ui->historyTable->rowCount()));
}
