

set(SHELL_EX_NAME shell_example)

project(${SHELL_EX_NAME} LANGUAGES CXX)

add_executable(${SHELL_EX_NAME} main.cpp)

set_target_properties(${SHELL_EX_NAME} PROPERTIES
	FOLDER "examples"
)

target_link_libraries(${SHELL_EX_NAME} PUBLIC ${SHELL_LIB_NAME})

