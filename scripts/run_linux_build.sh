#!/bin/bash
# scripts/test.sh
declare -i FAST_COMPILE; FAST_COMPILE=1
#
declare -x MY_BIN_PRO_RES_NAME; MY_BIN_PRO_RES_NAME="WeBookBrowser"; export MY_BIN_PRO_RES_NAME;
declare -x MY_QIF_PACKAGE_URI;  MY_QIF_PACKAGE_URI="packages/com.github.light-wizzard.webookbrowser/data"; export MY_QIF_PACKAGE_URI;
declare -x MY_OS;               MY_OS="Ubuntu";            export MY_OS;
declare -x MY_COMPILER;         MY_COMPILER="Qt";          export MY_COMPILER;
declare -x MY_DEPLOY;           MY_DEPLOY="linuxdeploy";   export MY_DEPLOY; # linuxdeploy linuxdeployqt windeployqt macdeployqt
declare -x MY_MAKE;             MY_MAKE="cmake";           export MY_MAKE; # cmake or qmake
declare -x MY_MAKE_MAJOR;       MY_MAKE_MAJOR="v3.16";     export MY_MAKE_MAJOR;
declare -x MY_MAKE_VERSION;     MY_MAKE_VERSION="3.16.9";  export MY_MAKE_VERSION;
declare -x MY_MAKE_INSTALL;     MY_MAKE_INSTALL="true";    export MY_MAKE_INSTALL;
declare -x MY_ZSYNC_UPDATE;     MY_ZSYNC_UPDATE="false";   export MY_ZSYNC_UPDATE;
declare -x MY_QT_VERSION;       MY_QT_VERSION="6.2.2";     export MY_QT_VERSION;
declare -x MY_QT_INSTALL;       MY_QT_INSTALL="/opt/Qt";   export MY_QT_VERSION;
declare -x MY_QT_UPDATE;        MY_QT_UPDATE="true";       export MY_QT_UPDATE;
declare -x MY_QT_IF_VERSION;    MY_QT_IF_VERSION="4.1.1";  export MY_QT_IF_VERSION
declare -x MY_VS_VERSION;       MY_VS_VERSION="2019";      export MY_VS_VERSION;
declare -x MY_PYTHON_VER;       MY_PYTHON_VER="3.9";       export MY_PYTHON_VER;
declare -x MY_PYTHON_REQUIRED;  MY_PYTHON_REQUIRED="true"; export MY_PYTHON_REQUIRED;
declare -x MY_UPGRADE_OS;       MY_UPGRADE_OS="false";     export MY_UPGRADE_OS; # apt-get upgrade: true or false only
declare -x RUNTIME_LINKAGE;     RUNTIME_LINKAGE="static";  export RUNTIME_LINKAGE;
declare -x COVERITY_BUILD_CANDIDATE; COVERITY_BUILD_CANDIDATE="True"; export COVERITY_BUILD_CANDIDATE;
declare -x GITHUB_USERNAME;     GITHUB_USERNAME="Light-Wizzard";  export GITHUB_USERNAME;
declare -x GITHUB_PROJECT;      GITHUB_PROJECT="WeBookBrowser";   export GITHUB_PROJECT;
declare -x GITHUB_REPO_NAME;  GITHUB_REPO_NAME="Light-Wizzard/WeBookBrowser"; export GITHUB_REPO_NAME;
declare -x PROJECT_ROOT_FOLDER; PROJECT_ROOT_FOLDER="$PWD";   export PROJECT_ROOT_FOLDER;
declare -x PLATFORM;            PLATFORM="x64";                   export PLATFORM;
declare -x CONFIGURATION;       CONFIGURATION="Release";          export CONFIGURATION;

bash -c "scripts/build_linux.sh"
################################ End of File ##################################
