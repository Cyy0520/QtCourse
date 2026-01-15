/**
 * @file alertwidget.cpp
 * @brief 天气预警展示组件实现
 */

#include "alertwidget.h"
#include "ui_alertwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

AlertWidget::AlertWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AlertWidget)
{
    ui->setupUi(this);
}

AlertWidget::~AlertWidget()
{
    delete ui;
}

void AlertWidget::updateAlerts(const QList<WeatherAlert> &alerts)
{
    clearAlertCards();
    
    if (alerts.isEmpty()) {
        ui->noAlertLabel->setVisible(true);
        ui->alertScrollArea->setVisible(false);
        return;
    }
    
    ui->noAlertLabel->setVisible(false);
    ui->alertScrollArea->setVisible(true);
    
    for (const WeatherAlert &alert : alerts) {
        QFrame *card = createAlertCard(alert);
        ui->alertLayout->addWidget(card);
        m_alertCards.append(card);
    }
    
    ui->alertLayout->addStretch();
}

void AlertWidget::clear()
{
    clearAlertCards();
    ui->noAlertLabel->setVisible(true);
    ui->alertScrollArea->setVisible(false);
}

void AlertWidget::clearAlertCards()
{
    for (QFrame *card : m_alertCards) {
        ui->alertLayout->removeWidget(card);
        delete card;
    }
    m_alertCards.clear();
    
    QLayoutItem *child;
    while ((child = ui->alertLayout->takeAt(0)) != nullptr) {
        delete child;
    }
}

QFrame* AlertWidget::createAlertCard(const WeatherAlert &alert)
{
    QFrame *card = new QFrame();
    QString levelColor = getLevelColor(alert.level);
    card->setStyleSheet(QString(
        "QFrame { background-color: white; border-radius: 8px; "
        "border-left: 4px solid %1; border: 1px solid #e4e7ed; }"
    ).arg(levelColor));
    
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setSpacing(8);
    layout->setContentsMargins(16, 12, 16, 12);
    
    // 标题行
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QLabel *iconLabel = new QLabel(getLevelIcon(alert.level));
    iconLabel->setStyleSheet("font-size: 20px;");
    headerLayout->addWidget(iconLabel);
    
    QLabel *titleLabel = new QLabel(alert.title);
    titleLabel->setStyleSheet(QString(
        "font-size: 15px; font-weight: bold; color: %1;"
    ).arg(levelColor));
    titleLabel->setWordWrap(true);
    headerLayout->addWidget(titleLabel, 1);
    
    QLabel *levelLabel = new QLabel(alert.level);
    levelLabel->setStyleSheet(QString(
        "font-size: 12px; font-weight: bold; padding: 2px 8px; "
        "border-radius: 4px; background-color: %1; color: white;"
    ).arg(levelColor));
    headerLayout->addWidget(levelLabel);
    
    layout->addLayout(headerLayout);
    
    // 发布信息
    QLabel *infoLabel = new QLabel(QString("%1 | %2").arg(alert.sender, alert.pubTime));
    infoLabel->setStyleSheet("font-size: 12px; color: #909399;");
    layout->addWidget(infoLabel);
    
    // 预警内容
    QLabel *textLabel = new QLabel(alert.text);
    textLabel->setWordWrap(true);
    textLabel->setStyleSheet("font-size: 13px; color: #606266; line-height: 1.6;");
    layout->addWidget(textLabel);
    
    return card;
}

QString AlertWidget::getLevelColor(const QString &level)
{
    if (level.contains("红") || level.contains("Red")) return "#F56C6C";
    if (level.contains("橙") || level.contains("Orange")) return "#E6A23C";
    if (level.contains("黄") || level.contains("Yellow")) return "#F2C037";
    if (level.contains("蓝") || level.contains("Blue")) return "#409EFF";
    return "#909399";
}

QString AlertWidget::getLevelIcon(const QString &level)
{
    if (level.contains("红") || level.contains("Red")) return "🔴";
    if (level.contains("橙") || level.contains("Orange")) return "🟠";
    if (level.contains("黄") || level.contains("Yellow")) return "🟡";
    if (level.contains("蓝") || level.contains("Blue")) return "🔵";
    return "⚪";
}
