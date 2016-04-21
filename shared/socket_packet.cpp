#include <bytestream.hpp>
#include <socket_packet.hpp>

#include <socket_utils.hpp>

ByteStream& operator<<(ByteStream& bs, Packet& pkt) {
	pkt.header_.packet_size = pkt.count() + sizeof(socket_header);

	bytebuffer buf(pkt.count());
	pkt >> buf;
	bs << pkt.header_ << buf;
	return bs;
}

ByteStream& operator>>(ByteStream& bs, Packet& pkt) {
	bytebuffer buf(bs.count());
	bs >> buf;
	pkt << buf;
	pkt >> pkt.header_;

	if (!SocketUtils::isLittleEndian) {
		SocketUtils::flipEndian(&pkt.header_.service_tag);
		SocketUtils::flipEndian(&pkt.header_.opcode);
		SocketUtils::flipEndian(&pkt.header_.packet_size);
	}

	return bs;
}
