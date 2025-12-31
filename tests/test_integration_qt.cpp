#include <QtTest>
#include <QStandardPaths>
#include <QFile>

#include "AuthService.h"
#include "DatabaseManager.h"

static void removeTestDatabaseIntegration()
{
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbPath = appData + "/marketplace.db";
    QFile f(dbPath);
    if (f.exists()) f.remove();
}

class IntegrationTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        QStandardPaths::setTestModeEnabled(true);
        removeTestDatabaseIntegration();
        DatabaseManager::getInstance();
    }

    void testUserRegistrationAndLogin() {
        QString phone = "19900000001";
        QString pwd = "integpass";
        auto reg = AuthService::registerUser(phone, pwd, "integ");
        QVERIFY(reg.first);

        auto login = AuthService::loginUser(phone, pwd);
        QVERIFY(login.first);
        QCOMPARE(login.second.getPhone(), phone);
        QVERIFY(!login.second.isAdmin());
    }

    void testAdminLoginIsAdmin() {
        // default admin created by DatabaseManager::executeSchema()
        auto login = AuthService::loginUser("13800138000", "admin123");
        QVERIFY(login.first);
        QVERIFY(login.second.isAdmin());
    }
};

// main provided by tests_runner.cpp
#include "test_integration_qt.moc"
