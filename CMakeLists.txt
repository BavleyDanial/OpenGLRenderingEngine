cmake_minimum_required (VERSION 3.29)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 20)

project (Main CXX)
file (GLOB_RECURSE SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp")
set (HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(CMAKE_BUILD_TYPE Debug) # Default to Debug if not specified
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Debug)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Debug)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Debug)
	set(CMAKE_OBJ_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin-int/Debug)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Release)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Release)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/Release)
	set(CMAKE_OBJ_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin-int/Debug)
endif()

# Ensure the directories exist
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(MAKE_DIRECTORY ${CMAKE_OBJ_OUTPUT_DIRECTORY})

add_executable(main "${SOURCE}")
target_include_directories(main PUBLIC "${HEADER}")

# Add a custom command to handle dependencies
add_custom_command(
	TARGET main POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_BINARY_DIR}/CMakeFiles/main.dir/src
		${CMAKE_OBJ_OUTPUT_DIRECTORY}
	COMMENT "Copying dependency files to ${CMAKE_BINARY_DIR}/dep"
)

# Optionally, add custom rules to clean up
add_custom_target(clean-all
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles/main.dir
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_OBJ_OUTPUT_DIRECTORY}
	COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_OUTPUT_DIRECTORY}
	COMMENT "Cleaning all build directories"
)

# Use this target to clean everything including object and dependency directories
add_custom_target(clean-all-all
	COMMAND ${CMAKE_COMMAND} --build . --target clean-all
	COMMENT "Cleaning all build artifacts"
)
