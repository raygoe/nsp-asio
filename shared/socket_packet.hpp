#ifndef SHARED_SOCKET_PACKET_HPP
#define SHARED_SOCKET_PACKET_HPP

#include <bytestream.hpp>
#include <socket_types.hpp>

//! Helper object for syntax sugar
/*! Aids the setup, forming and sending of individual packets. */
class Packet : public ByteStream
{
public:
    //! Construct a new packet.
    Packet() { }

    //! Construct a new packet given a service tag and opcode.
    Packet(int service_tag, int opcode) {
        header_.service_tag = service_tag;
        header_.opcode = opcode;
    }

    //! Allow access to the ByteStream private properties.
    friend ByteStream& operator<<(ByteStream& bs, Packet& pkt);

    //! Allow access to the ByteStream private properties.
    friend ByteStream& operator>>(ByteStream& bs, Packet& pkt);

    //! Return the header.
    /*!
     * \return socket_header This is the header of the packet.
     */
    socket_header& Header() {
        return header_;
    }
private:

    //! Packet header
    socket_header header_;
};

#endif /* SHARED_SOCKET_PACKET_HPP */
