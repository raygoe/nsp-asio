#include "services/echo_service.hpp"

#include <socket_types.hpp>

#include <iostream>

static uint64_t GetTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock> t;
    t = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count();
}

EchoService::EchoService() : ServiceHandler(ServiceTags::Char) { }

ServiceHandler::Pointer EchoService::Create()
{
    return std::static_pointer_cast<ServiceHandler>(
               EchoService::Pointer(new EchoService())
           );
}

void EchoService::HandlePacket(Connection::Pointer con, Packet& packet)
{
    CharEcho op;
    packet >> op;

    switch (packet.Header().opcode) {
        case 0: {
            // Echo this
            Packet packet(ServiceTags::Char, CharOpcodes::Response);
            CharEcho echo(op.str());
            packet << echo;
            con->Send(packet);
        }
        break;

        case 1: {
            // Got an Echo
            std::cout << "[" << GetTimeStamp() << "] Got Message: " << op.str() << std::endl;
        }
        break;
    };
}
