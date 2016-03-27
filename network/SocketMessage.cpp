#include "SocketConst.h"
#include "SocketMessage.h"


SocketMessage::SocketMessage() : _data(nullptr), _dataLength(0)
{
    
}

SocketMessage::~SocketMessage()
{
    if(this->_data)
    {
        SK_SAFE_DELETE(this->_data);
    }
}

char* SocketMessage::data() const
{
    return _data;
}

void SocketMessage::data(char* adata)
{
    _data = adata;
}

unsigned short SocketMessage::dataLength()
{
    return _dataLength;
}

void SocketMessage::dataLength(unsigned short adataLength)
{
    _dataLength = adataLength;
}