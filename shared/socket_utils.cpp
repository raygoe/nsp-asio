#include <socket_utils.hpp>

bool SocketUtils::isLittleEndian = true;

bool SocketUtils::NetCheckLittleEndian(const connection_handshake& hs)
{
    return (*reinterpret_cast<const char*>(&hs) == 0x33);
}

uint8_t SocketUtils::CheckOpcode(uint8_t service_tag)
{
    // The switch here checks all known service tags.
    // If the tag isn't there it returns zero. Zero will result in a failed opcode check.
    switch (service_tag) {
        case ServiceTags::Char:
            return CharOpcodes::EnumCount; // This value is always 1 higher than the highest enum.

        default:
            return 0;
    }
}
