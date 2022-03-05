#! /bin/bash
#
# WeBookServer
#
# Last Update: 2 June 2020
#
# I cd to my project folder and run shell?check without the ?, it fails if I remove it in a file its checking, then add all files, commit and push, stops on fail
# cd /mnt/qnap-light-wizzard/workspace/WeBook/projects/WeBookServer && shell?check -x tools/build-with-qmake.sh && git add -A && git commit -m "Update" && git push -u origin master
#
# Original Code is from: https://github.com/linuxdeploy/QtQuickApp TheAssassin
#
# This file is Open Source and I tried my best to make it cut and paste, so I am adding the Environment Variables here
# as well as the OS installer.
# I got some of my concepts and code from this project https://github.com/patrickelectric/mini-qml I use his upload.sh
#
# I run Shell Check, it requires strict Bash Standards, so the extra code is to pass the test. 
# replace the ? in shell check
# In my Environment I define DEV_CURRENT_PROJECT="path to root of this project"; 
# and I define Shell Check DEV_SHELL_CHECK="shell?check"; again remove the "?", 
# you can not use that word in any doc you check, it is a Key Word used only by it, you define exceptions to your code.
# cd "${DEV_CURRENT_PROJECT}/tools"; "$DEV_SHELL_CHECK" -x build-with-qmake.sh
# If you see no output, you have no warnings or errors.
# You can automate the checking of your scripts this way.
#
# I will not minimize this code, but if I did, you would understand why I put ";" to terminate all lines requiring them.
# 
# Debug Information, not always a good idea when not debugging, and thanks to the TheAssassin, this is now working.
# These are the setting you might want to change
declare -ix DEBUGGING;         DEBUGGING=0;          # Set 1=True and 0=False
declare -ix EXIT_ON_UNDEFINED; EXIT_ON_UNDEFINED=0;  # Set 1=True and 0=False
declare -i MY_OK; MY_OK=0;
declare MY_TARGET_PATH; MY_TARGET_PATH="/usr/bin"; # From WeBookServer.pro
# Below should be agnostic
# I use a Variable so I can turn it off on exit
if [ "${DEBUGGING}" -eq 1 ]; then set -x; fi
# Exit on error
set -e;
#
# If not defined it will use this as a default
if [ -z "${BIN_PRO_RES_NAME+x}" ]; then
    echo -e "FINDME Add BIN_PRO_RES_NAME to your Travis Settings Environment Variable with a value from Github value for Binary, pro, and resource Name ";
    if [ "${EXIT_ON_UNDEFINED}" -eq 1 ]; then exit 1; fi    
fi
# TRAVIS_REPO_SLUG should always have your GITHUB_USERNAME as the first part / GITHUB_PROJECT, so I split them to use later.
if [ -z "${GITHUB_USERNAME+x}" ] || [ -z "${GITHUB_PROJECT+x}" ]; then
    OLD_IFS="$IFS"; IFS='/'; read -ra repo_parts <<< "$TRAVIS_REPO_SLUG"; IFS="$OLD_IFS";
    declare -x GITHUB_PROJECT;
    GITHUB_USERNAME="${repo_parts[0]}";  GITHUB_PROJECT="${repo_parts[1]}";
fi
# Set our Artifacts for later
#
export ARTIFACT_APPIMAGE="${BIN_PRO_RES_NAME}-x86_64.AppImage";
export ARTIFACT_ZSYNC="${BIN_PRO_RES_NAME}-x86_64.AppImage.zsync";
export ARTIFACT_QIF="${BIN_PRO_RES_NAME}-Installer";
export ARTIFACT_ZIP="${BIN_PRO_RES_NAME}-Linux-Installer.zip";
#
# use RAM disk if possible (as in: not building on CI system like Travis, and RAM disk is available)
declare TEMP_BASE;
if [ "$CI" == "" ] && [ -d "/dev/shm" ]; then TEMP_BASE="/dev/shm"; else TEMP_BASE="/tmp"; fi
#
#echo "FINDME Make Temp Foler";
#
# building in temporary directory to keep system clean
declare BUILD_DIR; BUILD_DIR="$(mktemp -d -p "$TEMP_BASE" "${BIN_PRO_RES_NAME}-build-XXXXXX")";
# 
# make sure to clean up build dir, even if errors occur
function cleanup()
{
    if [ -d "$BUILD_DIR" ]; then rm -rf "$BUILD_DIR"; fi
    if [ "${DEBUGGING}" -eq 1 ]; then set +x; fi
}
trap "cleanup; exit;" SIGINT SIGTERM
#trap cleanup EXIT;
# 
# store repo root as variable
declare REPO_ROOT; REPO_ROOT="$(readlink -f "$(dirname "$(dirname "$0")")")";
declare OLD_CWD;   OLD_CWD="$(readlink -f .)";
# switch to build dir
pushd "${BUILD_DIR}";
#
#echo "FINDME QTV=$QTV";
# For some reason it hangs on source, but also runs as Qt4 without it.
#if [ -f "/opt/${QTV}/bin/${QTV}-env.sh" ]; then
#    echo "FINDME source /opt/${QTV}/bin/${QTV}-env.sh";
#    # shell check disable=SC1090
#    source "/opt/${QTV}/bin/${QTV}-env.sh";
#    echo "FINDME Completed source never gets here, maybe pass a argument to it?";
#else
#    echo "FINDME Error: File not found /opt/${QTV}/bin/${QTV}-env.sh";
#    ls "/opt/${QTV}/bin";
#fi
# This is what ${QTV}-env.sh does
declare -gx QT_BASE_DIR; QT_BASE_DIR="/opt/${QTV}";
declare -gx QTDIR; QTDIR="$QT_BASE_DIR";
declare -gx LD_LIBRARY_PATH;
declare -gx PKG_CONFIG_PATH;
export QTDIR="$QT_BASE_DIR";
export PATH="${QT_BASE_DIR}/bin:$PATH"
if [[ $(uname -m) == "x86_64" ]]; then
  export LD_LIBRARY_PATH="${QT_BASE_DIR}/lib/x86_64-linux-gnu:${QT_BASE_DIR}/lib:$LD_LIBRARY_PATH";
else
  export LD_LIBRARY_PATH="${QT_BASE_DIR}/lib/i386-linux-gnu:${QT_BASE_DIR}/lib:$LD_LIBRARY_PATH";
fi
export PKG_CONFIG_PATH="${QT_BASE_DIR}/lib/pkgconfig:$PKG_CONFIG_PATH"

#ls -LR /opt;
#ls /opt/${QTV}/bin;
# 
# configure build files with qmake
# "/opt/${QTV}/bin/qmake"
#
echo "FINDME run qmake -makefile ${REPO_ROOT}";
#ls "${REPO_ROOT}"; # show root of GitHub folder
qmake -makefile "${REPO_ROOT}";
# 
#echo "FINDME ******************************";
#qmake -query;
#echo "******************************";
#
#echo "FINDME make -j";
# build project and install files into AppDir
make -j"$(nproc)";

echo "FINDME make install";
# make install to this folder 
make install INSTALL_ROOT="${PWD}/AppDir";

# ../../lib/ ln
# AppDir/opt/qt514/lib/libQt5SslServer.so
# /tmp/[secure]-build-aCJVF3/3rdparty/qthttpserver/lib/libQt5SslServer.so
# /opt/qt514/lib/libQt5WebSockets.so /opt/qt514/lib/libQt5Network.so


ls; # ls = 3rdparty AppDir Makefile
echo "FINDME end make install";
# AppDir/usr/share/applications AppDir/usr/share/pixmaps

if [ -d "${PWD}/AppDir" ]; then
    echo "FINDME ./AppDir";
    ls "${PWD}/AppDir"; # usr share pixmaps
fi
if [ -d "${PWD}/AppDir/home" ]; then
    echo "FINDME ./AppDir/home";
    ls "${PWD}/AppDir/home"; # 
fi
if [ -d "${PWD}/AppDir/home/travis" ]; then
    echo "FINDME ./AppDir/home/travis";
    ls "${PWD}/AppDir/home/travis"; # 
fi
if [ -d "${PWD}/AppDir/home/travis/build" ]; then
    echo "FINDME ./AppDir/home/travis/build";
    ls "${PWD}/AppDir/home/travis/build"; # 
fi
#
if [ -d "${PWD}/AppDir/usr" ]; then
    echo "FINDME ./AppDir/usr";
    ls "${PWD}/AppDir/usr"; # AppDir/usr
fi
if [ -d "${PWD}/AppDir/usr/share" ]; then
    echo "FINDME ./AppDir/usr/share";
    ls "${PWD}/AppDir/usr/share"; # AppDir/usr/share
fi
if [ -d "${PWD}/AppDir/usr/share/applications" ]; then
    echo "FINDME ./AppDir/usr/share/applications";
    ls "${PWD}/AppDir/usr/share/applications"; # AppDir/usr/share/applications
fi


echo "FINDME find exe start";

find . -type f -executable -name "${BIN_PRO_RES_NAME}";
echo "FINDME end find exe end";
find "${TRAVIS_BUILD_DIR}" -type f -name "${BIN_PRO_RES_NAME}";

# TRAVIS_BUILD_DIR
# GitHub Root Folder
# Currently in BUILD_DIR
# 3rdparty 
# AppDir 
# Makefile
#cat Makefile;
#
# TRAVIS_BUILD_DIR/BUILD_DIR/AppDir
# AppDir/usr/share
# AppDir/usr/share/applications
# AppDir/usr/share/pixmaps
#
#
#
#
#


#echo "FINDME BUILD_DIR";
#ls "${BUILD_DIR}"; # 3rdparty AppDir Makefile
#ls "${BUILD_DIR}/AppDir"; # 3rdparty AppDir Makefile
#ls "${BUILD_DIR}/AppDir/usr"; # share
#ls "${BUILD_DIR}/3rdparty"; # 3rdparty AppDir Makefile
#ls "${BUILD_DIR}/3rdparty/qthttpserver"; # 3rdparty AppDir Makefile

#echo "FINDME end BUILD_DIR";

# its not in /usr/bin nor in "/opt" ->  google pyenv python qt514

#file "AppDir";

## Not sure where to do this
## I tried to use a define based on TARGET_FOLDER, but it did not work, 
## but I need these distro files to install service
## I am unsure as to when the pro files copy DISTFILES to TARGET 
## copy all support files over ${PWD}/
#if [ ! -d "${MY_APP_DIR}" ]; then
#    echo "FINDME Not Found ${MY_APP_DIR}";
#    mkdir -p "${MY_APP_DIR}";
#fi
# 
# Renamed -*x86_64.AppImage.zsync not sure what the * does, but if it does version numbers, I do not want it.
export UPDATE_INFORMATION="gh-releases-zsync|${GITHUB_USERNAME}|${GITHUB_PROJECT}|continuous|${ARTIFACT_ZSYNC}";
# 
# I will have an Example of using LinuxDeploy and LinuxDeployQt
#
declare -i LINUX_DEPLOY_USING;
# Set to 1 to enable LinuxDeploy
LINUX_DEPLOY_USING=0;
# This Fails for some unknown reason 
# -- Deploying executables -- 
# No such file or directory: WeBookServer
#
if [ "${LINUX_DEPLOY_USING}" -eq 1 ]; then
    echo "FINDME Running linuxdeploy";
    # now, build AppImage using linuxdeploy and linuxdeploy-plugin-qt
    # download linuxdeploy and its Qt plugin
    wget -c -nv "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage";
    wget -c -nv "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage";
    # 
    # make them executable
    chmod +x linuxdeploy*.AppImage;
    # 
    # AppImage update information
    # 
    # QtQuickApp does support "make install", but we don't use it because we want to show the manual packaging approach in this example
    # initialize AppDir, bundle shared libraries, add desktop file and icon, use Qt plugin to bundle additional resources, and build AppImage, all in one command
    # "${PWD}/AppDir${MY_TARGET_PATH}"
    ./linuxdeploy-x86_64.AppImage --appdir "AppDir" -e "${BIN_PRO_RES_NAME}" -i "${REPO_ROOT}/resources/${BIN_PRO_RES_NAME}.png" -d "${REPO_ROOT}/resources/${BIN_PRO_RES_NAME}.desktop" --plugin qt --output appimage;
    # 
    chmod +x "${BIN_PRO_RES_NAME}"*.AppImage*;
    mv "${BIN_PRO_RES_NAME}"*.AppImage* "$OLD_CWD";
    # Pop Directory for Qt Installer Framework
    echo "FINDME Finished linuxdeploy";
fi
# Set to 1 to enable LinuxDeployQt
LINUX_DEPLOY_USING=1;
if [ "${LINUX_DEPLOY_USING}" -eq 1 ]; then
    MY_OK=0;
    echo "FINDME Downloading LinuxDeployQt";
    wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage";
    chmod a+x "linuxdeployqt-continuous-x86_64.AppImage";
    # not sure why this is done for linuxdeployqt
    unset QTDIR; unset QT_PLUGIN_PATH ; unset LD_LIBRARY_PATH;
    export VERSION="travis";
    # Move our executable into the bin folder
    if [ -f "${PWD}/AppDir${MY_TARGET_PATH}/${BIN_PRO_RES_NAME}" ]; then
        cp -pv "${PWD}/AppDir${MY_TARGET_PATH}/${BIN_PRO_RES_NAME}" "${TRAVIS_BUILD_DIR}${MY_TARGET_PATH}";
        if [ -f "${TRAVIS_BUILD_DIR}${MY_TARGET_PATH}/${BIN_PRO_RES_NAME}" ]; then
            echo "FINDME Exe found";
        else
            echo "FINDME Error Exe not found AppDir${MY_TARGET_PATH}/";
            find "${REPO_ROOT}" -type f -executable;
            echo "FINDME end find 1";
            MY_OK=1;            
        fi
        ls "${TRAVIS_BUILD_DIR}${MY_TARGET_PATH}";
    fi
    if [ "${MY_OK}" -eq 1 ]; then
        if [ -f "${TRAVIS_BUILD_DIR}/${BIN_PRO_RES_NAME}.service" ]; then
            cp "${TRAVIS_BUILD_DIR}/${BIN_PRO_RES_NAME}.service" "${PWD}/AppDir${MY_TARGET_PATH}";   
            if [ -f "${PWD}/AppDir${MY_TARGET_PATH}/${BIN_PRO_RES_NAME}.service" ]; then
                echo "FINDME service copied to ${PWD}/AppDir${MY_TARGET_PATH}/my.service";
            else
                echo "FINDME Error service failed to copy to ${PWD}/AppDir${MY_TARGET_PATH}/my.service";
            fi
        else
            echo "FINDME my.service not found";
        fi            
        if [ -f "${TRAVIS_BUILD_DIR}/${BIN_PRO_RES_NAME}.socket" ]; then
            cp "${TRAVIS_BUILD_DIR}/${BIN_PRO_RES_NAME}.socket" "${PWD}/AppDir${MY_TARGET_PATH}";   
            if [ -f "${PWD}/AppDir${MY_TARGET_PATH}/${BIN_PRO_RES_NAME}.socket" ]; then
                echo "FINDME Socket copied to ${PWD}/AppDir${MY_TARGET_PATH}/my.socket";
            else
                echo "FINDME Error Socket failed to copy to ${PWD}/AppDir${MY_TARGET_PATH}/my.socket";
            fi
        else
            echo "FINDME my.socket not found";
        fi
    fi
    #
    echo "FINDME Starting linuxdeployqt-continuous-x86_64.AppImage";
    ./linuxdeployqt-continuous-x86_64.AppImage "${TRAVIS_BUILD_DIR}/usr/share/applications/${BIN_PRO_RES_NAME}.desktop" -extra-plugins=iconengines,imageformats -verbose=2 -appimage;
    ls;
    # Move them
    chmod +x "${BIN_PRO_RES_NAME}"*.AppImage*;
    mv "${BIN_PRO_RES_NAME}"*.AppImage* "$OLD_CWD";
    if [ -f "${ARTIFACT_APPIMAGE}" ]; then
        echo "FINDEME Created AppImage";
    else
        echo "FINDME Error did not make AppIame";
    fi
    echo "FINDME Finished linuxdeployqt-continuous-x86_64.AppImage"
fi
# Now to package it as a zip
declare -i MY_ZIPIT; MY_ZIPIT=0;
if [ "${MY_ZIPIT}" -eq 1 ]; then
    declare MY_EXE; MY_EXE="";

    if [ -d "${MY_APP_DIR}/${BIN_PRO_RES_NAME}" ]; then
        echo "FINDME found here ls ${MY_APP_DIR}/${BIN_PRO_RES_NAME}";
        ls "${MY_APP_DIR}/${BIN_PRO_RES_NAME}";
        if [ -f "${MY_APP_DIR}/${BIN_PRO_RES_NAME}/${BIN_PRO_RES_NAME}" ]; then
            MY_OK=1;
            MY_EXE="${MY_APP_DIR}/${BIN_PRO_RES_NAME}/${BIN_PRO_RES_NAME}";
        fi
    fi
    # if Ok
    declare MY_SERVICE_FILE; MY_SERVICE_FILE="";
    declare MY_SOCKET_FILE;  MY_SOCKET_FILE="";
    if [ "$MY_OK" -eq 1 ]; then
        if [ -f "${TRAVIS_BUILD_DIR}/${TARGET}/${BIN_PRO_RES_NAME}.service" ]; then
            echo "FINDME using path to Service";   # 
            MY_SERVICE_FILE="${TRAVIS_BUILD_DIR}/${TARGET}/${BIN_PRO_RES_NAME}.service";
        fi            
        if [ -f "${TRAVIS_BUILD_DIR}/${TARGET}/${BIN_PRO_RES_NAME}.socket" ]; then
            echo "FINDME using path to Socket ";   # 
            MY_SOCKET_FILE="${TRAVIS_BUILD_DIR}/${TARGET}/${BIN_PRO_RES_NAME}.socket";
        fi            
        # Now sanity check on all variables
        if [ ! -f "${MY_EXE}" ]; then
            echo "FINDME Failed to find MY_EXE";
            MY_OK=0;
        fi        
        if [ ! -f "${MY_SERVICE_FILE}" ]; then
            echo "FINDME Failed to find MY_SERVICE_FILE";
            MY_OK=0;
        fi        
        if [ ! -f "${MY_SOCKET_FILE}" ]; then
            echo "FINDME Failed to find MY_SOCKET_FILE";
            MY_OK=0;
        fi        
        # all the files exist, now Zip it        
        if [ "$MY_OK" -eq 1 ]; then
            # sees exe with no name as dir and stores it like one with full path
            # 7z a -sfx -y "${BIN_PRO_RES_NAME}-Linux-Installer.zip" "${MY_EXE}" "${MY_SERVICE_FILE}" "${MY_SOCKET_FILE}";
            zip "${BIN_PRO_RES_NAME}-Linux-Installer.zip" "${MY_EXE}" "${MY_SERVICE_FILE}" "${MY_SOCKET_FILE}";
            if [ -f "${BIN_PRO_RES_NAME}-Linux-Installer.zip" ]; then
                echo "FINDME Created now moving it to $OLD_CWD ${BIN_PRO_RES_NAME}-Linux-Installer.zip";
                ls "${BIN_PRO_RES_NAME}-Linux-Installer.zip";
                mv "${BIN_PRO_RES_NAME}-Linux-Installer.zip" "$OLD_CWD";
            else
                echo "FINDME Error: Failed to make ${BIN_PRO_RES_NAME}-Linux-Installer.zip";
            fi
        fi
    else
         echo "FINDME Error Not Found executable ${MY_EXE}";   # 
    fi
fi
#
declare TARGET_DEPLOY_LOCATION; TARGET_DEPLOY_LOCATION="${TARGET}";
declare -i DO_INSTALLATION_TEST; DO_INSTALLATION_TEST=0;
if [ "$DO_INSTALLATION_TEST" -eq 1 ]; then
    if [ ! -d "${TARGET_DEPLOY_LOCATION}" ]; then
        sudo mkdir -p "${TARGET_DEPLOY_LOCATION}";
    fi

    if [ -d "${TARGET_DEPLOY_LOCATION}" ]; then
        echo "FINDME Found folder ${TRAVIS_BUILD_DIR}/${TARGET} exists";
    fi
    sudo cp "${BIN_PRO_RES_NAME}" "${TARGET}/${BIN_PRO_RES_NAME}.service" "${TARGET}/${BIN_PRO_RES_NAME}.socket" "${TARGET_DEPLOY_LOCATION}";
fi
#
# Pop Directory for Qt Installer Framework
popd;
# 
declare -i QT_INSTALLER; QT_INSTALLER=0;
if [ "${QT_INSTALLER}" -eq 1 ]; then
    echo "FINDME Preparing for Qt Installer Framework";
    # 
    # Instead of trying to install Qt Installer Framework, I use 7zip to compress the bin folder
    # I will use a relative path from TRAVIS_BUILD_DIR
    # I hard code the path
    mkdir -pv qtinstallerframework;
    7z e "${QIF_ARCHIVE}" -o./qtinstallerframework;
    chmod -R +x ./qtinstallerframework;
    # 
    # Copy all the files that Qt Installer Framework needs
    ls "${TRAVIS_BUILD_DIR}";
    #
    # Copy both AppImages to where Qt Installer Framework needs them
    if [ -f "${TRAVIS_BUILD_DIR}/${ARTIFACT_APPIMAGE}" ]; then
        cp -pv "${TRAVIS_BUILD_DIR}/${ARTIFACT_APPIMAGE}" "${QIF_PACKAGE_URI}/data";
        cp -pv "${TRAVIS_BUILD_DIR}/${ARTIFACT_ZSYNC}" "${QIF_PACKAGE_URI}/data";
    else
        echo -e "FINDME Missing ${BUILD_DIR}/${ARTIFACT_APPIMAGE} ";
    fi
    # The packages/${QIF_PACKAGE_URI}/meta/installscript.qs creates this: cp -v "resources/${BIN_PRO_RES_NAME}.desktop" "${QIF_PACKAGE_URI}";
    cp -v "${TRAVIS_BUILD_DIR}/resources/${BIN_PRO_RES_NAME}.png" "${QIF_PACKAGE_URI}/data";
    cp -v "${TRAVIS_BUILD_DIR}/resources/${BIN_PRO_RES_NAME}.svg" "${QIF_PACKAGE_URI}/data";
    cp -v "${TRAVIS_BUILD_DIR}/resources/${BIN_PRO_RES_NAME}.ico" "${QIF_PACKAGE_URI}/data";
    rsync -Ravr "${TRAVIS_BUILD_DIR}/usr/share/icons" "${QIF_PACKAGE_URI}/icons";
    ls "${QIF_PACKAGE_URI}/data";
    # 
    echo "FINDME Running Qt Installer Framework";
    ./qtinstallerframework/binarycreator -c "${TRAVIS_BUILD_DIR}/config/config.xml" -p "${TRAVIS_BUILD_DIR}/packages" "${ARTIFACT_QIF}";
fi
echo -e "FINDME Completed build-with-qmake.sh";
################################ End of File ##################################
