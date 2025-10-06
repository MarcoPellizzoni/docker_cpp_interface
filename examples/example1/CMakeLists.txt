cmake_minimum_required(VERSION 3.18)

project(example1 LANGUAGES CXX)

add_executable(example1 main.cpp)

set_target_properties( example1 PROPERTIES
	FOLDER "EXAMPLES"
)

#add_subdirectory(${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/handler)

#target_include_directories(example1 PRIVATE ${DOCKER_HANDLER_INCLUDE_DIR})
#target_link_directories(example1 PRIVATE ${DOCKER_HANDLER_LIB})

target_link_libraries(example1 PUBLIC dockerapi)

target_include_directories(example1 PRIVATE ${NLOHMANN_JSON_DIR})
