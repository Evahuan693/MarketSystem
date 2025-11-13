// Copyright 2025 MarketSystem
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QVariantList>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>

class DatabaseManager {
 private:
    QSqlDatabase m_database;

    DatabaseManager();
    ~DatabaseManager();

    bool initializeDatabase();
    bool executeSchema();  // 不再需要从文件读取

 public:
    static DatabaseManager& getInstance();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase& getDatabase();
    bool isOpen() const;
    void close();
    QString getLastError() const;

    bool executeQuery(const QString& query, const QVariantList& params = {});
    QSqlQuery executeQueryWithResult(const QString& query, const QVariantList& params = {});
    int getLastInsertId() const;
};

#endif  // DATABASEMANAGER_H
