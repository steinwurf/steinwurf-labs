Running a C/C++ application on Android using adb shell
======================================================
In Steinwurf we like automated tests, first step to 
sucessfully implementing this is to run be able to build
and deploy via the command line. In this document
we descripe the steps needed in order to execute an
C/C++ application on an Android device using only the
command line.

Disclaimer
----------
These instructions have only tested on: 
- XUbuntu 12.04 64bit, Android-NDK-r8 and  Android-SDK-r18 

Installing prerequisites
-------------------------
We need to install the following tools:
1. System packages
::
 sudo aptitude install openjdk-7-jre ia32-libs

2. Android SDK
:: 
  wget http://dl.google.com/android/android-sdk_r18-linux.tgz
  tar xvf android-sdk_r18-linux.tgz

3. Android NDK
::
  wget http://dl.google.com/android/ndk/android-ndk-r8-linux-x86.tar.bz2
  tar xvf android-ndk-r8-linux-x86.tar.bz2

Updating the Android SDK
-------------------------

See the tools which can be installed:
:: 
  cd android-sdk-linux
  ./tools/android list sdk

We need the:
::
  1- Android SDK Tools, revision 19
  2- Android SDK Platform-tools, revision 11

To install these run:
::
  ./tools/android update sdk --filter 1,2 --no-ui

Create the an Android Stand-Alone toolchain
-------------------------------------------
Navigate to the Android NDK and use the build command to crate a 
stand-alone toolchain.
::
  cd android-ndk-linux


Running the Executable
----------------------
Instructions from here: http://stackoverflow.com/questions/10133274/

Make sure we have a device connected:
::
  cd platform-tools
  ./adb devices

If you see the following:
::
  List of devices attached 
  ???????????? no permissions

You have two options:
1. You can restart the server as root.
  ::
    sudo ./adb kill-server
    sudo ./adb start-server
2. You can add appropriate udev rules (which is more convenient in the long run). 



::
 ./adb devices
  List of devices attached 
  S58306c96d8b3 device





