#ifndef SHARED_SERVICES_ECHO_SERVICE_HPP
#define SHARED_SERVICES_ECHO_SERVICE_HPP

#include <socket_connection.hpp>
#include <socket_service.hpp>

#include <memory>

//! This generic Service Handler generates and handles echos.
class EchoService : public ServiceHandler, public std::enable_shared_from_this<EchoService>
{
public:
    //! Shared Pointer for EchoService.
    typedef std::shared_ptr<EchoService> Pointer;

    //! Creates a Service Handler Pointer
    /*!
     * \return ServiceHandler::Pointer result of creating an Echo Service.
     */
    static ServiceHandler::Pointer Create();

    //! This gets called every time you want to parse an Echo packet.
    /*!
     * \param Connection::Pointer conn Pointer to the server or client connection
     * \param Packet& packet reference to the current packet.
     */
    void HandlePacket(Connection::Pointer conn, Packet& packet);

    //! Returns the current shared pointer as a base object class.
    /*!
     * \return ServiceHandler::Pointer cast pointer to base class.
     */
    ServiceHandler::Pointer shared_from_derived() {
        return std::static_pointer_cast<ServiceHandler>(shared_from_this());
    }

private:

    //! Create a new Echo Service.
    EchoService();

};


#endif /* SHARED_SERVICES_ECHO_SERVICE_HPP */
