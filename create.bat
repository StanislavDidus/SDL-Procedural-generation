@ECHO OFF
ECHO y| rmdir /s build
cmake -G "Visual Studio 17 2022" -A x64 -B build
PAUSE