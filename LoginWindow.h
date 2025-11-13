// Copyright 2025 MarketSystem
#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include "User.h"
#include "AuthService.h"

class LoginWindow : public QDialog {
    Q_OBJECT

 private:
    QLineEdit* m_phoneInput;
    QLineEdit* m_passwordInput;
    QPushButton* m_loginButton;
    QPushButton* m_registerButton;
    QLabel* m_titleLabel;
    QLabel* m_statusLabel;
    User m_currentUser;  // Store the current logged in user

    void setupUI();
    void setupValidators();
    void setupConnections();
    void updateLoginButtonState();

 public:
    explicit LoginWindow(QWidget* parent = nullptr);
    ~LoginWindow();

    User getCurrentUser() const {
        return m_currentUser;
    }

 private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onPhoneChanged(const QString& text);
    void onPasswordChanged(const QString& text);
    // void updateLoginButtonState();

 signals:
    void loginSuccessful(const User& user);
};
#endif  // LOGINWINDOW_H
