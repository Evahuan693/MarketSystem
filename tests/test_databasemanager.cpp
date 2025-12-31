#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include "DatabaseManager.h"

static void removeTestDatabase()
{
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbPath = appData + "/marketplace.db";
    QFile f(dbPath);
    if (f.exists()) f.remove();
}

class DatabaseManagerTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        QStandardPaths::setTestModeEnabled(true);
        removeTestDatabase();
        // create singleton
        DatabaseManager::getInstance();
    }
};

TEST_F(DatabaseManagerTest, DatabaseOpens) {
    DatabaseManager& db = DatabaseManager::getInstance();
    EXPECT_TRUE(db.isOpen());
}

TEST_F(DatabaseManagerTest, ExecuteQueryInsertAndLastId) {
    DatabaseManager& db = DatabaseManager::getInstance();
    QString phone = "15000000000";
    QString pwd = "hash";
    QString q = "INSERT INTO users (phone, password, username) VALUES (?, ?, ?)";
    QVariantList params = { phone, pwd, QString("u") };
    bool ok = db.executeQuery(q, params);
    EXPECT_TRUE(ok);
    int id = db.getLastInsertId();
    EXPECT_GE(id, 1);
}

TEST_F(DatabaseManagerTest, ExecuteQueryWithResult) {
    DatabaseManager& db = DatabaseManager::getInstance();
    QString q = "SELECT COUNT(*) as count FROM users";
    QVariantList params = {};
    QSqlQuery result = db.executeQueryWithResult(q, params);
    EXPECT_FALSE(result.lastError().isValid());
    if (result.next()) {
        int cnt = result.value("count").toInt();
        EXPECT_GE(cnt, 0);
    } else {
        FAIL() << "No rows returned from count query";
    }
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
