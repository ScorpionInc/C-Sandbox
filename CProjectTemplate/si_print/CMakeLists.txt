cmake_minimum_required(VERSION 3.15...4.0)
project(
	si_print
	VERSION 1.0
	DESCRIPTION "Library of printing data functions."
	LANGUAGES C
)
### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
	message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

file(GLOB si_print_SRC CONFIGURE_DEPENDS "src/*.c")
add_library(_si_print STATIC ${si_print_SRC})
target_include_directories(_si_print PUBLIC include)
