#include "server.hpp"

#include <sstream>
#include <iostream>
#include <memory>
#include <functional>
#include <chrono>

#include <socket_utils.hpp>

#include <services/echo_service.hpp>

using namespace std;
using asio::ip::tcp;

static int pktcount = 0;

ServerConnection::~ServerConnection () {
    // Remove from the servers list
    std::cout << "User Disconnected" << endl;
    list->erase(std::remove(list->begin(), list->end(), this), list->end());
}

void ServerConnection::GenerateEchoRequest ( const std::string & message )
{
    Packet packet(0, 0);
                        
    std::stringstream sstr;
    sstr << message << " #" << pktcount;
    pktcount++;
    std::string newmsg = sstr.str();

    CharEcho echo(newmsg);
    packet << echo;
    
    Send ( packet );
}

void ServerConnection::Tick()
{
	Connection::Tick();
}

void ServerConnection::start()
{
    connected = true;
    send_handshake();
    receive_handshake();
}

ServerConnection::ServerConnection(asio::io_service& io, connection_list * list)
    : Connection(io), list(list)
{
	RegisterService(ServiceTags::Char, EchoService::Create());
}

void my_server::start_accept ()
{
    ServerConnection::Pointer new_connection = ServerConnection::Create(acceptor.get_io_service(), &clients);
        
    clients.push_back(new_connection.get());
    
    acceptor.async_accept(new_connection->Socket(),
    		std::bind(&my_server::handle_accept, this, new_connection, placeholders::_1));
}

void my_server::handle_accept (ServerConnection::Pointer new_connection, const asio::error_code& error)
{
    if (!error) {
        new_connection->start();
    }
    
    start_accept();
}
