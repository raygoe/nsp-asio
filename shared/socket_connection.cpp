#include <socket_connection.hpp>

#include <socket_utils.hpp>

Connection::Connection(asio::io_service& io) : sock(io),
    in_data(), out_data(), nextLen(0), connected(0), ready(0), service_handlers()
{
}

void Connection::Tick()
{
    // Check for outstanding writes.
    if (connected) {
        send_buffer = out_data.buffer(); // Transfer the output data buffer to send_buffer.

        // Check if the send buffer contains data.
        if (send_buffer.size()) {
            // Write the buffer over the socket.
            asio::async_write(sock, asio::buffer(send_buffer),
                              std::bind(&Connection::handle_write, shared_from_derived(), std::placeholders::_1));
        }
    }
}

void Connection::Send(Packet& packet)
{
    // Write the packet to the output data buffer.
    out_data << packet;
}

void Connection::RegisterService(uint8_t service_tag, ServiceHandler::Pointer handler)
{
    // This will add the service handler to the map.
    service_handlers[service_tag] = handler;
}

void Connection::RemoveService(uint8_t service_tag)
{
    // This function automatically checks if service_tag exists.
    // It removes it if it does.
    service_handlers.erase(service_tag);
}

void Connection::handle_write(const asio::error_code& error)
{
    // Nothing to do here.
}

void Connection::parse_packet()
{
    // Returns what service the packet requested.
    uint8_t service = recv_packet.Header().service_tag;

    if (service_handlers.count(service)) {
        // Send the packet to the service handler. Shared_from_derived gets the shared_ptr.
        service_handlers[service]->HandlePacket(shared_from_derived(), recv_packet);
    }
}

void Connection::handle_read_handshake(const asio::error_code& error, size_t bytes)
{
    if (bytes == sizeof(connection_handshake)) {
        // Make sure that we only process packets of the correct length.
        // This is a temporary buffer that contains the received bytes.
        std::vector<char> b(recv_buffer, recv_buffer + bytes);
        // Load the data from the temporary buffer.
        in_data.buffer(b);
        // We're expecting a connection handshake.
        connection_handshake cs;
        in_data >> cs; // Load the data into the structure.
        // The connection handshake is used to determine if the endpoint is little or big endian.
        SocketUtils::isLittleEndian = SocketUtils::NetCheckLittleEndian(cs);
        ready = true;
        // Start the next asynchronous receive.
        receive_next_packet();
    }
}

void Connection::handle_read_header(const asio::error_code& error, size_t bytes)
{
    if (bytes == sizeof(socket_header)) {
        // Make sure that we only process packets of the correct length.
        // This is a temporary buffer that contains the received bytes.
        std::vector<char> b(recv_buffer, recv_buffer + bytes);
        // Load the data from the temporary buffer.
        in_data.buffer(b);
        recv_packet.clear(); // Clear the receive packet buffer.
        // Note that Packet& is a ByteStream Buffer.
        in_data >> recv_packet; // Transfer the data to the packet.

        if (!headerIsGood(recv_packet.Header()))
            sock.close(); // If we have a corrupted header, we just close the socket.

        // We need to calculate the remaining packet size. This calculates that.
        nextLen = recv_packet.Header().packet_size - sizeof(socket_header);
        // Start the next asynchronous receive.
        receive_next_packet();
    }
}

void Connection::handle_read_msg(const asio::error_code& error, size_t bytes)
{
    if (bytes == nextLen) {
        // Make sure that we only process packets of the correct length.
        // This is a temporary buffer that contains the received bytes.
        std::vector<char> b(recv_buffer, recv_buffer + bytes);
        // Load the data from the temporary buffer.
        recv_packet.buffer(b);
        // Send the packet to the ServiceHandlers to parse it.
        parse_packet();
        // Reset the packet state machine.
        nextLen = 0;
        // Start the next asynchronous receive.
        receive_next_packet();
    }
}

void Connection::send_handshake()
{
    // We use 0x2233 to check to see if the client or server is big or little endian.
    connection_handshake conn {0x2233};
    out_data << conn;
}

void Connection::receive_handshake()
{
    // Receive a handshake.
    asio::async_read(sock, asio::buffer(recv_buffer), asio::transfer_exactly(sizeof(connection_handshake)),
                     std::bind(&Connection::handle_read_handshake, shared_from_derived(), std::placeholders::_1, std::placeholders::_2));
}

void Connection::receive_next_packet()
{
    if (nextLen == 0) {
        // Receive a header.
        asio::async_read(sock, asio::buffer(recv_buffer), asio::transfer_exactly(sizeof(socket_header)),
                         std::bind(&Connection::handle_read_header, shared_from_derived(), std::placeholders::_1, std::placeholders::_2));
    } else {
        // Receive a message.
        asio::async_read(sock, asio::buffer(recv_buffer), asio::transfer_exactly(nextLen),
                         std::bind(&Connection::handle_read_msg, shared_from_derived(), std::placeholders::_1, std::placeholders::_2));
    }
}

bool Connection::headerIsGood(const socket_header& header)
{
    // This checks the opcode against the service tag. See SocketUtils::CheckOpcode for more info.
    return (header.opcode < SocketUtils::CheckOpcode(header.service_tag));
}
