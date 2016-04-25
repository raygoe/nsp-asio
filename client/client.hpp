#ifndef CLIENT_CLIENT_HPP
#define CLIENT_CLIENT_HPP

#include <asio.hpp>
#include <bytestream.hpp>
#include <socket_connection.hpp>
#include <socket_packet.hpp>
#include <socket_types.hpp>

#define MAX_PKT_LEN 2048

using asio::ip::tcp;

//! Connection object specifically designed for Client connections.
class ClientConnection : public Connection, public std::enable_shared_from_this<ClientConnection>
{
public:
    //! Shared Pointer to the ClientConnection
    typedef std::shared_ptr<ClientConnection> Pointer;

    //! Factory method for creating a Client Connection.
    /*!
     * \param asio::io_service& io ASIO IO service reference
     * \return ClientConnection::Pointer shared pointer to the Client Connection.
     */
    static ClientConnection::Pointer Create(asio::io_service& io) {
        return ClientConnection::Pointer(new ClientConnection(io));
    }

    //! Connect to a given host and port.
    /*!
     * \param std::string server Host name to connect to
     * \param std::string Port Port or service to connect to
     */
    void Connect(const std::string& server, const std::string& port);

    //! Gets called every frame, keeps internal data structure current.
    virtual void Tick();

    //! Generate an echo request packet given a message.
    /*! Also make sure we make each message different.
     * \param std::string msg Message to echo.
     */
    void GenerateEchoRequest(const std::string& msg);

protected:

    //! Define the lower level virtual function for shared pointers.
    /*! This is used because we can only have one std::shared_from_this on a chain.
     *  We put it at the very end to keep it easy. The lower level virtual object needs
     *  a pointer to Connection so we give them this.
     *
     * \return Connection::Pointer Pointer to this object, cast to a base object pointer.
     */
    Connection::Pointer shared_from_derived() {
        return std::static_pointer_cast<Connection>(shared_from_this());
    }

private:

    //! Create a client connection given an ASIO::IO Service.
    ClientConnection(asio::io_service& io);

    //! Callback for Resolving an ASIO Query.
    /*!
     * \param asio::error_code& error Defined if there was some error resolving the query.
     * \param tcp::resolver::iterator endpoint The current endpoint involved.
     */
    void handle_resolve(const asio::error_code& error, tcp::resolver::iterator ep);

    //! Callback for Connecting to an endpoint.
    /*!
     * \param asio::error_code& error Defined if there was some error connecting to the endpoint.
     * \param tcp::resolver::iterator endpoint The current endpoint involved.
     */
    void handle_connect(const asio::error_code& error, tcp::resolver::iterator ep);

    //! Resolver for the Client.
    tcp::resolver resolver;
};

#endif /* CLIENT_CLIENT_HPP */
