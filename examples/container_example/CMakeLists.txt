

set(CONTAINER_EX_NAME container_example)

project(${CONTAINER_EX_NAME} LANGUAGES CXX)

add_executable(${CONTAINER_EX_NAME} main.cpp)

set_target_properties(${CONTAINER_EX_NAME} PROPERTIES
	FOLDER "examples"
)

target_link_libraries(${CONTAINER_EX_NAME} PUBLIC ${DOCKER_API_LIB_NAME})

