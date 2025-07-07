#!/bin/bash
echo 'Start build script.'
# Begin
# Remove any old build
rm -f './libsi_cbig.a'
# Use scons if available
if command -v scons >/dev/null 2>&1; then
	scons && exit 0
	exit 1
fi
# Now needs Make
command -v make >/dev/null 2>&1 || {
	echo 'Build script requires make.'
	exit 1
}
tool='make'
# Prefer a CMake generated Makefile
if command -v cmake >/dev/null 2>&1; then
	tool='cmake . -B ./build && cd ./build && make && cd ..'
fi
# Run Build Tool (either cmake & make or just make)
eval "$tool" || exit 1
# Copy result to .
cp './build/libsi_cbig.a' './'
# Cleanup
rm -rf ./build/*
# End
echo 'End of build script.'
