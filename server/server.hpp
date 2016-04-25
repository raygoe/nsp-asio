#ifndef SERVER_SERVER_HPP
#define SERVER_SERVER_HPP

#include <memory>

#include <asio.hpp>
#include <bytestream.hpp>
#include <socket_types.hpp>
#include <socket_packet.hpp>

#include <socket_connection.hpp>

#define PORT_NUM 10420
#define MAX_PKT_LEN 2048

//! Server Connection object. A new one gets created every time someone connects.
class ServerConnection : public Connection, public std::enable_shared_from_this<ServerConnection>
{
public:
    //! Perform the necessary shutdown procedures.
    virtual ~ServerConnection();

    //! Server Connection List (from the root object)
    typedef std::vector<ServerConnection*> connection_list;

    //! Shared pointer to the ServerConnection
    typedef std::shared_ptr<ServerConnection> Pointer;

    //! Create a new Server Connection. Be sure to pass in the pointer to the current connection_list.
    /*!
     * \param asio::io_service& io ASIO IO Service Reference
     * \param connection_list* list Pointer to the list of all connected clients.
     * \return ServerConnection::Pointer shared pointer to the new ServerConnection.
     */
    static ServerConnection::Pointer Create(asio::io_service& io, connection_list* list) {
        return ServerConnection::Pointer(new ServerConnection(io, list));
    }

    //! Start serving to that client.
    void start();

    //! Fires every frame, keeps the internal state consistent.
    virtual void Tick();

    //! Generate an echo request given a message.
    /*!
     * \param std::string msg message to echo
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

    //! Create a new ServerConnection.
    /*!
     * \param asio::io_service& io ASIO IO Service Reference
     * \param connection_list * root Pointer to the list of all connected clients.
     */
    ServerConnection(asio::io_service& io, connection_list* root);

    //! Callback for the resolver handler.
    /*!
     * \param asio::error_code& error Defined if there was some error in the operation.
     * \param asio::ip::tcp::resovler::iterator Endpoint in question
     */
    void handle_resolve(const asio::error_code& error, asio::ip::tcp::resolver::iterator ep);

    //! Callback for the resolver handler.
    /*!
     * \param asio::error_code& error Defined if there was some error in the operation.
     * \param asio::ip::tcp::resovler::iterator Endpoint in question
     */
    void handle_connect(const asio::error_code& error, asio::ip::tcp::resolver::iterator ep);

    //! Pointer to the list of all connected clients.
    connection_list* list;
};

//! Helper object to manage the accepting of connecting clients.
class my_server
{
public:

    //! Create a server management object
    /*!
     * \param asio::io_service & io ASIO IO Service Handler
     */
    my_server(asio::io_service& io)
        : acceptor(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 10420)), clients() {
        start_accept(); // Start the acceptance handler.
    }

    //! List of all connected clients.
    typedef std::vector<ServerConnection*> connection_list;

    //! Return a pointer to the list of all connected clients.
    /*!
     * \return connection_list* Pointer to the list of all connected clients.
     */
    connection_list* getConnectedClients() {
        return &clients;
    }

private:

    //! Start the asio async_accept handler.
    void start_accept();

    //! Callback for receiving a client that desires to connect.
    /*!
     * \param ServerConnection::Pointer new_connection the person trying to connect.
     * \param asio::error_code& error Defined if there was some problem accepting the user.
     */
    void handle_accept(ServerConnection::Pointer new_connection, const asio::error_code& error);

    //! Server Acceptor
    asio::ip::tcp::acceptor acceptor;

    //! List of all connected clients.
    connection_list clients;
};

#endif /* SERVER_SERVER_HPP */
