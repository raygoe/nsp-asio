#ifndef SHARED_SOCKET_CONNECTION_HPP
#define SHARED_SOCKET_CONNECTION_HPP

#include <asio.hpp>
#include <bytestream.hpp>
#include <socket_packet.hpp>
#include <socket_types.hpp>
#include <socket_service.hpp>

#include <map>

#include <memory>

#define MAX_PKT_LEN 2048

//! Basic object for Connection.
/*! All connection types (Both Client and Server) depend upon it.
 * It contains the Service Handler map for parsing opcodes, it has all
 * the low level ASIO code in it to perform receiving and sending of packets
 * as well as the management of the Byte Stream buffers.
 *
 * \sa ClientConnection, ServerConnection
 */
class Connection
{
public:
    //! Shared Pointer to the Object.
    typedef std::shared_ptr<Connection> Pointer;

    virtual ~Connection() {
        service_handlers.clear(); // Clear the service handlers.
    }

    //! This is called every frame. It keeps the internal structures up to date.
    virtual void Tick();

    //! Whether the handshaking has finished or not.
    bool isReady() {
        return ready;
    }

    //! Whether the connection has been established or not.
    bool isConnected() {
        return connected;
    }

    //! This will take a packet and add it to the output buffer.
    /*!
     * \param Packet& packet Packet to add to the output buffer.
     */
    void Send(Packet& packet);

    //! Return the ASIO socket.
    asio::ip::tcp::socket& Socket() {
        return sock;
    }

    //! Register a Service Handler for a given Service Tag.
    /*! There can only be one service handler per service tag.
     * \param uint8_t service_tag This is the particular enum for the service handled.
     * \param ServiceHandler::Pointer handler this is the handler object to use.
     */
    void RegisterService(uint8_t service_tag, ServiceHandler::Pointer handler);

    //! Remove a Service Handler for a given service tag.
    /*!
     * \param uint8_t service_tag This is the particular enum for the service handled.
     */
    void RemoveService(uint8_t service_tag);

protected:

    //! Create a Connection object given an IO service.
    Connection(asio::io_service& io_service);

    //! Callback for async_write.
    /*!
     * \param asio::error_code& error This is defined if there was an error during the operation.
     */
    void handle_write(const asio::error_code& error);

    //! Callback for async_read. Specifically to receive handshakes.
    /*!
     * \param asio::error_code& error This is defined if there was an error during the operation.
     * \param size_t bytes How many bytes were received.
     */
    void handle_read_handshake(const asio::error_code& error, size_t bytes);

    //! Callback for async_read. Specifically to receive Packet Headers.
    /*!
     * \param asio::error_code& error This is defined if there was an error during the operation.
     * \param size_t bytes How many bytes were received.
     */
    void handle_read_header(const asio::error_code& error, size_t bytes);

    //! Callback for async_read. Specifically to receive Message Data.
    /*!
     * \param asio::error_code& error This is defined if there was an error during the operation.
     * \param size_t bytes How many bytes were received.
     */
    void handle_read_msg(const asio::error_code& error, size_t bytes);

    //! Sends the handshake structure to the connected endpoint.
    void send_handshake();

    //! Sets up the receive operation for receiving handshakes.
    void receive_handshake();

    //! State machine for setting up to receive packet headers or message bodies.
    void receive_next_packet();

    //! Call the handler registered for a given service_tag on the current received packet.
    void parse_packet();

    //! Pure virtual function for the descendants to implement.
    /*!
     * \return std::shared_ptr<Connection> shared pointer to the object.
     */
    virtual std::shared_ptr<Connection> shared_from_derived() = 0;

    //! Whether the packet header is sane or not.
    /*!
     * \param socket_header& header Socket Header structure from the packet to check.
     * \return Whether or not the header seems sane.
     */
    bool headerIsGood(const socket_header& header);

    //! ASIO Socket
    asio::ip::tcp::socket sock;

    //! Current packet being built by the asio handlers.
    Packet recv_packet;

    //! Input Byte Stream
    ByteStream in_data;

    //! Output Byte Stream
    ByteStream out_data;

    //! Next Packet Body Length
    size_t nextLen = 0;

    //! Whether the connection has been established
    bool connected = false;

    //! Whether the connection has received its handshakes
    bool ready = false;

    //! Send output buffer for the asio handler
    std::vector<char> send_buffer;

    //! Receive input buffer for the asio handler
    char recv_buffer[MAX_PKT_LEN];

    //! Service Handler map for std::pair<service_tag, ServiceHandler*>'s.
    std::map<uint8_t, ServiceHandler::Pointer> service_handlers;

};

#endif /* SHARED_SOCKET_CONNECTION_HPP */
