#ifndef SHARED_SOCKET_PACKET_HPP
#define SHARED_SOCKET_PACKET_HPP

#include <bytestream.hpp>
#include <socket_types.hpp>

class Packet : public ByteStream {
public:
	Packet ( ) { }
	Packet ( int service_tag, int opcode ) {
		header_.service_tag = service_tag;
		header_.opcode = opcode;
	}

	friend ByteStream& operator<<(ByteStream& bs, Packet& pkt);
	friend ByteStream& operator>>(ByteStream& bs, Packet& pkt);

	socket_header & Header() { return header_; }
private:
	socket_header header_;
};

#endif /* SHARED_SOCKET_PACKET_HPP */
