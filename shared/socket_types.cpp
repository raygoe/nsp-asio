#include <bytestream.hpp>
#include <socket_types.hpp>

#include <socket_utils.hpp>

ByteStream& operator<<(ByteStream& bs, CharEcho& ce)
{
	bs << ce.ed << ce.msg;
	return bs;
}

ByteStream& operator>>(ByteStream& bs, CharEcho& ce)
{
	bs >> ce.ed;

	if (!SocketUtils::isLittleEndian) {
		SocketUtils::FlipEndian(&ce.ed.string_length);
	}

	bytebuffer bb(ce.ed.string_length);
	bs >> bb;

	ce.msg = bb.str();
	return bs;
}
