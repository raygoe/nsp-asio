#ifndef SHARED_SOCKET_TYPES_HPP
#define SHARED_SOCKET_TYPES_HPP

#include <cstdint>

#include <bytestream.hpp>

//! Connection Handshake Structure
/*! This is to detect the particular endian of the connection. */
struct connection_handshake {
    uint16_t magic_num; /*!< 0x2233 by default */
};

//! Header for each packet
/*! Every packet begins with this structure.
 * It tells you how long the packet is, what the service tag used is,
 * and the opcode that the service tag should operate on.
 */
struct socket_header {
    uint16_t packet_size; /*!< Size of the packet (including header) in bytes */
    uint8_t service_tag; /*!< Handler identifier to use to parse packet. */
    uint8_t opcode; /*!< Type of operation to pass to the handler. */
};

//! Service tags for the handlers.
struct ServiceTags {
    enum uint8_t {
        Char
    };
};

//! For the Char Service Tag, This is the sub-data.
struct char_echo_data {
    uint64_t string_length;
};

//! This is the Opcodes in particular for the Char Service Tag.
/*! Note the EnumCount, that is used to detect the size of the enum */
struct CharOpcodes {
    enum uint8_t {
        Request, Response,
        EnumCount
    };
};

//! Char Echo Helper Object
/*! This object simplifies the pulling down (from a packet) of Char service tag types */
class CharEcho
{
public:
    CharEcho() { }

    //! Generate a structure given a message string.
    /*!
     * \param std::string Message to echo.
     */
    CharEcho(std::string msg) : msg(msg) {
        ed.string_length = msg.length();
    }

    //! Allow access to the ByteStream private properties.
    friend ByteStream& operator<<(ByteStream& bs, CharEcho& ce);

    //! Allow access to the ByteStream private properties.
    friend ByteStream& operator>>(ByteStream& bs, CharEcho& ce);

    //! Return the Char Echo string.
    /*!
     * \return std::string message to echo or has been echo'd.
     */
    std::string str() const {
        return msg;
    }

private:

    //! Internal echo data structure.
    char_echo_data ed;

    //! Message to echo.
    std::string msg;
};

#endif /* SHARED_SOCKET_TYPES_HPP */
