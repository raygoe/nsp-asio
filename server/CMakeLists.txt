include_directories ( "${PROJECT_SOURCE_DIR}/server" )

add_executable ( nsp_server main.cpp server.cpp )
install ( TARGETS nsp_server DESTINATION bin )