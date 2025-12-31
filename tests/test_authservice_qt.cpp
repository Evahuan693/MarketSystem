#include <QtTest>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include "AuthService.h"
#include "DatabaseManager.h"
#include "User.h"

static void removeTestDatabaseAuth()
{
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString dbPath = appData + "/marketplace.db";
    QFile f(dbPath);
    if (f.exists()) f.remove();
}

class AuthServiceTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        QStandardPaths::setTestModeEnabled(true);
        removeTestDatabaseAuth();
        DatabaseManager::getInstance();
    }

    void cleanupTestCase() {
        // optional cleanup
    }

    void testHashPasswordConsistency() {
        QString p = "password123";
        QCOMPARE(AuthService::hashPassword(p), AuthService::hashPassword(p));
        QVERIFY(!AuthService::hashPassword(p).isEmpty());
    }

    void testValidateUserInputRejectBadPhone() {
        auto res = AuthService::validateUserInput("123", "password", "user");
        QVERIFY(!res.first);
    }

    void testValidateUserInputRejectShortPassword() {
        auto res = AuthService::validateUserInput("13800138000", "123", "user");
        QVERIFY(!res.first);
    }

    void testRegisterAndLoginSuccess() {
        QString phone = "13900000001";
        QString password = "securepwd";
        auto reg = AuthService::registerUser(phone, password, "tester");
        QVERIFY(reg.first);

        auto login = AuthService::loginUser(phone, password);
        QVERIFY(login.first);
        QCOMPARE(login.second.getPhone(), phone);
    }

    void testRegisterDuplicateFails() {
        QString phone = "13900000002";
        QString password = "pwd12345";
        auto r1 = AuthService::registerUser(phone, password, "a");
        QVERIFY(r1.first);
        auto r2 = AuthService::registerUser(phone, password, "b");
        QVERIFY(!r2.first);
    }

    void testLoginWrongPasswordFails() {
        QString phone = "13900000003";
        QString password = "rightpass";
        AuthService::registerUser(phone, password, "u");
        auto login = AuthService::loginUser(phone, "wrongpass");
        QVERIFY(!login.first);
    }

    void testBanCheck() {
        QString phone = "13900000004";
        QString password = "banpwd";
        auto r = AuthService::registerUser(phone, password, "victim");
        QVERIFY(r.first);
        DatabaseManager& db = DatabaseManager::getInstance();
        QString q = "UPDATE users SET is_banned = 1 WHERE phone = ?";
        QVariantList params = { phone };
        QVERIFY(db.executeQuery(q, params));
        QVERIFY(AuthService::isUserBanned(phone));
    }
};

// QTEST_MAIN removed: main is provided by tests_runner.cpp
#include "test_authservice_qt.moc"
