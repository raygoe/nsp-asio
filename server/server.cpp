#include "server.hpp"

#include <sstream>
#include <iostream>
#include <memory>
#include <functional>
#include <chrono>

#include <socket_utils.hpp>

using namespace std;
using asio::ip::tcp;

static int pktcount = 0;

static uint64_t GetTimeStamp() {
    std::chrono::time_point<std::chrono::system_clock> t;
    t = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count();
}

ServerConnection::~ServerConnection () {
    // Remove from the servers list
    std::cout << "User Disconnected" << endl;
    list->erase(std::remove(list->begin(), list->end(), this), list->end());
}

void ServerConnection::generate_echo_request ( const std::string & message )
{
    Packet packet(0, 0);
                        
    std::stringstream sstr;
    sstr << message << " #" << pktcount;
    pktcount++;
    std::string newmsg = sstr.str();

    CharEcho echo(newmsg);
    packet << echo;
    
    send ( packet );
}

void ServerConnection::generate_echo_response ( const std::string & msg )
{
    Packet packet(0, 1);
    CharEcho echo(msg);
    packet << echo;

    send ( packet );
}

void ServerConnection::parse_packet ( )
{
    switch (recv_packet.header().service_tag) {
        case 0:
        {
        	CharEcho op;
        	recv_packet >> op;

            switch (recv_packet.header().opcode) {
                case 0:
                {
                    // Echo this
                    generate_echo_response ( op.str() );
                }
                break;
                case 1:
                {
                    // Got an Echo
                    std::cout << "[" << GetTimeStamp() << "] Got Message: " << op.str() << std::endl;
                }
                break;
            
            };
        }
        break;
    };
}

void ServerConnection::tick()
{
	Connection::tick();
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
}

void my_server::start_accept ()
{
    ServerConnection::pointer new_connection = ServerConnection::Create(acceptor.get_io_service(), &clients);
        
    clients.push_back(new_connection.get());
    
    acceptor.async_accept(new_connection->socket(),
    		std::bind(&my_server::handle_accept, this, new_connection, placeholders::_1));
}

void my_server::handle_accept (ServerConnection::pointer new_connection, const asio::error_code& error)
{
    if (!error) {
        new_connection->start();
    }
    
    start_accept();
}
