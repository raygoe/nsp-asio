#ifndef SHARED_SOCKET_UTILS_HPP
#define SHARED_SOCKET_UTILS_HPP

#include <socket_types.hpp>

#include <algorithm>

class SocketUtils
{
public:

	static bool net_is_little_endian ( const connection_handshake& hs );

	template <typename T>
	static T flipEndian ( T data, size_t length = 0 );

	template <typename T>
	static void flipEndian ( T* data, size_t length = 0 );

	static uint8_t CheckOpcode ( uint8_t service_tag );

	static bool isLittleEndian;
};



template <typename T>
T SocketUtils::flipEndian ( T data, size_t length ) {
	if (length) {
		std::reverse(reinterpret_cast<char*>(&data), reinterpret_cast<char*>(&data) + length);
	} else {
		std::reverse(reinterpret_cast<char*>(&data), reinterpret_cast<char*>(&data) + sizeof(data));
	}

	return data;
}

template <typename T>
void SocketUtils::flipEndian ( T* data, size_t length ) {
	if (length) {
		std::reverse(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data) + length);
	} else {
		std::reverse(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data) + sizeof(data));
	}
}

#endif /* SHARED_SOCKET_UTILS_HPP */
