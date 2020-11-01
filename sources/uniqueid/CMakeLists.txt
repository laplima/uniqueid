cmake_minimum_required(VERSION 3.14)
project(uniqueid)

add_executable(uniqueid ${SKELETONS}
	#${IDL}Impl.cpp
    UniqueIDGenImpl.cpp
	server_ns.cpp)

target_link_libraries(uniqueid ${LIBS})

install(TARGETS uniqueid RUNTIME DESTINATION bin)
