cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(KaosPolosApp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets Sql)

add_executable(KaosPolos src/main.cpp
                         src/database.cpp
                         src/include/database.h
                         src/res/main_res.qrc
                         src/kaospoloswindow.cpp
                         src/include/kaospoloswindow.h
                         src/ui/kaospoloswindow.ui
                         src/addproduk.cpp
                         src/include/addproduk.h
                         src/ui/addproduk.ui
                         src/dockproduk.cpp
                         src/include/dockproduk.h
                         src/produkmodel.cpp
                         src/include/produkmodel.h
                         src/ui/updatestockdialog.ui
                         src/include/updatestockdialog.h
                         src/updatestockdialog.cpp
                         )

target_include_directories(KaosPolos PRIVATE src/include
                                             src/ui)
target_link_libraries(KaosPolos PRIVATE Qt6::Widgets Qt6::Sql)

# add_subdirectory(qttest)

