#ifndef CLIENT_CLIENT_HPP
#define CLIENT_CLIENT_HPP

#include <asio.hpp>
#include <bytestream.hpp>
#include <socket_connection.hpp>
#include <socket_packet.hpp>
#include <socket_types.hpp>

#define MAX_PKT_LEN 2048

using asio::ip::tcp;

class ClientConnection : public Connection, public std::enable_shared_from_this<ClientConnection>
{
public:
	typedef std::shared_ptr<ClientConnection> pointer;
	static pointer Create ( asio::io_service& io ) {
		return pointer(new ClientConnection(io));
	}

	void Connect ( const std::string & server, const std::string & port );

    virtual void tick ( );
    
    void generate_echo_request ( const std::string & msg );
    
    void generate_echo_response ( const std::string & msg );
    
protected:
    std::shared_ptr<Connection> shared_from_derived ( ) {
    	return std::static_pointer_cast<Connection>(shared_from_this());
    }

private:

    ClientConnection ( asio::io_service& io );

    void handle_resolve ( const asio::error_code& error, tcp::resolver::iterator ep );

    void handle_connect ( const asio::error_code& error, tcp::resolver::iterator ep );
    
    virtual void parse_packet ( );

    tcp::resolver resolver;
};

#endif /* CLIENT_CLIENT_HPP */
