#include <QtTest>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include "DatabaseManager.h"

static void removeTestDatabaseDB()
{
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbPath = appData + "/marketplace.db";
    QFile f(dbPath);
    if (f.exists()) f.remove();
}

class DatabaseManagerTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        QStandardPaths::setTestModeEnabled(true);
        removeTestDatabaseDB();
        DatabaseManager::getInstance();
    }

    void testDatabaseOpens() {
        DatabaseManager& db = DatabaseManager::getInstance();
        QVERIFY(db.isOpen());
    }

    void testExecuteQueryInsertAndLastId() {
        DatabaseManager& db = DatabaseManager::getInstance();
        QString phone = "15000000000";
        QString pwd = "hash";
        QString q = "INSERT INTO users (phone, password, username) VALUES (?, ?, ?)";
        QVariantList params = { phone, pwd, QString("u") };
        QVERIFY(db.executeQuery(q, params));
        int id = db.getLastInsertId();
        QVERIFY(id >= 1);
    }

    void testExecuteQueryWithResult() {
        DatabaseManager& db = DatabaseManager::getInstance();
        QString q = "SELECT COUNT(*) as count FROM users";
        QVariantList params = {};
        QSqlQuery result = db.executeQueryWithResult(q, params);
        QVERIFY(!result.lastError().isValid());
        QVERIFY(result.next());
        int cnt = result.value("count").toInt();
        QVERIFY(cnt >= 0);
    }
};

// QTEST_MAIN removed: main is provided by tests_runner.cpp
#include "test_databasemanager_qt.moc"
