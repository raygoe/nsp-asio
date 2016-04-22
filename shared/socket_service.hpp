#ifndef SHARED_SOCKET_SERVICE_HPP
#define SHARED_SOCKET_SERVICE_HPP

#include <socket_packet.hpp>

#include <memory>

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;

class ServiceHandler
{
public:
    typedef std::shared_ptr<ServiceHandler> Pointer;

    virtual ~ServiceHandler() { }

    virtual void HandlePacket(ConnectionPtr conn, Packet& packet) = 0;

    virtual std::shared_ptr<ServiceHandler> shared_from_derived() = 0;

    uint8_t GetServiceTag() {
        return service_tag;
    }

protected:

    ServiceHandler(uint8_t service_tag) : service_tag(service_tag) { }

    uint8_t service_tag;
};


#endif /* SHARED_SOCKET_SERVICE_HPP */
