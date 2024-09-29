cmake_minimum_required(VERSION 3.13)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

project(OpenGLRenderer)
# Set Build Type to Debug by default
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Set architecture to either 32 or 64 depending on system
set(ARCHITECTURE "32")
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ARCHITECTURE "64")
endif()

# OutputDir to be used for all projects
set(OutputDir "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE SOURCE "${CMAKE_SOURCE_DIR}/src/**.cpp")
set(HEADER "${CMAKE_SOURCE_DIR}/header/")

file(GLOB_RECURSE GLAD_SRC "${CMAKE_SOURCE_DIR}/vendor/glad/**.c")
set(GLAD_HEADER "${CMAKE_SOURCE_DIR}/vendor/glad/include")

set(GLM_HEADER "${CMAKE_SOURCE_DIR}/vendor/glm")

add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/glfw/")
find_package( OpenGL REQUIRED )

add_subdirectory("${CMAKE_SOURCE_DIR}/vendor/tinyobjloader/")
add_compile_definitions(TINYOBJLOADER_IMPLEMENTATION)

set(BIN_NAME "OGLR-${CMAKE_SYSTEM_NAME}-${ARCHITECTURE}")
add_executable(${BIN_NAME} "${SOURCE}" "${GLAD_SRC}")

target_link_libraries(${BIN_NAME} glfw)
target_link_libraries(${BIN_NAME} OpenGL::GL)
target_link_libraries(${BIN_NAME} tinyobjloader)
target_include_directories(${BIN_NAME} PUBLIC "${HEADER}" "${GLAD_HEADER}" "${GLM_HEADER}")
set_target_properties(${BIN_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OutputDir}"
)
