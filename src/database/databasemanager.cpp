/**
 * @file databasemanager.cpp
 * @brief 数据库管理器类实现
 */

#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

const QString DatabaseManager::CONNECTION_NAME = "WeatherAnalysisDB";

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_isConnected(false)
{
}

DatabaseManager::~DatabaseManager()
{
    close();
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize(const QString &dbPath)
{
    if (m_isConnected) {
        return true;
    }
    
    // 确定数据库路径
    QString path = dbPath;
    if (path.isEmpty()) {
        QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(dataDir);
        if (!dir.exists()) {
            dir.mkpath(".");
        }
        path = dataDir + "/weather_analysis.db";
    }
    
    qDebug() << "Database path:" << path;
    
    // 创建数据库连接
    m_database = QSqlDatabase::addDatabase("QSQLITE", CONNECTION_NAME);
    m_database.setDatabaseName(path);
    
    if (!m_database.open()) {
        m_lastError = m_database.lastError().text();
        qCritical() << "Failed to open database:" << m_lastError;
        emit errorOccurred(m_lastError);
        emit initialized(false);
        return false;
    }
    
    // 启用外键约束
    QSqlQuery query(m_database);
    query.exec("PRAGMA foreign_keys = ON");
    
    // 创建表
    if (!createTables()) {
        emit initialized(false);
        return false;
    }
    
    m_isConnected = true;
    qDebug() << "Database initialized successfully";
    emit initialized(true);
    return true;
}

void DatabaseManager::close()
{
    if (m_isConnected) {
        m_database.close();
        m_isConnected = false;
    }
    QSqlDatabase::removeDatabase(CONNECTION_NAME);
}

QSqlDatabase& DatabaseManager::database()
{
    return m_database;
}

bool DatabaseManager::isConnected() const
{
    return m_isConnected;
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::createTables()
{
    bool success = true;
    
    success &= createCityTable();
    success &= createWeatherCurrentTable();
    success &= createWeatherForecastTable();
    success &= createWeatherHistoryTable();
    success &= createUserSettingsTable();
    
    return success;
}

bool DatabaseManager::createCityTable()
{
    QSqlQuery query(m_database);
    
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS city (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            city_id VARCHAR(32) UNIQUE NOT NULL,
            name VARCHAR(64) NOT NULL,
            province VARCHAR(64),
            country VARCHAR(64) DEFAULT 'CN',
            latitude REAL,
            longitude REAL,
            is_favorite INTEGER DEFAULT 0,
            favorite_order INTEGER DEFAULT 0,
            create_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            update_time DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to create city table:" << m_lastError;
        emit errorOccurred(m_lastError);
        return false;
    }
    
    // 创建索引
    query.exec("CREATE INDEX IF NOT EXISTS idx_city_name ON city(name)");
    query.exec("CREATE INDEX IF NOT EXISTS idx_city_favorite ON city(is_favorite)");
    
    qDebug() << "City table created successfully";
    return true;
}

bool DatabaseManager::createWeatherCurrentTable()
{
    QSqlQuery query(m_database);
    
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS weather_current (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            city_id VARCHAR(32) NOT NULL,
            temperature REAL,
            feels_like REAL,
            humidity INTEGER,
            pressure INTEGER,
            visibility INTEGER,
            wind_speed REAL,
            wind_direction VARCHAR(16),
            wind_degree INTEGER,
            weather_code VARCHAR(16),
            weather_desc VARCHAR(64),
            weather_icon VARCHAR(32),
            cloud_cover INTEGER,
            uv_index REAL,
            aqi INTEGER,
            aqi_level VARCHAR(16),
            pm25 REAL,
            pm10 REAL,
            o3 REAL,
            sunrise_time VARCHAR(8),
            sunset_time VARCHAR(8),
            observation_time DATETIME,
            update_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (city_id) REFERENCES city(city_id) ON DELETE CASCADE
        )
    )";
    
    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to create weather_current table:" << m_lastError;
        emit errorOccurred(m_lastError);
        return false;
    }
    
    query.exec("CREATE INDEX IF NOT EXISTS idx_weather_current_city ON weather_current(city_id)");
    
    qDebug() << "Weather current table created successfully";
    return true;
}

bool DatabaseManager::createWeatherForecastTable()
{
    QSqlQuery query(m_database);
    
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS weather_forecast (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            city_id VARCHAR(32) NOT NULL,
            forecast_date DATE NOT NULL,
            forecast_hour INTEGER,
            high_temp REAL,
            low_temp REAL,
            temperature REAL,
            humidity INTEGER,
            weather_code_day VARCHAR(16),
            weather_desc_day VARCHAR(64),
            weather_icon_day VARCHAR(32),
            weather_code_night VARCHAR(16),
            weather_desc_night VARCHAR(64),
            weather_icon_night VARCHAR(32),
            wind_speed REAL,
            wind_direction VARCHAR(16),
            precipitation_prob INTEGER,
            precipitation REAL,
            uv_index REAL,
            sunrise_time VARCHAR(8),
            sunset_time VARCHAR(8),
            update_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (city_id) REFERENCES city(city_id) ON DELETE CASCADE,
            UNIQUE(city_id, forecast_date, forecast_hour)
        )
    )";
    
    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to create weather_forecast table:" << m_lastError;
        emit errorOccurred(m_lastError);
        return false;
    }
    
    query.exec("CREATE INDEX IF NOT EXISTS idx_forecast_city_date ON weather_forecast(city_id, forecast_date)");
    
    qDebug() << "Weather forecast table created successfully";
    return true;
}

bool DatabaseManager::createWeatherHistoryTable()
{
    QSqlQuery query(m_database);
    
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS weather_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            city_id VARCHAR(32) NOT NULL,
            record_date DATE NOT NULL,
            avg_temp REAL,
            max_temp REAL,
            min_temp REAL,
            humidity INTEGER,
            pressure INTEGER,
            wind_speed REAL,
            weather_code VARCHAR(16),
            weather_desc VARCHAR(64),
            precipitation REAL,
            create_time DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (city_id) REFERENCES city(city_id) ON DELETE CASCADE,
            UNIQUE(city_id, record_date)
        )
    )";
    
    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to create weather_history table:" << m_lastError;
        emit errorOccurred(m_lastError);
        return false;
    }
    
    query.exec("CREATE INDEX IF NOT EXISTS idx_history_city_date ON weather_history(city_id, record_date)");
    
    qDebug() << "Weather history table created successfully";
    return true;
}

bool DatabaseManager::createUserSettingsTable()
{
    QSqlQuery query(m_database);
    
    QString sql = R"(
        CREATE TABLE IF NOT EXISTS user_settings (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            setting_key VARCHAR(64) UNIQUE NOT NULL,
            setting_value TEXT,
            setting_type VARCHAR(16) DEFAULT 'string',
            description VARCHAR(256),
            update_time DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";
    
    if (!query.exec(sql)) {
        m_lastError = query.lastError().text();
        qCritical() << "Failed to create user_settings table:" << m_lastError;
        emit errorOccurred(m_lastError);
        return false;
    }
    
    // 插入默认设置
    QStringList defaultSettings = {
        "INSERT OR IGNORE INTO user_settings (setting_key, setting_value, setting_type, description) "
        "VALUES ('temperature_unit', 'celsius', 'string', '温度单位: celsius/fahrenheit')",
        
        "INSERT OR IGNORE INTO user_settings (setting_key, setting_value, setting_type, description) "
        "VALUES ('wind_speed_unit', 'km/h', 'string', '风速单位: km/h, m/s, mph')",
        
        "INSERT OR IGNORE INTO user_settings (setting_key, setting_value, setting_type, description) "
        "VALUES ('pressure_unit', 'hPa', 'string', '气压单位: hPa, mmHg')",
        
        "INSERT OR IGNORE INTO user_settings (setting_key, setting_value, setting_type, description) "
        "VALUES ('theme', 'light', 'string', '主题: light/dark')",
        
        "INSERT OR IGNORE INTO user_settings (setting_key, setting_value, setting_type, description) "
        "VALUES ('icon_style', 'simple', 'string', '图标风格: simple/realistic')",
        
        "INSERT OR IGNORE INTO user_settings (setting_key, setting_value, setting_type, description) "
        "VALUES ('auto_refresh_interval', '30', 'int', '自动刷新间隔(分钟)')",
        
        "INSERT OR IGNORE INTO user_settings (setting_key, setting_value, setting_type, description) "
        "VALUES ('current_city_id', '', 'string', '当前选中城市ID')"
    };
    
    for (const QString &sql : defaultSettings) {
        query.exec(sql);
    }
    
    qDebug() << "User settings table created successfully";
    return true;
}
