include_directories ( "${PROJECT_SOURCE_DIR}/client" )

add_library ( nsp_shared STATIC socket_utils.cpp socket_types.cpp socket_packet.cpp socket_connection.cpp services/echo_service.cpp )
