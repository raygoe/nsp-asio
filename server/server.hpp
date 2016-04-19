#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include <memory>

#include <asio.hpp>
#include <bytestream.hpp>
#include <socket_types.hpp>

#define PORT_NUM 10420
#define MAX_PKT_LEN 2048



class tcp_connection : public std::enable_shared_from_this<tcp_connection>
{
public:
    typedef std::shared_ptr<tcp_connection> pointer;
    typedef std::vector<tcp_connection*> connection_list;
    
    virtual ~tcp_connection ();
    
    static pointer create ( asio::io_service& io, connection_list* list ) {
        return pointer(new tcp_connection(io, list));
    }
    
    asio::ip::tcp::socket& socket() { return sock; }
    
    bool isReady () { return ready; }
    bool isConnected() { return started; }
    
    void generate_echo_request ( const std::string & msg );
    
    void tick();
    
    void start();
    
private:
    obytestream obs;
    ibytestream ibs;
    bool ready = false;
    bool started = false;
    bool needToFlip;
    asio::ip::tcp::socket sock;
    char recv_buffer[MAX_PKT_LEN];
    std::vector<char> send_buffer;
    socket_header current_msg_header;
    connection_list * connected;
    size_t nextLen;
    
    tcp_connection(asio::io_service& io, connection_list * list);

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

};

class my_server
{
public:
    my_server(asio::io_service & io)
        : acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 10420)), clients()
    {
        start_accept();
    }
    
    typedef std::vector<tcp_connection*> connection_list;
    
    connection_list * getConnectedClients() {
        return &clients;
    }
    
private:
    void start_accept ();
    
    void handle_accept (tcp_connection::pointer new_connection, const asio::error_code& error);

    asio::ip::tcp::acceptor acceptor;
    connection_list clients;
};

#endif /* SERVER_SERVER_HPP */
