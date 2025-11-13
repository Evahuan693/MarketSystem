// Copyright 2025 MarketSystem
#ifndef ADMIN_H
#define ADMIN_H

#include <QString>
#include "User.h"

class Admin : public User {
 private:
    int m_adminId;

 public:
    Admin();
    Admin(int adminId, const User& user);

    // Getters
    int getAdminId() const { return m_adminId; }

    // Setters
    void setAdminId(int adminId) { m_adminId = adminId; }

    // Utility methods
    bool isValid() const;
    QString toString() const;
};
#endif  // ADMIN_H
