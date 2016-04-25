#ifndef SHARED_BYTESTREAM_HPP
#define SHARED_BYTESTREAM_HPP

#include <sstream>
#include <cstring>
#include <string>
#include <vector>

#include <iomanip>

//! Helper class for defined, fixed byte-width messages.
/*!
 * The byte buffer is used generally to pull down a N-byte
 * wide string of bytes into a defined vector. This is useful
 * in several parts of the ByteStream.
 */
class bytebuffer
{
public:
    //! ByteBuffer constructor. Takes an argument for the size.
    /*!
     * Because the ByteBuffer is a fixed width, we need the width
     * of the buffer on construction.
     * \param size_t size This is the size of the string it will contain.
     */
    bytebuffer(size_t size) : bufferSize(size), buf(size) { }

    //! Returns the size of the byte buffer.
    /*!
     * \return size_t Byte Buffer Size
     */
    size_t count() const {
        return bufferSize;
    }

    //! This will return the ByteBuffer data in the form of a std::string.
    /*!
     * \return std::string byte buffer data.
     */
    std::string str() const {
        return std::string(buf.begin(), buf.end());
    }

    //! This will return the internal std::vector buffer.
    /*!
     * \return std::vector<char>& Internal byte buffer data object reference.
     */
    std::vector<char>& buffer() {
        return buf;
    }

private:
    //! This is the expected size of the internal buffer.
    size_t bufferSize;

    //! This is the internal buffer.
    std::vector<char> buf;

};

//! This is a wrapper around the std::stringstream object.
/*!
 * It is used to push arbitrarily large structured data onto
 * a streaming buffer.
 *
 * This buffer is generally used as a one way buffer (either in
 * or out) since it is not a circular buffer.
 *
 * It is a FIFO, so be wary about trying to push data while
 * pulling data at the same time.
 *
 * This is derived by the Packet object.
 * \sa Packet
 */
class ByteStream
{
public:
    //! This constructor sets up the internal stringstream buffer.
    ByteStream() : net_buffer() { }
    virtual ~ByteStream() { }

    //! This returns a copy of the bytestream buffer as a vector.
    /*!
     * \return std::vector<char> Byte Stream Buffer as a vector.
     */
    std::vector<char> buffer() {
        // First, let's pull the stringstream down as a string object.
        std::string msg = net_buffer.str();
        // Let's copy that string to a vector buffer.
        std::vector<char> buf = std::vector<char>(msg.begin(), msg.end());
        // Clear the bytestream buffer.
        net_buffer.str("");
        // Return the byte array.
        return buf;
    }

    //! This will allow the user to set the buffer to whatever they desire.
    /*!
     * \param std::vector<char> buf This buffer is copied over the internal buffer.
     */
    void buffer(std::vector<char> buf) {
        net_buffer.str(std::string(buf.begin(), buf.end()));
    }

    //! This function allows someone to write some N length of bytes from a vector to the buffer.
    /*!
     * \param std::vector<char>& from Buffer reference to write from.
     * \param size_t length The length of the buffer from [0, length) to write.
     */
    void write(std::vector<char>& from, size_t length) {
        from.resize(length);
        net_buffer.write(&*from.begin(), length);
    }

    //! This function allows someone to read some N length of the buffer to the vector.
    /*!
     * \param std::vector<char>& to Buffer reference to read to.
     * \param size_t length The length of the buffer from [0, length) to read.
     */
    void read(std::vector<char>& from, size_t length) {
        from.resize(length);
        net_buffer.read(&*from.begin(), length);
    }

    //! This returns the number of bytes in the byte stream.
    /*!
     * \return size_t The size (in bytes) of the buffer.
     */
    size_t count() {
        size_t count;
        size_t loc = net_buffer.tellg(); // Store off the current pointer.
        // Set the stream pointer to the end, from the beginning.
        net_buffer.seekg(0, std::ios::end);
        // Get the offset.
        count = net_buffer.tellg();
        // Put the pointer back to what it was before.
        net_buffer.seekg(loc, std::ios::beg);
        // Return the offset.
        return count;
    }

    //! Stream a bytestream into another bytestream.
    /*! Makes use of the Byte Buffers.
     * \param ByteStream& bs This is the bytestream to write to.
     * \return ByteStream& bytestream for the next operator.
     */
    ByteStream& operator>>(ByteStream& bs) {
        // Create a temporary byte buffer the current size of the stream.
        bytebuffer bbuf(this->count());
        // Copy this data onto the temporary buffer.
        (*this) >> bbuf;
        // Push the buffer onto the byte stream.
        bs << bbuf;
        return *this;
    }

    //! Stream a bytestream into a byte buffer.
    /*!
     * \param bytebuffer& bbuf Byte Buffer to write to.
     * \return ByteStream& bytestream for the next operation
     */
    ByteStream& operator>>(bytebuffer& bbuf) {
        // Read the data into the byte buffer.
        net_buffer.read(&*bbuf.buffer().begin(), bbuf.count());
        return *this;
    }

    //! This is the generic write operator.
    /*! As long as the value type in use has a defined size,
     * this will function as planned. Example for usage that
     * it does not work on: std::string, std::vector.
     *
     * \param T& t This is the type to write the buffer data into.
     * \return ByteStream& bytestream for the next operation
     */
    template <typename T>
    ByteStream& operator>>(T& t) {
        // Create a temporary buffer for holding the data.
        std::vector<char> buf(sizeof(t));
        // Read the buffer data into the vector.
        net_buffer.read(&*buf.begin(), sizeof(t));
        // Copy that data over the top of the pointer to the type.
        std::memcpy(&t, &*buf.begin(), sizeof(t));
        return *this;
    }

    //! This will read from one bytestream into the current one.
    /*!
     * \param ByteStream& bs This byte stream will be read into the buffer.
     * \return ByteStream& for the next operation
     */
    ByteStream& operator<<(ByteStream& bs) {
        // Create a byte buffer of the correct size.
        bytebuffer bbuf(bs.count());
        // Copy the internal buffer into the byte buffer.
        bs >> bbuf;
        (*this) << bbuf;
        return *this;
    }

    //! This will read from a byte buffer directly.
    /*!
     * \param bytebuffer& buf This buffer will be written to the singstream buffer.
     * \return ByteStream& for the next operation
     */
    ByteStream& operator<<(const bytebuffer& bbuf) {
        net_buffer << bbuf.str(); // Copy from the bbody string.
        return *this;
    }

    //! This accepts a std::string to add to the internal buffer.
    /*!
     * \param std::string str String to write to the buffer.
     * \return ByteStream& For the next operation
     */
    template <typename CharType>
    ByteStream& operator<<(const std::basic_string<CharType>& str) {
        net_buffer << str;
        return *this;
    }

    //! This will read from an arbitrary structure and place it in the buffer.
    /*!
     * \param T& t This is the object to transfer into the buffer.
     * \return ByteStream& For the next operation.
     */
    template <typename T>
    ByteStream& operator<<(const T& t) {
        // Create a temporary buffer
        std::vector<char> buf(sizeof(t));
        // Copy the arbitrary data into the temporary buffer.
        std::memcpy(&*buf.begin(), &t, sizeof(t));
        // Write the buffer into the stringstream
        net_buffer << std::string(buf.begin(), buf.end());
        return *this;
    }

    //! This will clear the internal buffer.
    void clear() {
        net_buffer.str("");
    }

private:

    //! Internal stringstream buffer
    std::stringstream net_buffer;

};

#endif /* SHARED_BYTESTREAM_HPP */
