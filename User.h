// Copyright 2025 MarketSystem
#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>

class User {
 private:
    int m_id;
    QString m_phone;
    QString m_password;
    QString m_username;
    QDateTime m_createdAt;
    bool m_isAdmin;
    bool m_isBanned;

 public:
    User();
    User(int id, const QString& phone, const QString& password,
         const QString& username = "",
         const QDateTime& createdAt = QDateTime::currentDateTime(),
         bool isAdmin = false, bool isBanned = false);

    // Getters
    int getId() const { return m_id; }
    QString getPhone() const { return m_phone; }
    QString getPassword() const { return m_password; }
    QString getUsername() const { return m_username; }
    QDateTime getCreatedAt() const { return m_createdAt; }
    bool isAdmin() const { return m_isAdmin; }
    bool isBanned() const { return m_isBanned; }

    // Setters
    void setId(int id) { m_id = id; }
    void setPhone(const QString& phone) { m_phone = phone; }
    void setPassword(const QString& password) { m_password = password; }
    void setUsername(const QString& username) { m_username = username; }
    void setCreatedAt(const QDateTime& createdAt) { m_createdAt = createdAt; }
    void setIsAdmin(bool isAdmin) { m_isAdmin = isAdmin; }
    void setIsBanned(bool isBanned) { m_isBanned = isBanned; }

    // Utility methods
    bool isValid() const;
    QString toString() const;
};
#endif  // USER_H
