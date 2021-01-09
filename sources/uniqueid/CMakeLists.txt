cmake_minimum_required(VERSION 3.14)

project(uniqueid)

add_executable(${PROJECT_NAME} ${SKELETONS}
	#${IDL}Impl.cpp
    UniqueIDGenImpl.cpp
	server_ns.cpp)

target_link_libraries(${PROJECT_NAME} ${LIBS})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

