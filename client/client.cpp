#include "client.hpp"

#include <sstream>
#include <iostream>
#include <memory>
#include <functional>
#include <chrono>

#include <socket_utils.hpp>

static int pktcount = 0;

static uint64_t GetTimeStamp() {
    std::chrono::time_point<std::chrono::system_clock> t;
    t = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count();
}

ClientConnection::ClientConnection ( asio::io_service& io )
	: Connection(io), resolver(io)
{
}

void ClientConnection::Connect ( const std::string & server, const std::string & port )
{
    tcp::resolver::query query(server, port);
    resolver.async_resolve(query,
            std::bind(&ClientConnection::handle_resolve, shared_from_this(),
                std::placeholders::_1, std::placeholders::_2));
}

void ClientConnection::tick ( )
{
	Connection::tick ();
}

void ClientConnection::parse_packet ( )
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

void ClientConnection::generate_echo_request ( const std::string & msg )
{
    Packet packet(0, 0);
                        
    std::stringstream sstr;
    sstr << msg << " #" << pktcount;
    pktcount++;
    std::string newmsg = sstr.str();

    CharEcho echo(newmsg);
    packet << echo;
    
    send ( packet );
}

void ClientConnection::generate_echo_response ( const std::string & msg )
{
    Packet packet(0, 1);
    CharEcho echo(msg);
    packet << echo;

    send ( packet );
}

void ClientConnection::handle_resolve ( const asio::error_code& error, tcp::resolver::iterator ep )
{
    if (!error)
    {
        tcp::endpoint endpoint = *ep;
        sock.async_connect(endpoint,
            std::bind(&ClientConnection::handle_connect, shared_from_this(),
                std::placeholders::_1, ++ep));
    } else {
        std::cerr << "Failed to Resolve: " << error.message() << std::endl;
    }
}

void ClientConnection::handle_connect ( const asio::error_code& error, tcp::resolver::iterator ep )
{
    if (!error)
    {
        connected = true;
        // We're good. Send the handshake.
        send_handshake();
        // And make sure we read the server's incoming handshake
        receive_handshake();
    } else if (ep != tcp::resolver::iterator() ) {
        // Connection failed. Try the next endpoint.
        sock.close();
        tcp::endpoint endpoint = *ep;
        sock.async_connect(endpoint,
            std::bind(&ClientConnection::handle_connect, shared_from_this(),
                std::placeholders::_1, ++ep));
    } else {
        std::cerr << "Failed to Connect: " << error.message() << std::endl;
    }
}
