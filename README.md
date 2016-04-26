# Net Service Pump

The Net Service Pump library is effectively a code example on how I put together a Client/Server
framework in ASIO (asynchronously, of course). The server and client both operate asynchronously.

Once the server and the client are connected, the server will send a handshaking struct that contains
a 16-bit integer (0x2233) this is to detect if the client is of different endian than the server. The
client will also send one of these to the server so the server will know if it has to flip the endian
incoming as well.

Once the handshaking is done, the server and client both expect to read back the header structure. The
header structure will then tell the client or the server how long to read next for the rest of the data.

Both the client and the server are setup to read only a set amount of bytes. Note that this makes it
very important to detect errors on the bus.

You can just look through the code yourself. It's pretty straight forward and not complicated.

## Byte Stream Buffer

The core feature that makes the Net Service Pump work is the Byte Stream Buffer. The Byte Stream Buffer
contains an internal C++ buffer that is used to simplify the transfer of C/C++ structures across any
medium (such as TCP/IP, File I/O, etc.)

Effectively, you write what you want to copy into the Byte Stream Buffer which the internal code can then
query as a std::vector for writing it to some I/O device.

### Byte Buffer

Because of the way the Byte Stream Buffer works, it needs a way of pulling a set number of bytes off the
buffer. This is where the Byte Buffer object comes in. This object allows you to pull N bytes from the
byte stream and it contains that data. It can output the data as an array of bytes or as a C++ string.

### Packet

A special type of Byte Stream Buffer is called a Packet object. This byte stream buffer will automatically
put the packet header on the buffer as it is placed into the output queue. This greatly simplifies the process
of sending and receiving packets.

## Connection

The Connection object is a simplified and unified interface to the ASIO networking library. Both the Server
and the Client expand from this library and make use of its virtual functions.

The Connection object contains a list of Service Handlers which are defined as objects that parse a certain
type of packet. Each packet has a service and an opcode so the connection will check if the service is a
known service type. If the service is known, it will see if there is a handler registered for that service.
Most likely there will be so it sends it to that handler which parses the packet.

### Serivce Handlers

These are pretty simple objects that just have a ServicePacket method that takes a shared pointer to the
Connection object and a reference to the Packet. It will then do whatever work is requested according to
the specific opcode.

## Current Structure

The code structure at the moment contains a basic echo client and echo server whose job is to spam each other
and respond to echo requests. The Client and Server connections register an EchoService defined in the shared
code that automatically sets up the response and request handling.

Both the server and the client are setup to request an echo message in their main loops.
