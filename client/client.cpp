#include "client.hpp"

#include <sstream>
#include <iostream>
#include <memory>
#include <functional>
#include <chrono>

#include <socket_utils.hpp>

#include <services/echo_service.hpp>

//! Used for the Generate Echo Request as a counter
static int pktcount = 0;

ClientConnection::ClientConnection(asio::io_service& io)
    : Connection(io), resolver(io)
{
    // Register the generic EchoService Handler for the Char Service Tag
    RegisterService(ServiceTags::Char, EchoService::Create());
}

void ClientConnection::Connect(const std::string& server, const std::string& port)
{
    // We've bee told to connect to a server and port.
    // We have to create a query first.
    tcp::resolver::query query(server, port);
    // Then try to resolve the server endpoint based on that query.
    resolver.async_resolve(query,
                           std::bind(&ClientConnection::handle_resolve, shared_from_this(),
                                     std::placeholders::_1, std::placeholders::_2));
}

void ClientConnection::Tick()
{
    Connection::Tick(); // Call the base method.
}

void ClientConnection::GenerateEchoRequest(const std::string& msg)
{
    // Let's create a new Char Service, Request Opcode Packet.
    Packet packet(ServiceTags::Char, CharOpcodes::Request);
    // We're going to concatenate a counter to the message.
    std::stringstream sstr;
    sstr << msg << " #" << pktcount;
    pktcount++;
    std::string newmsg = sstr.str();
    // Let's make sure to echo the concatenated string.
    CharEcho echo(newmsg);
    packet << echo;
    Send(packet);
}

void ClientConnection::handle_resolve(const asio::error_code& error, tcp::resolver::iterator ep)
{
    if (!error) {
        // We've come up with an endpoint. Let's connect.
        tcp::endpoint endpoint = *ep;
        sock.async_connect(endpoint,
                           std::bind(&ClientConnection::handle_connect, shared_from_this(),
                                     std::placeholders::_1, ++ep));
    } else {
        std::cerr << "Failed to Resolve: " << error.message() << std::endl;
    }
}

void ClientConnection::handle_connect(const asio::error_code& error, tcp::resolver::iterator ep)
{
    if (!error) {
        connected = true;
        // We're good. Send the handshake.
        send_handshake();
        // And make sure we read the server's incoming handshake
        receive_handshake();
    } else if (ep != tcp::resolver::iterator()) {
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
