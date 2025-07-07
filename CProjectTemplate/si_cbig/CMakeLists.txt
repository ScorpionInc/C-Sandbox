cmake_minimum_required(VERSION 3.15...4.0)
project(
	si_cbig
	VERSION 1.0
	DESCRIPTION "Library for working with big number types."
	LANGUAGES C
)
### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
	message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

file(GLOB si_cbig_SRC CONFIGURE_DEPENDS "src/*.c")
add_library(si_cbig STATIC ${si_cbig_SRC})
target_include_directories(si_cbig PUBLIC include)
