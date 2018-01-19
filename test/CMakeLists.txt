include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${HTTPMOCKSERVER_INCLUDE_DIRS}
    ${CPR_INCLUDE_DIRS}
    ${MHD_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
)

add_executable(test-server test-server.cc)

target_link_libraries(test-server
    ${HTTPMOCKSERVER_LIBRARIES}
    ${MHD_LIBRARIES}
    ${CPR_LIBRARIES}
    ${GTEST_LIBRARIES}
    -lpthread
)

add_test(NAME test-server COMMAND test-server)
