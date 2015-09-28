# thesis
Source code for implementation part of the master thesis.

Files located in cgal dir are modified versions of original CGAL header files, so make sure to replace them.

## Dependencies
In order to run executable file (ds1_example.exe for now) in environments without Visual Studio 2013 and CGAL and Boost libraries installed, following DLL files are included in the package:

- boost_chrono_vc120-mt-1_57.dll
- boost_system_vc120-mt-1_57.dll
- boost_thread_vc120-mt-1_57.dll
- CGAL-vc120-mt-4.5.dll

## How to install in Windows

http://www.cgal.org/download/windows.html

## Build

#### Build solution

Use cmake-gui. For source dir specify the projects's path and for build dir use whatever directory you wish. Make sure there's a cmake entry CGAL_DIR with value _pathToCGAL_/build.

#### Build executables from source files

Open solution file, located in build directory, with Visual Studio. To build executables, use build command, e.g. build *.cpp . In Build->Configuration manager check that Release mode is used and not Debug and uncheck ALL_BUILD line. Also, set the project as startup project. 
