#include "message.h"
#include "SocketClient.h"


Message::Message():data(NULL)
{
    
}

Message::~Message()
{
	if (data!=NULL)
    {
		delete[] data;
	}
}

int Message::datalength()
{
    return SocketClient::bytesToInt(length)+13;
}


int NewMessage::datalength()
{
    return length;
}