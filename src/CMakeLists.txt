include_directories(
    ${HTTPMOCKSERVER_INCLUDE_DIRS}
    ${MHD_INCLUDE_DIRS}
)

add_library(httpmockserver STATIC mock_server.cc)

install(TARGETS httpmockserver ARCHIVE DESTINATION lib)
