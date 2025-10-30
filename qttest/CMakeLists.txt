# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(KDBTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Test Widgets)

# Tambahkan resource
qt_add_resources(RESOURCES ../src/res/main_res.qrc)

add_library(Database STATIC
    ../src/database.cpp
    ../src/include/database.h
    ${RESOURCES}
)

target_link_libraries(Database Qt6::Core Qt6::Sql Qt6::Widgets)
target_include_directories(Database PUBLIC ../src/include)

# --- Unit Test ---
enable_testing()

add_executable(tst_database
    tst_database.cpp
)

add_executable(claude_test
    claude_test.cpp
)

target_link_libraries(tst_database
    Database
    Qt6::Test
    Qt6::Core
    Qt6::Sql
)

target_link_libraries(claude_test
    Database
    Qt6::Test
    Qt6::Core
    Qt6::Sql
)

add_test(NAME DatabaseTest COMMAND tst_database)
add_test(NAME ClaudeTest COMMAND claude_test)