
set(SHELL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SHELL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(BUILD_SHARED_LIBS)
	add_library(${SHELL_LIB_NAME} SHARED)
	target_compile_definitions(${SHELL_LIB_NAME} 
		PRIVATE 
			_SHELL_LIB_EXPORT
			_BUILD_SHELL_LIB_DLL 
	)
    set(SHELL_LIB_OUTPUT_NAME "shellif")
else()
	add_library(${SHELL_LIB_NAME} STATIC)
    set(SHELL_LIB_OUTPUT_NAME "shellifS")
endif()

if(UNIX)
	target_compile_definitions(${SHELL_LIB_NAME} 
		PRIVATE 
			USE_UNIX
	)
	target_sources(${SHELL_LIB_NAME} 
		PRIVATE 
			${SHELL_SRC_DIR}/ShellUnix.hpp
	)
else()
	target_compile_definitions(${SHELL_LIB_NAME}
		PRIVATE 
			USE_WIN32
	)
	target_sources(${SHELL_LIB_NAME}
		PRIVATE 
			${SHELL_SRC_DIR}/ShellWin32.hpp
	)
endif()

target_sources(${SHELL_LIB_NAME} 
	PRIVATE 
		${SHELL_SRC_DIR}/Shell.cpp
		${SHELL_INCLUDE_DIR}/Shell.h
)

set_target_properties(${SHELL_LIB_NAME} PROPERTIES
    OUTPUT_NAME   ${SHELL_LIB_OUTPUT_NAME}
    DEBUG_POSTFIX "D"
	 PUBLIC_HEADER ${SHELL_INCLUDE_DIR}/Shell.h
)

target_include_directories(${SHELL_LIB_NAME} 
	PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

