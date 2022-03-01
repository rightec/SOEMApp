
set(SOURCES simple_test.c slaveinfo_bis.c ebox_bis.c)
add_executable(simple_test ${SOURCES})
target_link_libraries(simple_test soem)
install(TARGETS simple_test DESTINATION bin)
