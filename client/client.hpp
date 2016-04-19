#ifndef CLIENT_CLIENT_HPP
#define CLIENT_CLIENT_HPP

#include <asio.hpp>
#include <bytestream.hpp>
#include <socket_types.hpp>

#define MAX_PKT_LEN 2048

using asio::ip::tcp;

class my_client
{
public:
    my_client (asio::io_service& io_service, const std::string& server, const std::string& port);
    
    void tick ( );
    
    void generate_echo_request ( const std::string & msg );
    
    bool isReady ();
    bool isConnected();

private:

    void handle_resolve ( const asio::error_code& error, tcp::resolver::iterator ep );

    void handle_connect ( const asio::error_code& error, tcp::resolver::iterator ep );

    void handle_write ( const asio::error_code& error );

    void handle_read_handshake ( const asio::error_code& error, size_t bytes );
    
    void handle_read_header ( const asio::error_code& error, size_t bytes );
    
    void handle_read_msg ( const asio::error_code& error, size_t bytes );
    
    void send_handshake ( );
    
    void receive_handshake ( );
    
    void receive_next_packet ( );
    
    void parse_packet ( );
    
    bool headerIsGood ( const socket_header & header );
    
    void generate_echo_response ( const std::string & msg );

    tcp::resolver resolver;
    tcp::socket socket;
    ibytestream ibs;
    obytestream obs;
    size_t nextLen = 0;
    socket_header current_msg_header;
    bool connected = false;
    bool ready = false;
    bool needToFlip = false;
    std::vector<char> send_buffer;
    char recv_buffer[MAX_PKT_LEN];
};

#endif /* CLIENT_CLIENT_HPP */
