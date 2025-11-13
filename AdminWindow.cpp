// Copyright 2025 MarketSystem
#include "AdminWindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QHeaderView>
#include <QDateTime>
#include <QScrollBar>
#include <QGroupBox>
#include <QInputDialog>
#include <QColor>
#include <utility>

AdminWindow::AdminWindow(const User& adminUser, QWidget* parent)
    : QDialog(parent), m_adminUser(adminUser) {
    setWindowTitle("Admin Panel - " + adminUser.getUsername());
    setWindowIcon(QIcon(":/icons/admin_icon.png"));

    // Set window size and position
    resize(1000, 700);

    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        move((screenGeometry.width() - width()) / 2,
             (screenGeometry.height() - height()) / 2);
    }

    setupUI();
    setupConnections();

    // Load initial data
    loadUsers();
    loadReports();
}

AdminWindow::~AdminWindow() {
}

void AdminWindow::setupUI() {
    // Create main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // Create tab widget
    m_tabWidget = new QTabWidget(this);

    // Setup tabs
    setupUsersTab();
    setupReportsTab();

    mainLayout->addWidget(m_tabWidget);
}

void AdminWindow::setupUsersTab() {
    m_usersTab = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(m_usersTab);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    // Header
    QHBoxLayout* headerLayout = new QHBoxLayout();

    QLabel* titleLabel = new QLabel("User Management", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");

    m_refreshUsersButton = new QPushButton("Refresh", this);
    m_refreshUsersButton->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border: none; border-radius: 4px; padding: 8px; }"
        "QPushButton:hover { background-color: #2980b9; }");

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_refreshUsersButton);

    // Users table
    m_usersTable = new QTableWidget(this);
    m_usersTable->setColumnCount(5);
    m_usersTable->setHorizontalHeaderLabels({"ID", "Phone", "Username", "Created", "Status"});
    m_usersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_usersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_usersTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // Action buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_banUserButton = new QPushButton("Ban User", this);
    m_banUserButton->setEnabled(false);
    m_banUserButton->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; border: none; border-radius: 4px; padding: 8px; }"
        "QPushButton:hover { background-color: #c0392b; }");

    m_unbanUserButton = new QPushButton("Unban User", this);
    m_unbanUserButton->setEnabled(false);
    m_unbanUserButton->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; border: none; border-radius: 4px; padding: 8px; }"
        "QPushButton:hover { background-color: #2196f3; }");

    m_viewBannedUsersButton = new QPushButton("View Banned Users", this);
    m_viewBannedUsersButton->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border: none; border-radius: 4px; padding: 8px; }"
        "QPushButton:hover { background-color: #2980b9; }");

    buttonLayout->addWidget(m_banUserButton);
    buttonLayout->addWidget(m_unbanUserButton);
    buttonLayout->addWidget(m_viewBannedUsersButton);

    // Add all sections to main layout
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(m_usersTable, 1);
    mainLayout->addLayout(buttonLayout);

    m_tabWidget->addTab(m_usersTab, "Users");
}

void AdminWindow::setupReportsTab() {
    m_reportsTab = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(m_reportsTab);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    // Header
    QHBoxLayout* headerLayout = new QHBoxLayout();

    QLabel* titleLabel = new QLabel("Report Management", this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50;");

    m_refreshReportsButton = new QPushButton("Refresh", this);
    m_refreshReportsButton->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border: none; border-radius: 4px; padding: 8px; }"
        "QPushButton:hover { background-color: #2980b9; }");

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_refreshReportsButton);

    // Reports table
    m_reportsTable = new QTableWidget(this);
    m_reportsTable->setColumnCount(3);
    m_reportsTable->setHorizontalHeaderLabels({"ID", "Reporter", "Report Details"});
    m_reportsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_reportsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_reportsTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // Report details section
    QGroupBox* detailsGroup = new QGroupBox("Report Details", this);
    detailsGroup->setStyleSheet("QGroupBox { font-weight: bold; border: 1px solid #ddd; border-radius: 4px; margin-top: 1ex; }");

    m_reportDetailsText = new QTextEdit(this);
    m_reportDetailsText->setReadOnly(true);

    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsGroup);
    detailsLayout->addWidget(m_reportDetailsText);

    // Action buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_resolveReportButton = new QPushButton("Resolve Report", this);
    m_resolveReportButton->setEnabled(false);
    m_resolveReportButton->setStyleSheet(
        "QPushButton { background-color: #27ae60; color: white; border: none; border-radius: 4px; padding: 8px; }"
        "QPushButton:hover { background-color: #2196f3; }");

    buttonLayout->addWidget(m_resolveReportButton);

    // Add all sections to main layout
    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(m_reportsTable, 1);
    mainLayout->addWidget(detailsGroup);
    mainLayout->addLayout(buttonLayout);

    m_tabWidget->addTab(m_reportsTab, "Reports");
}

void AdminWindow::setupConnections() {
    connect(m_refreshUsersButton, &QPushButton::clicked, this, &AdminWindow::onRefreshUsersClicked);
    connect(m_banUserButton, &QPushButton::clicked, this, &AdminWindow::onBanUserClicked);
    connect(m_unbanUserButton, &QPushButton::clicked, this, &AdminWindow::onUnbanUserClicked);
    connect(m_viewBannedUsersButton, &QPushButton::clicked, this, &AdminWindow::onViewBannedUsersClicked);
    connect(m_usersTable, &QTableWidget::cellClicked, this, &AdminWindow::onUserSelected);

    connect(m_refreshReportsButton, &QPushButton::clicked, this, &AdminWindow::onRefreshReportsClicked);
    connect(m_resolveReportButton, &QPushButton::clicked, this, &AdminWindow::onResolveReportClicked);
    connect(m_reportsTable, &QTableWidget::cellClicked, this, &AdminWindow::onReportSelected);
}

void AdminWindow::loadUsers() {
    m_usersTable->clearContents();
    m_usersTable->setRowCount(0);

    QPair<bool, QList<User>> result = AdminService::getAllUsers();

    if (result.first) {
        m_usersTable->setRowCount(result.second.size());

        for (int i = 0; i < result.second.size(); ++i) {
            const User& user = result.second[i];

            m_usersTable->setItem(i, 0, new QTableWidgetItem(QString::number(user.getId())));
            m_usersTable->setItem(i, 1, new QTableWidgetItem(user.getPhone()));
            m_usersTable->setItem(i, 2, new QTableWidgetItem(user.getUsername()));
            m_usersTable->setItem(i, 3, new QTableWidgetItem(user.getCreatedAt().toString("yyyy-MM-dd hh:mm:ss")));

            QTableWidgetItem* statusItem = new QTableWidgetItem(user.isBanned() ? "Banned" : "Active");
            if (user.isBanned()) {
                statusItem->setBackground(QColor(255, 150, 150));
            } else {
                statusItem->setBackground(QColor(200, 255, 200));
            }
            m_usersTable->setItem(i, 4, statusItem);
        }
    } else {
        QMessageBox::warning(this, "Error", "Failed to load users");
    }
}

void AdminWindow::loadReports() {
    m_reportsTable->clearContents();
    m_reportsTable->setRowCount(0);

    QPair<bool, QList<QPair<User, QString>>> result = AdminService::getReports();

    if (result.first) {
        m_reportsTable->setRowCount(result.second.size());

        for (int i = 0; i < result.second.size(); ++i) {
            const User& reporter = result.second[i].first;
            const QString& details = result.second[i].second;

            m_reportsTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
            m_reportsTable->setItem(i, 1, new QTableWidgetItem(reporter.getUsername()));
            m_reportsTable->setItem(i, 2, new QTableWidgetItem(details));
        }
    } else {
        QMessageBox::warning(this, "Error", "Failed to load reports");
    }
}

void AdminWindow::showReportDetails(int row) {
    if (row < 0 || row >= m_reportsTable->rowCount()) {
        return;
    }

    QString details = m_reportsTable->item(row, 2)->text();
    m_reportDetailsText->setText(details);
}

void AdminWindow::onRefreshUsersClicked() {
    loadUsers();
}

void AdminWindow::onBanUserClicked() {
    if (m_usersTable->selectedItems().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a user to ban");
        return;
    }

    int userId = m_usersTable->item(m_usersTable->currentRow(), 0)->text().toInt();

    QPair<bool, QString> result = AdminService::banUser(userId, "Banned by admin");

    if (result.first) {
        QMessageBox::information(this, "Success", result.second);
        loadUsers();
    } else {
        QMessageBox::warning(this, "Error", result.second);
    }
}

void AdminWindow::onUnbanUserClicked() {
    if (m_usersTable->selectedItems().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a user to unban");
        return;
    }

    int userId = m_usersTable->item(m_usersTable->currentRow(), 0)->text().toInt();

    QPair<bool, QString> result = AdminService::unbanUser(userId);

    if (result.first) {
        QMessageBox::information(this, "Success", result.second);
        loadUsers();
    } else {
        QMessageBox::warning(this, "Error", result.second);
    }
}

void AdminWindow::onViewBannedUsersClicked() {
    QPair<bool, QList<User>> result = AdminService::getBannedUsers();

    if (result.first) {
        m_usersTable->clearContents();
        m_usersTable->setRowCount(0);

        m_usersTable->setRowCount(result.second.size());

        for (int i = 0; i < result.second.size(); ++i) {
            const User& user = result.second[i];

            m_usersTable->setItem(i, 0, new QTableWidgetItem(QString::number(user.getId())));
            m_usersTable->setItem(i, 1, new QTableWidgetItem(user.getPhone()));
            m_usersTable->setItem(i, 2, new QTableWidgetItem(user.getUsername()));
            m_usersTable->setItem(i, 3, new QTableWidgetItem(user.getCreatedAt().toString("yyyy-MM-dd hh:mm:ss")));

            QTableWidgetItem* statusItem = new QTableWidgetItem("Banned");
            statusItem->setBackground(QColor(255, 150, 150));
            m_usersTable->setItem(i, 4, statusItem);
        }
    } else {
        QMessageBox::warning(this, "Error", "Failed to load banned users");
    }
}

void AdminWindow::onUserSelected(int row, int column) {
    Q_UNUSED(column);

    if (row < 0 || row >= m_usersTable->rowCount()) {
        return;
    }

    bool isBanned = m_usersTable->item(row, 4)->text() == "Banned";
    m_banUserButton->setEnabled(!isBanned);
    m_unbanUserButton->setEnabled(isBanned);
}

void AdminWindow::onRefreshReportsClicked() {
    loadReports();
}

void AdminWindow::onResolveReportClicked() {
    if (m_reportsTable->selectedItems().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a report to resolve");
        return;
    }

    int row = m_reportsTable->currentRow();
    QString reportDetails = m_reportsTable->item(row, 2)->text();

    QStringList options = {"resolved", "rejected"};
    bool ok;
    QString action = QInputDialog::getItem(this, "Resolve Report",
                                           "Select action for this report:\n" + reportDetails,
                                           options, 0, false, &ok);

    if (!ok || action.isEmpty()) {
        return;
    }

    // In a real implementation, we would need the actual report ID
    // For this simplified version, we'll use the row index + 1
    int reportId = row + 1;

    QPair<bool, QString> result = AdminService::resolveReport(reportId, action);

    if (result.first) {
        QMessageBox::information(this, "Success", result.second);
        loadReports();
    } else {
        QMessageBox::warning(this, "Error", result.second);
    }
}

void AdminWindow::onReportSelected(int row, int column) {
    Q_UNUSED(column);

    if (row < 0 || row >= m_reportsTable->rowCount()) {
        return;
    }

    showReportDetails(row);
    m_resolveReportButton->setEnabled(true);
}
