#include <bytestream.hpp>
#include <socket_types.hpp>

#include <socket_utils.hpp>

ByteStream& operator<<(ByteStream& bs, CharEcho& ce)
{
    // Push the CharEcho structure onto the byte stream.
    // Followed by the message.
    bs << ce.ed << ce.msg;
    return bs;
}

ByteStream& operator>>(ByteStream& bs, CharEcho& ce)
{
    // Pull down the EchoData structure.
    bs >> ce.ed;

    // Flip the endians if necessary
    if (!SocketUtils::isLittleEndian) {
        SocketUtils::FlipEndian(&ce.ed.string_length);
    }

    // Create a byte buffer long enough to hold the msg string.
    bytebuffer bb(ce.ed.string_length);
    // Transfer the data to the byte buffer.
    bs >> bb;
    // Pull down the byte buffer data as a std::string.
    ce.msg = bb.str();
    return bs;
}
