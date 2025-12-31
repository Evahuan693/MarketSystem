// Copyright 2025 MarketSystem
#include "LoginWindow.h"
#include <QFont>
#include <QIcon>
#include <QApplication>
#include <QScreen>
#include <QStyle>
#include <QGuiApplication>
#include <utility>

LoginWindow::LoginWindow(QWidget* parent)
    : QDialog(parent), m_currentUser(User()) {
    setWindowTitle("Network Marketplace - Login");
    setWindowIcon(QIcon(":/icons/app_icon.png"));

    // Set modal and window flags
    setModal(true);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // Set size and position
    resize(600, 500);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setupUI();
    setupValidators();
    setupConnections();

    // Center window on screen
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        move((screenGeometry.width() - width()) / 2,
             (screenGeometry.height() - height()) / 2);
    }

    updateLoginButtonState();
}

LoginWindow::~LoginWindow() {
}

void LoginWindow::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 20, 30, 20);
    mainLayout->setSpacing(15);

    // Title
    m_titleLabel = new QLabel("Network Marketplace", this);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setStyleSheet("color: #2c3e50;");

    // Phone input
    QLabel* phoneLabel = new QLabel("Phone Number:", this);
    phoneLabel->setStyleSheet("font-weight: bold;");

    m_phoneInput = new QLineEdit(this);
    m_phoneInput->setPlaceholderText("Enter your phone number (11 digits)");
    m_phoneInput->setFocus();
    m_phoneInput->setStyleSheet("padding: 5px; border: 1px solid #ddd; border-radius: 4px;");

    // Password input
    QLabel* passwordLabel = new QLabel("Password:", this);
    passwordLabel->setStyleSheet("font-weight: bold;");

    m_passwordInput = new QLineEdit(this);
    m_passwordInput->setPlaceholderText("Enter your password (at least 6 characters)");
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setStyleSheet("padding: 5px; border: 1px solid #ddd; border-radius: 4px;");

    // Status label
    m_statusLabel = new QLabel("", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");

    // Buttons
    m_loginButton = new QPushButton("Login", this);
    m_loginButton->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border: none; border-radius: 4px; padding: 8px; font-weight: bold; }"
        "QPushButton:hover { background-color: #2980b9; }"
        "QPushButton:disabled { background-color: #95a5a6; }");
    m_loginButton->setCursor(Qt::PointingHandCursor);
    m_loginButton->setEnabled(false);

    m_registerButton = new QPushButton("Register", this);
    m_registerButton->setStyleSheet(
        "QPushButton { background-color: #2ecc71; color: white; border: none; border-radius: 4px; padding: 8px; font-weight: bold; }"
        "QPushButton:hover { background-color: #27ae60; }");
    m_registerButton->setCursor(Qt::PointingHandCursor);

    // Add widgets to layout
    mainLayout->addWidget(m_titleLabel);
    mainLayout->addWidget(phoneLabel);
    mainLayout->addWidget(m_phoneInput);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(m_passwordInput);
    mainLayout->addWidget(m_statusLabel);
    mainLayout->addWidget(m_loginButton);
    mainLayout->addWidget(m_registerButton);

    setLayout(mainLayout);
}

void LoginWindow::setupValidators() {
    QRegularExpression phoneRegex("^1[3-9]\\d{9}$");
    QRegularExpressionValidator* phoneValidator = new QRegularExpressionValidator(phoneRegex, this);
    m_phoneInput->setValidator(phoneValidator);

    QRegularExpression passwordRegex("^[\\w\\W]{6,}$");
    QRegularExpressionValidator* passwordValidator = new QRegularExpressionValidator(passwordRegex, this);
    m_passwordInput->setValidator(passwordValidator);
}

void LoginWindow::setupConnections() {
    connect(m_loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(m_registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(m_phoneInput, &QLineEdit::textChanged, this, &LoginWindow::onPhoneChanged);
    connect(m_passwordInput, &QLineEdit::textChanged, this, &LoginWindow::onPasswordChanged);
}

void LoginWindow::onLoginClicked() {
    QString phone = m_phoneInput->text().trimmed();
    QString password = m_passwordInput->text();

    m_statusLabel->setText("Logging in...");
    m_loginButton->setEnabled(false);

    // INTENTIONAL: null pointer dereference (will crash if executed)
    QLabel* nullLabel = nullptr;
    nullLabel->setText("this will dereference a null pointer");

    // 首先检查用户是否被Ban
    if (AuthService::isUserBanned(phone)) {
        m_statusLabel->setText("Your account has been banned!\nPlease contact administrator for assistance.");
        m_statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
        m_loginButton->setEnabled(true);
        updateLoginButtonState();
        return;
    }

    QPair<bool, User> result = AuthService::loginUser(phone, password);

    if (result.first) {
        m_currentUser = result.second;  // Store the logged in user
        m_statusLabel->setText("Login successful!");
        m_statusLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
        emit loginSuccessful(m_currentUser);
        accept();  // Close the dialog with Accepted result
    } else {
        // 检查是否是被Ban用户
        if (AuthService::isUserBanned(phone)) {
            m_statusLabel->setText("Your account has been banned!\nPlease contact administrator for assistance.");
            m_statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
        } else {
            m_statusLabel->setText("Invalid phone number or password");
            m_statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
        }
        m_loginButton->setEnabled(true);
        updateLoginButtonState();
    }
}

void LoginWindow::onRegisterClicked() {
    QString phone = m_phoneInput->text().trimmed();
    QString password = m_passwordInput->text();

    QPair<bool, QString> validation = AuthService::validateUserInput(phone, password);
    if (!validation.first) {
        QMessageBox::warning(this, "Validation Error", validation.second);
        return;
    }

    QPair<bool, User> result = AuthService::registerUser(phone, password);

    if (result.first) {
        QMessageBox::information(this, "Registration Successful",
                                 "Your account has been created successfully!\nPlease login with your credentials.");
        m_statusLabel->setText("Registration successful. Please login.");
        m_statusLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
    } else {
        QMessageBox::warning(this, "Registration Failed",
                             "Failed to register account. Phone number may already be registered.");
        m_statusLabel->setText("Registration failed. Phone may be taken.");
        m_statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold;");
    }
}

void LoginWindow::onPhoneChanged(const QString& text) {
    Q_UNUSED(text);
    updateLoginButtonState();
}

void LoginWindow::onPasswordChanged(const QString& text) {
    Q_UNUSED(text);
    updateLoginButtonState();
}

void LoginWindow::updateLoginButtonState() {
    bool isValidPhone = m_phoneInput->hasAcceptableInput() && !m_phoneInput->text().trimmed().isEmpty();
    bool isValidPassword = m_passwordInput->hasAcceptableInput() && !m_passwordInput->text().isEmpty();
    m_loginButton->setEnabled(isValidPhone && isValidPassword);
}
