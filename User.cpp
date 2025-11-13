// Copyright 2025 MarketSystem
#include "User.h"
#include <QDebug>

User::User()
    : m_id(0), m_isAdmin(false) {
}

User::User(int id, const QString& phone, const QString& password,
           const QString& username, const QDateTime& createdAt,
           bool isAdmin, bool isBanned)
    : m_id(id), m_phone(phone), m_password(password), m_username(username),
    m_createdAt(createdAt), m_isAdmin(isAdmin), m_isBanned(isBanned) {
}

bool User::isValid() const {
    return !m_phone.isEmpty() && !m_password.isEmpty() && m_id > 0;
}

QString User::toString() const {
    return QString("User[id=%1, phone=%2, username=%3, isAdmin=%4]")
        .arg(m_id)
        .arg(m_phone)
        .arg(m_username)
        .arg(m_isAdmin ? "true" : "false");
}
