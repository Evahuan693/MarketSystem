// Copyright 2025 MarketSystem
#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include <QString>
#include <QPair>
#include "User.h"
#include "DatabaseManager.h"

class AuthService {
 public:
    static QPair<bool, User> registerUser(const QString& phone, const QString& password, const QString& username = "");
    static QPair<bool, User> loginUser(const QString& phone, const QString& password);
    static QString hashPassword(const QString& password);
    static bool isPhoneRegistered(const QString& phone);
    static QPair<bool, QString> validateUserInput(const QString& phone, const QString& password, const QString& username = "");
    static bool isUserBanned(const QString& phone);
    static bool isUserBannedById(int userId);
};
#endif  // AUTHSERVICE_H
