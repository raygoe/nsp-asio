include_directories ( "${PROJECT_SOURCE_DIR}/server" )

add_executable ( nsp_server main.cpp server.cpp )
target_link_libraries ( nsp_server nsp_shared )
install ( TARGETS nsp_server DESTINATION . )
