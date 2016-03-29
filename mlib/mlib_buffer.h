#ifndef mlib_buffer_h
#define mlib_buffer_h

#include "mlib_helpers.h"
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <set>
#include <vector>
#include <map>

#include "mlib_exception.h"

#define __MLIB_BUFFER_DEFAULT_CAPA 8

MLIB_NS_BEGIN
class MBuffer;

class IMBufferable
{
public:
    virtual ~IMBufferable() {}
    
    virtual void serialize(MBuffer& buf) const = 0;
    virtual void deserialize(const MBuffer& buf) = 0;
};

class MBuffer
{
public:
    MBuffer(size_t size = 0, size_t capa = __MLIB_BUFFER_DEFAULT_CAPA);
    explicit MBuffer(const std::string& buf);
    explicit MBuffer(const char* data, size_t dataLen);
    explicit MBuffer(const char* data);
    MBuffer(const MBuffer& buf) = delete;
    MBuffer(MBuffer&& buf);
    virtual ~MBuffer();
    
    void reserve(size_t size);
    void resize(size_t size);
    size_t size() const;
    void clear();
    
    size_t pos() const { return _pos; }
    size_t remain() const { return _size - _pos; }
    bool seek(size_t pos) const;
    void seekBegin() const;
    void seekEnd() const;
    bool isEOB() const { return (_pos == _size); }
    
    const char* getData() const { return _buffer; }
    
    template <class T>
    void append(T val) { seekEnd(); write(val); }
    template <class T, class U>
    void append(T val, U val2) { seekEnd(); write(val, val2); }
    
    
    template <class T>
    bool readInteger(T& val, size_t maxSize = 0) const; // read integer value, with max size
    template <class T>
    void writeInteger(const T& val, size_t maxSize = 0); // read integer value, with max size
    void write(uint8_t val) { writeInteger(val); }
    void write(uint16_t val) { writeInteger(val); }
    void write(uint32_t val) { writeInteger(val); }
    void write(uint64_t val) { writeInteger(val); }
    void write(int8_t val) { writeInteger(val); }
    void write(int16_t val) { writeInteger(val); }
    void write(int32_t val) { writeInteger(val); }
    void write(int64_t val) { writeInteger(val); }
    bool read(uint8_t& val) const { return readInteger(val); }
    bool read(uint16_t& val) const { return readInteger(val); }
    bool read(uint32_t& val) const { return readInteger(val); }
    bool read(uint64_t& val) const { return readInteger(val); }
    bool read(int8_t& val) const { return readInteger(val); }
    bool read(int16_t& val) const { return readInteger(val); }
    bool read(int32_t& val) const { return readInteger(val); }
    bool read(int64_t& val) const { return readInteger(val); }
    
    bool readData(char* dest, size_t n) const; // read piece of data block
    void writeData(const char* dest, size_t n); // write piece of data block
    void appendData(const char* dest, size_t n) { seekEnd(); writeData(dest, n); } // append piece of data block
    
    bool read(std::string& val) const;
    void write(const std::string& val);
    
    bool read(MBuffer& val) const;
    void write(const MBuffer& val);
    
    void write(const char* val) { write(std::string(val)); }
    void write(char c, size_t n);
    
    void write(const IMBufferable& obj);
    bool read(IMBufferable& obj) const;
    
    template <typename T1, typename T2> bool read(std::pair<T1, T2> &val) const;
    template <typename T1, typename T2> void write(const std::pair<T1, T2> &val);
    template <class T> bool read(std::vector<T>& val) const;
    template <class T> void write(const std::vector<T>& val);
    template <class T> bool read(std::set<T>& val) const;
    template <class T> void write(const std::set<T>& val);
    template <class K, class V> bool read(std::map<K, V>& val) const;
    template <class K, class V> void write(const std::map<K, V>& val);
    
    template <class T>
    MBuffer& operator << (const T& val);
    template <class T>
    const MBuffer& operator >> (T& val) const;
    
    operator const char *() { return _buffer; }
    operator std::string() { return std::string(_buffer, _size); }
    
    MBuffer& operator= (const char* data) { this->resize(0); this->writeData(data, strlen(data)); return (*this); }
    MBuffer& operator= (const MBuffer& other) { this->resize(0); this->writeData(other.getData(), other.size()); return (*this); }
    MBuffer& operator= (MBuffer&& other) { _buffer = other._buffer; _capa = other._capa; _size = other._size; _pos = other._pos; other._buffer = NULL; other._capa = other._size = other._pos = 0; return (*this); }
    
protected:
    void requestNewCapa(size_t size);
    
    bool readVariableInteger(size_t& val) const;
    void writeVariableInteger(size_t);
    
    static void* memrcpy(void* dest, const void* src, size_t n);
    
private:
    char*               _buffer;
    size_t              _capa;
    size_t              _size;
    
    // For reading purpose, can be mutable
    mutable size_t                  _pos;
};

template<class T>
inline bool MBuffer::readInteger(T & val, size_t maxSize) const
{
    size_t size = maxSize ? (maxSize > sizeof(T) ? sizeof(T) : maxSize) : sizeof(T);
    
    if (_size < _pos + size)
    {
        return false;
    }
    
#ifdef BIG_ENDIAN_MACHINE
    memcpy(&val, _buffer + _pos, size);
#else
    memrcpy(&val, _buffer + _pos, size);
#endif
    _pos += size;
    return true;
}

template<class T>
inline void MBuffer::writeInteger(const T & val, size_t maxSize)
{
    size_t size = maxSize ? (maxSize > sizeof(T) ? sizeof(T) : maxSize) : sizeof(T);
    
    if (_capa < _pos + size)
    {
        requestNewCapa(size);
    }
    
#ifdef BIG_ENDIAN_MACHINE
    memcpy(_buffer + _pos, &val, size);
#else
    memrcpy(_buffer + _pos, &val, size);
#endif
    
    _pos += size;
    if (_pos > _size) _size = _pos;
}

template<class T> inline MBuffer& MBuffer::operator << (const T& val)
{
    this->write(val);
    return (*this);
}

template<class T> inline const MBuffer& MBuffer::operator >>(T& val) const
{
    if (!this->read(val))
    {
        M_THROW("Not enough bytes to be read from MBuffer! pos = " << _pos << ", size = " << _size);
    }
    return (*this);
}

template <typename T1, typename T2> void MBuffer::write(const std::pair<T1, T2> &val)
{
    write(val.first);
    write(val.second);
}

template <typename T1, typename T2> bool MBuffer::read(std::pair<T1, T2> &val) const
{
    return read(val.first) && read(val.second);
}

template<class T> inline bool MBuffer::read(std::vector<T> & val) const
{
    size_t size;
    if (!this->readVariableInteger(size)) return false;
    val.clear();
    val.reserve(size);
    T tmp;
    while (val.size() < size)
    {
        if (!this->read(tmp)) return false;
        val.push_back(tmp);
    }
    return true;
}

template<class T> inline void MBuffer::write(const std::vector<T> & val)
{
    this->writeVariableInteger(val.size());
    
    for (typename std::vector<T>::const_iterator iter = val.begin();
         iter != val.end(); ++iter)
    {
        this->write(*iter);
    }
}

template<class T> inline bool MBuffer::read(std::set<T> & val) const
{
    size_t size;
    if (!this->readVariableInteger(size)) return false;
    val.clear();
    T tmp;
    while (val.size() < size)
    {
        if (!this->read(tmp)) return false;
        val.insert(tmp);
    }
    return true;
}

template<class T> inline void MBuffer::write(const std::set<T> & val)
{
    this->writeVariableInteger(val.size());
    
    for (typename std::set<T>::const_iterator iter = val.begin();
         iter != val.end(); ++iter)
    {
        this->write(*iter);
    }
}

template<class K, class V> inline bool MBuffer::read(std::map<K, V> & val) const
{
    size_t size;
    if (!this->readVariableInteger(size)) return false;
    val.clear();
    K k; V v;
    while (val.size() < size)
    {
        if (!this->read(k) || !this->read(v)) return false;
        val[k] = v;
    }
    return true;
}

template<class K, class V> inline void MBuffer::write(const std::map<K, V> & val)
{
    this->writeVariableInteger(val.size());
    
    for (typename std::map<K, V>::const_iterator iter = val.begin();
         iter != val.end(); ++iter)
    {
        this->write(iter->first);
        this->write(iter->second);
    }
}


MLIB_NS_END

#endif // mlib_buffer_h
