#ifndef SHARED_BYTESTREAM_HPP
#define SHARED_BYTESTREAM_HPP

#include <sstream>
#include <cstring>
#include <string>
#include <vector>

#include <iomanip>

class obytestream {
public:
    obytestream ( ) : net_buffer() { };
    virtual ~obytestream ( ) { };
    
    template <typename T>
    obytestream& operator<<(const T& t) {
        std::vector<char> buf(sizeof(t));
        std::memcpy(&*buf.begin(), &t, sizeof(t));
        net_buffer << std::string(buf.begin(), buf.end());
        return *this;
    }
    
    std::vector<char> buffer () {
        std::string msg = net_buffer.str();
        std::vector<char> buf = std::vector<char>(msg.begin(), msg.end());
        net_buffer.str("");
        return buf;
    }
    
    template <typename T>
    void write(T& from, size_t length) {
        net_buffer.write(&*from.begin(), length);
    }
    
private:
    std::ostringstream net_buffer;
};

class ibytestream {
public:
    ibytestream ( ) : net_buffer() { };
    virtual ~ibytestream ( ) { };
    
    template <typename T>
    ibytestream& operator>>(T& t) {
        std::vector<char> buf(sizeof(t));
        net_buffer.read(&*buf.begin(), sizeof(t));
        std::memcpy(&t, &*buf.begin(), sizeof(t));
        return *this;
    }
    
    template <typename T>
    void read(T& from, size_t length) {
        net_buffer.read(&*from.begin(), length);
    }
    
    void buffer (std::vector<char> buf) {
        net_buffer.str(std::string(buf.begin(), buf.end()));
    }
    
private:
    std::istringstream net_buffer;
};

#endif /* SHARED_BYTESTREAM_HPP */
