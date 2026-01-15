/**
 * @file citywidget.cpp
 * @brief 城市管理页面类实现
 */

#include "citywidget.h"
#include "../services/cityservice.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>

CityWidget::CityWidget(QWidget *parent)
    : QWidget(parent)
    , m_searchEdit(nullptr)
    , m_cityListView(nullptr)
    , m_addBtn(nullptr)
    , m_removeBtn(nullptr)
    , m_refreshBtn(nullptr)
    , m_statusLabel(nullptr)
    , m_cityModel(new CityModel(this))
    , m_filterModel(new CityFilterModel(this))
{
    setupUI();
    setupConnections();
    loadCities();
}

CityWidget::~CityWidget()
{
}

void CityWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // 标题
    QLabel *titleLabel = new QLabel(tr("🏙️ 城市管理"), this);
    titleLabel->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #2c3e50; padding: 10px 0;"
    );
    mainLayout->addWidget(titleLabel);
    
    // 搜索栏
    QHBoxLayout *searchLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText(tr("搜索城市（支持拼音首字母）..."));
    m_searchEdit->setStyleSheet(
        "QLineEdit {"
        "    padding: 10px 15px;"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    background: white;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #3498db;"
        "}"
    );
    searchLayout->addWidget(m_searchEdit);
    mainLayout->addLayout(searchLayout);
    
    // 按钮栏
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    m_addBtn = new QPushButton(tr("➕ 添加城市"), this);
    m_removeBtn = new QPushButton(tr("➖ 移除城市"), this);
    m_refreshBtn = new QPushButton(tr("🔄 刷新"), this);
    
    QString btnStyle = 
        "QPushButton {"
        "    padding: 8px 16px;"
        "    border: none;"
        "    border-radius: 6px;"
        "    font-size: 13px;"
        "    background-color: #3498db;"
        "    color: white;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1f6dad;"
        "}";
    
    m_addBtn->setStyleSheet(btnStyle);
    m_removeBtn->setStyleSheet(btnStyle.replace("#3498db", "#e74c3c").replace("#2980b9", "#c0392b").replace("#1f6dad", "#a93226"));
    m_refreshBtn->setStyleSheet(
        "QPushButton {"
        "    padding: 8px 16px;"
        "    border: none;"
        "    border-radius: 6px;"
        "    font-size: 13px;"
        "    background-color: #27ae60;"
        "    color: white;"
        "}"
        "QPushButton:hover {"
        "    background-color: #219a52;"
        "}"
    );
    
    btnLayout->addWidget(m_addBtn);
    btnLayout->addWidget(m_removeBtn);
    btnLayout->addWidget(m_refreshBtn);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);
    
    // 城市列表
    m_cityListView = new QListView(this);
    m_cityListView->setStyleSheet(
        "QListView {"
        "    border: 2px solid #bdc3c7;"
        "    border-radius: 8px;"
        "    background: white;"
        "    padding: 5px;"
        "}"
        "QListView::item {"
        "    padding: 12px 15px;"
        "    border-bottom: 1px solid #ecf0f1;"
        "    border-radius: 4px;"
        "}"
        "QListView::item:selected {"
        "    background-color: #3498db;"
        "    color: white;"
        "}"
        "QListView::item:hover {"
        "    background-color: #ebf5fb;"
        "}"
    );
    m_cityListView->setAlternatingRowColors(true);
    m_cityListView->setDragEnabled(true);
    m_cityListView->setAcceptDrops(true);
    m_cityListView->setDropIndicatorShown(true);
    m_cityListView->setDragDropMode(QAbstractItemView::InternalMove);
    
    // 设置模型
    m_filterModel->setSourceModel(m_cityModel);
    m_cityListView->setModel(m_filterModel);
    m_cityListView->setModelColumn(CityModel::ColName);
    
    mainLayout->addWidget(m_cityListView, 1);
    
    // 状态栏
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: #7f8c8d; font-size: 12px; padding: 5px;");
    mainLayout->addWidget(m_statusLabel);
}

void CityWidget::setupConnections()
{
    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &CityWidget::onSearchTextChanged);
    
    connect(m_cityListView, &QListView::clicked,
            this, &CityWidget::onCityClicked);
    
    connect(m_cityListView, &QListView::doubleClicked,
            this, &CityWidget::onCityDoubleClicked);
    
    connect(m_addBtn, &QPushButton::clicked,
            this, &CityWidget::onAddCityClicked);
    
    connect(m_removeBtn, &QPushButton::clicked,
            this, &CityWidget::onRemoveCityClicked);
    
    connect(m_refreshBtn, &QPushButton::clicked,
            this, &CityWidget::onRefreshClicked);
}

void CityWidget::loadCities()
{
    QList<CityInfo> cities = CityService::instance().getAllCities();
    
    if (cities.isEmpty()) {
        addDefaultCities();
        cities = CityService::instance().getAllCities();
    }
    
    m_cityModel->setCities(cities);
    m_statusLabel->setText(tr("共 %1 个城市").arg(cities.count()));
}

void CityWidget::addDefaultCities()
{
    // 添加一些默认城市
    QList<CityInfo> defaultCities = {
        {0, "101010100", "北京", "北京", "CN", 39.9042, 116.4074, true, 1},
        {0, "101020100", "上海", "上海", "CN", 31.2304, 121.4737, true, 2},
        {0, "101280101", "广州", "广东", "CN", 23.1291, 113.2644, true, 3},
        {0, "101280601", "深圳", "广东", "CN", 22.5431, 114.0579, true, 4},
        {0, "101030100", "天津", "天津", "CN", 39.0842, 117.2009, false, 0},
        {0, "101040100", "重庆", "重庆", "CN", 29.4316, 106.9123, false, 0},
        {0, "101210101", "杭州", "浙江", "CN", 30.2741, 120.1551, false, 0},
        {0, "101190101", "南京", "江苏", "CN", 32.0603, 118.7969, false, 0},
        {0, "101200101", "武汉", "湖北", "CN", 30.5928, 114.3055, false, 0},
        {0, "101270101", "成都", "四川", "CN", 30.5728, 104.0668, false, 0},
        {0, "101250101", "长沙", "湖南", "CN", 28.2282, 112.9388, false, 0},
        {0, "101110101", "西安", "陕西", "CN", 34.3416, 108.9398, false, 0},
    };
    
    for (const CityInfo &city : defaultCities) {
        CityService::instance().addCity(city);
    }
}

void CityWidget::onSearchTextChanged(const QString &text)
{
    m_filterModel->setSearchKeyword(text);
    
    int visibleCount = m_filterModel->rowCount();
    int totalCount = m_cityModel->rowCount();
    
    if (text.isEmpty()) {
        m_statusLabel->setText(tr("共 %1 个城市").arg(totalCount));
    } else {
        m_statusLabel->setText(tr("找到 %1 个城市（共 %2 个）").arg(visibleCount).arg(totalCount));
    }
}

void CityWidget::onCityClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    
    QModelIndex sourceIndex = m_filterModel->mapToSource(index);
    CityInfo city = m_cityModel->cityAt(sourceIndex.row());
    
    m_statusLabel->setText(tr("已选择: %1 (%2)").arg(city.name).arg(city.province));
}

void CityWidget::onCityDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    
    QModelIndex sourceIndex = m_filterModel->mapToSource(index);
    CityInfo city = m_cityModel->cityAt(sourceIndex.row());
    
    emit citySelected(city.cityId);
}

void CityWidget::onAddCityClicked()
{
    bool ok;
    QString cityName = QInputDialog::getText(this, tr("添加城市"),
                                              tr("请输入城市名称:"), QLineEdit::Normal,
                                              "", &ok);
    if (ok && !cityName.isEmpty()) {
        CityInfo city;
        city.cityId = QString::number(QDateTime::currentMSecsSinceEpoch());
        city.name = cityName;
        city.province = tr("未知");
        city.country = "CN";
        city.isFavorite = false;
        
        if (CityService::instance().addCity(city)) {
            loadCities();
            QMessageBox::information(this, tr("成功"), tr("城市 %1 已添加").arg(cityName));
        } else {
            QMessageBox::warning(this, tr("错误"), tr("添加城市失败"));
        }
    }
}

void CityWidget::onRemoveCityClicked()
{
    QModelIndex index = m_cityListView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择要移除的城市"));
        return;
    }
    
    QModelIndex sourceIndex = m_filterModel->mapToSource(index);
    CityInfo city = m_cityModel->cityAt(sourceIndex.row());
    
    int ret = QMessageBox::question(this, tr("确认移除"),
                                     tr("确定要移除城市 %1 吗？").arg(city.name),
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        if (CityService::instance().deleteCity(city.cityId)) {
            loadCities();
            m_statusLabel->setText(tr("城市 %1 已移除").arg(city.name));
        }
    }
}

void CityWidget::onRefreshClicked()
{
    loadCities();
    m_searchEdit->clear();
    m_statusLabel->setText(tr("列表已刷新"));
}
