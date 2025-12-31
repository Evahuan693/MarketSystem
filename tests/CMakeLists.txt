cmake_minimum_required(VERSION 3.16)
project(MarketSystemTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Sql REQUIRED)
find_package(GTest REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/..)

add_executable(test_runner
    test_authservice.cpp
    test_databasemanager.cpp
)

target_link_libraries(test_runner PRIVATE
    Qt6::Core
    Qt6::Sql
    GTest::gtest_main
)

enable_testing()
add_test(NAME MarketSystemTests COMMAND test_runner)
