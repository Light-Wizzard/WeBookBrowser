cmake_minimum_required(VERSION 3.2)
# I tried to put stuff you need to exit at the top
project(WeBookBrowser VERSION 0.1.0.0 DESCRIPTION "WeBookBrowser" LANGUAGES C CXX)
# Set this to your URL
set(MY_URL "light-wizzard.github.io")
# ON OFF
set(MY_QML OFF)
# Set Target Name
set(MY_TARGET ${CMAKE_PROJECT_NAME})
# If you have a project that has an SQL verion for Desktops, and No SQL for WebAssempy set both below comments
set(USE_SQL_FLAG OFF)
#
add_compile_definitions(USE_SQL_FLAG)
# UI Files
set(MY_UI_FILES
    src/MainWindow.ui
    src/certificateerrordialog.ui
    src/passworddialog.ui
)
# TS Files
set(MY_TS_FILES
    translations/${MY_TARGET}_ar.ts
    translations/${MY_TARGET}_de.ts
    translations/${MY_TARGET}_en.ts
    translations/${MY_TARGET}_es.ts
    translations/${MY_TARGET}_fr.ts
    translations/${MY_TARGET}_it.ts
    translations/${MY_TARGET}_ja.ts
    translations/${MY_TARGET}_no.ts
    translations/${MY_TARGET}_ru.ts
    translations/${MY_TARGET}_sv.ts
    translations/${MY_TARGET}_zh_CN.ts
)
# QM Files
set(MY_QM_FILES
    translations/${MY_TARGET}_ar.qm
    translations/${MY_TARGET}_de.qm
    translations/${MY_TARGET}_en.qm
    translations/${MY_TARGET}_es.qm
    translations/${MY_TARGET}_fr.qm
    translations/${MY_TARGET}_it.qm
    translations/${MY_TARGET}_ja.qm
    translations/${MY_TARGET}_no.qm
    translations/${MY_TARGET}_ru.qm
    translations/${MY_TARGET}_sv.qm
    translations/${MY_TARGET}_zh_CN.qm
)
# Header Files
set(HEADERS
    src/BookmarkMenu.h
    src/BookmarkTreeWidgetView.h
    src/DownloadList.h
    src/DownloadManagerWidget.h
    src/HelpView.h
    src/MainWindow.h
    src/WebPage.h
    src/WebView.h
    src/XbelMenuReader.h
    src/XbelTreeReader.h
    src/XbelTreeWriter.h
    src/MyLanguageData.h
    src/MyLanguageModel.h
    src/MyOrgSettings.h
    src/MySystemInformation.h
    src/qaesencryption.h
)
# Source Files
set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/BookmarkMenu.cpp
    src/BookmarkTreeWidgetView.cpp
    src/DownloadList.cpp
    src/DownloadManagerWidget.cpp
    src/HelpView.cpp
    src/WebPage.cpp
    src/WebView.cpp
    src/XbelMenuReader.cpp
    src/XbelTreeReader.cpp
    src/XbelTreeWriter.cpp
    src/MyLanguageData.cpp
    src/MyLanguageModel.cpp
    src/MyOrgSettings.cpp
    src/MySystemInformation.cpp
    src/qaesencryption.cpp
    ${MY_TARGET}.qrc
    ${HEADERS}
    #${MY_TS_FILES}
)
#
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2 -msse3 -msse4")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
# All Qt includes
#3DAnimation             Help                    QuickWidgets
#3DCore                  LinguistTools           RemoteObjects
#3DExtras                Location                RepParser
#3DInput                 Multimedia              Scxml
#3DLogic                 MultimediaWidgets       Sensors
#3DQuick                 Network                 SerialPort
#3DQuickAnimation        Nfc                     Sql
#3DQuickExtras           OpenGL                  Svg
#3DQuickInput            OpenGLExtensions        Test
#3DQuickRender           Positioning             TextToSpeech
#3DQuickScene2D          PositioningQuick        UiPlugin
#3DRender                PrintSupport            UiTools
#AndroidExtras           Qml                     WebChannel
#Bluetooth               Quick                   WebSockets
#Concurrent              QuickCompiler           WebView
#Core                    QuickControls2          Widgets
#Gamepad                 QuickTemplates2         Xml
#Gui                     QuickTest               XmlPatterns        WebEngineWidgets
set(QT_FEATURE_intelcet ON)
# Get Qt
set(QT_VERSION_MIN "6.2.2")
# Find packages
# WebEngineQuick WebEngineCore
if(DEFINED USE_SQL_FLAG)
    if(MY_QML)
        # QML
        find_package(QT NAMES Qt6 ${QT_VERSION_MIN} COMPONENTS Core Sql Qml Quick QuickControls2 PrintSupport Network Multimedia Concurrent LinguistTools Xml Svg QuickWidgets WebChannel WebEngineWidgets REQUIRED)
        find_package(Qt6 COMPONENTS Core Sql Qml Quick QuickControls2 PrintSupport Network Multimedia Concurrent LinguistTools Xml Svg QuickWidgets WebChannel WebEngineWidgets REQUIRED)
    else()
        # C++
        find_package(QT NAMES Qt6 ${QT_VERSION_MIN} COMPONENTS Core Gui Widgets Sql PrintSupport Network Multimedia Concurrent LinguistTools Xml Svg QuickWidgets WebChannel WebEngineWidgets REQUIRED)
        find_package(Qt6 COMPONENTS Core Gui Widgets Sql PrintSupport Network Multimedia Concurrent LinguistTools Xml Svg QuickWidgets WebChannel WebEngineWidgets REQUIRED)
    endif()
else()
    if(MY_QML)
        # QML
        find_package(QT NAMES Qt6 ${QT_VERSION_MIN} COMPONENTS Core Qml Quick QuickControls2 PrintSupport Network Multimedia Concurrent LinguistTools Xml Svg QuickWidgets WebChannel WebEngineWidgets REQUIRED)
        find_package(Qt6 COMPONENTS Core Qml Quick QuickControls2 PrintSupport Network Multimedia Concurrent LinguistTools Xml Svg QuickWidgets WebChannel WebEngineWidgets REQUIRED)
    else()
        # C++
        find_package(QT NAMES Qt6 ${QT_VERSION_MIN} COMPONENTS Core Gui Widgets PrintSupport Network Multimedia Concurrent LinguistTools Xml Svg QuickWidgets WebChannel WebEngineWidgets REQUIRED)
        find_package(Qt6 COMPONENTS Core Gui Widgets PrintSupport Network Multimedia Concurrent LinguistTools Xml Svg QuickWidgets WebChannel WebEngineWidgets REQUIRED)
    endif()
endif()
#
#
#QT5_WRAP_UI(PROJECT_SOURCES ${MY_UI_FILES})
#qt6_wrap_ui(PROJECT_SOURCES ${MY_UI_FILES})
qt_wrap_ui(PROJECT_SOURCES ${MY_UI_FILES})
#
find_program(Python3_EXECUTABLE NAMES python3 HINTS $ENV{PYTHON3_PATH})
if(NOT Python3_EXECUTABLE)
    find_package(Python3 3.9)
endif()
find_package(Ninja 1.8.0)
# You can turn these ON OFF individually for Translations
option(DO_LUPDATE "Create TS files lupdate, turn off after you created them" ON)
option(DO_LRELEASE "Create QM files lrelease, turn off after you created them" ON)
#
# Include GNUInstallDirs to get canonical paths
include(GNUInstallDirs)
# Set include current directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Auto
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# The lines below this should not need to be altered from one project to the next
# Set Properties and add executable
qt_add_executable(${MY_TARGET} MANUAL_FINALIZATION ${PROJECT_SOURCES})
# Define target properties for Android with Qt 6 as:
set_property(TARGET ${MY_TARGET} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

#qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${MY_TS_FILES})
# Set Definitions and Library
target_compile_definitions(${MY_TARGET} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
# Libraries
#  Qt6::WebEngineQuick Qt6::WebEngineCore
if(DEFINED USE_SQL_FLAG)
    if(MY_QML)
        # QML
        target_link_libraries(${MY_TARGET} PRIVATE Qt6::Core Qt6::Sql Qt6::PrintSupport Qt6::Network Qt6::Multimedia Qt6::Quick Qt6::QuickControls2 Qt6::Xml Qt6::QuickWidgets Qt6::Svg Qt6::WebChannel Qt6::WebEngineWidgets)
    else()
        # C++
        target_link_libraries(${MY_TARGET} PRIVATE Qt6::Core Qt6::Sql Qt6::Gui Qt6::Widgets Qt6::PrintSupport Qt6::Network Qt6::Multimedia Qt6::Xml Qt6::QuickWidgets Qt6::Svg Qt6::WebChannel Qt6::WebEngineWidgets)
    endif()
else()
    if(MY_QML)
        # QML
        target_link_libraries(${MY_TARGET} PRIVATE Qt6::Core Qt6::PrintSupport Qt6::Network Qt6::Multimedia Qt6::Quick Qt6::QuickControls2 Qt6::Xml Qt6::QuickWidgets Qt6::Svg Qt6::WebChannel Qt6::WebEngineWidgets)
    else()
        # C++
        target_link_libraries(${MY_TARGET} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::PrintSupport Qt6::Network Qt6::Multimedia Qt6::Xml Qt6::QuickWidgets Qt6::Svg Qt6::WebChannel Qt6::WebEngineWidgets)
    endif()
endif()
# Set Target Properties to include MACOS
set_target_properties(${MY_TARGET} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER ${MY_URL}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/macos/info.plist
)
# genearats ts and qm file searching recursively THIS_RC_DIR
function(generate_translations THIS_CUSTOM_TARGET THIS_TS_DIR THIS_TS_FILES THIS_RC_DIR)
    # lupdate
    if(DO_LUPDATE)
        message(STATUS "run lupdate")
        set(UPADTE_TS_TARGET_NAME ${THIS_CUSTOM_TARGET}_ts)
        add_custom_target(${UPADTE_TS_TARGET_NAME}
            COMMAND ${Qt5_LUPDATE_EXECUTABLE} -recursive ${THIS_RC_DIR} -ts ${THIS_TS_FILES}
            WORKING_DIRECTORY ${THIS_TS_DIR})
    endif()
    # lrelease
    if(DO_LRELEASE)
        message(STATUS "run lrelease")
        set(UPADTE_QM_TARGET_NAME ${THIS_CUSTOM_TARGET}_qm)
        add_custom_target(${UPADTE_QM_TARGET_NAME}
            COMMAND ${Qt5_LRELEASE_EXECUTABLE} ${THIS_TS_FILES}
            WORKING_DIRECTORY ${THIS_TS_DIR})
        add_dependencies(${UPADTE_QM_TARGET_NAME} ${UPADTE_TS_TARGET_NAME} )
        add_dependencies(${THIS_CUSTOM_TARGET} ${UPADTE_QM_TARGET_NAME})
    endif()
endfunction()
# Set variables to pass in
set(MY_TS_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(MY_SOURCE_LOOKUP_DIR ${CMAKE_CURRENT_SOURCE_DIR})
# Generate Tralations
#generate_translations(${MY_TARGET} "${MY_TS_DIR}" "${MY_TS_FILES}" "${MY_SOURCE_LOOKUP_DIR}")
# Install QM files if not APPLE
if(UNIX AND NOT APPLE)
    install(FILES ${MY_QM_FILES} DESTINATION ${CMAKE_SOURCE_DIR}/translations)
endif()
# Qt 6 stuff
qt_import_qml_plugins(${MY_TARGET})
qt_finalize_executable(${MY_TARGET})
# Install Target Destinations
install(TARGETS ${MY_TARGET}
  RUNTIME DESTINATION /usr/bin
  BUNDLE DESTINATION  /usr/bin
  LIBRARY DESTINATION /usr/lib
  ARCHIVE DESTINATION /usr/lib
)
###### End of File ######
