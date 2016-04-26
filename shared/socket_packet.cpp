#include <bytestream.hpp>
#include <socket_packet.hpp>

#include <socket_utils.hpp>

ByteStream& operator<<(ByteStream& bs, Packet& pkt)
{
    // Set the packet size equal to the byte stream length plus the socket header size.
    pkt.header_.packet_size = pkt.count() + sizeof(socket_header);
    // Create a temporary byte buffer to hold the packet.
    bytebuffer buf(pkt.count());
    // Transfer the packet to the byte buffer.
    pkt >> buf;
    // Transfer the header and then the buffer to the ByteStream.
    bs << pkt.header_ << buf;
    return bs;
}

ByteStream& operator>>(ByteStream& bs, Packet& pkt)
{
    // Create a temporary buffer to hold the ByteStream data.
    bytebuffer buf(bs.count());
    // Transfer the byte stream data to the temporary buffer.
    bs >> buf;
    // Transfer the buffer to the backet.
    pkt << buf;
    // Remove the header.
    pkt >> pkt.header_;

    // If the user is big endian, flip the endians of the data values.
    if (!SocketUtils::isLittleEndian) {
        SocketUtils::FlipEndian(&pkt.header_.service_tag);
        SocketUtils::FlipEndian(&pkt.header_.opcode);
        SocketUtils::FlipEndian(&pkt.header_.packet_size);
    }

    return bs;
}
