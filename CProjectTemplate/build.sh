#!/bin/bash
echo 'Script builds project with CMake.'
echo 'Cleanup from past runs.'
echo
rm -f ./project
rm -f ./build/CMakeCache.txt
rm -rf ./build/CMakeFiles/
echo 'Generating build scripts.'
echo
cmake -B ./build
cd ./build
echo 'Building project & unit tests'
echo
make
echo 'Running unit tests.'
echo
find . -mindepth 1 -maxdepth 1 -name "*_test" |
	xargs -n1 valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s
find . -mindepth 1 -maxdepth 1 -name "*_test" |
	xargs -n1 rm -f
cd ..
echo 'Extracting binary from build folder.'
mv ./build/project ./project
echo 'Starting project binary.'
echo
./project
echo
echo 'End of bash build script.'
