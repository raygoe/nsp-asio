#ifndef SHARED_BYTESTREAM_HPP
#define SHARED_BYTESTREAM_HPP

#include <sstream>
#include <cstring>
#include <string>
#include <vector>

#include <iomanip>

class bytebuffer
{
public:
    bytebuffer(size_t size) : bufferSize(size), buf(size) { }

    size_t count() const {
        return bufferSize;
    }
    std::string str() const {
        return std::string(buf.begin(), buf.end());
    }
    std::vector<char>& buffer() {
        return buf;
    }

private:
    size_t bufferSize;
    std::vector<char> buf;

};

class ByteStream
{
public:
    ByteStream() : net_buffer() { }
    virtual ~ByteStream() { }

    std::vector<char> buffer() {
        std::string msg = net_buffer.str();
        std::vector<char> buf = std::vector<char>(msg.begin(), msg.end());
        net_buffer.str("");
        return buf;
    }

    void buffer(std::vector<char> buf) {
        net_buffer.str(std::string(buf.begin(), buf.end()));
    }

    void write(std::vector<char>& from, size_t length) {
        from.resize(length);
        net_buffer.write(&*from.begin(), length);
    }

    void read(std::vector<char>& from, size_t length) {
        from.resize(length);
        net_buffer.read(&*from.begin(), length);
    }

    size_t count() {
        size_t count;
        size_t loc = net_buffer.tellg();
        net_buffer.seekg(0, std::ios::end);
        count = net_buffer.tellg();
        net_buffer.seekg(loc, std::ios::beg);
        return count;
    }

    ByteStream& operator>>(ByteStream& bs) {
        bytebuffer bbuf(this->count());
        (*this) >> bbuf;
        bs << bbuf;
        return *this;
    }

    ByteStream& operator>>(bytebuffer& bbuf) {
        net_buffer.read(&*bbuf.buffer().begin(), bbuf.count());
        return *this;
    }

    template <typename T>
    ByteStream& operator>>(T& t) {
        std::vector<char> buf(sizeof(t));
        net_buffer.read(&*buf.begin(), sizeof(t));
        std::memcpy(&t, &*buf.begin(), sizeof(t));
        return *this;
    }

    ByteStream& operator<<(ByteStream& bs) {
        bytebuffer bbuf(bs.count());
        bs >> bbuf;
        (*this) << bbuf;
        return *this;
    }

    ByteStream& operator<<(const bytebuffer& bbuf) {
        net_buffer << bbuf.str();
        return *this;
    }

    template <typename CharType>
    ByteStream& operator<<(const std::basic_string<CharType>& str) {
        net_buffer << str;
        return *this;
    }

    template <typename T>
    ByteStream& operator<<(const T& t) {
        std::vector<char> buf(sizeof(t));
        std::memcpy(&*buf.begin(), &t, sizeof(t));
        net_buffer << std::string(buf.begin(), buf.end());
        return *this;
    }

    void clear() {
        net_buffer.str("");
    }

private:
    std::stringstream net_buffer;

};

#endif /* SHARED_BYTESTREAM_HPP */
