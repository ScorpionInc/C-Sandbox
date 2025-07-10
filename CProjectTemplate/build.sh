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
tests=$(find . -mindepth 1 -maxdepth 1 -name "*_test")
for t in $tests
do
	#echo "Running Unit test: '$t'"
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s --fair-sched=yes --error-exitcode=1 "$t" >/tmp/valgrind_output 2>&1
	vgec=$?
	if [[ "$vgec" -eq 0 ]]; then
		rm -f "$t"
	else
		echo "Valgrind exited with code: $vgec"
		cat /tmp/valgrind_output
	fi
done
cd ..
echo 'Extracting binary from build folder.'
mv ./build/project ./project
echo 'Starting project binary.'
echo
./project
echo
echo 'End of bash build script.'
