TARGET = nsp_client nsp_server
LIBS = -pthread
CC = g++
CFLAGS = -g -Wall -std=c++11 -Ishared -DASIO_STANDALONE

CFLAGS_CLIENT = $(CFLAGS) -Iclient
CFLAGS_SERVER = $(CFLAGS) -Iserver

.PHONY: default all clean

default: nsp_client nsp_server
all: default

CLIENT_OBJECTS = $(patsubst %.cpp, %.o, $(wildcard client/*.cpp))
SERVER_OBJECTS = $(patsubst %.cpp, %.o, $(wildcard server/*.cpp))
SHARED_OBJECTS = $(patsubst %.cpp, %.o, $(wildcard shared/*.cpp))
HEADERS = $(wildcard *.hpp)

client/%.o: client/%.cpp $(HEADERS)
	$(CC) $(CFLAGS_CLIENT) -c $< -o $@

server/%.o: server/%.cpp $(HEADERS)
	$(CC) $(CFLAGS_CLIENT) -c $< -o $@

shared/%.o: shared/%.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

nsp_client: $(CLIENT_OBJECTS) $(SHARED_OBJECTS)
	$(CC) $(CLIENT_OBJECTS) $(SHARED_OBJECTS) -Wall $(LIBS) -o $@

nsp_server: $(SERVER_OBJECTS) $(SHARED_OBJECTS)
	$(CC) $(SERVER_OBJECTS) $(SHARED_OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f client/*.o
	-rm -f server/*.o
	-rm -f shared/*.o
	-rm -f $(TARGET)
