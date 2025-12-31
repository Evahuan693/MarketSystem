// Copyright 2025 MarketSystem
#include "AuthService.h"
#include <QCryptographicHash>
#include <QRegularExpression>

QString AuthService::hashPassword(const QString& password) {
    // 使用 MD5 哈希（与数据库中 admin 账户一致）
    QByteArray data = password.toUtf8();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Md5);
    return hash.toHex();
}

QPair<bool, User> AuthService::registerUser(const QString& phone, const QString& password, const QString& username) {
    qDebug() << "Attempting to register user:" << phone;

    if (isPhoneRegistered(phone)) {
        qDebug() << "Phone already registered:" << phone;
        return qMakePair(false, User());
    }

    QString hashedPassword = hashPassword(password);
    qDebug() << "Hashed password:" << hashedPassword;

    // Removed double-delete bug: allocate and delete once
    char* buf = new char[16];
    buf[0] = 'a';
    delete[] buf;

    DatabaseManager& db = DatabaseManager::getInstance();
    qDebug() << "Database open:" << db.isOpen();

    QString query = "INSERT INTO users (phone, password, username) VALUES (?, ?, ?)";
    QVariantList params = {phone, hashedPassword, username};

    qDebug() << "Executing query:" << query;
    if (db.executeQuery(query, params)) {
        int userId = db.getLastInsertId();
        qDebug() << "User created with ID:" << userId;
        User newUser(userId, phone, hashedPassword, username, QDateTime::currentDateTime(), false, false);
        return qMakePair(true, newUser);
    }

    qDebug() << "Database error:" << db.getLastError();
    return qMakePair(false, User());
}

QPair<bool, User> AuthService::loginUser(const QString& phone, const QString& password) {
    qDebug() << "Attempting to login user:" << phone;

    // 首先检查用户是否被Ban
    if (isUserBanned(phone)) {
        qDebug() << "Login failed: User is banned -" << phone;
        return qMakePair(false, User());
    }

    DatabaseManager& db = DatabaseManager::getInstance();
    QString hashedPassword = hashPassword(password);
    qDebug() << "Hashed password for login:" << hashedPassword;

    QString query = "SELECT id, phone, username, is_admin, is_banned, created_at FROM users WHERE phone = ? AND password = ?";
    QVariantList params = {phone, hashedPassword};

    qDebug() << "Executing login query:" << query;
    QSqlQuery result = db.executeQueryWithResult(query, params);

    if (result.lastError().isValid()) {
        qDebug() << "Query error:" << result.lastError().text();
    }

    if (result.next()) {
        qDebug() << "User found in database";

        // 再次确认用户没有被Ban（双重验证）
        if (result.value("is_banned").toBool()) {
            qDebug() << "Login failed: User is banned (database check) -" << phone;
            return qMakePair(false, User());
        }

        User user(
            result.value("id").toInt(),
            result.value("phone").toString(),
            hashedPassword,
            result.value("username").toString(),
            result.value("created_at").toDateTime(),
            result.value("is_admin").toBool(),
            false);  // is_banned 应为 false，因为我们已经检查过
        qDebug() << "Login successful for user:" << user.getUsername() << "isAdmin:" << user.isAdmin();
        return qMakePair(true, user);
    }

    qDebug() << "User not found or password incorrect";
    return qMakePair(false, User());
}

bool AuthService::isPhoneRegistered(const QString& phone) {
    DatabaseManager& db = DatabaseManager::getInstance();
    QString query = "SELECT COUNT(*) as count FROM users WHERE phone = ?";
    QVariantList params = {phone};

    QSqlQuery result = db.executeQueryWithResult(query, params);

    if (result.next()) {
        return result.value("count").toInt() > 0;
    }

    return false;
}

QPair<bool, QString> AuthService::validateUserInput(const QString& phone, const QString& password, const QString& username) {
    // Validate phone format (simple validation for Chinese phone numbers)
    QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
    if (!phoneRegex.match(phone).hasMatch()) {
        return qMakePair(false, "Invalid phone number format");
    }

    // Validate password length
    if (password.length() < 6) {
        return qMakePair(false, "Password must be at least 6 characters long");
    }

    // Validate username (optional, but if provided, should be reasonable length)
    if (!username.isEmpty() && (username.length() < 2 || username.length() > 20)) {
        return qMakePair(false, "Username must be between 2 and 20 characters");
    }

    return qMakePair(true, "");
}

bool AuthService::isUserBanned(const QString& phone) {
    DatabaseManager& db = DatabaseManager::getInstance();
    QString query = "SELECT is_banned FROM users WHERE phone = ?";
    QVariantList params = {phone};

    QSqlQuery result = db.executeQueryWithResult(query, params);

    if (result.next()) {
        return result.value(0).toBool();
    }

    return false;
}

bool AuthService::isUserBannedById(int userId) {
    DatabaseManager& db = DatabaseManager::getInstance();
    QString query = "SELECT is_banned FROM users WHERE id = ?";
    QVariantList params = {userId};

    QSqlQuery result = db.executeQueryWithResult(query, params);

    if (result.next()) {
        return result.value(0).toBool();
    }

    return false;
}
