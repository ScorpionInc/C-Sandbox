#!/bin/bash
sourceDir="./src/"
testsDir="./tests_src/"
outputDir="./build/"
libName="libsi_thread.a"
unitySrc="../tests_src/unity.c"
unityInclude="../tests_include/"

echo 'Building from source via CMake & make.'
cmake -S . -B "$outputDir"
cd "$outputDir"
make
if [ ! -f "./${libName}" ]; then
	echo "Library build failed."
	exit
fi
cd ./..
libPath="${outputDir}${libName}"

echo 'Start of si_thread unit tests'
tests=$(find "$testsDir" -mindepth 1 -maxdepth 1 -name "*_test.c")
for test in $tests
do
	echo
	filenameExt="${test##*/}"
	filename="${filenameExt%.*}"
	outputFile="${filename}.out"
	outputFinal="${outputDir}${outputFile}"
	echo "Building unit test: '$test' -> '$outputFinal'."
	gcc -ggdb "$test" "$libPath" ../si_data/src/* ../si_core/src/* "../si_data/build/libsi_data.a" "$unitySrc" -I./include -I../si_data/include -I../si_core/include -I./tests_include -I"$unityInclude" -lm -o "$outputFinal"
	#valgrind -s --log-fd=1 --fair-sched=yes --leak-check=full --show-leak-kinds=all --track-origins=yes "$outputFinal"
	valgrind --tool=helgrind -s "$outputFinal"
	#./"${outputFinal}"
	#rm -f "${outputFinal}"
done
echo
echo 'End of si_thread unit tests.'
