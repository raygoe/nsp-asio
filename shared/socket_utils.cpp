#include <socket_utils.hpp>

bool SocketUtils::isLittleEndian = true;

bool SocketUtils::net_is_little_endian ( const connection_handshake& hs ) {
	return (*reinterpret_cast<const char*>(&hs) == 0x33);
}

uint8_t SocketUtils::CheckOpcode ( uint8_t service_tag ) {
	switch (service_tag) {
	case ServiceTags::Char:
		return CharOpcodes::EnumCount;
	default:
		return 0;
	}
}
