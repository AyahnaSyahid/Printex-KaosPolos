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
                         src/ui/produkinfo.ui
                         src/include/produkinfo.h
                         src/produkinfo.cpp
                         src/include/dockkonsumen.h
                         src/dockkonsumen.cpp
                         src/include/konsumenmodel.h
                         src/konsumenmodel.cpp
                         src/ui/addkonsumen.ui
                         src/include/addkonsumen.h
                         src/addkonsumen.cpp
                         src/ui/konsumeninfo.ui
                         src/include/konsumeninfo.h
                         src/konsumeninfo.cpp
                         src/ui/widgetpenjualan.ui
                         src/include/widgetpenjualan.h
                         src/widgetpenjualan.cpp
                         src/ui/pembuatnota.ui
                         src/include/pembuatnota.h
                         src/pembuatnota.cpp
                         src/include/pembuatnotamodel.h
                         src/pembuatnotamodel.cpp
                         )

target_include_directories(KaosPolos PRIVATE src/include
                                             src/ui)
target_link_libraries(KaosPolos PRIVATE Qt6::Widgets Qt6::Sql)

# add_subdirectory(qttest)

