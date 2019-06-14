


include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/Catch2/single_include/catch2)


if (BUILD_HTTP_TEST)
add_executable(http_test http_test.cpp)
target_link_libraries(http_test  paio_http Catch2::Catch2)
add_test(NAME http_test COMMAND $<TARGET_FILE:http_test>) 
endif (BUILD_HTTP_TEST)

add_executable(object_test object_test.cpp)
target_link_libraries(object_test Catch2::Catch2)
add_test(NAME object_test COMMAND $<TARGET_FILE:object_test>) 

if (BUILD_BROKER_TEST)
add_executable(broker_test broker_test.cpp)
target_link_libraries(broker_test  paio_broker Catch2::Catch2)
add_test(NAME broker_test COMMAND $<TARGET_FILE:broker_test>) 
endif (BUILD_BROKER_TEST)
