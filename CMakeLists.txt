cmake_minimum_required(VERSION 3.5)
add_compile_options(-std=gnu++17)
enable_testing()

project(paio LANGUAGES CXX VERSION 0.1.0)


option(BUILD_TEST "Build Test Option" ON)

# add_subdirectory(src)
# add_subdirectory(example)

if (BUILD_TEST)
message("Building Test Profiles.....")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/Catch2)
add_subdirectory(test)
endif (BUILD_TEST)
