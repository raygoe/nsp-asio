#ifndef SHARED_SOCKET_UTILS_HPP
#define SHARED_SOCKET_UTILS_HPP

#include <socket_types.hpp>

#include <algorithm>

//! Static object for several different utilities.
class SocketUtils
{
public:

    //! Checks if the given connection handshake demonstrates little endianness.
    /*!
     * \param connection_handshake& hs Handshake reference.
     * \return bool whether the handshake represents little endian.
     */
    static bool NetCheckLittleEndian(const connection_handshake& hs);

    //! Flips the endian of the given data and returns it.
    /*!
     * \param T data Data to flip endian.
     * \param size_t length Length of the data (defaults to zero, use sizeof)
     * \return T Flipped endian version of data.
     */
    template <typename T>
    static T FlipEndian(T data, size_t length = 0);

    //! Flips the endian of the given data in place.
    /*!
     * \param T* data Data to flip endian in place.
     * \param size_t length Length of the data (defaults to zero, use sizeof)
     */
    template <typename T>
    static void FlipEndian(T* data, size_t length = 0);

    //! Checks whether the opcode is sane.
    /*! Checks sanity by returning the number of opcodes in a given
     *  service tag. If the opcode or service_tag does not compute, the
     *  connection can detect it and close the socket.
     *
     * \param uint8_t service_tag service_tag to return amount of opcodes.
     * \return uint8_t Returns the opcode amount given a service_tag.
     */
    static uint8_t CheckOpcode(uint8_t service_tag);

    //! Whether the application indicates little endian or not.
    static bool isLittleEndian;
};



template <typename T>
T SocketUtils::FlipEndian(T data, size_t length)
{
    if (length) {
        std::reverse(reinterpret_cast<char*>(&data), reinterpret_cast<char*>(&data) + length);
    } else {
        std::reverse(reinterpret_cast<char*>(&data), reinterpret_cast<char*>(&data) + sizeof(data));
    }

    return data;
}

template <typename T>
void SocketUtils::FlipEndian(T* data, size_t length)
{
    if (length) {
        std::reverse(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data) + length);
    } else {
        std::reverse(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data) + sizeof(data));
    }
}

#endif /* SHARED_SOCKET_UTILS_HPP */
