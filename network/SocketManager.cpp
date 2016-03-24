#include "SocketManager.h"
#include "Json.h"


static SocketManager* instance = NULL;

SocketManager::SocketManager()
{
    
}

SocketManager* SocketManager::getInstance()
{
    if(instance == NULL)
    {
        instance = new SocketManager();
    }
    
    return instance;
}

void SocketManager::startSocket()
{
//    _socket = new SocketClient("192.168.1.111",11009,1,1);
    _socket = new SocketClient("182.92.129.73",1111,1,1);
    _socket->start();
}

void SocketManager::sendMessage(const char* data, int commandId)
{
    Message *msg=_socket->constructMessage(data, commandId);
    _socket->sendMessage_(msg, false);
}

void SocketManager::sendMessage(Json::Value value, int commandId)
{
    NewMessage *msg=_socket->constructMessage(value, commandId);
    _socket->sendMessage_(msg, false);
}

void SocketManager::sendMessage(std::string value)
{
    NewMessage *msg=_socket->constructMessage(value);
    _socket->sendMessage_(msg, false);
}




