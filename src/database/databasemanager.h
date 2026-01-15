/**
 * @file databasemanager.h
 * @brief 数据库管理器类声明
 */

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>

/**
 * @class DatabaseManager
 * @brief SQLite数据库管理单例类
 * 
 * 负责数据库连接、表创建和基础CRUD操作
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     * @return DatabaseManager单例引用
     */
    static DatabaseManager& instance();
    
    /**
     * @brief 初始化数据库
     * @param dbPath 数据库文件路径，默认为应用数据目录
     * @return 初始化是否成功
     */
    bool initialize(const QString &dbPath = QString());
    
    /**
     * @brief 关闭数据库连接
     */
    void close();
    
    /**
     * @brief 获取数据库连接
     * @return QSqlDatabase引用
     */
    QSqlDatabase& database();
    
    /**
     * @brief 检查数据库是否已连接
     * @return 连接状态
     */
    bool isConnected() const;
    
    /**
     * @brief 获取最后的错误信息
     * @return 错误信息字符串
     */
    QString lastError() const;

signals:
    /**
     * @brief 数据库初始化完成信号
     * @param success 是否成功
     */
    void initialized(bool success);
    
    /**
     * @brief 数据库错误信号
     * @param error 错误信息
     */
    void errorOccurred(const QString &error);

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    
    // 禁止拷贝和赋值
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    /**
     * @brief 创建所有数据表
     * @return 创建是否成功
     */
    bool createTables();
    
    /**
     * @brief 创建城市表
     * @return 创建是否成功
     */
    bool createCityTable();
    
    /**
     * @brief 创建当前天气表
     * @return 创建是否成功
     */
    bool createWeatherCurrentTable();
    
    /**
     * @brief 创建天气预报表
     * @return 创建是否成功
     */
    bool createWeatherForecastTable();
    
    /**
     * @brief 创建历史天气表
     * @return 创建是否成功
     */
    bool createWeatherHistoryTable();
    
    /**
     * @brief 创建用户设置表
     * @return 创建是否成功
     */
    bool createUserSettingsTable();

private:
    QSqlDatabase m_database;
    QString m_lastError;
    bool m_isConnected;
    
    static const QString CONNECTION_NAME;
};

#endif // DATABASEMANAGER_H
