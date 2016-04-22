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

void do_test_bs ()
{
	ByteStream data;

	{
		Packet packet(0, 0);
		CharEcho echo_a("This is a test."), echo_b("This is another test.");
		packet << echo_a << echo_b;
		data << packet;
	}

	{
		Packet packet;
		CharEcho echo_a, echo_b;
		data >> packet;
		packet >> echo_a >> echo_b;

		cout << echo_a.str() << endl << echo_b.str() << endl;
	}
}

int main(int argc, char** argv) {
	try {
        asio::io_service io;
        ClientConnection::Pointer client = ClientConnection::Create(io);
        client->Connect(argv[1], argv[2]);
        
        while (true) {
            io.poll();
            
            if (client->isConnected()) {
                client->Tick();
                
                if (client->isReady()) {
                    client->GenerateEchoRequest ( std::string("This is from the client.") );
                }
            }
            
			std::this_thread::sleep_for(std::chrono::microseconds(15000));
        }
        
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
