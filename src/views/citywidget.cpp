/**
 * @file citywidget.cpp
 * @brief 城市管理页面类实现
 */

#include "citywidget.h"
#include "ui_citywidget.h"
#include "../services/cityservice.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>

CityWidget::CityWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CityWidget)
    , m_cityModel(new CityModel(this))
    , m_filterModel(new CityFilterModel(this))
{
    ui->setupUi(this);
    
    // 设置模型
    m_filterModel->setSourceModel(m_cityModel);
    ui->cityListView->setModel(m_filterModel);
    ui->cityListView->setModelColumn(CityModel::ColName);
    
    setupConnections();
    loadCities();
}

CityWidget::~CityWidget()
{
    delete ui;
}

void CityWidget::setupConnections()
{
    connect(ui->searchEdit, &QLineEdit::textChanged,
            this, &CityWidget::onSearchTextChanged);
    
    connect(ui->cityListView, &QListView::clicked,
            this, &CityWidget::onCityClicked);
    
    connect(ui->cityListView, &QListView::doubleClicked,
            this, &CityWidget::onCityDoubleClicked);
    
    connect(ui->addBtn, &QPushButton::clicked,
            this, &CityWidget::onAddCityClicked);
    
    connect(ui->removeBtn, &QPushButton::clicked,
            this, &CityWidget::onRemoveCityClicked);
    
    connect(ui->favoriteBtn, &QPushButton::clicked,
            this, &CityWidget::onFavoriteClicked);
    
    connect(ui->refreshBtn, &QPushButton::clicked,
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
    ui->statusLabel->setText(tr("共 %1 个城市").arg(cities.count()));
}

void CityWidget::addDefaultCities()
{
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
        ui->statusLabel->setText(tr("共 %1 个城市").arg(totalCount));
    } else {
        ui->statusLabel->setText(tr("找到 %1 个城市（共 %2 个）").arg(visibleCount).arg(totalCount));
    }
}

void CityWidget::onCityClicked(const QModelIndex &index)
{
    if (!index.isValid()) return;
    
    QModelIndex sourceIndex = m_filterModel->mapToSource(index);
    CityInfo city = m_cityModel->cityAt(sourceIndex.row());
    
    QString favText = city.isFavorite ? tr("⭐") : "";
    ui->statusLabel->setText(tr("已选择: %1 %2 (%3)").arg(favText).arg(city.name).arg(city.province));
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
    QModelIndex index = ui->cityListView->currentIndex();
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
            ui->statusLabel->setText(tr("城市 %1 已移除").arg(city.name));
        }
    }
}

void CityWidget::onFavoriteClicked()
{
    QModelIndex index = ui->cityListView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, tr("提示"), tr("请先选择城市"));
        return;
    }
    
    QModelIndex sourceIndex = m_filterModel->mapToSource(index);
    CityInfo city = m_cityModel->cityAt(sourceIndex.row());
    
    bool newFavorite = !city.isFavorite;
    if (CityService::instance().setFavorite(city.cityId, newFavorite)) {
        loadCities();
        QString msg = newFavorite ? tr("已收藏 %1").arg(city.name) : tr("已取消收藏 %1").arg(city.name);
        ui->statusLabel->setText(msg);
    }
}

void CityWidget::onRefreshClicked()
{
    loadCities();
    ui->searchEdit->clear();
    ui->statusLabel->setText(tr("列表已刷新"));
}
