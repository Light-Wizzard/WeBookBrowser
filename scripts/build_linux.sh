#!/bin/bash
#
# Last Update: 4 December 2021
#
# I use shell check, delete the ? to run it, but leave that in this files so it does not fail when it sees it.
# shell?check -x scripts/build_script.sh
#
# Original Code is from: https://github.com/linuxdeploy/QtQuickApp TheAssassin
#
# This file is Open Source and I tried my best to make it cut and paste,
# so I am adding the Environment Variables here as well as the OS installer.
#
################################ cmake ###############################
declare -xg CMAKE_PREFIX_PATH;
#
if [ -z "$CMAKE_PREFIX_PATH" ]; then
    CMAKE_PREFIX_PATH="$(which cmake)";
fi
echo "*** build_script Linux $MY_OS Linux Deploy is $MY_DEPLOY using make $MY_MAKE $MY_MAKE_VERSION CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH installed THE_MAKE_VERSION=$THE_MAKE_VERSION";
#
# Debug Information, not always a good idea when not THE_DEBUGGING, and thanks to the TheAssassin, this is now working.
# These are the setting you might want to change
declare -ix THE_DEBUGGING;         THE_DEBUGGING=0;          # Set 1=True and 0=False
declare -ix THE_SHOW_PATH;         THE_SHOW_PATH=1;          # Set 1=True and 0=False
declare -ix THE_EXIT_ON_UNDEFINED; THE_EXIT_ON_UNDEFINED=0;  # Set 1=True and 0=False
declare -i THE_GET_QT_FOLDER_INFO; THE_GET_QT_FOLDER_INFO=1;
# Below should be agnostic
# I use a Variable so I can turn it off on exit
if [ "${THE_DEBUGGING}" -eq 1 ]; then set -x; fi
# Exit on error
set -e;
#
# If not defined it will use this as a default
if [ -z "${MY_BIN_PRO_RES_NAME+x}" ]; then
    echo -e "Add MY_BIN_PRO_RES_NAME to your Settings Environment Variable with a value from Github value for Binary, pro, and resource Name ";
    if [ "${THE_EXIT_ON_UNDEFINED}" -eq 1 ]; then exit 1; fi
fi
# GITHUB_REPO_NAME should always have your GITHUB_USERNAME as the first part / GITHUB_PROJECT, so I split them to use later.
if [ -z "${GITHUB_USERNAME+x}" ] || [ -z "${GITHUB_PROJECT+x}" ]; then
    declare THE_OLD_IFS; 
    THE_OLD_IFS="$IFS"; IFS='/'; 
    read -ra repo_parts <<< "$GITHUB_REPO_NAME"; IFS="$THE_OLD_IFS";
    declare -x GITHUB_PROJECT;
    GITHUB_USERNAME="${repo_parts[0]}";  GITHUB_PROJECT="${repo_parts[1]}";
fi
# Set our Artifacts for later
#
export THE_ARTIFACT_APPIMAGE="${MY_BIN_PRO_RES_NAME}-x86_64.AppImage";
#export ARTIFACT_ZSYNC="${MY_BIN_PRO_RES_NAME}-x86_64.AppImage.zsync";
export THE_ARTIFACT_QIF="${MY_BIN_PRO_RES_NAME}-Linux-Installer";
# use RAM disk if possible (as in: not building on CI system, and RAM disk is available)
declare THE_TEMP_BASE;
if [ "$CI" == "" ] && [ -d "/dev/shm" ]; then THE_TEMP_BASE="/dev/shm"; else THE_TEMP_BASE="/tmp"; fi
#
# building in temporary directory to keep system clean
declare -x THE_BUILD_DIR; 
THE_BUILD_DIR="$(mktemp -d -p "$THE_TEMP_BASE" "${MY_BIN_PRO_RES_NAME}-build-XXXXXX")";
#
echo -e "Make Temp Build Folder: $THE_BUILD_DIR";
#
# make sure to clean up build dir, even if errors occur
function cleanup()
{
    if [ -d "$THE_BUILD_DIR" ]; then rm -rf "$THE_BUILD_DIR"; fi
    if [ "${THE_DEBUGGING}" -eq 1 ]; then set +x; fi
}
trap "cleanup; exit;" SIGINT SIGTERM
#trap cleanup EXIT;
#
# store repo root as variable
declare THE_REPO_ROOT; THE_REPO_ROOT="$(readlink -f "$(dirname "$(dirname "$0")")")";
declare THE_OLD_CWD; THE_OLD_CWD="$(readlink -f .)";
#
# switch to build dir
pushd "$THE_BUILD_DIR";
# Make AppDir folder at the THE_BUILD_DIR level, I should not need to do this normally, but I am not able to get cmake to work
#if [ -d "AppDir" ]; then rm -r AppDir; fi
#mkdir AppDir;
# x86
declare QT_BASE_DIR;
if [[ "$PLATFORM" == "x86" ]]; then
    QT_BASE_DIR="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64";
    # Matrix does not show a gcc_32 or 86
    PATH="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin:${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/lib:${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/include:$PATH";
    export PATH="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin:${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/lib:${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/include:$PATH";
    PATH="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/clang_64/bin:$PATH";
    export PATH="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/clang_64/bin:$PATH";
    if [ ! -d "${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin" ]; then
        echo "Qt x86 not found: ${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin";
        ls "${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/";
    fi
    if [ ! -f "${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin/qmake" ]; then echo "Qt x86 qmake not found: ${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin/qmake"; fi
    if [[ "$MY_PYTHON_REQUIRED" == "true" ]]; then
        PATH="/usr/lib/python${MY_PYTHON_VER}:/usr/include/python${MY_PYTHON_VER}:$PATH";
        export PATH="/usr/lib/python${MY_PYTHON_VER}:/usr/include/python${MY_PYTHON_VER}:$PATH";
        # source ${HOME}/venv${MY_PYTHON_VER}/bin/activate
        # export PATH=${HOME}/venv${MY_PYTHON_VER}:${HOME}/venv${MY_PYTHON_VER}/bin:${HOME}/venv${MY_PYTHON_VER}/lib:$PATH
        if [ ! -d "/usr/lib/python${MY_PYTHON_VER}" ]; then    echo "Python x86 not found: /usr/lib/python${MY_PYTHON_VER}"; fi
        if [ ! -d "/usr/include/python${MY_PYTHON_VER}" ]; then echo "Python x86 include not found: /usr/lib/python${MY_PYTHON_VER}"; fi
    fi
fi
# x64
if [[ "$PLATFORM" == "x64" ]]; then
    QT_BASE_DIR="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64";
    PATH="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin:${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/lib:${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/include:$PATH";
    export PATH="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin:${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/lib:${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/include:$PATH";
    PATH="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/clang_64/bin:$PATH";
    export PATH="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/clang_64/bin:$PATH";
    # Check Qt
    if [ ! -d "${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin" ]; then echo "Qt x64 not found: ${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin"; fi
    if [ ! -f "${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin/qmake" ]; then echo "Qt x64 qmake not found: ${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/bin/qmake"; fi
    if [[ "$MY_PYTHON_REQUIRED" == "true" ]]; then
        PATH="/usr/lib/python${MY_PYTHON_VER}:/usr/include/python${MY_PYTHON_VER}:$PATH";
        export PATH="/usr/lib/python${MY_PYTHON_VER}:/usr/include/python${MY_PYTHON_VER}:$PATH";
        # Check Python
        # source ${HOME}/venv${MY_PYTHON_VER}/bin/activate
        # export PATH=${HOME}/venv${MY_PYTHON_VER}:${HOME}/venv${MY_PYTHON_VER}/bin:${HOME}/venv${MY_PYTHON_VER}/lib:$PATH
        if [ ! -d "/usr/lib/python${MY_PYTHON_VER}" ]; then     echo "Python x64 not found: /usr/lib/python${MY_PYTHON_VER}"; fi
        if [ ! -d "/usr/include/python${MY_PYTHON_VER}" ]; then echo "Python x64 include not found: /usr/lib/python${MY_PYTHON_VER}"; fi
    fi
fi
#
QTDIR="$QT_BASE_DIR";
export QTDIR;
#
LD_LIBRARY_PATH="${QT_BASE_DIR}/lib:${QT_BASE_DIR}/plugins:${THE_REPO_ROOT}/build/AppDir/usr/lib/:AppDir:$LD_LIBRARY_PATH";
export LD_LIBRARY_PATH;
#
if [ "${MY_QT_VERSION:0:1}" == "5" ]; then
    export PKG_CONFIG_PATH="$QT_BASE_DIR/lib/pkgconfig:$PKG_CONFIG_PATH";
fi
#
if [ -n "$QT_INSTALL_BINS" ]; then
    PATH="${QT_INSTALL_BINS}:$PATH";
fi
#
if [ "${THE_SHOW_PATH}" -eq 1 ]; then echo "PATH=$PATH"; fi
# cmake
declare -gx DESTDIR;
DESTDIR=AppDir;
# qmake
declare -gx INSTALL_ROOT;
INSTALL_ROOT=AppDir;
#
mkdir -p AppDir;
mkdir -p AppDir/usr;
mkdir -p AppDir/usr/bin;
mkdir -p AppDir/usr/share;
mkdir -p AppDir/usr/share/applications;
mkdir -p AppDir/usr/share/metainfo;
mkdir -p AppDir/usr/share/icons;
mkdir -p AppDir/usr/share/icons/hicolor;
mkdir -p AppDir/usr/share/icons/hicolor/256x256;
mkdir -p AppDir/usr/share/icons/hicolor/256x256/apps;
mkdir -p AppDir/usr/share/icons/scalable;
mkdir -p AppDir/usr/share/icons/scalable/apps;
cp -rf "${THE_REPO_ROOT}/usr" ./AppDir;
# FIXME are these required?
if [ ! -d "Vulkan-Headers" ]; then
    git clone https://github.com/KhronosGroup/Vulkan-Headers
fi
declare -x MY_VULKAN; MY_VULKAN="${THE_BUILD_DIR}/Vulkan-Headers";
Vulkan_INCLUDE_DIR="${THE_BUILD_DIR}/Vulkan-Headers";
export Vulkan_INCLUDE_DIR;
#
Qt6WebChannel_DIR="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/lib";
export Qt6WebChannel_DIR;
Qt6WebEngineWidgets_DIR="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/lib";
export Qt6WebEngineWidgets_DIR;
Qt6Multimedia_DIR="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/lib";
export Qt6Multimedia_DIR;
# FIXME what goes here, path or file
QTWEBENGINEPROCESS_PATH="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/libexec/:${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/libexec/QtWebEngineProcess";
export QTWEBENGINEPROCESS_PATH;
DIR_QT_LIBRARY_DATA="${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/resources/";
export DIR_QT_LIBRARY_DATA;
#
echo "Vars: Vulkan_INCLUDE_DIR=$Vulkan_INCLUDE_DIR Qt6WebChannel_DIR=$Qt6WebChannel_DIR Qt6WebEngineWidgets_DIR=$Qt6WebEngineWidgets_DIR Qt6Multimedia_DIR=$Qt6Multimedia_DIR"
#-- Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR) 
#-- Could NOT find Qt6WebChannel (missing: Qt6WebChannel_DIR)
#-- Could NOT find Qt6WebEngineWidgets (missing: Qt6WebEngineWidgets_DIR)
#-- Could NOT find Qt6Multimedia (missing: Qt6Multimedia_DIR)
#
if [ "$THE_GET_QT_FOLDER_INFO" -eq 1 ]; then
    echo "#####################################################################";
#    echo "Listing: ${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/lib";
#    ls "${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/lib";
#    echo "Listing: ${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/include";
#    ls "${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/include";
#    echo "Listing: ${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/modules";
#    ls "${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/modules";
#    echo "Listing: ${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/plugins";
#    ls "${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/plugins";
    echo "Listing: ${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/libexec";
    ls "${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/libexec";
    #
    if [ -f "${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/libexec/qt.conf" ]; then
        echo "qt.conf found";
    else
        echo "qt.conf not found";
    fi
    #
    if [ -f "${MY_QT_INSTALL}/Qt/Tools/CMake/bin/cmake" ]; then
        echo "Qt cmake found";
    else
        echo "Qt cmake not found";
    fi
    #
    if [ -d "${MY_QT_INSTALL}/Qt/Tools/QtInstallerFramework" ]; then
        echo "Qt QtInstallerFramework found";
    else
        echo "Qt QtInstallerFramework not found";
    fi
    echo "#####################################################################";
fi
#
if [ "${MY_MAKE}" == "qmake" ]; then
    echo "qmake build";
    qmake --version; 
    qmake "${THE_REPO_ROOT}";
    # build project and install files into AppDir
    make -j"$(nproc)";
    make test;
    make INSTALL_ROOT=AppDir install
    #INSTALL_ROOT=AppDir make install
else
    echo "cmake build";
    # cmake
    cmake --version;
    # cmake "${THE_REPO_ROOT}" -G "Unix Makefiles" -DBUILD_SHARED_LIBS:BOOL=ON -DCMAKE_BUILD_TYPE="${CONFIGURATION}" -DCMAKE_INSTALL_PREFIX="/usr";
    cmake "${THE_REPO_ROOT}" -G "Unix Makefiles" -DVulkan_INCLUDE_DIRS="$MY_VULKAN" -DVulkan_LIBRARIES="$MY_VULKAN" -DQX_QT_DIR="$QT_BASE_DIR" -D_QX_ENABLE_QT_GUI=1 -D_QX_ENABLE_QT_NETWORK=1 -DBUILD_SHARED_LIBS:BOOL=ON -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE="${CONFIGURATION}" -DCMAKE_CXX_FLAGS="-std=c++17" -DENABLE_TESTING=ON -DCMAKE_INSTALL_PREFIX=/usr ..
    NUMBER_OF_PROCESSORS="$(nproc --all)";
    export NUMBER_OF_PROCESSORS;
    # build project and install files into AppDir
    make -j"$(nproc)";
    make test;
    if make DESTDIR=AppDir -j"$(nproc)" install; then
        echo "make passed";
    else
        echo "make failed";
    fi
fi
#
find AppDir/
# 
cp ${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/libexec/QtWebEngineProcess AppDir/usr/bin
cp ${MY_QT_INSTALL}/Qt/${MY_QT_VERSION}/gcc_64/resources/* AppDir/usr/bin
#
if [ "${MY_DEPLOY}" == "linuxdeploy" ]; then
    # now, build AppImage using linuxdeploy and linuxdeploy-plugin-qt
    # download linuxdeploy and its Qt plugin
    wget -c -nv https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage;
    wget -c -nv https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage;
    # make them executable
    chmod +x linuxdeploy*.AppImage;
    # make sure Qt plugin finds QML sources so it can deploy the imported files
    if [ -d "${THE_REPO_ROOT}/qml" ]; then
        export QML_SOURCES_PATHS="${THE_REPO_ROOT}/qml";
    fi
    # MY_RELEASE_FOLDER="";
    # ;webenginewidgets
    EXTRA_QT_PLUGINS="xml";
    export EXTRA_QT_PLUGINS;
    #
    if [[ "$MY_ZSYNC_UPDATE" == "true" ]]; then
        export UPDATE_INFORMATION="zsync|https://github.com/${GITHUB_REPO_NAME}/releases/download/continuous/${MY_BIN_PRO_RES_NAME}-latest.AppImage.zsync"
    fi
    #
    # ${MY_BIN_PRO_RES_NAME}-$PLATFORM.AppImage
    #export TARGET_APPIMAGE="${MY_BIN_PRO_RES_NAME}-$PLATFORM.AppImage";
    # QtQuickApp does support "make install", but we don't use it because we want to show the manual packaging approach in this example
    # initialize AppDir, bundle shared libraries, add desktop file and icon, use Qt plugin to bundle additional resources, and build AppImage, all in one command
    # env TARGET_APPIMAGE="${MY_BIN_PRO_RES_NAME}-$PLATFORM.AppImage" APPIMAGE_EXTRACT_AND_RUN=1
    # 
    ./linuxdeploy-x86_64.AppImage --appdir=AppDir -i "${THE_REPO_ROOT}/data/${MY_BIN_PRO_RES_NAME}.png" -d "${THE_REPO_ROOT}/data/${MY_BIN_PRO_RES_NAME}.desktop" --plugin qt --output appimage;
    #./linuxdeploy-x86_64.AppImage --appdir=AppDir -i "${THE_REPO_ROOT}/desktop/${MY_BIN_PRO_RES_NAME}.png" -d "${THE_REPO_ROOT}/desktop/${MY_BIN_PRO_RES_NAME}.desktop" --plugin qt --output appimage;
    chmod +x "${MY_BIN_PRO_RES_NAME}"*.AppImage*;
    cp -v "${MY_BIN_PRO_RES_NAME}"*.AppImage* AppDir/usr/bin/;
    cp -v "${PROJECT_ROOT_FOLDER}/README.md" AppDir/usr/bin/;
    7z a -tzip -r "${MY_BIN_PRO_RES_NAME}-${MY_OS}-${CONFIGURATION}-${PLATFORM}.zip" AppDir;
    cp "${MY_BIN_PRO_RES_NAME}-${MY_OS}-${CONFIGURATION}-${PLATFORM}.zip" "${THE_OLD_CWD}";
else
    ###########################################################################
    #
    #unset QTDIR; unset QT_PLUGIN_PATH ; unset LD_LIBRARY_PATH;
    #
    export VERSION="appveyor" # linuxdeployqt uses this for naming the file
    #
    wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
    chmod a+x linuxdeployqt-continuous-x86_64.AppImage    
    #
    ./linuxdeployqt-continuous-x86_64.AppImage "AppDir/usr/share/applications/${MY_BIN_PRO_RES_NAME}.desktop" -appimage -exclude-libs="libnss3.so,libnssutil3.so"
    #
    chmod +x "${MY_BIN_PRO_RES_NAME}"*.AppImage*;
    cp -v "${MY_BIN_PRO_RES_NAME}"*.AppImage* AppDir/usr/bin/;
    cp -v "${PROJECT_ROOT_FOLDER}/README.md" AppDir/usr/bin/;
    7z a -tzip -r "${MY_BIN_PRO_RES_NAME}-${MY_OS}-${CONFIGURATION}-${PLATFORM}.zip" AppDir;
    cp "${MY_BIN_PRO_RES_NAME}-${MY_OS}-${CONFIGURATION}-${PLATFORM}.zip" "${THE_OLD_CWD}";
fi
#
#
# AppImage move to Artifacts
mv "${MY_BIN_PRO_RES_NAME}"*.AppImage* "$THE_OLD_CWD";
#
# Pop Directory for Qt Installer Framework
popd;
# Qt Installer Framework
installer()
{
    echo "Preparing for Qt Installer Framework";
    #
    #
    # Copy all the files that Qt Installer Framework needs
    ls "${PROJECT_ROOT_FOLDER}";
    #
    # Copy both AppImages to where Qt Installer Framework needs them
    MY_QIF_PACKAGE_URI='packages/com.url.qtappveyor/data'
    if [ -f "${PROJECT_ROOT_FOLDER}/${THE_ARTIFACT_APPIMAGE}" ]; then
        cp -pv "${PROJECT_ROOT_FOLDER}/${THE_ARTIFACT_APPIMAGE}" "${PROJECT_ROOT_FOLDER}/${MY_QIF_PACKAGE_URI}/data";
        #cp -pv "${PROJECT_ROOT_FOLDER}/${ARTIFACT_ZSYNC}" "${PROJECT_ROOT_FOLDER}/${MY_QIF_PACKAGE_URI}/data";
    else
        echo -e "Missing ${THE_BUILD_DIR}/${THE_ARTIFACT_APPIMAGE} ";
    fi
    # The packages/${MY_QIF_PACKAGE_URI}/meta/installscript.qs creates this: cp -v "desktop/${MY_BIN_PRO_RES_NAME}.desktop" "${MY_QIF_PACKAGE_URI}";
    cp -v "${PROJECT_ROOT_FOLDER}/README.md" "${PROJECT_ROOT_FOLDER}/${MY_QIF_PACKAGE_URI}/data";
    ls "${PROJECT_ROOT_FOLDER}/${MY_QIF_PACKAGE_URI}/data";
    #
    # I use Qt Installer Framework
    # https://download.qt.io/official_releases/qt-installer-framework
    #
    echo "Running Qt Installer Framework";
    "${PROJECT_ROOT_FOLDER}/scripts/QtInstallerFramework-linux.run" -c "${PROJECT_ROOT_FOLDER}/config/config.xml" -p "${PROJECT_ROOT_FOLDER}/packages" "${THE_ARTIFACT_QIF}";
    declare -i BINARY_CREATOR_INSTALLED; BINARY_CREATOR_INSTALLED=0;
    declare MyQtInstallerFramework; MyQtInstallerFramework="binarycreator";
    #
    echo "First Check fails for find it";
    if command -v "$MyQtInstallerFramework" &> /dev/null; then BINARY_CREATOR_INSTALLED=1; echo "MyQtInstallerFramework=$MyQtInstallerFramework"; fi
        #
        #echo "Second Check fails with error code";
        if [ "$BINARY_CREATOR_INSTALLED" -eq 0 ]; then
            MyQtInstallerFramework="$(type -p binarycreator)";
            if [ -n "$MyQtInstallerFramework" ]; then
                if command -v "$MyQtInstallerFramework" &> /dev/null; then
                    BINARY_CREATOR_INSTALLED=1;
                    echo "MyQtInstallerFramework=$MyQtInstallerFramework";
                fi
            fi
        fi
        #
    if [ "$BINARY_CREATOR_INSTALLED" -eq 1 ]; then
        "$MyQtInstallerFramework" -c "${PROJECT_ROOT_FOLDER}/config/config.xml" -p "${PROJECT_ROOT_FOLDER}/packages" "${THE_ARTIFACT_QIF}";
    fi
} # end installer
#
#installer;
#
echo -e "Completed build-script.sh";
################################ End of File ##################################
