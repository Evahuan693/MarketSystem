// Copyright 2025 MarketSystem
#include <QApplication>
#include <QMessageBox>
#include <QFontDatabase>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "LoginWindow.h"
#include "mainwindow.h"
#include "AdminWindow.h"
#include "DatabaseManager.h"
#include "User.h"


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application style
    app.setStyle("Fusion");

    // Set application font
    QFont defaultFont = QApplication::font();
    defaultFont.setPointSize(10);
    QApplication::setFont(defaultFont);

    // Debug: Show database file location
    // QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/marketplace.db";
    // qDebug() << "Database file should be at:" << dbPath;

    // Ensure application data directory exists
    QDir appDataDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!appDataDir.exists()) {
        qDebug() << "Creating application data directory:" << appDataDir.path();
        appDataDir.mkpath(".");
    }

    // Initialize database
    DatabaseManager& dbManager = DatabaseManager::getInstance();
    qDebug() << "Database initialized:" << dbManager.isOpen();

    // Test database connection using the correct method
    if (dbManager.isOpen()) {
        QSqlQuery query(dbManager.getDatabase());
        if (query.exec("SELECT COUNT(*) as count FROM users")) {
            if (query.next()) {
                qDebug() << "User count in database:" << query.value("count").toInt();
            } else {
                qDebug() << "No rows returned from query";
            }
        } else {
            qDebug() << "Failed to query users:" << query.lastError().text();
        }
    }

    // Show login window
    LoginWindow loginWindow;
    if (loginWindow.exec() == QDialog::Accepted) {
        User currentUser = loginWindow.getCurrentUser();

        qDebug() << "Logged in user:" << currentUser.getUsername()
                 << "isAdmin:" << currentUser.isAdmin()
                 << "isBanned:" << currentUser.isBanned();

        // 全局检查：确保被Ban用户无法进入系统
        if (currentUser.isBanned() || AuthService::isUserBannedById(currentUser.getId())) {
            QMessageBox::critical(nullptr, "Access Denied",
                                  "Your account has been banned. Please contact administrator for assistance.");
            return 0;
        }

        // 创建和显示适当的窗口
        if (currentUser.isAdmin()) {
            qDebug() << "Admin user logged in:" << currentUser.getUsername();
            AdminWindow adminWindow(currentUser);
            adminWindow.exec();
        } else {
            qDebug() << "Regular user logged in:" << currentUser.getUsername();
            MainWindow mainWindow(currentUser);
            mainWindow.show();
            return app.exec();
        }
    }

    return 0;
}
