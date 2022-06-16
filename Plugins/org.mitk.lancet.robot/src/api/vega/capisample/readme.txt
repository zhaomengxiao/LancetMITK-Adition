Table of Contents
=================
1.0 About CAPIsample
2.0 Using CAPIsample
3.0 Building CAPIsample
 3.1 Required Tools
 3.2 Build Commands

1.0 About CAPIsample
=================
The Combined API (CAPI) sample application demonstrates how to use the CAPI in a native client
application to communicate with an NDI measurement system. The source code is intended as a
reference to help developers become familiar with NDI's API.

Please see license.txt and the copyright notices in source files for the terms associated with the
provision of this sample code.

2.0 Using CAPIsample
=================
The usage of the CAPIsample console application is:

   1. Connect an NDI measurement system to the host machine and ensure the device is visible on the OS.
      a) For serial devices, ensure the USB drivers are installed and you see the COM port or /dev device.
      b) For ethernet devices, ensure the host machine can ping the hostname or IP address of the NDI device.
   2. Compile CAPIsample following the instructions in this readme.
   3. Run from the terminal like: ./CAPIsample [hostname] [scu_hostname]
      [hostname] (required) The measurement device's hostname, IP address, or serial port.
      [scu_hostname] (optional) A System Control Unit (SCU) hostname, used to connect active tools.
      Eg: Connecting to device by IP address: 169.254.8.50
      Eg: Connecting to device by zeroconf hostname: P9-B0103.local
      Eg: Connecting to serial port varies by OS: COM10 (Win), /dev/ttyUSB0 (Linux), /dev/cu.usbserial-001014FA (Mac)

By default, CAPIsample will connect to a device and issue some API commands that illustrate how the
CAPI works. The application does not issue every possible API command, but performs a few common
tasks including:

   - Connecting to an ethernet or serial port device (via serial-to-USB converter)
   - Initializing the system
   - Loading and initializing passive, active-wireless, and active tools (if an SCU is connected)
   - Getting/setting user parameters stored in the device firmware
   - Sending BX or BX2 to retrieve tracking data and printing it to the terminal in .csv format
   - Printing a small amount of tracking data to a .csv file called "example.csv"

The source code for CAPIsample is provided so it can be analyzed and modified as desired.
Developers may want to:

   - Explore simulating alerts and how the alert data is transmitted to the application
   - Investigate using dummy tools to return 3D data
   - Refer to the API guide for their system to add commands that CAPIsample didn't implement
   - Completely gut and rewrite CAPIsample as a starting point for their project

3.0 Building CAPIsample
=================
CAPIsample can be compiled on Windows, Mac or Linux using CMake and a command line compiler.
Developers may wish to use an IDE of their choice (eg. Visual Studio, Eclipse etc...). A Visual
Studio 2012 .sln is provided for convenience.

3.1 Required Tools
------------------
1) [Windows/Mac/Linux] CMake version 2.8 or later (see: http://cmake.org/)
2) [Windows]   Microsoft Visual Studio command line compiler (devenv.com)
   [Mac/Linux] make
3) [Optional - Windows/Mac/Linux] Doxygen (see: http://www.stack.nl/~dimitri/doxygen/index.html)

The project was tested with Visual Studio 2012 Update 4 on Windows, and GNU make v3.81 on MacOSX 10.11
and Ubuntu 16.04 (make v. Doxygen 1.8.12 on Windows was used to compile HTML documentation.

Note: If your Windows environment has Cygwin installed, and Cygwin has another version of CMake, you
may need to fix your PATH to point to the version of CMake that can build Visual Studio solutions:

   echo 'Configuring the PATH to avoid confusing the cygwin cmake with the native Windows cmake ...'
   SET PATH="C:\Program Files (x86)\cmake-3.4.3-win32-x86\bin";%PATH%

3.2 Build Commands
------------------
Beginning in this directory where the capisample source code is, the following terminal commands can
be used to compile CAPIsample and its Doxygen documentation.

[Windows 64-bit]
build-win64.bat
doxygen doxygen.conf

[Mac]
./build-mac.sh
doxygen doxygen.conf

[Linux]
./build-linux.sh
doxygen doxygen.conf
