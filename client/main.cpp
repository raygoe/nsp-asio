#include <iostream>
#include <ctime>
#include <client.hpp>
#include <asio.hpp>
#include <chrono>
#include <thread>

using namespace std;
using asio::ip::tcp;


int main(int argc, char** argv) {
    try {
        asio::io_service io;
        my_client client(io, argv[1], argv[2]);
        
        while (true) {
            io.poll();
            
            if (client.isConnected()) {
                client.tick();
                
                if (client.isReady()) {
                    client.generate_echo_request ( std::string("This is from the client.") );
                }
            }
            
			std::this_thread::sleep_for(std::chrono::microseconds(15000));
        }
        
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
