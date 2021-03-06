Running a command-line C++ application on Android using adb shell
=================================================================

In Steinwurf we like automated tests, the first step to achieve this is to be able to build
and deploy via the command line. 
This document describes how to run a C/C++ application on an Android device using only the command line.

Disclaimer
----------
These instructions have been tested for:
- Android SDK 21 and Android NDK r8d

Installing prerequisites
-------------------------
We need to install the following tools:

1. System packages
::
 sudo aptitude install openjdk-7-jre ia32-libs

2. Android SDK (check `here <http://developer.android.com/sdk>`_ for the newest version, 
click the "DOWNLOAD FOR OTHER PLATFORMS" to see the SDK only downloads)
:: 
  wget http://dl.google.com/android/android-sdk_r21.0.1-linux.tgz
  tar xvf android-sdk_r21.0.1-linux.tgz

3. Android NDK (check `here <http://developer.android.com/sdk/ndk>`_ for the newest version)
::
  wget http://dl.google.com/android/ndk/android-ndk-r8d-linux-x86.tar.bz2
  tar xvf android-ndk-r8d-linux-x86.tar.bz2

Updating the Android SDK
-------------------------

See the tools which can be installed:
:: 
  cd android-sdk-linux
  ./tools/android list sdk

We need the:
::
  1- Android SDK Platform-tools, revision 16

To install these run:
::
  ./tools/android update sdk --filter 1 --no-ui

You may want to copy the sdk to some suitable location (I have it in ~/dev):
::
  cp -R android-sdk-linux/ ~/dev/android-sdk-linux-r21.0.1

Create the standalone Android toolchain
---------------------------------------

Navigate to the Android NDK and use the following command to create a standalone toolchain.
This shell script works on Linux, Mac OSX and Cygwin (Windows 7).
::
  cd android-ndk-r8d
  ./build/tools/make-standalone-toolchain.sh --platform=android-9 --install-dir=/home/youruser/dev/android-ndk-r8d-platform9-toolchain --ndk-dir=.

If you omit the --install-dir path the script will create a tar.bz2 archive in your /tmp folder.
Here we have used the long install-dir to be able to remember which ndk version etc. was used etc.

Download the pre-compiled toolchain for your platform
#####################################################

If you cannot run the shell script yourself (e.g. on Windows), you can also download the pre-compiled toolchain 
for your platform from here:

http://176.28.49.184:12344/toolchains/

Build a C++ Executable using the NDK
------------------------------------
Now that we have the toolchain we simply have to create an executable. 
Lets do a simple hello android application.

Open you favorite editor and paste the following save the file as ``main.cpp``:
::
  #include <iostream>
  int main()
  {
     std::cout << "Hello Android!" << std::endl;
     return 0;
  }

Make sure the compiler can be found and then compile the application:
::
  export PATH=~/dev/android-ndk-r8b-platform9-arm-4.6/bin:$PATH
  arm-linux-androideabi-g++ main.cpp -o hello_android

If everything was configured correctly, the executable ``hello_android`` is generated. 

Running the Executable
----------------------
Instructions from `here <http://stackoverflow.com/questions/10133274/>`_

We will use the ``adb`` tool from the android SDK to deploy and run our
application on our Android device. Make sure the ``adb`` command is avaible
and check which devices we have connected:
::
  export PATH=~/dev/android-sdk-linux-r21.0.1/platform-tools/:$PATH
  adb devices

If you see the following:
::
  List of devices attached 
  ???????????? no permissions

You have two options:

1. You can restart the server as root.
   ::
     sudo ./adb kill-server
     sudo ./adb start-server

2. You can add appropriate udev rules (which is more convenient in the 
   long run). See this `page <http://developer.android.com/tools/device.html>`_ for more information.

Once the Android device is correctly attached you should see the 
following output of the ``adb devices`` command:
::
 ./adb devices
  List of devices attached 
  S58306c96d8b3 device

Now push our application to the device and run it:
::
  adb push hello_android /data/local/tmp/hello_android
  adb shell /data/local/tmp/hello_android

You shold now see ``Hello Android!`` printed to your screen. 

Typically for automated testing we want the return code to see whether 
something went wrong (from `here <http://stackoverflow.com/questions/9379400/>`_):
::
  adb shell '/data/local/tmp/hello_android; echo $?'

To direct commands to a specific USB device or emulator, the serial number can be specified like so:
::
  adb -s <serial number> <command>
