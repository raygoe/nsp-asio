#include "client.hpp"

#include <sstream>
#include <iostream>
#include <memory>
#include <functional>
#include <chrono>

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


my_client::my_client (asio::io_service& io_service, const std::string& server, const std::string& port)
    : resolver(io_service), socket(io_service), ibs(), obs(), nextLen(0), connected(0), ready(0), needToFlip(0)
{
    tcp::resolver::query query(server, port);
    resolver.async_resolve(query,
            std::bind(&my_client::handle_resolve, this,
                std::placeholders::_1, std::placeholders::_2));
}
    
bool my_client::isReady () { return ready; }
bool my_client::isConnected() { return connected; }

void my_client::tick ( )
{
    // Check for outstanding writes.
    if (connected)
    {
        send_buffer = obs.buffer();
        
        if (send_buffer.size()) {
            // Write the buffer to the client.
            asio::async_write(socket, asio::buffer(send_buffer), std::bind(&my_client::handle_write, this, std::placeholders::_1));
        }
    }
}

void my_client::parse_packet ( )
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

bool my_client::headerIsGood ( const socket_header & header )
{
    return (header.service_tag == 0 && header.opcode <= 1);
}

void my_client::generate_echo_request ( const std::string & msg )
{
    socket_header header;
    char_echo_data ed;
    header.service_tag = 0;
    header.opcode = 0;
                        
    std::stringstream sstr;
    sstr << msg << " #" << pktcount;
    pktcount++;
    
    std::string newmsg = sstr.str();
    
    header.packet_size = sizeof(socket_header) + sizeof(char_echo_data) + newmsg.length();
    ed.string_length = newmsg.length();
    std::vector<char> buf(newmsg.begin(), newmsg.end());
    obs << header << ed;
    obs.write(buf, newmsg.length());
}

void my_client::generate_echo_response ( const std::string & msg )
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

void my_client::handle_resolve ( const asio::error_code& error, tcp::resolver::iterator ep )
{
    if (!error)
    {
        tcp::endpoint endpoint = *ep;
        socket.async_connect(endpoint,
            std::bind(&my_client::handle_connect, this,
                std::placeholders::_1, ++ep));
    } else {
        std::cerr << "Failed to Resolve: " << error.message() << std::endl;
    }
}

void my_client::handle_connect ( const asio::error_code& error, tcp::resolver::iterator ep )
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
        socket.close();
        tcp::endpoint endpoint = *ep;
        socket.async_connect(endpoint,
            std::bind(&my_client::handle_connect, this,
                std::placeholders::_1, ++ep));
    } else {
        std::cerr << "Failed to Connect: " << error.message() << std::endl;
    }
}

void my_client::handle_write ( const asio::error_code& error )
{
    if (error)
    {
        std::cerr << "Write Error." << std::endl;
    }
}

void my_client::handle_read_handshake ( const asio::error_code& error, size_t bytes )
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

void my_client::handle_read_header ( const asio::error_code& error, size_t bytes )
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
            socket.close(); //uhhh...
            
        nextLen = current_msg_header.packet_size - sizeof(socket_header);
        
        receive_next_packet();
    }
}

void my_client::handle_read_msg ( const asio::error_code& error, size_t bytes )
{
    if (bytes == nextLen) {
        std::vector<char> b(recv_buffer, recv_buffer + bytes);
        ibs.buffer(b);
        
        parse_packet();
        
        nextLen = 0;
        receive_next_packet();
    }
}

void my_client::send_handshake ( )
{
    connection_handshake conn{0x2233};
    obs << conn;
}

void my_client::receive_handshake ( )
{
    asio::async_read(socket, asio::buffer(recv_buffer), asio::transfer_exactly(sizeof(connection_handshake)),
        std::bind(&my_client::handle_read_handshake, this, std::placeholders::_1, std::placeholders::_2));
}

void my_client::receive_next_packet ( )
{
    if (nextLen == 0) {
        // Receive a header.
        asio::async_read(socket, asio::buffer(recv_buffer), asio::transfer_exactly(sizeof(socket_header)),
            std::bind(&my_client::handle_read_header, this, std::placeholders::_1, std::placeholders::_2));
        
    } else {
        // Receive a message.
        asio::async_read(socket, asio::buffer(recv_buffer), asio::transfer_exactly(nextLen),
            std::bind(&my_client::handle_read_msg, this, std::placeholders::_1, std::placeholders::_2));
    }
}
