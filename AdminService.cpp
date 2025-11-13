// Copyright 2025 MarketSystem
#include "AdminService.h"
#include <QDebug>

DatabaseManager& AdminService::getDatabase() {
    return DatabaseManager::getInstance();
}

QPair<bool, QList<User>> AdminService::getAllUsers(int page, int pageSize) {
    DatabaseManager& db = getDatabase();
    int offset = page * pageSize;

    QString query = "SELECT id, phone, username, created_at, "
    "is_admin, is_banned FROM users ORDER BY created_at DESC LIMIT ? OFFSET ?";
    QVariantList params = {pageSize, offset};
    QSqlQuery result = db.executeQueryWithResult(query, params);

    if (result.lastError().isValid()) {
        qWarning() << "Error fetching users:" << result.lastError().text();
        return qMakePair(false, QList<User>());
    }

    QList<User> users;
    while (result.next()) {
        User user(
            result.value("id").toInt(),
            result.value("phone").toString(),
            "",  // Password not returned
            result.value("username").toString(),
            result.value("created_at").toDateTime(),
            result.value("is_admin").toBool(),
            result.value("is_banned").toBool());
        users.append(user);
    }

    return qMakePair(true, users);
}

QPair<bool, QString> AdminService::banUser(int userId, const QString& reason) {
    Q_UNUSED(reason);
    DatabaseManager& db = getDatabase();

    // First check if user exists and isn't already banned
    QString checkQuery = "SELECT is_banned FROM users WHERE id = ?";
    QSqlQuery checkResult = db.executeQueryWithResult(checkQuery, {userId});

    if (!checkResult.next()) {
        return qMakePair(false, "User not found");
    }

    if (checkResult.value(0).toBool()) {
        return qMakePair(false, "User is already banned");
    }

    // Update user to banned
    QString updateQuery = "UPDATE users SET is_banned = 1 WHERE id = ?";
    if (db.executeQuery(updateQuery, {userId})) {
        return qMakePair(true, "User banned successfully");
    }

    return qMakePair(false, "Failed to ban user: " + db.getLastError());
}

QPair<bool, QString> AdminService::unbanUser(int userId) {
    DatabaseManager& db = getDatabase();

    // First check if user exists and is banned
    QString checkQuery = "SELECT is_banned FROM users WHERE id = ?";
    QSqlQuery checkResult = db.executeQueryWithResult(checkQuery, {userId});

    if (!checkResult.next()) {
        return qMakePair(false, "User not found");
    }

    if (!checkResult.value(0).toBool()) {
        return qMakePair(false, "User is not banned");
    }

    // Update user to unbanned
    QString updateQuery = "UPDATE users SET is_banned = 0 WHERE id = ?";
    if (db.executeQuery(updateQuery, {userId})) {
        return qMakePair(true, "User unbanned successfully");
    }

    return qMakePair(false, "Failed to unban user: " + db.getLastError());
}

QPair<bool, QList<User>> AdminService::getBannedUsers(int page, int pageSize) {
    DatabaseManager& db = getDatabase();
    int offset = page * pageSize;

    QString query = "SELECT id, phone, username, created_at, is_admin, "
        "is_banned FROM users WHERE is_banned = 1 "
        "ORDER BY created_at DESC LIMIT ? OFFSET ?";
    QVariantList params = {pageSize, offset};
    QSqlQuery result = db.executeQueryWithResult(query, params);

    if (result.lastError().isValid()) {
        qWarning() << "Error fetching banned users:"
                   << result.lastError().text();
        return qMakePair(false, QList<User>());
    }

    QList<User> users;
    while (result.next()) {
        User user(
            result.value("id").toInt(),
            result.value("phone").toString(),
            "",  // Password not returned
            result.value("username").toString(),
            result.value("created_at").toDateTime(),
            result.value("is_admin").toBool(),
            result.value("is_banned").toBool());
        users.append(user);
    }

    return qMakePair(true, users);
}

QPair<bool, QList<QPair<User, QString>>>
AdminService::getReports(int page, int pageSize) {
    DatabaseManager& db = getDatabase();
    int offset = page * pageSize;

    QString query = "SELECT r.*, u.username AS reporter_name, "
                    "u2.username AS reported_name "
                    "FROM reports r "
                    "JOIN users u ON r.reporter_id = u.id "
                    "JOIN users u2 ON r.reported_user_id = u2.id "
                    "ORDER BY r.created_at DESC LIMIT ? OFFSET ?";

    QVariantList params = {pageSize, offset};
    QSqlQuery result = db.executeQueryWithResult(query, params);

    if (result.lastError().isValid()) {
        qWarning() << "Error fetching reports:" << result.lastError().text();
        return qMakePair(false, QList<QPair<User, QString>>());
    }

    QList<QPair<User, QString>> reports;
    while (result.next()) {
        User reporter(
            result.value("reporter_id").toInt(),
            "",  // Phone not returned
            "",  // Password not returned
            result.value("reporter_name").toString());

        QString reportDetails = QString("Reported %1: %2 (Status: %3)")
                                .arg(result.value("reported_name").toString())
                                .arg(result.value("reason").toString())
                                .arg(result.value("status").toString());

        reports.append(qMakePair(reporter, reportDetails));
    }

    return qMakePair(true, reports);
}

QPair<bool, QString> AdminService::resolveReport
    (int reportId, const QString& action, const QString& comment) {
     Q_UNUSED(comment);
    DatabaseManager& db = getDatabase();

    // First check if report exists
    QString checkQuery = "SELECT status FROM reports WHERE id = ?";
    QSqlQuery checkResult = db.executeQueryWithResult(checkQuery, {reportId});

    if (!checkResult.next()) {
        return qMakePair(false, "Report not found");
    }

    if (checkResult.value(0).toString() != "pending") {
        return qMakePair(false, "Report has already been processed");
    }

    // Update report status
    QString updateQuery = "UPDATE reports SET status = ?, "
        "report_time = CURRENT_TIMESTAMP WHERE id = ?";
    if (db.executeQuery(updateQuery, {action, reportId})) {
        return qMakePair(true, "Report resolved successfully");
    }

    return qMakePair(false, "Failed to resolve report: " + db.getLastError());
}
