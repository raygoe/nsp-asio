#ifndef SHARED_SERVICES_ECHO_SERVICE_HPP
#define SHARED_SERVICES_ECHO_SERVICE_HPP

#include <socket_connection.hpp>
#include <socket_service.hpp>

#include <memory>

class EchoService : public ServiceHandler, public std::enable_shared_from_this<EchoService>
{
public:
    typedef std::shared_ptr<EchoService> Pointer;

    static ServiceHandler::Pointer Create();

    void HandlePacket(Connection::Pointer conn, Packet& packet);

    ServiceHandler::Pointer shared_from_derived() {
        return std::static_pointer_cast<ServiceHandler>(shared_from_this());
    }

private:

    EchoService();

};


#endif /* SHARED_SERVICES_ECHO_SERVICE_HPP */
