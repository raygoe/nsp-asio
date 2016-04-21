#ifndef SHARED_SOCKET_CONNECTION_HPP
#define SHARED_SOCKET_CONNECTION_HPP

#include <asio.hpp>
#include <bytestream.hpp>
#include <socket_packet.hpp>
#include <socket_types.hpp>

#include <memory>

#define MAX_PKT_LEN 2048

class Connection
{
public:
	virtual ~Connection() {}

    virtual void tick ( );

    bool isReady () { return ready; }

    bool isConnected() { return connected; }

    void send ( Packet & packet );

    asio::ip::tcp::socket& socket ( ) { return sock; }

protected:

	Connection (asio::io_service& io_service);

    void handle_write ( const asio::error_code& error );

    void handle_read_handshake ( const asio::error_code& error, size_t bytes );

    void handle_read_header ( const asio::error_code& error, size_t bytes );

    void handle_read_msg ( const asio::error_code& error, size_t bytes );

    void send_handshake ( );

    void receive_handshake ( );

    void receive_next_packet ( );

    virtual void parse_packet ( ) { }

    virtual std::shared_ptr<Connection> shared_from_derived () = 0;

    bool headerIsGood ( const socket_header & header );

    asio::ip::tcp::socket sock;
    Packet recv_packet;
    ByteStream in_data;
    ByteStream out_data;
    size_t nextLen = 0;
    bool connected = false;
    bool ready = false;
    std::vector<char> send_buffer;
    char recv_buffer[MAX_PKT_LEN];

};

#endif /* SHARED_SOCKET_CONNECTION_HPP */
