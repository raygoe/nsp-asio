#include <iostream>
#include <ctime>
#include <client.hpp>
#include <asio.hpp>
#include <chrono>
#include <thread>

#include <socket_packet.hpp>
#include <socket_types.hpp>
#include <bytestream.hpp>
#include <vector>
#include <string>

using namespace std;
using asio::ip::tcp;

int main(int argc, char** argv)
{
    try {
        // Create the IO service for the client.
        asio::io_service io;
        // Create a Client Connection
        ClientConnection::Pointer client = ClientConnection::Create(io);
        client->Connect(argv[1], argv[2]); // Connect to the listed host and port

        while (true) {
            io.poll(); // Receive one event.

            if (client->isConnected()) {
                // If the client is connected, tick the Client Connection
                client->Tick();

                if (client->isReady()) {
                    // If the client has received a handshake, Generate some Echo Requests.
                    client->GenerateEchoRequest(std::string("This is from the client."));
                }
            }

            // This emulates about 60 ticks per second.
            std::this_thread::sleep_for(std::chrono::microseconds(15000));
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
