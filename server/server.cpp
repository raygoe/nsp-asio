#include "server.hpp"

#include <sstream>
#include <iostream>
#include <memory>
#include <functional>
#include <chrono>


using namespace std;
using asio::ip::tcp;

static bool net_is_little_endian ( const connection_handshake& hs ) {
    return (*reinterpret_cast<const char*>(&hs) == 0x33);
}

static int pktcount = 0;

static uint64_t GetTimeStamp() {
    std::chrono::time_point<std::chrono::system_clock> t;
    t = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count();
}

template <typename T>
static T flipEndian ( T data, size_t length = 0 ) {
    if (length) {
        std::reverse(reinterpret_cast<char*>(&data), reinterpret_cast<char*>(&data) + length);
    } else {
        std::reverse(reinterpret_cast<char*>(&data), reinterpret_cast<char*>(&data) + sizeof(data));
    }
    
    return data;
}

template <typename T>
static void flipEndian ( T* data, size_t length = 0 ) {
    if (length) {
        std::reverse(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data) + length);
    } else {
        std::reverse(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data) + sizeof(data));
    }
}

tcp_connection::~tcp_connection () {
    // Remove from the servers list
    std::cout << "User Disconnected" << endl;
    connected->erase(std::remove(connected->begin(), connected->end(), this), connected->end());
}

void tcp_connection::generate_echo_request ( const std::string & message )
{
    socket_header header;
    char_echo_data ced;
    header.service_tag = 0;
    header.opcode = 0;
    
                        
    std::stringstream sstr;
    sstr << message << " #" << pktcount;
    pktcount++;
    
    std::string newmsg = sstr.str();
    
    header.packet_size = sizeof(socket_header) + sizeof(char_echo_data) + newmsg.length();
    ced.string_length = newmsg.length();
    std::vector<char> buf(newmsg.begin(), newmsg.end());
    obs << header << ced;
    obs.write(buf, newmsg.length());
}

void tcp_connection::generate_echo_response ( const std::string & msg )
{
    socket_header header;
    char_echo_data ed;
    header.service_tag = 0;
    header.opcode = 1;
    header.packet_size = sizeof(socket_header) + sizeof(char_echo_data) + msg.length();
    ed.string_length = msg.length();
    std::vector<char> buf(msg.begin(), msg.end());
    obs << header << ed;
    obs.write(buf, msg.length());
}

void tcp_connection::parse_packet ( )
{
    switch (current_msg_header.service_tag) {
        case 0:
        {
            switch (current_msg_header.opcode) {
                case 0:
                {
                    // Echo this
                    char_echo_data op;
                    ibs >> op;
                    
                    if (needToFlip)
                        flipEndian(&op.string_length);
                    
                    std::vector<char> msg(op.string_length);
                    ibs.read(msg, op.string_length);
                    std::string echoMsg(msg.begin(), msg.end());
                    
                    generate_echo_response ( echoMsg );
                }
                break;
                case 1:
                {
                    // Got an Echo
                    char_echo_data op;
                    ibs >> op;
                    
                    if (needToFlip)
                        flipEndian(&op.string_length);
                        
                    std::vector<char> msg(op.string_length);
                    ibs.read(msg, op.string_length);
                    
                    std::cout << "[" << GetTimeStamp() << "] Got Message: " << std::string(msg.begin(), msg.end()) << std::endl;
                }
                break;
            
            };
        }
        break;
    };
}

void tcp_connection::tick()
{
    // Check for outstanding writes.
    if (started)
    {
        send_buffer = obs.buffer();
        
        if (send_buffer.size()) {
            // Write the buffer to the client.
            asio::async_write(sock, asio::buffer(send_buffer), std::bind(&tcp_connection::handle_write, shared_from_this(), std::placeholders::_1));
        }
    }
}

void tcp_connection::start()
{
    started = true;
    connection_handshake conn{0x2233};
    obs << conn;
    std::cout << "Sent Connection Handshake" << std::endl;
    
    asio::async_read(sock, asio::buffer(recv_buffer), asio::transfer_exactly(sizeof(connection_handshake)), bind(&tcp_connection::handle_read_handshake, shared_from_this(), placeholders::_1, placeholders::_2));
}

tcp_connection::tcp_connection(asio::io_service& io, connection_list * list)
    : obs(), ibs(), sock(io), recv_buffer(), send_buffer(), current_msg_header(), connected(list), nextLen(0)
{
}

bool tcp_connection::headerIsGood ( const socket_header & header )
{
    return (header.service_tag == 0 && header.opcode <= 1);
}

void tcp_connection::handle_write ( const asio::error_code& error )
{
}

void tcp_connection::receive_handshake ( )
{
    asio::async_read(sock, asio::buffer(recv_buffer), asio::transfer_exactly(sizeof(connection_handshake)),
        std::bind(&tcp_connection::handle_read_handshake, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void tcp_connection::receive_next_packet ( )
{
    if (nextLen == 0) {
        // Receive a header.
        asio::async_read(sock, asio::buffer(recv_buffer), asio::transfer_exactly(sizeof(socket_header)),
            std::bind(&tcp_connection::handle_read_header, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        
    } else {
        // Receive a message.
        asio::async_read(sock, asio::buffer(recv_buffer), asio::transfer_exactly(nextLen),
            std::bind(&tcp_connection::handle_read_msg, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }
}

void tcp_connection::handle_read_handshake ( const asio::error_code& error, size_t bytes )
{
    if (bytes == sizeof(connection_handshake)) {
        std::vector<char> b(recv_buffer, recv_buffer + bytes);
        ibs.buffer(b);
        
        connection_handshake cs;
        ibs >> cs;
        needToFlip = !net_is_little_endian(cs);
        ready = true;
        
        receive_next_packet();
    }
}

void tcp_connection::handle_read_header ( const asio::error_code& error, size_t bytes )
{
    if (bytes == sizeof(socket_header)) {
        std::vector<char> b(recv_buffer, recv_buffer + bytes);
        ibs.buffer(b);
        
        ibs >> current_msg_header;
        
        if (needToFlip) {
            flipEndian(&current_msg_header.service_tag);
            flipEndian(&current_msg_header.opcode);
        } 
        
        if (!headerIsGood(current_msg_header))
            sock.close(); //uhhh...
            
        nextLen = current_msg_header.packet_size - sizeof(socket_header);
        
        receive_next_packet();
    }
}

void tcp_connection::handle_read_msg ( const asio::error_code& error, size_t bytes )
{
    if (bytes == nextLen) {
        std::vector<char> b(recv_buffer, recv_buffer + bytes);
        ibs.buffer(b);
        
        parse_packet();
        
        nextLen = 0;
        receive_next_packet();
    }
}

void tcp_connection::send_handshake ( )
{
    connection_handshake conn{0x2233};
    obs << conn;
}

void my_server::start_accept ()
{
    tcp_connection::pointer new_connection = tcp_connection::create(acceptor.get_io_service(), &clients);
        
    clients.push_back(new_connection.get());
    
    acceptor.async_accept(new_connection->socket(), bind(&my_server::handle_accept, this, new_connection, placeholders::_1));
}

void my_server::handle_accept (tcp_connection::pointer new_connection, const asio::error_code& error)
{
    if (!error) {
        new_connection->start();
    }
    
    start_accept();
}
