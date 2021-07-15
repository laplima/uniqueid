cmake_minimum_required(VERSION 3.17)

project(uniqueid)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")

set(IDL_DIR "${PROJECT_SOURCE_DIR}/idl")
set(IDL UniqueIDGen)
set(STUBS ${IDL_DIR}/${IDL}C.cpp)
set(SKELETONS ${STUBS} ${IDL_DIR}/${IDL}S.cpp)

include_directories(
    "${IDL_DIR}"
    "$ENV{COLIBRY}/include"
    "$ENV{ACE_ROOT}"
    "$ENV{TAO_ROOT}"
    "$ENV{TAO_ROOT}/orbsvcs")

set(LIBS
	ACE TAO TAO_PortableServer TAO_AnyTypeCode TAO_CosNaming
	colibry_static
	pthread)

# additional os-specific libraries
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(LIBS ${LIBS} rt)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# set(LIBS ${LIBS} _______________)
endif()


link_directories("$ENV{ACE_ROOT}/lib" "$ENV{ORBUTILS}/lib" "$ENV{COLIBRY}/lib")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_compile_options(-Wno-deprecated-declarations -fdelayed-template-parsing)
endif()

# add_compile_definitions(DEBUG)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

add_subdirectory(sources/uniqueid)
add_subdirectory(sources/uid_test)
