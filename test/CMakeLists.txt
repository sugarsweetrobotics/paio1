


include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty/Catch2/single_include/catch2)


if (BUILD_HTTP_TEST)
add_executable(http_test http_test.cpp)
target_link_libraries(http_test  paio_http Catch2::Catch2)
add_test(NAME http_test COMMAND $<TARGET_FILE:http_test>) 
endif (BUILD_HTTP_TEST)

if (BUILD_OBJ_TEST)
add_executable(object_test object_test.cpp)
target_link_libraries(object_test Catch2::Catch2)
add_test(NAME object_test COMMAND $<TARGET_FILE:object_test>) 
endif (BUILD_OBJ_TEST)

if (BUILD_OBJDIC_TEST)
add_executable(object_dictionary_test object_dictionary_test.cpp)
target_link_libraries(object_dictionary_test paio_object Catch2::Catch2)
add_test(NAME object_dictionary_test COMMAND $<TARGET_FILE:object_dictionary_test>) 
endif (BUILD_OBJDIC_TEST)

if (BUILD_OBJSERV_TEST)
add_executable(object_server_test object_server_test.cpp)
target_link_libraries(object_server_test paio_object paio_object_server paio_json paio_http Catch2::Catch2)
add_test(NAME object_server_test COMMAND $<TARGET_FILE:object_server_test>) 
endif (BUILD_OBJSERV_TEST)

if (BUILD_OBJBROKER_TEST)
add_executable(object_broker_test object_broker_test.cpp)
target_link_libraries(object_broker_test paio_object paio_object_server paio_json paio_http Catch2::Catch2)
add_test(NAME object_broker_test COMMAND $<TARGET_FILE:object_broker_test>) 
endif (BUILD_OBJBROKER_TEST)

if (BUILD_JSON_TEST)
add_executable(json_test json_test.cpp)
target_link_libraries(json_test paio_json Catch2::Catch2)
add_test(NAME json_test COMMAND $<TARGET_FILE:json_test>) 
endif (BUILD_JSON_TEST)

if (BUILD_PROCESS_TEST)
add_executable(process_test process_test.cpp)
target_link_libraries(process_test  Catch2::Catch2)
add_test(NAME process_test COMMAND $<TARGET_FILE:process_test>) 

add_executable(process_function_test process_function_test.cpp)
target_link_libraries(process_function_test  Catch2::Catch2)
add_test(NAME process_function_test COMMAND $<TARGET_FILE:process_function_test>) 


add_executable(process_dictionary_test process_dictionary_test.cpp)
target_link_libraries(process_dictionary_test  Catch2::Catch2)
add_test(NAME process_dictionary_test COMMAND $<TARGET_FILE:process_dictionary_test>) 
endif (BUILD_PROCESS_TEST)

if (BUILD_BROKER_TEST)
add_executable(broker_test broker_test.cpp)
target_link_libraries(broker_test  paio_broker Catch2::Catch2)
add_test(NAME broker_test COMMAND $<TARGET_FILE:broker_test>) 
endif (BUILD_BROKER_TEST)


