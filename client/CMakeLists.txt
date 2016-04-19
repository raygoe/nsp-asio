include_directories ( "${PROJECT_SOURCE_DIR}/client" )

add_executable ( nsp_client main.cpp client.cpp )
install (TARGETS nsp_client DESTINATION bin)