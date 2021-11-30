## WeBook Browser

WeBook is a Content Management System (CMS) that uses one of the Tabs of the Viper Browser, for the GUI interface.

My goal with Viper is convert the Bookmark and Download popup dialog boxes with Tabs, after that I can start work on the CMS tab.

Currently I am working on getting this to build on AppVeyor, 
my goal is to make Mac and Windows builds as well as Linux.


## Viper Browser

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/faef22a2f30e4f1cbea3b15db777cab3)](https://www.codacy.com/app/LeFroid/Viper-Browser?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=LeFroid/Viper-Browser&amp;utm_campaign=Badge_Grade) [![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/LeFroid/Viper-Browser.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/LeFroid/Viper-Browser/context:cpp) [![Build Status](https://github.com/LeFroid/Viper-Browser/actions/workflows/cmake.yml/badge.svg)](https://github.com/LeFroid/Viper-Browser/actions/workflows/cmake.yml) 

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
