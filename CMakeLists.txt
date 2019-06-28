cmake_minimum_required(VERSION 3.5)

#set(CMAKE_REQUIRED_FLAGS -std=c++20)

set(CMAKE_CXX_EXTENSIONS OFF)  
set(CMAKE_CXX_STANDARD 20)

enable_testing()

project(paio LANGUAGES CXX VERSION 0.1.0)


option(BUILD_TEST "Build Test Option" ON)
option(BUILD_JSON_TEST "Build JSON Option" OFF)
option(BUILD_OBJDIC_TEST "Build ObjectDictionaryTest Option" OFF)
option(BUILD_OBJSERV_TEST "Build ObjectServerTest Option" OFF)
option(BUILD_OBJBROKER_TEST "Build ObjectBrokerTest Option" ON)
option(BUILD_OBJ_TEST "Build Object Test Option" ON)
option(BUILD_HTTP_TEST "Build HTTP Option" OFF)
option(BUILD_PROCESS_TEST "Build Process Test" ON)

add_subdirectory(src)
# add_subdirectory(example)

if (BUILD_TEST)
message("Building Test Profiles.....")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Catch2)
add_subdirectory(test)
endif (BUILD_TEST)
