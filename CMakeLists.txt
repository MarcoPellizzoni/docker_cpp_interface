cmake_minimum_required( VERSION 3.22 )

project( DockerCppInterface VERSION 1.0.0 )

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE STRING "Install directory" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set (CMAKE_MESSAGE_LOG_LEVEL "STATUS" CACHE STRING "Select cmake message log level")
set_property(CACHE CMAKE_MESSAGE_LOG_LEVEL PROPERTY STRINGS
    "WARNING" "STATUS" "DEBUG" "TRACE"
)

#[[
    Put all the runtime stuff in the same directory.  By default, CMake puts each targets'
    output into their own directory.  We want all the targets to be put in the same
    directory, and we can do this by setting these variables.
]]
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#[[
    Force visual studio to follow the foldering schema if the source code
]]
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#[[
    Windows defines it own configuration type via CMAKE_CONFIGURATION_TYPES variable, linux do not. 
    Set the build type for linux.
]]
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES) # TODO: use GENERATOR_IS_MULTI_CONFIG
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#[[
	Add compilation flags and definitions usefull for windows
]]
if(MSVC)
    #add_link_options("/verbose:lib") # enables verbose output of linker researches of dependencies
    #add_link_options("/NODEFAULTLIB:MSVCRT")
    add_compile_definitions( "WIN32" )
    add_compile_definitions( "WIN32_LEAN_AND_MEAN" ) # this prevents Winsock.h from being included by the Windows.h header
    add_compile_options( "/MP" ) # multi process compilation
    #add_compile_options( "/EHsc" ) # for exceptions
    add_compile_options( "/WX" ) # All warnigs as errors
    add_compile_options( "/W1" ) # Warning level
    #add_compile_options("/permissive") # set the compilation to check conformance issues with the given standard
    add_link_options("/INCREMENTAL:NO") # more safe linking process
endif()
if(UNIX)
    add_compile_definitions( "UNIX" )
    add_compile_options("-Wall")
	add_compile_options("-fPIC")
endif()


list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
#include(utils.cmake)

#[[
	Set the option to build shared or static version of the library
]]
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

#[[
    Add the compilation flag -fPIC which compiles shared libs in a way that the same
    library code can run properly regardless of the position in memmory in which is
    being loaded.
]]
if(UNIX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()


#[[
	Using the GNUInstallDirs which provides several predefined variables to be used for installation.
	This follows the standard for constructing the install directory tree where there will be an include dir
	for the headers (INCLUDEDIR), a lib dir for static libraries (LIBDIR) and a bin dir for dynamic libraries (BINDIR)
	All the variable are prefixed with "CMAKE_INSTALL_"
	Paths are all relative to CMAKE_INSTALL_PREFIX, which needs to be set
]]
include(GNUInstallDirs)


###################################################
# Docker target configuration
###################################################

option(BUILD_TESTS		"Build Tests"		OFF)
option(BUILD_EXAMPLES	"Build Examples"	OFF)

add_subdirectory(tools)

set(DOCKER_API_LIB_NAME    "docker")
set(DOCKER_API_SOURCE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/src" )
set(DOCKER_API_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" )

file(GLOB DOCKER_API_SOURCE_FILES
    "${DOCKER_API_SOURCE_DIR}/*.c"
    "${DOCKER_API_SOURCE_DIR}/*.cpp"
)

file(GLOB DOCKER_API_HEADER_FILES
    "${DOCKER_API_INCLUDE_DIR}/*.h"
    "${DOCKER_API_SOURCE_DIR}/*.hpp"
)

if(BUILD_SHARED_LIBS)
    add_library(${DOCKER_API_LIB_NAME} SHARED)
    set(_OUTPUT_NAME "dockercppif")
	target_compile_definitions( ${DOCKER_API_LIB_NAME} 
		PRIVATE 
			_DOCKER_LIB_EXPORT
			_BUILD_DOCKER_LIB_DLL
	)
else()
    add_library(${DOCKER_API_LIB_NAME} STATIC)
    set(_OUTPUT_NAME "dockercppifS")
endif()


target_sources(${DOCKER_API_LIB_NAME}
    PRIVATE 
        ${DOCKER_API_SOURCE_FILES}
        ${DOCKER_API_HEADER_FILES}
)

target_include_directories(${DOCKER_API_LIB_NAME}
	PUBLIC
        $<BUILD_INTERFACE:${DOCKER_API_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE 
        ${DOCKER_API_SOURCE_DIR}
)

target_link_libraries(${DOCKER_API_LIB_NAME} 
	PUBLIC
		${SHELL_LIB_NAME}
)

add_dependencies(${DOCKER_API_LIB_NAME}	
	${SHELL_LIB_NAME}
)

set_target_properties(${DOCKER_API_LIB_NAME} PROPERTIES
    OUTPUT_NAME   ${_OUTPUT_NAME}
    DEBUG_POSTFIX "D"
    FOLDER        "Docker"
	 PUBLIC_HEADER ${DOCKER_API_INCLUDE_DIR}/docker.h
)


if(BUILD_TESTS)
	add_subdirectory( test )
endif()

if(BUILD_EXAMPLES)
	add_subdirectory( examples )
endif()


# Now that everything is done, indicate that we have finished configuring at least once.
# We use this variable to set certain defaults only on the first pass, so that we don't
# continually set them over and over again.
set(PASSED_FIRST_CONFIGURE ON CACHE INTERNAL "Already Configured once?")


########################################################################################
# INSTALL
########################################################################################
message(STATUS "Binaries will be installed at ${CMAKE_INSTALL_PREFIX}")


####################################################################
# INSTALL EXPORTS for insclude in other projects using find_package

#[[
	Install artefacts in the correspondig dirs 
	
	EXPORT KEYWORD:
	this is needed in order to export the target infos to be used for creating a package config.
	It creates an export target that can be imported by other projects
]]
install(TARGETS ${DOCKER_API_LIB_NAME} ${SHELL_LIB_NAME}
    EXPORT dockerapi-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # install the API header
)

#[[
	Generates and install a CMake file containing code to import targets from the installation 
	tree into another project
	Target installations are associated with the export <export-name> using the EXPORT option of the install(TARGETS).
	The NAMESPACE option will prepend <namespace> to the target names as they are written to the import file.
	By default the generated file will be called <export-name>.cmake but the FILE option may be used 
	to specify a different name. The value given to the FILE option must be a file name with the .cmake extension.
]]
install(EXPORT dockerapi-targets
    NAMESPACE docker::                      # add a namespace to find the package
    DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake  # dir in which to export the cmake export files to be used to create a package config
)

#[[
	Add helper functions to create package config files
]]
include(CMakePackageConfigHelpers)

#[[
	Gets the config input file to produce the cmake config file that will be used by other
	project to find the package
]]
configure_package_config_file(
	${CMAKE_SOURCE_DIR}/cmake/dockerapi-config.cmake.in	# input
	${CMAKE_BINARY_DIR}/cmake/dockerapi-config.cmake		# output
	# The following property is mandatory.
	# Is used to compute the relative paths of the library directories
	# given the only absolute that will be known by other projects, that is the path to the config file
	INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake

	# optional: sets the paths to locations in the install directory.
	# In the dummy-config.cmake.in is the defined the counterpart which takes the value of this variable.
	# The counterparts are the variables that will be available by the other projects
	PATH_VARS CMAKE_INSTALL_INCLUDEDIR
	PATH_VARS CMAKE_INSTALL_BINDIR
    PATH_VARS CMAKE_INSTALL_LIBDIR
)


#[[
	Generate a version config file. This file is used by cmake to check
	the backward compatibility attribute of the library.
	If it will be fully backward compatible than the value to set is AnyNewerVersion
	In this case we consider backward compatible only versions that falls under the same
	major version
]]
write_basic_package_version_file(
	${CMAKE_BINARY_DIR}/cmake/dockerapi-config-version.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)

#[[
	Install the generated files to the destination dir
]]
install(FILES
	${CMAKE_BINARY_DIR}/cmake/dockerapi-config.cmake
	${CMAKE_BINARY_DIR}/cmake/dockerapi-config-version.cmake
	DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake
)


message ("")
message (STATUS "BUILD ENVIRONMENT INFO:")
message (STATUS "System name           -> ${CMAKE_SYSTEM_NAME}")
message (STATUS "System version        -> ${CMAKE_SYSTEM_VERSION}")
message (STATUS "Compiler Path         -> ${CMAKE_CXX_COMPILER}")
message (STATUS "Compiler Version      -> ${CMAKE_CXX_COMPILER_ID} version ${CMAKE_CXX_COMPILER_VERSION}")
message (STATUS "Compiler Flags        -> ${CMAKE_CXX_FLAGS}")
message (STATUS "Linker library suffix -> ${CMAKE_LINK_LIBRARY_SUFFIX}")
message ("")


