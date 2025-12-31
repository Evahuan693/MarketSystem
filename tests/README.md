MarketSystem tests

This folder contains Google Test based unit tests for MarketSystem.

Prerequisites
- Qt6 (Core, Sql) development libraries available
- Google Test (GTest) available
- CMake >= 3.16 and a C++17 toolchain

Build (Linux / WSL / CI)
1. mkdir build && cd build
2. cmake .. -DCMAKE_BUILD_TYPE=Debug
3. cmake --build . -- -j
4. ctest --output-on-failure

Build (Windows PowerShell + MSYS/MinGW with CMake)
# from tests directory
mkdir build; cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
ctest --output-on-failure

Notes
- Tests create a test-mode application data directory by calling QStandardPaths::setTestModeEnabled(true).
- Tests use DatabaseManager singleton and will create an SQLite database in the test appdata location. Tests attempt to remove any existing test database before first run.

Coverage (Linux/CI)
# after building with coverage flags (e.g. -fprofile-arcs -ftest-coverage)
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info.cleaned
genhtml coverage.info.cleaned --output-directory out-html
# open out-html/index.html to view coverage report
