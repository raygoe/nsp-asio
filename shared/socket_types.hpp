#ifndef SHARED_SOCKET_TYPES_HPP
#define SHARED_SOCKET_TYPES_HPP

#include <cstdint>

#include <bytestream.hpp>

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

struct ServiceTags {
enum uint8_t
{
	Char
};
};

struct char_echo_data
{
    uint64_t string_length;
};

struct CharOpcodes {
enum uint8_t
{
	Request, Response,
	EnumCount
};
};

class CharEcho
{
public:
	CharEcho ( ) { }
	CharEcho ( std::string msg ) : msg(msg) { ed.string_length = msg.length(); }

	friend ByteStream& operator<<(ByteStream& bs, CharEcho& ce);
	friend ByteStream& operator>>(ByteStream& bs, CharEcho& ce);

	std::string str () const { return msg; }

private:
	char_echo_data ed;
	std::string msg;
};

#endif /* SHARED_SOCKET_TYPES_HPP */
