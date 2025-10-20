cmake_minimum_required(VERSION 3.15...4.0)
project(
	si_cnet
	VERSION 1.0
	DESCRIPTION "Library of structs and functions for networking"
	LANGUAGES C
)
### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
	message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

### Include Unity.h for unit tests
include(FetchContent)
if(COMMAND FetchContent_Declare AND COMMAND FetchContent_MakeAvailable)
	FetchContent_Declare(
		Unity
		GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
		GIT_TAG v2.6.1 # Specific release tag
	)
	FetchContent_MakeAvailable(Unity)
else()
	find_package(Unity REQUIRED)
endif()

find_library(si_data NAMES si_data PATHS ../si_data/)
find_library(si_core NAMES si_core PATHS ../si_core/)
find_library(si_thread NAMES si_thread PATHS ../si_thread/)

include_directories(../si_data/include)
include_directories(../si_core/include)
include_directories(../si_thread/include)

file(GLOB si_cnet_SRC CONFIGURE_DEPENDS "src/*.c")
add_library(si_cnet STATIC ${si_cnet_SRC})
target_include_directories(si_cnet PUBLIC include)
target_link_libraries(si_cnet PUBLIC si_data)
target_link_libraries(si_cnet PUBLIC si_core)
target_link_libraries(si_cnet PUBLIC si_thread)
