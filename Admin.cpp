// Copyright 2025 MarketSystem
#include "Admin.h"
#include <QDebug>

Admin::Admin()
    : m_adminId(0) {
}

Admin::Admin(int adminId, const User& user)
    : User(user), m_adminId(adminId) {
}

bool Admin::isValid() const {
    return User::isValid() && m_adminId > 0;
}

QString Admin::toString() const {
    return QString("Admin[adminId=%1, %2]")
        .arg(m_adminId)
        .arg(User::toString());
}
