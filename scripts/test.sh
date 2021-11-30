#!/bin/bash
# scripts/test.sh
export MY_BIN_PRO_RES_NAME="WeBookClient";
export MY_QIF_PACKAGE_URI="packages/com.lightwizzard.webookclient/data";
export MY_OS="Ubuntu";
export MY_COMPILER="Qt";
export MY_MAKE="qmake"; # cmake
export MY_ZSYNC_UPDATE="false";
export MY_QT_VERSION="5.15.2";
export MY_VS_VERSION="2019";
export MY_QT_IF_VERSION="4.1.1";
export MY_PYTHON_VER="3.9";
export MY_PYTHON_REQUIRED="true";
export MY_UPGRADE_OS="false"; # apt-get upgrade: true or false only
export RUNTIME_LINKAGE="static";
export COVERITY_BUILD_CANDIDATE="True";
export GITHUB_USERNAME="Light-Wizzard";
export GITHUB_PROJECT="WeBookClient";
export APPVEYOR_REPO_NAME="Light-Wizzard/WeBookClient";
export APPVEYOR_BUILD_FOLDER="$PWD";

bash -c "scripts/build_script.sh"
