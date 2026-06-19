cd %~dp0\..
mkdir build
cd build
cmake -G"Visual Studio 17 2022" %~dp0\..\code

copy ..\third_party\FFmpeg\bin\*.dll .\
pause