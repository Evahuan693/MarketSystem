// Copyright 2025 MarketSystem
#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>
#include <QDateTime>
#include <QStatusBar>  // 添加QStatusBar头文件
#include <QTimer>      // 添加QTimer头文件
#include <QLabel>      // 添加QLabel头文件
#include <utility>
MainWindow::MainWindow(const User& currentUser, QWidget* parent)
    : QMainWindow(parent), m_currentUser(currentUser) {
    setWindowTitle(QString("Network Marketplace - %1").arg(currentUser.getUsername()));
    setWindowIcon(QIcon(":/icons/app_icon.png"));

    // Set window size and position
    resize(1000, 700);

    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        move((screenGeometry.width() - width()) / 2,
             (screenGeometry.height() - height()) / 2);
    }

    setupUI();
    setupMenuBar();
    setupStatusBar();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    // Create central widget and layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // Create tab widget
    m_tabWidget = new QTabWidget(this);

    // Setup tabs
    setupHomeTab();

    mainLayout->addWidget(m_tabWidget);
    setCentralWidget(centralWidget);
}

void MainWindow::setupHomeTab() {
    QWidget* homeTab = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(homeTab);

    // Welcome message
    m_welcomeLabel = new QLabel(QString("Welcome, %1!").arg(m_currentUser.getPhone()), homeTab);
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    m_welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");

    // User role
    m_roleLabel = new QLabel(m_currentUser.isAdmin() ? "Role: Administrator" : "Role: User", homeTab);
    m_roleLabel->setAlignment(Qt::AlignCenter);
    m_roleLabel->setStyleSheet("font-size: 18px; color: #7f8c8d;");

    // Message
    QLabel* messageLabel = new QLabel("This is a simplified version of the Network Marketplace system.\n"
                                      , homeTab);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);

    // Add widgets to layout
    layout->addStretch();
    layout->addWidget(m_welcomeLabel);
    layout->addWidget(m_roleLabel);
    layout->addWidget(messageLabel);
    layout->addStretch();

    m_tabWidget->addTab(homeTab, "Home");
}

void MainWindow::setupMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);

    // File menu
    QMenu* fileMenu = menuBar->addMenu("&File");
    QAction* logoutAction = new QAction("&Logout", this);
    logoutAction->setShortcut(QKeySequence("Ctrl+L"));
    connect(logoutAction, &QAction::triggered, this, &MainWindow::onLogoutTriggered);

    fileMenu->addAction(logoutAction);

    // View menu
    QMenu* viewMenu = menuBar->addMenu("&View");
    QAction* refreshAction = new QAction("&Refresh", this);
    refreshAction->setShortcut(QKeySequence::Refresh);
    connect(refreshAction, &QAction::triggered, [this]() { /* Refresh logic */ });

    viewMenu->addAction(refreshAction);

    // Help menu
    QMenu* helpMenu = menuBar->addMenu("&Help");
    QAction* aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, [this]() {
        QMessageBox::about(this, "About Network Marketplace",
                           "Network Marketplace v1.0\n"
                           "A simplified C++/Qt application for user management.\n"
                           "Developed for Software Engineering Course Experiment 3.");
    });

    helpMenu->addAction(aboutAction);

    setMenuBar(menuBar);
}

void MainWindow::setupStatusBar() {
    // 创建状态栏
    QStatusBar* statusBar = new QStatusBar(this);

    // 创建用户标签
    QLabel* userLabel = new QLabel(QString("User: %1").arg(m_currentUser.getUsername()), statusBar);
    userLabel->setStyleSheet("font-weight: bold; color: #2c3e50;");

    // 创建时间标签
    QLabel* timeLabel = new QLabel(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), statusBar);
    timeLabel->setStyleSheet("color: #7f8c8d;");

    // 添加部件到状态栏
    statusBar->addWidget(userLabel);
    statusBar->addPermanentWidget(timeLabel);

    // 设置状态栏
    setStatusBar(statusBar);

    // 创建定时器更新时间
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [timeLabel]() {
        timeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    });
    timer->start(1000);
}

void MainWindow::onLogoutTriggered() {
    close();
}
