# Net Message Pump

The Net Message Pump library is effectively a code example on how I put together a Client/Server
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