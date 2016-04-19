#include <iostream>
#include <asio.hpp>
#include <chrono>
#include <thread>

#include "server.hpp"

using namespace std;
using asio::ip::tcp;

int main(int argc, char** argv) {
    try {
        asio::io_service io;
        my_server server(io);
        
        while (true) {
            io.poll();
            
            for ( auto* conn : *server.getConnectedClients() ) {
                conn->tick();
                
                if (conn->isConnected() && conn->isReady()) {
                    conn->generate_echo_request ( std::string("This is from the server!") );
                }
            }

			std::this_thread::sleep_for(std::chrono::microseconds(15000));
        }
    } catch (std::exception& e) {
        cerr << e.what() << endl;
    }

    return 0;

}
