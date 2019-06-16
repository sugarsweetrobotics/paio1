cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_EXTENSIONS OFF)  
set(CMAKE_CXX_STANDARD 17)
enable_testing()

project(paio LANGUAGES CXX VERSION 0.1.0)


option(BUILD_TEST "Build Test Option" ON)
option(BUILD_JSON_TEST "Build JSON Option" ON)
option(BUILD_HTTP_TEST "Build HTTP Option" ON)

add_subdirectory(src)
# add_subdirectory(example)

if (BUILD_TEST)
message("Building Test Profiles.....")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Catch2)
add_subdirectory(test)
endif (BUILD_TEST)
