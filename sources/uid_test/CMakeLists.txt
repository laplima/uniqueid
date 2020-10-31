cmake_minimum_required(VERSION 3.14)
project(uid_test)

add_executable(uid_test ${STUBS} uid_test_ns.cpp)

target_link_libraries(uid_test ${LIBS})

install(TARGETS uid_test RUNTIME DESTINATION bin)
