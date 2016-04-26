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

//! Packet Count for the Echo Request.
static int pktcount = 0;

ServerConnection::~ServerConnection()
{
    // Remove from the servers list
    std::cout << "User Disconnected" << endl;
    list->erase(std::remove(list->begin(), list->end(), this), list->end());
}

void ServerConnection::GenerateEchoRequest(const std::string& message)
{
    // Create a Char Service, Echo Request packet.
    Packet packet(ServiceTags::Char, CharOpcodes::Request);
    // Let's add a number after each one of the messages to keep them separated.
    std::stringstream sstr;
    sstr << message << " #" << pktcount;
    pktcount++;
    std::string newmsg = sstr.str();
    // Make sure we echo the new concatenated message.
    CharEcho echo(newmsg);
    packet << echo;
    Send(packet);
}

void ServerConnection::Tick()
{
    Connection::Tick(); // Call the base method.
}

void ServerConnection::start()
{
    // This server connection has been told its officially connected.
    connected = true;
    // Let's setup the send and receive events for handshaking
    send_handshake();
    receive_handshake();
}

ServerConnection::ServerConnection(asio::io_service& io, connection_list* list)
    : Connection(io), list(list)
{
    // This will add the generic EchoService Handler for the Char service tag.
    RegisterService(ServiceTags::Char, EchoService::Create());
}

void my_server::start_accept()
{
    // This callback creates the ServerConnection instance per connected client
    ServerConnection::Pointer new_connection = ServerConnection::Create(acceptor.get_io_service(), &clients);
    // Put them in the clients list.
    clients.push_back(new_connection.get());
    // Start the accept operation
    acceptor.async_accept(new_connection->Socket(),
                          std::bind(&my_server::handle_accept, this, new_connection, placeholders::_1));
}

void my_server::handle_accept(ServerConnection::Pointer new_connection, const asio::error_code& error)
{
    if (!error) {
        new_connection->start(); // Start the ServerConnection
    }

    // Accept another client.
    start_accept();
}
