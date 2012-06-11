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
 * XUbuntu 12.04, Android-NDK-r8 and  Android-SDK-r18 

Installing prerequisites
-------------------------
We need to install the following tools:
1. Java Runtime Environment 
 ::
 sudo aptitude install openjdk-7-jre

2. Android SDK
  :: 
  wget http://dl.google.com/android/android-sdk_r18-linux.tgz

3. Android NDK
  ::
  wget http://dl.google.com/android/ndk/android-ndk-r8-linux-x86.tar.bz2



