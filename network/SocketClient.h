#ifndef _CDATA_NETSOCKET_H_
#define _CDATA_NETSOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <queue>
#include <pthread.h>
#include <unistd.h>

#include "Json.h"
#include "SocketBuffer.h"

const int	SOCKET_CLIENT_WAIT_CONNECT = 0;
const int	SOCKET_CLIENT_OK = 1;
const int	SOCKET_CLIENT_DESTROY = 2;


class SocketMessage;

class SocketClient
{
private:
	int m_hSocket;

	char m_serverId;
	char m_clientId;
    std::string m_host;
	int m_iport;

	std::vector<char> m_serverId2;
	std::vector<char> m_clientId2;
	std::vector<std::string> m_host2;
	std::vector<int> m_iport2;//链接服务器1失败后，如果服务器2存在将连服务器2
	
	//发送和接收缓冲区，发送缓冲区满的时候，会断开连接，并提示信号不好
	SocketBuffer m_cbRecvBuf;
	SocketBuffer m_cbSendBuf;
	
	//收到服务端消息
    std::queue<SocketMessage*> m_receivedNewMessageQueue;
	
	//需要发送到服务端的消息
    std::queue<SocketMessage*> m_sendNewMessageQueue;
	
	int m_iState;
	
	//接收线程
	bool m_bThreadRecvCreated;
	pthread_t pthread_t_receive;
	
	//发送线程
	bool m_bThreadSendCreated;
	pthread_t pthread_t_send;
    
	pthread_mutex_t m_thread_cond_mutex;//pthread_mutex_t 互斥锁
	pthread_cond_t m_threadCond;
	
    //聊天线程
    bool m_bThreadChatCreated;
    pthread_t pthread_t_chat;
    pthread_mutex_t m_thread_chat_mutex;
    //pthread_cond_t m_threadCond;
	//发送队列同步锁
	pthread_mutex_t m_sendqueue_mutex;
	
private:
	//连接服务器
	bool  connectServer();
	
	static void* ThreadReceiveMessageOld(void *p);
    static void* ThreadReceiveMessage(void *p);
	static void* ThreadSendMessage(void *p);
	
public:
    SocketClient(std::string host, int port, signed char clientId, signed char serverId);
	
	~SocketClient();
	void start();
	void stop(bool b);
	
	bool isWaitConnect();
	//发送数据
    void sendMessage_(SocketMessage* msg,bool b);
	
	SocketMessage* popReceivedMessage();
	SocketMessage* pickReceivedMessage();
	
	void pushReceivedMessage(SocketMessage* msg);
	
    SocketMessage* constructMessage(std::string);
    static int bytesToInt(signed char* data);
    static signed char* intToByte(int i);
    void swhlie(int commandId);
};

#endif
