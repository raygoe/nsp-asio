#include <socket_connection.hpp>

#include <socket_utils.hpp>

Connection::Connection ( asio::io_service& io ) : sock(io),
		in_data(), out_data(), nextLen(0), connected(0), ready(0), service_handlers()
{
}

void Connection::Tick ( ) {
    // Check for outstanding writes.
    if (connected)
    {
        send_buffer = out_data.buffer();

        if (send_buffer.size()) {
            // Write the buffer to the client.
            asio::async_write(sock, asio::buffer(send_buffer),
            		std::bind(&Connection::handle_write, shared_from_derived(), std::placeholders::_1));
        }
    }
}

void Connection::Send ( Packet & packet ) {
	out_data << packet;
}

void Connection::RegisterService ( uint8_t service_tag, ServiceHandler::Pointer handler ) {
	service_handlers[service_tag] = handler;
}

void Connection::RemoveService ( uint8_t service_tag ) {
	service_handlers.erase(service_tag);
}

void Connection::handle_write ( const asio::error_code& error ) {

}

void Connection::parse_packet ( )
{
	uint8_t service = recv_packet.Header().service_tag;
	if (service_handlers.count(service)) {
		service_handlers[service]->HandlePacket(shared_from_derived(), recv_packet);
	}
}

void Connection::handle_read_handshake ( const asio::error_code& error, size_t bytes ) {
    if (bytes == sizeof(connection_handshake)) {
        std::vector<char> b(recv_buffer, recv_buffer + bytes);
        in_data.buffer(b);

        connection_handshake cs;
        in_data >> cs;
        SocketUtils::isLittleEndian = SocketUtils::NetCheckLittleEndian(cs);
        ready = true;

        receive_next_packet();
    }
}

void Connection::handle_read_header ( const asio::error_code& error, size_t bytes ) {
    if (bytes == sizeof(socket_header)) {
        std::vector<char> b(recv_buffer, recv_buffer + bytes);
        in_data.buffer(b);
        recv_packet.clear();

        in_data >> recv_packet;

        if (!headerIsGood(recv_packet.Header()))
            sock.close(); //uhhh...

        nextLen = recv_packet.Header().packet_size - sizeof(socket_header);

        receive_next_packet();
    }
}

void Connection::handle_read_msg ( const asio::error_code& error, size_t bytes ) {
    if (bytes == nextLen) {
        std::vector<char> b(recv_buffer, recv_buffer + bytes);
        recv_packet.buffer(b);

        parse_packet();

        nextLen = 0;
        receive_next_packet();
    }
}

void Connection::send_handshake ( ) {
    connection_handshake conn{0x2233};
    out_data << conn;
}

void Connection::receive_handshake ( ) {
    asio::async_read(sock, asio::buffer(recv_buffer), asio::transfer_exactly(sizeof(connection_handshake)),
        std::bind(&Connection::handle_read_handshake, shared_from_derived(), std::placeholders::_1, std::placeholders::_2));
}

void Connection::receive_next_packet ( ) {
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

bool Connection::headerIsGood ( const socket_header & header ) {
    return (header.opcode < SocketUtils::CheckOpcode(header.service_tag));
}
