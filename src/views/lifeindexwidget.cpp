/**
 * @file lifeindexwidget.cpp
 * @brief 生活指数展示组件实现
 */

#include "lifeindexwidget.h"
#include "ui_lifeindexwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDate>

LifeIndexWidget::LifeIndexWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LifeIndexWidget)
{
    ui->setupUi(this);
    setupConnections();
}

LifeIndexWidget::~LifeIndexWidget()
{
    delete ui;
}

void LifeIndexWidget::setupConnections()
{
    connect(ui->refreshBtn, &QPushButton::clicked,
            this, &LifeIndexWidget::onRefreshClicked);
}

void LifeIndexWidget::setCity(const QString &cityId, const QString &cityName)
{
    m_currentCityId = cityId;
    m_currentCityName = cityName;
    ui->cityLabel->setText(cityName);
    ui->dateLabel->setText(QDate::currentDate().toString("yyyy年MM月dd日"));
    clear();
}

void LifeIndexWidget::updateLifeIndex(const QList<LifeIndex> &indices)
{
    clearIndexCards();
    
    int row = 0, col = 0;
    const int maxCols = 3;
    
    for (const LifeIndex &index : indices) {
        QFrame *card = createIndexCard(index);
        ui->indexGridLayout->addWidget(card, row, col);
        m_indexCards.append(card);
        
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
    
    // 添加弹性空间
    ui->indexGridLayout->setRowStretch(row + 1, 1);
}

void LifeIndexWidget::clear()
{
    clearIndexCards();
}

void LifeIndexWidget::clearIndexCards()
{
    for (QFrame *card : m_indexCards) {
        ui->indexGridLayout->removeWidget(card);
        delete card;
    }
    m_indexCards.clear();
}

QFrame* LifeIndexWidget::createIndexCard(const LifeIndex &index)
{
    QFrame *card = new QFrame();
    card->setStyleSheet(
        "QFrame { background-color: white; border-radius: 12px; border: 1px solid #e4e7ed; }"
    );
    card->setMinimumHeight(140);
    
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setSpacing(8);
    layout->setContentsMargins(16, 16, 16, 16);
    
    // 顶部：图标和名称
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QLabel *iconLabel = new QLabel(getIndexIcon(index.type));
    iconLabel->setStyleSheet("font-size: 28px;");
    headerLayout->addWidget(iconLabel);
    
    QLabel *nameLabel = new QLabel(index.name);
    nameLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #303133;");
    headerLayout->addWidget(nameLabel);
    
    headerLayout->addStretch();
    
    // 等级标签
    QLabel *levelLabel = new QLabel(index.category);
    levelLabel->setStyleSheet(QString(
        "font-size: 12px; font-weight: bold; padding: 4px 8px; "
        "border-radius: 4px; background-color: %1; color: white;"
    ).arg(getLevelColor(index.level)));
    headerLayout->addWidget(levelLabel);
    
    layout->addLayout(headerLayout);
    
    // 描述
    QLabel *descLabel = new QLabel(index.description);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("font-size: 13px; color: #606266; line-height: 1.5;");
    descLabel->setMinimumHeight(50);
    layout->addWidget(descLabel);
    
    layout->addStretch();
    
    return card;
}

void LifeIndexWidget::onRefreshClicked()
{
    if (!m_currentCityId.isEmpty()) {
        emit refreshRequested(m_currentCityId);
    }
}

QString LifeIndexWidget::getIndexIcon(const QString &type)
{
    static QMap<QString, QString> iconMap = {
        {"1", "🏃"},   // 运动指数
        {"2", "🚗"},   // 洗车指数
        {"3", "👔"},   // 穿衣指数
        {"4", "🎣"},   // 钓鱼指数
        {"5", "☀️"},   // 紫外线指数
        {"6", "🌡️"},  // 旅游指数
        {"7", "🤧"},   // 过敏指数
        {"8", "😷"},   // 舒适度指数
        {"9", "🤒"},   // 感冒指数
        {"10", "🌬️"}, // 空气污染扩散条件指数
        {"11", "🌤️"}, // 空调开启指数
        {"12", "🕶️"}, // 太阳镜指数
        {"13", "💄"},  // 化妆指数
        {"14", "🧺"},  // 晾晒指数
        {"15", "🚌"},  // 交通指数
        {"16", "🛡️"}  // 防晒指数
    };
    
    return iconMap.value(type, "📊");
}

QString LifeIndexWidget::getLevelColor(const QString &level)
{
    bool ok;
    int lvl = level.toInt(&ok);
    if (!ok) lvl = 3;
    
    if (lvl <= 1) return "#67C23A";      // 优/适宜 - 绿色
    if (lvl <= 2) return "#409EFF";      // 良/较适宜 - 蓝色
    if (lvl <= 3) return "#E6A23C";      // 中等 - 黄色
    if (lvl <= 4) return "#F56C6C";      // 较差 - 橙色
    return "#909399";                     // 差 - 灰色
}
