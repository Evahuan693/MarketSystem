#include <QCoreApplication>
#include <QString>
#include <QByteArray>
#include <QTextStream>
#include <QVariantList>
#include <iostream>
#include "../src/AuthService.h"
#include "../src/DatabaseManager.h"

static void processLine(const QString &line) {
    QStringList parts = line.split(' ', Qt::SkipEmptyParts);
    if (parts.isEmpty()) return;
    QString cmd = parts[0].toUpper();

    if (cmd == "REGISTER") {
        QString phone = parts.value(1);
        QString pass = parts.value(2);
        QString user = parts.value(3);
        AuthService::registerUser(phone, pass, user);
    } else if (cmd == "LOGIN") {
        QString phone = parts.value(1);
        QString pass = parts.value(2);
        AuthService::loginUser(phone, pass);
    } else if (cmd == "HASH") {
        QString pass = parts.value(1);
        (void)AuthService::hashPassword(pass);
    } else if (cmd == "VALIDATE") {
        QString phone = parts.value(1);
        QString pass = parts.value(2);
        QString user = parts.value(3);
        (void)AuthService::validateUserInput(phone, pass, user);
    } else if (cmd == "ISBANNED") {
        QString phone = parts.value(1);
        (void)AuthService::isUserBanned(phone);
    } else if (cmd == "ISBANNEDID") {
        int id = parts.value(1).toInt();
        (void)AuthService::isUserBannedById(id);
    } else if (cmd == "SQL") {
        // join remaining as a single SQL
        QString sql = line.mid(cmd.length()).trimmed();
        DatabaseManager::getInstance().executeQuery(sql, {});
    } else if (cmd == "LASTID") {
        (void)DatabaseManager::getInstance().getLastInsertId();
    }
}

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    // Ensure database initialized
    DatabaseManager::getInstance();

    // Read all stdin
    QByteArray all;
    const int bufSize = 1 << 20;
    char *buf = (char*)malloc(bufSize);
    if (!buf) return 0;
    ssize_t r;
    while ((r = fread(buf, 1, bufSize, stdin)) > 0) {
        all.append(buf, (int)r);
    }
    free(buf);

    QString data = QString::fromUtf8(all);
    QStringList lines = data.split('\n');
    for (const QString &ln : lines) processLine(ln);
    return 0;
}
