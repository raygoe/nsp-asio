include_directories ( "${PROJECT_SOURCE_DIR}/client" )

add_executable ( nsp_client main.cpp client.cpp )
target_link_libraries ( nsp_client nsp_shared )
install (TARGETS nsp_client DESTINATION . )
