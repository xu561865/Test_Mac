#include "mlib_buffer.h"

#include "mlib_assert.h"

#include <zlib.h>

MLIB_NS_BEGIN

MBuffer::~MBuffer()
{
    clear();
}

MBuffer::MBuffer(size_t size, size_t capa) :
_buffer(NULL), _capa(0), _size(0),
_pos(0)
{
    if (capa > size) reserve(capa);
    resize(size);
}

MBuffer::MBuffer(const std::string & buf) :
_buffer(NULL), _capa(0), _size(0),
_pos(0)
{
    write(buf);
}

MBuffer::MBuffer(const char *data, size_t dataLen) :
_buffer(NULL), _capa(0), _size(0),
_pos(0)
{
    M_ASSERT(data != nullptr);
    
    writeData(data, dataLen);
}

//MBuffer::MBuffer(const MBuffer& buf) :
//_buffer(NULL), _capa(0), _size(0),
//_pos(0)
//{
//    writeData(buf.getData(), buf.size());
//}

MBuffer::MBuffer(MBuffer&& buf) :
_buffer(buf._buffer), _capa(buf._capa), _size(buf._size),
_pos(buf._pos)
{
    buf._buffer = NULL;
    buf._capa = buf._size = buf._pos = 0;
}

MBuffer::MBuffer(const char *data) :
_buffer(NULL), _capa(0), _size(0),
_pos(0)
{
    size_t len = strlen(data);
    if (len) writeData(data, len);
}

void MBuffer::clear()
{
    delete [] _buffer;
    _buffer = NULL;
    _capa = _pos = _size = 0;
}

void MBuffer::reserve(size_t size)
{
    if (size > _capa)
    {
        char* newBuf = new char [size];
        if (_buffer)
        {
            memcpy(newBuf, _buffer, _capa);
        }
        delete [] _buffer;
        _buffer = newBuf;
        _capa = size;
        // _pos and _size remain
    }
}

void MBuffer::resize(size_t size)
{
    reserve(size);
    
    _size = size;
    _pos = _size;
}

size_t MBuffer::size() const
{
    return _size;
}

// --- seeking function ---
bool MBuffer::seek(size_t pos) const
{
    if (pos > _size) return false;
    _pos = pos;
    return true;
}

void MBuffer::seekBegin() const
{
    _pos = 0;
}

void MBuffer::seekEnd() const
{
    _pos = _size;
}

void MBuffer::requestNewCapa(size_t size)
{
    while (_capa < _size + size)
    {
        reserve(_capa ? _capa * 2 : __MLIB_BUFFER_DEFAULT_CAPA);
    }
}

// --- String reader/writer ---

bool MBuffer::read(std::string& val) const
{
    size_t len;
    if (!readVariableInteger(len)) return false;
    
    if (_pos + len > _size) return false;
    
    val.clear();
    val.append(_buffer + _pos, len);
    _pos += len;
    return true;
}

void MBuffer::write(const std::string & val)
{
    writeVariableInteger(val.size());
    if (_capa < _pos + val.size())
    {
        requestNewCapa(val.size());
    }
    
    memcpy(_buffer + _pos, val.c_str(), val.size());
    _pos += val.size();
    if (_pos > _size) _size = _pos;
}

// --- MBuffer reader/writer

bool MBuffer::read(MBuffer & val) const
{
    size_t len;
    if (!readVariableInteger(len)) return false;
    
    if (_pos + len > _size) return false;
    
    val.resize(0);
    val.writeData(_buffer + _pos, len);
    _pos += len;
    return true;
}

void MBuffer::write(const MBuffer & val)
{
    writeVariableInteger(val.size());
    if (_capa < _pos + val.size())
    {
        requestNewCapa(val.size());
    }
    
    memcpy(_buffer + _pos, val.getData(), val.size());
    _pos += val.size();
    if (_pos > _size) _size = _pos;
}

bool MBuffer::readData(char *dest, size_t n) const
{
    M_ASSERT(dest != nullptr);
    
    if (_pos + n > _size) return false;
    
    memcpy(dest, _buffer + _pos, n);
    _pos += n;
    
    return true;
}

void MBuffer::writeData(const char *dest, size_t n)
{
    M_ASSERT(dest != nullptr);

    if (_capa < _pos + n)
    {
        requestNewCapa(n);
    }
    
    memcpy(_buffer + _pos, dest, n);
    _pos += n;
    if (_pos > _size) _size = _pos;
}

void MBuffer::write(char c, size_t n)
{
    if (_capa < _pos + n)
    {
        requestNewCapa(n);
    }
    
    memset(_buffer + _pos, c, n);
    _pos += n;
    if (_pos > _size) _size = _pos;
}

bool MBuffer::readVariableInteger(size_t & val) const
{
    uint8_t b;
    val = 0;
    if (!readInteger(b)) return false;
    bool nextByte;
    size_t count = 1;
    do {
        nextByte = (b & 0x80);
        val <<= 7;
        val |= (b & 0x7f);
    } while (nextByte && count++ < sizeof(size_t) && readInteger(b));
    
    if (nextByte) return false;
    
    return true;
}

void MBuffer::writeVariableInteger(size_t val)
{
    uint8_t b[8];
    size_t i = 0;
    do
    {
        b[i] = val & 0x7f;
        val >>= 7;
        b[i] |= i ? 0x80 : 0; // turn on 1st bit if have more bytes after it
        ++i;
    } while (val);
    
    if (_pos + i > _capa)
    {
        requestNewCapa(i);
    }
    memrcpy(_buffer + _pos, b, i);
    _pos += i;
    if (_pos > _size) _size = _pos;
}

void* MBuffer::memrcpy(void *dest, const void *src, size_t n)
{
    M_ASSERT(dest != nullptr && src != nullptr);
    
    char *dest_ = (char*)(dest);
    const char *src_ = (const char*)(src);
    for(size_t i = 0;i < n;++i){
        dest_[i] = src_[n - 1 - i];
    }
    return dest;
}


//MBuffer & MBuffer::operator <<(const IMBufferable & obj)
void MBuffer::write(const IMBufferable& obj)
{
    //ZTT_DEBUG("Oveloaded write for IMBufferable!");
    obj.serialize(*this);
}



//const MBuffer & MBuffer::operator >>(IMBufferable & obj) const
bool MBuffer::read(IMBufferable& obj) const
{
    //ZTT_DEBUG("Oveloaded load for IMBufferable!");
    
    obj.deserialize(*this);
    
    return true;
}
MLIB_NS_END