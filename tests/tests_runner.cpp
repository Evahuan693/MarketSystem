#include <QtTest>
#include "test_authservice_qt.cpp"
#include "test_databasemanager_qt.cpp"
#include "test_integration_qt.cpp"
#include "test_integration_ban_qt.cpp"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    int status = 0;

    AuthServiceTest authTest;
    status |= QTest::qExec(&authTest, argc, argv);

    DatabaseManagerTest dbTest;
    status |= QTest::qExec(&dbTest, argc, argv);

    IntegrationTest integrationTest;
    status |= QTest::qExec(&integrationTest, argc, argv);

    IntegrationBanTest integrationBanTest;
    status |= QTest::qExec(&integrationBanTest, argc, argv);

    return status;
}
