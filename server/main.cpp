#include <iostream>
#include <asio.hpp>
#include <chrono>
#include <thread>

#include "server.hpp"

using namespace std;
using asio::ip::tcp;

int main(int argc, char** argv)
{
    try {
        // Create the io service for ASIO.
        asio::io_service io;
        // Create the server object
        my_server server(io);

        while (true) {
            io.poll(); // Poll the events once.

            for (auto * conn : *server.getConnectedClients()) {
                // For every connected client, tick their structure.
                conn->Tick();

                if (conn->isConnected() && conn->isReady()) {
                    // Generate an echo request for every connected client that has
                    // sent a handshake.
                    conn->GenerateEchoRequest(std::string("This is from the server!"));
                }
            }

            // This emulates about 60 ticks per second.
            std::this_thread::sleep_for(std::chrono::microseconds(15000));
        }
    } catch (std::exception& e) {
        cerr << e.what() << endl;
    }

    return 0;
}
