cmake_minimum_required(VERSION 3.16)

project(vkcloud VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

include_directories(${CMAKE_SOURCE_DIR})

# Include FetchContent module
include(FetchContent)
# Configure FetchContent to retrieve spdlog
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.15.1  # Specify the desired version
)
# Make spdlog available
FetchContent_MakeAvailable(spdlog)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(vkcloud
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        logindialog.h logindialog.cpp logindialog.ui
        registerdialog.h registerdialog.cpp registerdialog.ui
        resource.qrc
        global.h global.cpp
        singleton.h
        httpmanager.h httpmanager.cpp
        timerbutton.h timerbutton.cpp
        configurationmanager.h configurationmanager.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET vkcloud APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(vkcloud SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(vkcloud
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# include(cmake/spdlog.cmake)
# add_dependencies(vkcloud spdlog_project)
# target_link_libraries(vkcloud PRIVATE spdlog)
# target_include_directories(vkcloud PRIVATE ${SPDLOG_INSTALL_DIR}/include)
# Link spdlog
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

# Optionally, define SPDLOG_ACTIVE_LEVEL to control logging level at compile time
target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG)

target_link_libraries(vkcloud PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.vkcloud)
endif()
set_target_properties(vkcloud PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS vkcloud
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(vkcloud)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Linux-specific configurations
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  file(COPY config.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  file(COPY config.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.app/Contents/MacOS)
else()
    message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()



# set(CONFIG_FILE "${CMAKE_SOURCE_DIR}/config.ini")

