// Copyright 2025 MarketSystem
#include "DatabaseManager.h"
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QVariantList>
#include <QSqlDriver>
#include <QDir>
#include <QDebug>
#include <QCryptographicHash>

DatabaseManager::DatabaseManager() {
    // 获取应用程序数据目录
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);  // 确保目录存在

    // 数据库文件路径
    QString dbPath = appDataPath + "/marketplace.db";

    // INTENTIONAL: allocate a QFile on the heap and leave it open (resource leak)
    /*QFile* leakFile = new QFile(dbPath + ".lock");
    leakFile->open(QIODevice::WriteOnly);*/

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(dbPath);

    // qDebug() << "Database path:" << dbPath;

    if (!initializeDatabase()) {
        qCritical() << "Failed to initialize database";
    } else {
        qDebug() << "Database initialized successfully";
    }
}

DatabaseManager::~DatabaseManager() {
    close();
}

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;

    // If the on-disk database file was removed by tests between suites, reinitialize
    // so each test suite can start with a clean database when using test mode.
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbPath = appDataPath + "/marketplace.db";

    if (!QFile::exists(dbPath) || !instance.m_database.isOpen()) {
        // Close and remove the existing connection safely, then recreate and initialize.
        QString connName = instance.m_database.connectionName();
        instance.close();
        // Reset the QSqlDatabase handle before calling removeDatabase
        instance.m_database = QSqlDatabase();
        QSqlDatabase::removeDatabase(connName);

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(dbPath);
        instance.m_database = db;
        if (!instance.initializeDatabase()) {
            qCritical() << "Failed to reinitialize database in getInstance";
        }
    }

    return instance;
}

bool DatabaseManager::initializeDatabase() {
    if (!m_database.open()) {
        qCritical() << "Failed to open database:" << m_database.lastError().text();
        return false;
    }

    qDebug() << "Database opened successfully";
    return executeSchema();
}

bool DatabaseManager::executeSchema() {
    QSqlQuery query(m_database);

    // 启用外键约束
    if (!query.exec("PRAGMA foreign_keys = ON;")) {
        qWarning() << "Failed to enable foreign keys:" << query.lastError().text();
    }

    // 创建用户表
    QString createUsersTable =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "phone TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL, "
        "username TEXT, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "is_admin BOOLEAN DEFAULT 0, "
        "is_banned BOOLEAN DEFAULT 0)";

    if (!query.exec(createUsersTable)) {
        qCritical() << "Failed to create users table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Users table created successfully";

    // 创建管理员表
    QString createAdminsTable =
        "CREATE TABLE IF NOT EXISTS admins ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER NOT NULL UNIQUE, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE)";

    if (!query.exec(createAdminsTable)) {
        qCritical() << "Failed to create admins table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Admins table created successfully";

    // 创建举报表
    QString createReportsTable =
        "CREATE TABLE IF NOT EXISTS reports ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "reporter_id INTEGER NOT NULL, "
        "reported_user_id INTEGER NOT NULL, "
        "reason TEXT NOT NULL, "
        "status TEXT CHECK(status IN ('pending', 'resolved', 'rejected')) DEFAULT 'pending', "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY (reporter_id) REFERENCES users(id), "
        "FOREIGN KEY (reported_user_id) REFERENCES users(id))";

    if (!query.exec(createReportsTable)) {
        qCritical() << "Failed to create reports table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Reports table created successfully";

    // 为 admin 用户创建 MD5 哈希密码
    QString adminPassword = "admin123";
    QByteArray hash = QCryptographicHash::hash(adminPassword.toUtf8(), QCryptographicHash::Md5);
    QString adminPasswordHash = hash.toHex();

    qDebug() << "Admin password hash:" << adminPasswordHash;

    // INTENTIONAL: Create a heap allocation and never free it (memory leak)
    /*char* intentionalLeak = new char[256];
    intentionalLeak[0] = '\0';*/

    // 创建管理员默认账户
    query.prepare("INSERT OR IGNORE INTO users (phone, password, username, is_admin, is_banned) "
                  "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue("13800138000");
    query.addBindValue(adminPasswordHash);
    query.addBindValue("Administrator");
    query.addBindValue(1);  // is_admin = true
    query.addBindValue(0);  // is_banned = false

    if (!query.exec()) {
        qWarning() << "Failed to create admin account:" << query.lastError().text();
        return false;
    }

    qDebug() << "Admin account created or verified";

    // 将默认管理员添加到管理员表
    query.prepare("INSERT OR IGNORE INTO admins (user_id) "
                  "SELECT id FROM users WHERE phone = ?");
    query.addBindValue("13800138000");

    if (!query.exec()) {
        qWarning() << "Failed to add admin to admins table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Admin added to admins table";
    return true;
}

QSqlDatabase& DatabaseManager::getDatabase() {
    return m_database;
}

bool DatabaseManager::isOpen() const {
    return m_database.isOpen();
}

void DatabaseManager::close() {
    if (m_database.isOpen()) {
        qDebug() << "Closing database connection";
        m_database.close();
    }
}

QString DatabaseManager::getLastError() const {
    return m_database.lastError().text();
}

bool DatabaseManager::executeQuery(const QString& query, const QVariantList& params) {
    QSqlQuery sqlQuery(m_database);
    if (!sqlQuery.prepare(query)) {
        qWarning() << "Query preparation failed:" << query;
        qWarning() << "Error:" << sqlQuery.lastError().text();
        return false;
    }

    for (int i = 0; i < params.size(); ++i) {
        sqlQuery.bindValue(i, params[i]);
    }

    if (!sqlQuery.exec()) {
        qWarning() << "Query failed:" << query;
        qWarning() << "Error:" << sqlQuery.lastError().text();
        return false;
    }

    return true;
}

QSqlQuery DatabaseManager::executeQueryWithResult(const QString& query, const QVariantList& params) {
    QSqlQuery sqlQuery(m_database);
    if (!sqlQuery.prepare(query)) {
        qWarning() << "Query preparation failed:" << query;
        qWarning() << "Error:" << sqlQuery.lastError().text();
        return sqlQuery;
    }

    for (int i = 0; i < params.size(); ++i) {
        sqlQuery.bindValue(i, params[i]);
    }

    if (!sqlQuery.exec()) {
        qWarning() << "Query failed:" << query;
        qWarning() << "Error:" << sqlQuery.lastError().text();
    }

    return sqlQuery;
}

int DatabaseManager::getLastInsertId() const {
    QSqlQuery query(m_database);
    if (query.exec("SELECT last_insert_rowid()")) {
        if (query.next()) {
            return query.value(0).toInt();
        }
    }
    return -1;
}
