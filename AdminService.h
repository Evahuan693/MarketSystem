// Copyright 2025 MarketSystem
#ifndef ADMINSERVICE_H
#define ADMINSERVICE_H

#include <QString>
#include <QList>
#include <QPair>
#include "User.h"
#include "DatabaseManager.h"

class AdminService {
 public:
    static QPair<bool, QList<User>> getAllUsers(int page = 0, int pageSize = 10);

    static QPair<bool, QString> banUser(int userId, const QString& reason);

    static QPair<bool, QString> unbanUser(int userId);

    static QPair<bool, QList<User>> getBannedUsers(int page = 0, int pageSize = 10);

    static QPair<bool, QList<QPair<User, QString>>> getReports(int page = 0, int pageSize = 10);

    static QPair<bool, QString> resolveReport(int reportId, const QString& action, const QString& comment = "");

 private:
    static DatabaseManager& getDatabase();
};
#endif  // ADMINSERVICE_H
