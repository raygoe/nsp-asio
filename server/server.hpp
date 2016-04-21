#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include <memory>

#include <asio.hpp>
#include <bytestream.hpp>
#include <socket_types.hpp>
#include <socket_packet.hpp>

#include <socket_connection.hpp>

#define PORT_NUM 10420
#define MAX_PKT_LEN 2048

class ServerConnection : public Connection, public std::enable_shared_from_this<ServerConnection>
{
public:
	virtual ~ServerConnection();

    typedef std::vector<ServerConnection*> connection_list;

	typedef std::shared_ptr<ServerConnection> pointer;
	static pointer Create ( asio::io_service& io, connection_list * list ) {
		return pointer(new ServerConnection(io, list));
	}

	void start ( );

    virtual void tick ( );
    
    void generate_echo_request ( const std::string & msg );
    
    void generate_echo_response ( const std::string & msg );
    
protected:
    std::shared_ptr<Connection> shared_from_derived ( ) {
    	return std::static_pointer_cast<Connection>(shared_from_this());
    }

private:

    ServerConnection ( asio::io_service& io, connection_list * root );

    void handle_resolve ( const asio::error_code& error, asio::ip::tcp::resolver::iterator ep );

    void handle_connect ( const asio::error_code& error, asio::ip::tcp::resolver::iterator ep );
    
    virtual void parse_packet ( );

    connection_list * list;
};

class my_server
{
public:
    my_server(asio::io_service & io)
        : acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 10420)), clients()
    {
        start_accept();
    }
    
    typedef std::vector<ServerConnection*> connection_list;
    
    connection_list * getConnectedClients() {
        return &clients;
    }
    
private:
    void start_accept ();
    
    void handle_accept (ServerConnection::pointer new_connection, const asio::error_code& error);

    asio::ip::tcp::acceptor acceptor;
    connection_list clients;
};

#endif /* SERVER_SERVER_HPP */
