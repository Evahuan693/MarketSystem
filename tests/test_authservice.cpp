#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

#include "AuthService.h"
#include "DatabaseManager.h"
#include "User.h"

static void removeTestDatabase()
{
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appData);
    QString dbPath = appData + "/marketplace.db";
    QFile f(dbPath);
    if (f.exists()) {
        f.remove();
    }
}

class AuthServiceTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        QStandardPaths::setTestModeEnabled(true);
        removeTestDatabase();
        // ensure singleton is created fresh
        DatabaseManager::getInstance();
    }

    static void TearDownTestSuite() {
        // nothing special; database file remains in test location
    }
};

TEST_F(AuthServiceTest, HashPasswordConsistency) {
    QString p = "password123";
    QString h1 = AuthService::hashPassword(p);
    QString h2 = AuthService::hashPassword(p);
    EXPECT_EQ(h1, h2);
    EXPECT_FALSE(h1.isEmpty());
}

TEST_F(AuthServiceTest, ValidateUserInputRejectBadPhone) {
    auto res = AuthService::validateUserInput("123", "password", "user");
    EXPECT_FALSE(res.first);
}

TEST_F(AuthServiceTest, ValidateUserInputRejectShortPassword) {
    auto res = AuthService::validateUserInput("13800138000", "123", "user");
    EXPECT_FALSE(res.first);
}

TEST_F(AuthServiceTest, RegisterAndLoginSuccess) {
    QString phone = "13900000001";
    QString password = "securepwd";
    // ensure not registered
    if (AuthService::isPhoneRegistered(phone)) {
        // no-op in case previous test left data
    }

    auto reg = AuthService::registerUser(phone, password, "tester");
    EXPECT_TRUE(reg.first);

    auto login = AuthService::loginUser(phone, password);
    EXPECT_TRUE(login.first);
    User u = login.second;
    EXPECT_EQ(u.getPhone(), phone);
}

TEST_F(AuthServiceTest, RegisterDuplicateFails) {
    QString phone = "13900000002";
    QString password = "pwd12345";
    auto r1 = AuthService::registerUser(phone, password, "a");
    EXPECT_TRUE(r1.first);
    auto r2 = AuthService::registerUser(phone, password, "b");
    EXPECT_FALSE(r2.first);
}

TEST_F(AuthServiceTest, LoginWrongPasswordFails) {
    QString phone = "13900000003";
    QString password = "rightpass";
    AuthService::registerUser(phone, password, "u");
    auto login = AuthService::loginUser(phone, "wrongpass");
    EXPECT_FALSE(login.first);
}

TEST_F(AuthServiceTest, BanCheck) {
    QString phone = "13900000004";
    QString password = "banpwd";
    auto r = AuthService::registerUser(phone, password, "victim");
    EXPECT_TRUE(r.first);
    // simulate ban by updating DB directly
    DatabaseManager& db = DatabaseManager::getInstance();
    QString q = "UPDATE users SET is_banned = 1 WHERE phone = ?";
    QVariantList params = { phone };
    bool ok = db.executeQuery(q, params);
    EXPECT_TRUE(ok);
    EXPECT_TRUE(AuthService::isUserBanned(phone));
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
