#include <QtTest>
#include <QStandardPaths>
#include <QFile>

#include "AuthService.h"
#include "DatabaseManager.h"

static void removeTestDatabaseBan()
{
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbPath = appData + "/marketplace.db";
    QFile f(dbPath);
    if (f.exists()) f.remove();
}

class IntegrationBanTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        QStandardPaths::setTestModeEnabled(true);
        removeTestDatabaseBan();
        DatabaseManager::getInstance();
    }

    void testAdminBansUserAndPreventsLogin() {
        QString phone = "19900000002";
        QString pwd = "banpass";
        auto reg = AuthService::registerUser(phone, pwd, "toban");
        QVERIFY(reg.first);

        auto login1 = AuthService::loginUser(phone, pwd);
        QVERIFY(login1.first);

        
        DatabaseManager& db = DatabaseManager::getInstance();
        QString q = "UPDATE users SET is_banned = 1 WHERE phone = ?";
        QVariantList params = { phone };
        QVERIFY(db.executeQuery(q, params));

        QVERIFY(AuthService::isUserBanned(phone));

        auto login2 = AuthService::loginUser(phone, pwd);
        QVERIFY(!login2.first);
    }
};

// main provided by tests_runner.cpp
#include "test_integration_ban_qt.moc"
