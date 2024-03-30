cmake_minimum_required(VERSION 3.20)

project(uniqueid)   # name of the executable

set(sources
	${SKELETONS}
	${IDL_IMPL}
	server_ns.cpp)

include("${MYMODULES_DIR}/taoidl.cmake")

add_executable(${PROJECT_NAME} ${sources})
target_compile_options(${PROJECT_NAME} PUBLIC)
target_link_libraries(${PROJECT_NAME} ${mylibs} fmt::fmt)
