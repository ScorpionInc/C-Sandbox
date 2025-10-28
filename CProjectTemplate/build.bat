COLOR 0A
ECHO OFF
CLS
ECHO Clearing old build...
rmdir /S /Q build
mkdir build
ECHO Running CMAKE...
cmake -S . -B build/
cmake --build build/
pause