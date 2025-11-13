// Copyright 2025 MarketSystem
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include "User.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

 private:
    User m_currentUser;
    QTabWidget* m_tabWidget;
    QLabel* m_welcomeLabel;
    QLabel* m_roleLabel;

    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupHomeTab();

 public:
    explicit MainWindow(const User& currentUser, QWidget* parent = nullptr);
    ~MainWindow();

 private slots:
    void onLogoutTriggered();
};
#endif  // MAINWINDOW_H
