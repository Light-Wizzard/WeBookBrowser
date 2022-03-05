# WeBook Browser

WeBook is a Content Management System (CMS) that will use one of the Tabs of this Browser, 
for the GUI interface.

This is from a fork of Viper Browser https://GitHub.com/Light-Wizzard/Viper-Browser,
my goal with Viper is convert the Bookmark and Download popup dialog boxes to Tabs, 
after that I can start work on the CMS tab.

I am not using the same code base as Viper, 
only the concepts that I wanted to incorporate.

Currently I am working on getting this to build on Jenkins, 
my goal is to make Linux, Mac and Windows builds.

# Table of Content
- [Deployment](https://github.com/Light-Wizzard/WeBookBrowser#Deployment)
- [Deploy Folder layout](https://github.com/Light-Wizzard/WeBookBrowser#Deploy-Folder-layout)
- [shellcheck](https://github.com/Light-Wizzard/WeBookBrowser#shellcheck)
- [Development](https://github.com/Light-Wizzard/WeBookBrowser#Development)
- [Viper Browser](https://github.com/Light-Wizzard/WeBookBrowser#Viper-Browser)
- [Features](https://github.com/Light-Wizzard/WeBookBrowser#Features)
- [Building](https://github.com/Light-Wizzard/WeBookBrowser#Building)
- [Thanks](https://github.com/Light-Wizzard/WeBookBrowser#Thanks)

## Deployment

I am currently learning about Jenkins https://Jenkins.io,
once I figure out how to use it, I will update this page.

## Deploy Folder layout

**For Linux Deploy, Linux Deploy Qt, or Mac Deploy**
 
If you use an App Bundle Deploy, you want to store your .desktop installer file under share applications,
in the build script it uses the variable ${MY_BIN_PRO_RES_NAME}.desktop, and store another copy under data,
in case you use Linux Deploy vs Linux Deploy Qt, on Mac just use the below.

If you have Libraries you need to deploy put them in lib, the script will link to this location:


```
└── usr
    ├── bin
    │   └── your_app
    ├── lib
    └── share
        ├── applications
        │   └── your_app.desktop
        └── icons
            └── hicolor
                └── 256x256 
                    └── apps 
                        └── your_app.png

```

**On Windows**

Use the data folder.

## shellcheck

https://www.shellcheck.net/

I run shellcheck to ensure that the syntax is right in the build script, 
and why you will find a lot of extra code like a semicolon; to end all lines that should have a termination.

```bash
shellcheck -x scripts/install.sh
shellcheck -x scripts/build_script.sh
```

## Development

Qt 6.2.1, Jenkins

Linux: Ubuntu 20.04 and linuxdeploy
Mac: macdeployqt
Windows: windeployqt

If you copy the Qt libexec and resource folder into the current AppDir bin, this app will run.

### snapshoots

![Main](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/main.png)

![preferences](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/preferences.png)

![Bookmarks](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/bookmark.png)

![downloads](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/downloads.png)

![history](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/history.png)

![cookie-manager](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/cookie-manager.png)

![magage-ad-blocker](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/magage-ad-blocker.png)

![magage-advanced](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/magage-advanced.png)

![magage-content](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/magage-content.png)

![magage-privacy](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/magage-privacy.png)

![magage-search](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/magage-search.png)

![manage-user-scripts](https://github.com/Light-Wizzard/WeBookBrowser/blob/main/data/screenshoots/manage-user-scripts.png)

# Viper Browser

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/faef22a2f30e4f1cbea3b15db777cab3)](https://www.codacy.com/app/LeFroid/Viper-Browser?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=LeFroid/Viper-Browser&amp;utm_campaign=Badge_Grade) 
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/LeFroid/Viper-Browser.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/LeFroid/Viper-Browser/context:cpp) [![Build Status](https://github.com/LeFroid/Viper-Browser/actions/workflows/cmake.yml/badge.svg)](https://github.com/LeFroid/Viper-Browser/actions/workflows/cmake.yml) 

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/fc7007604e974be3b262e20d4b3c0461)](https://www.codacy.com/gh/Light-Wizzard/WeBookBrowser/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Light-Wizzard/WeBookBrowser&amp;utm_campaign=Badge_Grade)

![AppVeyor Build](https://img.shields.io/appveyor/build/Light-Wizzard/webookbrowser?style=plastic)

![Code Coverage](https://img.shields.io/gitlab/coverage/Light-Wizzard/WeBookBrowser/main?style=plastic)

A powerful yet lightweight web browser built with the Qt framework

Licensed under GPLv3

## Features

*   All development is done with a focus on privacy, minimalism, and customization ability
*   Bookmark management
*   Built-in ad blocker, compatible with AdBlock Plus and uBlock Origin filters
*   Cookie viewer, editor, and support for cookie filters (QtWebEngine 5.11+ only)
*   Compatible with Pepper Plugin API
*   Custom user agent support
*   Fast and lightweight
*   Fullscreen support
*   Granular control over browser settings and web permissions
*   Gives the user control over their data, no invasions of privacy like other browsers are known to do..
*   GreaseMonkey-style UserScript support
*   Multiple options for home page- any URL, blank page, or a card layout page with favorite and most visited websites
*   PDF.js embedded into the browser
*   Save and restore browsing sessions, local tab history, pinned tabs
*   Secure AutoFill manager (disabled by default)
*   Tab drag-and-drop support for HTML links, local files, other browser window tabs, etc
*   Tab hibernation / wake up support
*   Traditional browser UI design instead of WebUI and chromium-based interfaces

## Building

The browser can be built using the cmake build system, by either importing the root CMakeLists file into your IDE of choice or performing the following commands from a console:

```console
$ git clone https://github.com/LeFroid/Viper-Browser.git viper-browser
$ mkdir -p viper-browser/build && cd viper-browser/build
$ cmake ..
$ make
```

Tests can be ran by setting the `ENABLE_TESTING` in the cmake configuration.

The binary will be located in the `build/src/app` folder when following the commands listed above.

## Thanks

This project is possible thanks to the work of others, including those involved in the following projects:

*   Qt Framework
*   PDF.js 
*   Arora QT Browser
*   Qt Tab Browser example (From Qt 5.5 Webkit example archive)
*   Code Editor example
*   Qupzilla - for parts of AdBlockPlus implementation
*   Otter Browser - for parts of AdBlockPlus implementation

All licenses and credits can be found in the file `LICENSE`

### End of Read Me
