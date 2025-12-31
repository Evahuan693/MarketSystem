TEMPLATE = app
CONFIG += console
QT += core sql testlib
CONFIG += c++17

SOURCES += test_authservice_qt.cpp \
           test_databasemanager_qt.cpp \
           test_integration_qt.cpp \
           test_integration_ban_qt.cpp \
           tests_runner.cpp

# Link project implementation files so tests resolve symbols
SOURCES += ../AuthService.cpp \
           ../DatabaseManager.cpp \
           ../User.cpp

INCLUDEPATH += ../
