// Copyright 2025 MarketSystem
#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QDialog>
#include <QTabWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTextEdit>
#include "User.h"
#include "AdminService.h"

class AdminWindow : public QDialog {
    Q_OBJECT

 private:
    User m_adminUser;
    QTabWidget* m_tabWidget;

    // Users tab
    QWidget* m_usersTab;
    QTableWidget* m_usersTable;
    QPushButton* m_refreshUsersButton;
    QPushButton* m_banUserButton;
    QPushButton* m_unbanUserButton;
    QPushButton* m_viewBannedUsersButton;

    // Reports tab
    QWidget* m_reportsTab;
    QTableWidget* m_reportsTable;
    QPushButton* m_refreshReportsButton;
    QPushButton* m_resolveReportButton;
    QTextEdit* m_reportDetailsText;

    void setupUI();
    void setupUsersTab();
    void setupReportsTab();
    void setupConnections();
    void loadUsers();
    void loadReports();
    void showReportDetails(int row);

 public:
    explicit AdminWindow(const User& adminUser, QWidget* parent = nullptr);
    ~AdminWindow();

 private slots:
    void onRefreshUsersClicked();
    void onBanUserClicked();
    void onUnbanUserClicked();
    void onViewBannedUsersClicked();
    void onUserSelected(int row, int column);
    void onRefreshReportsClicked();
    void onResolveReportClicked();
    void onReportSelected(int row, int column);
};
#endif  // ADMINWINDOW_H
