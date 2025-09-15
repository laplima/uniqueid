# ACETAO CMake
# (C) 2023 Luiz Lima Jr. All rights reserved.

cmake_minimum_required(VERSION 3.20)

project(uniqueid)

set(IDL UniqueIDGen.idl)			# IDL files
set(IDL_DIR .)					# IDL files location

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_BUILD_TYPE Debug)

set(SRC_DIR "sources")
set(SUBPROJECTS
	uniqueid
	uid_test)

# find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

# Generate ${mylibs}

set(libs
	ACE TAO
	TAO_PortableServer
	TAO_AnyTypeCode
	TAO_CosNaming
	TAO_IORTable
	colibry_static)

# Platform-dependent additional libraries
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(libs ${libs} )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(libs ${libs} )
endif()

set(mylibs)
foreach(lib_ ${libs})
	set(alib_ ${lib}-NOTFOUND)
	find_library(alib_ ${lib_} PATHS
		"$ENV{ACE_ROOT}/lib" "$ENV{COLIBRY}/lib" "/opt/local/lib")
	set(mylibs ${mylibs} ${alib_})
endforeach(lib_)

# IDL

set(IDL_DIR "${PROJECT_SOURCE_DIR}/${IDL_DIR}")
set(STUBS_DIR ${CMAKE_CURRENT_BINARY_DIR})		# stubs in the build dir

set(IDL_IMPL)
set(IDL_OUTPUT)
set(SKELETONS)
set(STUBS)
foreach(idl ${IDL})
	get_filename_component(idl ${idl} NAME_WE)
	set(SKELETONS ${SKELETONS}
		${STUBS_DIR}/${idl}S.cpp
		${STUBS_DIR}/${idl}C.cpp)
	set(STUBS ${STUBS}
		${STUBS_DIR}/${idl}C.cpp)
	set(IDL_OUTPUT ${IDL_OUTPUT}
		${STUBS_DIR}/${idl}C.h
		${STUBS_DIR}/${idl}C.inl
		${STUBS_DIR}/${idl}S.h)
	set(IDL_IMPL ${IDL_IMPL} ${idl}I.cpp)
endforeach(idl)
set(IDL_OUTPUT ${IDL_OUTPUT} ${SKELETONS})

# INCLUDES

include_directories(
    "${STUBS_DIR}"
    "$ENV{ACE_ROOT}"
    "$ENV{TAO_ROOT}"
    "$ENV{TAO_ROOT}/orbsvcs"
    "$ENV{COLIBRY}/include")

# PLATFORM-DEPENDENT COMPILER OPTIONS

add_compile_options(-Wno-deprecated-declarations)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_compile_options(-m64)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	include_directories("/opt/local/include" $ENV{HOME}/Platforms/spdlog/include)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(MYMODULES_DIR "${PROJECT_SOURCE_DIR}/cmake")

# Subprojects
foreach(proj ${SUBPROJECTS})
	add_subdirectory("${SRC_DIR}/${proj}")
endforeach(proj)
