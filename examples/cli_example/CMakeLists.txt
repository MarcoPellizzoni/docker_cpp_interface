

set(CLI_EX_NAME CLI_example)

project(${CLI_EX_NAME} LANGUAGES CXX)

add_executable(${CLI_EX_NAME} main.cpp)

set_target_properties(${CLI_EX_NAME} PROPERTIES
	FOLDER "examples"
)

target_link_libraries(${CLI_EX_NAME} PUBLIC ${DOCKER_API_LIB_NAME})

