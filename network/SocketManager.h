#ifndef __client1__SocketManager__
#define __client1__SocketManager__


#include "SocketMessage.h"
#include "SocketClient.h"
#include "Json.h"

class SocketManager
{
public:
    static SocketManager* getInstance();
    void startSocket();

    void sendMessage(std::string);
    
private:
    SocketManager();
    ~SocketManager();
    
    SocketClient* _socket;
};


#endif /* defined(__client1__SocketManager__) */
