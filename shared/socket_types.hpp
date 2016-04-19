#ifndef SHARED_SOCKET_TYPES_HPP
#define SHARED_SOCKET_TYPES_HPP

#include <cstdint>

struct connection_handshake
{
    uint16_t magic_num;
};

struct socket_header
{
    uint16_t packet_size;
    uint8_t service_tag;
    uint8_t opcode;
};

struct char_echo_data
{
    uint64_t string_length;
};

#endif /* SHARED_SOCKET_TYPES_HPP */
