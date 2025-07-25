cmake_minimum_required(VERSION 3.15...4.0)
project(
	CProjectTemplate
	VERSION 1.0
	DESCRIPTION "A Template C Project"
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

### Allow setting build type
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
		STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_subdirectory(si_print)
add_subdirectory(si_data)
add_subdirectory(si_thread)

add_executable(project src/main.c)
target_link_libraries(project _si_print)
target_link_libraries(project _si_data)
target_link_libraries(project _si_thread)

add_executable(array_test src/si_array_test.c)
target_link_libraries(array_test _si_data)
add_executable(stack_test src/si_stack_test.c)
target_link_libraries(stack_test _si_data)
add_executable(queue_test src/si_queue_test.c)
target_link_libraries(queue_test _si_data)
add_executable(list_test src/si_linked_list_test.c)
target_link_libraries(list_test _si_data)
add_executable(adler_test src/si_adler_test.c)
target_link_libraries(adler_test _si_data)
add_executable(map_test src/si_map_test.c)
target_link_libraries(map_test _si_data)
add_executable(hashmap_test src/si_hashmap_test.c)
target_link_libraries(hashmap_test _si_data)

add_executable(bits_test src/si_bits_test.c)
target_link_libraries(bits_test _si_print)
add_executable(hex_test src/si_hex_test.c)
target_link_libraries(hex_test _si_print)

add_executable(thread_test src/si_thread_test.c)
target_link_libraries(thread_test _si_thread)
