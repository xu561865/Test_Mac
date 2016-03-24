#include "SocketClient.h"
#include "message.h"
#include <errno.h>
#include <signal.h>
#include "Util.h"

#define CC_SAFE_RELEASE(p)            do { if(p) { (p)->release(); } } while(0)

SocketClient::SocketClient(std::string host, int port, byte clientid, byte serverid):
                            m_iState(SocketClient_WAIT_CONNECT),
                            m_cbRecvBuf(1024*60),m_cbSendBuf(1024*60)
{   
	m_isvalidSeq = false;
	memset(m_sabcde, 0, 6*8);
	
    pthread_mutex_init (&m_sendqueue_mutex,NULL);
	pthread_mutex_init(&m_thread_cond_mutex,NULL);
	pthread_cond_init(&m_threadCond, NULL);
	
	m_hSocket = -1;
	
	this->m_host = host;
	this->m_iport = port;
	this->m_clientId = clientid;
	this->m_serverId = serverid;
	
	m_bThreadRecvCreated = false;
	m_bThreadSendCreated = false;
}

SocketClient::~SocketClient()
{
    m_iState = SocketClient_DESTROY;
    if( m_hSocket!=-1)
    {
        close(m_hSocket);
    }
    
    pthread_mutex_destroy(&m_sendqueue_mutex);
    pthread_mutex_destroy(&m_thread_cond_mutex);
    pthread_cond_destroy(&m_threadCond);
    
    while (!m_receivedMessageQueue.empty())
    {
        Message* m = m_receivedMessageQueue.front();
        m_receivedMessageQueue.pop();
//        CC_SAFE_RELEASE(m);
    }
    
    while (!m_sendMessageQueue.empty())
    {
        Message* m = m_sendMessageQueue.front();
        m_sendMessageQueue.pop();
//        CC_SAFE_RELEASE(m);
    }
    
    while (!m_sendNewMessageQueue.empty())
    {
        NewMessage* m = m_sendNewMessageQueue.front();
        m_sendNewMessageQueue.pop();
//        CC_SAFE_RELEASE(m);
    }
}

void SocketClient::start()
{
	if(!m_bThreadSendCreated)
    {
		pthread_create(&pthread_t_send, NULL,ThreadSendMessage, this);
		m_bThreadSendCreated = true;
	}
}

bool SocketClient::isWaitConnect()
{
	return m_iState == SocketClient_WAIT_CONNECT;
}

Message* constructErrorMessage(int type,int errCode, std::string error)
{
	Message* msg = new Message();
    //	msg->type = 0;
    //	msg->type_selfdefine = type;//TYPE_SELF_DEINE_MESSAGE_CONNECT_FAIL;
    //	ByteBuffer* buf = new ByteBuffer(1024);
    //	buf->putInt(errCode);
    //	buf->putUTF(error);
    //
    //	msg->data = buf->toByteArray();
    //	delete buf;
	
	return msg;
}

int SocketClient::bytesToInt(byte* bytes)
{
    int addr = bytes[3] & 0xFF;
    addr |= ((bytes[2] << 8) & 0xFF00);
    addr |= ((bytes[1] << 16) & 0xFF0000);
    addr |= ((bytes[0] << 24) & 0xFF000000);
    
    return addr;
}

byte* SocketClient::intToByte(int i)
{
    byte* abyte0 = new byte[4];
    abyte0[3] = (byte) (0xff & i);
    abyte0[2] = (byte) ((0xff00 & i) >> 8);
    abyte0[1] = (byte) ((0xff0000 & i) >> 16);
    abyte0[0] = (byte) ((0xff000000 & i) >> 24);
    
    return abyte0;
}

NewMessage* SocketClient::constructMessage(std::string value)
{
    NewMessage *msg = new NewMessage;
    
    const char *pValue = value.c_str();
    unsigned short valueLength = strlen(pValue);
    msg->length = valueLength + 2;
    msg->data = new char(msg->length);
    
    void *pLeng = static_cast<void*>(&valueLength);
    memcpy(msg->data, static_cast<char*>(pLeng) + 1, sizeof(char));
    memcpy(static_cast<char*>(msg->data) + 1, &valueLength, sizeof(char));
    
    memcpy(static_cast<char*>(msg->data) + 2, pValue, valueLength);
    
    return msg;
}

NewMessage* SocketClient::constructMessage(Json::Value value, int commandId)
{
    NewMessage *msg = new NewMessage;
    
    void *pValue = static_cast<void*>(&value);
    unsigned short valueLength = strlen(static_cast<char*>(pValue));
    msg->length = valueLength + 2;
    msg->data = new char(msg->length);
    
    void *pLeng = static_cast<void*>(&valueLength);
    memcpy(msg->data, static_cast<char*>(pLeng) + 1, sizeof(char));
    
    memcpy(static_cast<char*>(msg->data) + 1, &valueLength, sizeof(char));
    
    memcpy(static_cast<char*>(msg->data) + 2, pValue, valueLength);
    
    return msg;
}

Message* SocketClient::constructMessage(const char* data,int commandId)
{
    Message* msg = new Message();
    
    msg->HEAD0=78;
    msg->HEAD1=37;
    msg->HEAD2=38;
    msg->HEAD3=48;
    msg->ProtoVersion=9;
    
    int a=0;
    msg->serverVersion[3]=(byte)(0xff&a);;
    msg->serverVersion[2]=(byte)((0xff00&a)>>8);
    msg->serverVersion[1]=(byte)((0xff0000&a)>>16);
    msg->serverVersion[0]=(byte)((0xff000000&a)>>24);
    
    int b=strlen(data)+4;
    
    msg->length[3]=(byte)(0xff&b);;
    msg->length[2]=(byte)((0xff00&b)>>8);
    msg->length[1]=(byte)((0xff0000&b)>>16);
    msg->length[0]=(byte)((0xff000000&b)>>24);
    
    int c=commandId;
    msg->commandId[3]=(byte)(0xff&c);
    msg->commandId[2]=(byte)((0xff00&c)>>8);
    msg->commandId[1]=(byte)((0xff0000&c)>>16);
    msg->commandId[0]=(byte)((0xff000000&c)>>24);
    
    msg->data = new char[msg->datalength()];
    memcpy(msg->data+0,&msg->HEAD0,1);
    memcpy(msg->data+1,&msg->HEAD1,1);
    memcpy(msg->data+2,&msg->HEAD2,1);
    memcpy(msg->data+3,&msg->HEAD3,1);
    memcpy(msg->data+4,&msg->ProtoVersion,1);
    memcpy(msg->data+5,&msg->serverVersion,4);
    memcpy(msg->data+9,&msg->length,4);
    memcpy(msg->data+13,&msg->commandId,4);
    memcpy(msg->data+17,data,strlen(data));
    
	return msg;
}

void SocketClient::stop(boolean b)
{
	m_iState = SocketClient_DESTROY;

	{
		MyLock lock(&m_thread_cond_mutex);
		pthread_cond_signal(&m_threadCond);
	}
    
	if(m_bThreadRecvCreated)
	{
        pthread_join(pthread_t_receive, NULL);
    }
	pthread_join(pthread_t_send, NULL);
}

bool SocketClient::connectServer()
{
	if( m_host.length() < 1 || m_iport == 0)
    {
        return false;
    }
    
	if( m_hSocket != -1)
    {
		close(m_hSocket);
	}
    
	m_hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_hSocket == -1)
	{
		return false;
	}
	
	sockaddr_in socketAddr;
	memset(&socketAddr, 0, sizeof(socketAddr));
    
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(m_iport);
	socketAddr.sin_addr.s_addr = inet_addr(m_host.c_str());
    
    memset(&(socketAddr.sin_zero), 0, sizeof(socketAddr.sin_zero));
    
	int iErrorCode = connect(m_hSocket, (sockaddr*)&socketAddr, sizeof(socketAddr));
	if (iErrorCode == -1)
	{
		printf("socket connect error:%d\n",errno);
		return false;
	}
    
	if(!m_bThreadRecvCreated)
    {
        if(pthread_create(&pthread_t_receive, NULL, SocketClient::ThreadReceiveMessage, this) != 0)
        {
			return false;
        }
        
		m_bThreadRecvCreated = true;
	}
    
	m_iState = SocketClient_OK;
	
	return true;
}

void SocketClient::sendMessage_(Message* msg,bool b)
{
	if(m_iState == SocketClient_DESTROY)
    {
		delete msg;
		return;
	}
    
	{
		MyLock lock(&m_sendqueue_mutex);
		m_sendMessageQueue.push(msg);
	}
    
	if( m_iState == SocketClient_OK)
	{
		MyLock lock(&m_thread_cond_mutex);
		pthread_cond_signal(&m_threadCond);
	}
}

void SocketClient::sendMessage_(NewMessage* msg, bool b)
{
    if(m_iState == SocketClient_DESTROY)
    {
        delete msg;
        return;
    }
    
    {
        MyLock lock(&m_sendqueue_mutex);
        m_sendNewMessageQueue.push(msg);
    }
    
    if( m_iState == SocketClient_OK)
    {
        MyLock lock(&m_thread_cond_mutex);
        pthread_cond_signal(&m_threadCond);
    }
}

void* SocketClient::ThreadSendMessage(void *p)
{
	SocketClient* This = static_cast<SocketClient*>(p) ;
	
	while(This->m_iState == SocketClient_WAIT_CONNECT && !This->connectServer())
    {
		if( This->m_iport2.size()> 0)
        {
			This->m_host = This->m_host2[0];
			This->m_iport = This->m_iport2[0];
			This->m_serverId = This->m_serverId2[0];
			This->m_clientId = This->m_clientId2[0];
			
			This->m_host2.erase(This->m_host2.begin());
			This->m_iport2.erase(This->m_iport2.begin());
			This->m_serverId2.erase(This->m_serverId2.begin());
			This->m_clientId2.erase(This->m_clientId2.begin());
		}
		else
        {
			This->m_iState = SocketClient_DESTROY;
			std::string error("连网失败,请检查网络设置");
            
			{
				MyLock lock(&This->m_sendqueue_mutex);
                
				This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CONNECT_FAIL,errno,error));
			}
            
			return ((void *)0);
		}
	}
	
	ByteBuffer& sendBuff = This->m_cbSendBuf;
	int socket = This->m_hSocket;
	
	while (This->m_iState != SocketClient_DESTROY)
    {
		if( This->m_iState == SocketClient_OK)
        {
            //发送缓冲器有遗留的数据要发送
            if(sendBuff.getPosition() > 0)
            {
                sendBuff.flip();
                int ret = send(socket,(char *)sendBuff.getBuffer(),sendBuff.getLimit(),0);
                if(ret == -1)
                {
                    This->m_iState = SocketClient_DESTROY ;
                    
                    std::string err("发送数据，网络异常！");
                    
                    MyLock lock(&This->m_sendqueue_mutex);
                    This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CANNOT_SEND_MESSAGE,errno,err));
                    return ((void *)0);
                }
                else
                {
                    //printf("SocketClient::ThreadSendMessage(), send message to server, size = %d\n",ret);
                }
                sendBuff.setPosition(sendBuff.getPosition()+ret);
                sendBuff.compact();
            }
            
//            Message* msg = NULL;
            NewMessage* msg = NULL;
            while( This->m_iState != SocketClient_DESTROY && This->m_sendNewMessageQueue.size()> 0)
            {
                {
                    MyLock lock(&This->m_sendqueue_mutex);
//                    msg = This->m_sendMessageQueue.front();
//                    This->m_sendMessageQueue.pop();
                    msg = This->m_sendNewMessageQueue.front();
                    This->m_sendNewMessageQueue.pop();
                }
                
//                printf(" sendData length: %d  %ld" ,  msg->datalength(), sizeof(char));
//                if(msg->datalength() + sendBuff.getPosition() > sendBuff.getLimit())
//                {
//                    This->m_iState = SocketClient_DESTROY;
//                    printf("send buffer is full, send thread stop!");
//                    MyLock lock(&This->m_sendqueue_mutex);
//                    This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CANNOT_SEND_MESSAGE,0,"发送缓冲器已满，您的网络环境好像出现了问题！"));
//                    return ((void *)0);
//                }
                sendBuff.put(static_cast<char*>(msg->data), 0, msg->datalength());
                sendBuff.flip();
                
                int ret = send(socket,(char *)sendBuff.getBuffer(),sendBuff.getLimit(),0);
                if(ret == -1)
                {
                    This->m_iState = SocketClient_DESTROY;
                    std::string err("发送数据，网络异常！");
                    MyLock lock(&This->m_sendqueue_mutex);
                    This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CANNOT_SEND_MESSAGE,errno,err));
                    return ((void *)0);
                }

                sendBuff.setPosition(sendBuff.getPosition()+ret);
                sendBuff.compact();
                
                delete msg;
            }
		}
		
//		if(This->m_iState != SocketClient_DESTROY && This->m_sendMessageQueue.size() == 0)
        if(This->m_iState != SocketClient_DESTROY && This->m_sendNewMessageQueue.size() == 0)
        {
			//sleep
			struct timeval tv;
			struct timespec ts;
			gettimeofday(&tv, NULL);
			ts.tv_sec = tv.tv_sec + 5;
			ts.tv_nsec = 0;
			
			MyLock lock(&(This->m_thread_cond_mutex));
//			if(This->m_iState != SocketClient_DESTROY && This->m_sendMessageQueue.size() == 0)
            if(This->m_iState != SocketClient_DESTROY && This->m_sendNewMessageQueue.size() == 0)
            {
				pthread_cond_timedwait(&(This->m_threadCond),&(This->m_thread_cond_mutex),&ts);
			}
		}
	}
    
	return (void*)0;
}

bool g_bcheckReceivedMessage = true;

void* SocketClient::ThreadReceiveMessageOld(void *p)
{
	fd_set fdRead;
	
	struct timeval	aTime;
	aTime.tv_sec = 1;
	aTime.tv_usec = 0;
    
	//最大多少秒，连接上收不到数据就提示用户，重新登录
	int maxIdleTimeInSeconds = 60*3;
	
	//最大多少秒，连接上收不到数据就提示用户，选择重连
	int hint2TimeInSeconds = 60;
	
	//多长时间没有收到任何数据，提示用户
	int hintTimeInSeconds = 30;
	
	struct timeval lastHintUserTime;
	struct timeval lastReceiveDataTime;
	struct timeval now;
	
	gettimeofday(&lastReceiveDataTime, NULL);
	lastHintUserTime = lastReceiveDataTime;
	
	SocketClient* This = static_cast<SocketClient*>(p) ;
	
	ByteBuffer* recvBuff = &This->m_cbRecvBuf;
    
	while (This->m_iState != SocketClient_DESTROY)
	{
		if(This->m_iState != SocketClient_OK)
        {
			usleep(1000);
			continue;
		}
		FD_ZERO(&fdRead);
        
		FD_SET(This->m_hSocket,&fdRead);
        
		aTime.tv_sec = 1;
		aTime.tv_usec = 0;
		
        int ret = select(This->m_hSocket+1,&fdRead,NULL,NULL,&aTime);
		if (ret == -1 )
		{
			if(errno == EINTR)
            {
				printf("======   收到中断信号，什么都不处理＝＝＝＝＝＝＝＝＝");
			}
            else
            {
				This->m_iState = SocketClient_DESTROY;
				MyLock lock(&This->m_sendqueue_mutex);
				This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CONNECT_TERMINATE,errno,"连接异常中断"));
				return ((void *)0);
			}
		}
		else if(ret==0)
		{
			gettimeofday(&now, NULL);
			if( g_bcheckReceivedMessage )
            {
                if(now.tv_sec - lastReceiveDataTime.tv_sec > maxIdleTimeInSeconds && now.tv_sec - lastHintUserTime.tv_sec > hintTimeInSeconds)
                {
                    lastHintUserTime = now;
                    
                    MyLock lock(&This->m_sendqueue_mutex);
                    
                    while( This->m_receivedMessageQueue.size()>0)
                    {
                        Message* msg = This->m_receivedMessageQueue.front();
                        This->m_receivedMessageQueue.pop();
//                        CCLog("删除消息");
                        delete msg;
                    }
                    This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_RECONNECT_FORCE,0,"您的网络已经出问题了！"));
                    
                }
                else if(now.tv_sec - lastReceiveDataTime.tv_sec > hint2TimeInSeconds && now.tv_sec - lastHintUserTime.tv_sec > hintTimeInSeconds)
                {
                    lastHintUserTime = now;
                    MyLock lock(&This->m_sendqueue_mutex);
                    This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_RECONNECT_HINT,0,"您的网络好像出问题了！"));
                }
                else if(now.tv_sec - lastReceiveDataTime.tv_sec > hintTimeInSeconds && now.tv_sec - lastHintUserTime.tv_sec > hintTimeInSeconds)
                {
                    lastHintUserTime = now;
                    MyLock lock(&This->m_sendqueue_mutex);
                    This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_IDLE_TIMEOUT,0,"您的网络好像出问题了！"));
                }
            }
            else
            {
                lastHintUserTime = now;
                lastReceiveDataTime= now;
            }
		}
		else if (ret > 0)
		{
			if (FD_ISSET(This->m_hSocket,&fdRead))
			{
				int iRetCode = 0;
                printf(" recv data %d \n", recvBuff->remaining());
				if(recvBuff->remaining() > 0)
                {
					iRetCode = recv(This->m_hSocket,recvBuff->getBuffer()+recvBuff->getPosition(), recvBuff->remaining(),0);
				}
                
                printf(" recv data later  %d   %d \n", recvBuff->remaining(), iRetCode);
				if (iRetCode == -1)
				{
					This->m_iState = SocketClient_DESTROY;
					MyLock lock(&This->m_sendqueue_mutex);
					
					while( This->m_receivedMessageQueue.size()>0)
                    {
						Message* msg = This->m_receivedMessageQueue.front();
						This->m_receivedMessageQueue.pop();
//                        CCLog("删除消息");
						delete msg;
					}
					
                    std::string tmp("网络连接中断！");
					This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CONNECT_TERMINATE,errno,tmp));
					return ((void *)0);
				}
                else if(iRetCode == 0 && recvBuff->remaining() > 0)
                {
					This->m_iState = SocketClient_DESTROY;
					MyLock lock(&This->m_sendqueue_mutex);
					while( This->m_receivedMessageQueue.size()>0)
                    {
						Message* msg = This->m_receivedMessageQueue.front();
						This->m_receivedMessageQueue.pop();
//                        CCLog("删除消息");
						delete msg;
					}
					This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_SERVER_CLOSE_CONNECTION,errno,"服务器主动关闭连接!"));
					
					return ((void *)0);
				}
				else
				{
					gettimeofday(&lastReceiveDataTime, NULL);
					
					recvBuff->setPosition(recvBuff->getPosition()+ iRetCode);
					recvBuff->flip();
					int tmpOffset = 17;
					while(recvBuff->remaining() > tmpOffset)
                    {
						int pos = recvBuff->getPosition();
						int length= recvBuff->getLength(9);
						
						if(recvBuff->remaining()+tmpOffset >= length)
                        {
							Message* message = new Message();
  
                            message->HEAD0 = recvBuff->getByte();
                            message->HEAD1 = recvBuff->getByte();
                            message->HEAD2 = recvBuff->getByte();
                            message->HEAD3 = recvBuff->getByte();
                            message->ProtoVersion = recvBuff->getByte();
                            recvBuff->getAsBytes(message->serverVersion);
                            recvBuff->getAsBytes(message->length);
                            recvBuff->getAsBytes(message->commandId);
                            
							printf("message length: %d commandId: %d \n", bytesToInt(message->length),bytesToInt(message->commandId));
                            
							char* tmp = new char[length-3];
							recvBuff->get(tmp,0,length-4);
                            tmp[length-4] = '\0';
                            message->data = tmp;
                            printf("%s",tmp);
                            MyLock lock(&This->m_sendqueue_mutex);

                            This->m_receivedMessageQueue.push(message);
                            if(bytesToInt(message->commandId)==218)
                            {
//                                CData::getCData()->m_newlevel_dic->setObject(message, 218);
                            }
                            else
                            {
//                                CData::getCData()->m_dictionary->setObject(message,bytesToInt(message->commandId));
                            }
						}
                        else if(length>recvBuff->getCapacity())
                        {
							This->m_iState = SocketClient_DESTROY;
                          
							MyLock lock(&This->m_sendqueue_mutex);
							
							while( This->m_receivedMessageQueue.size()>0)
                            {
								Message* msg = This->m_receivedMessageQueue.front();
								This->m_receivedMessageQueue.pop();
//                                CCLog("删除消息");
								delete msg;
							}
							
							This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CONNECT_TERMINATE,0,"数据包太大，连接中断！"));
							return ((void *)0);
						}
                        else
                        {
							recvBuff->setPosition(pos);
							break;
						}
					}

					recvBuff->compact();
				}
				
			}
		}
	}
    
	return (void*)0;
}

void* SocketClient::ThreadReceiveMessage(void *p)
{
    fd_set fdRead;
    
    struct timeval	aTime;
    aTime.tv_sec = 1;
    aTime.tv_usec = 0;
    
    //最大多少秒，连接上收不到数据就提示用户，重新登录
    int maxIdleTimeInSeconds = 60*3;
    
    //最大多少秒，连接上收不到数据就提示用户，选择重连
    int hint2TimeInSeconds = 60;
    
    //多长时间没有收到任何数据，提示用户
    int hintTimeInSeconds = 30;
    
    struct timeval lastHintUserTime;
    struct timeval lastReceiveDataTime;
    struct timeval now;
    
    gettimeofday(&lastReceiveDataTime, NULL);
    lastHintUserTime = lastReceiveDataTime;
    
    SocketClient* This = static_cast<SocketClient*>(p) ;
    
    ByteBuffer* recvBuff = &This->m_cbRecvBuf;
    
    while (This->m_iState != SocketClient_DESTROY)
    {
        if(This->m_iState != SocketClient_OK)
        {
            usleep(1000);
            continue;
        }
        FD_ZERO(&fdRead);
        
        FD_SET(This->m_hSocket,&fdRead);
        
        aTime.tv_sec = 1;
        aTime.tv_usec = 0;
        
        int ret = select(This->m_hSocket+1,&fdRead,NULL,NULL,&aTime);
        if (ret == -1 )
        {
            if(errno == EINTR)
            {
                printf("======   收到中断信号，什么都不处理＝＝＝＝＝＝＝＝＝");
            }
            else
            {
                This->m_iState = SocketClient_DESTROY;
                MyLock lock(&This->m_sendqueue_mutex);
                return ((void *)0);
            }
        }
        else if(ret==0)
        {
            gettimeofday(&now, NULL);
            if( g_bcheckReceivedMessage )
            {
                if(now.tv_sec - lastReceiveDataTime.tv_sec > maxIdleTimeInSeconds && now.tv_sec - lastHintUserTime.tv_sec > hintTimeInSeconds)
                {
                    lastHintUserTime = now;
                    
                    MyLock lock(&This->m_sendqueue_mutex);
                    
                    while( This->m_receivedNewMessageQueue.size()>0)
                    {
                        NewMessage* msg = This->m_receivedNewMessageQueue.front();
                        This->m_receivedNewMessageQueue.pop();
//                        CCLog("删除消息");
                        delete msg;
                    }
                }
                else if(now.tv_sec - lastReceiveDataTime.tv_sec > hint2TimeInSeconds && now.tv_sec - lastHintUserTime.tv_sec > hintTimeInSeconds)
                {
                    lastHintUserTime = now;
                    MyLock lock(&This->m_sendqueue_mutex);
                }
                else if(now.tv_sec - lastReceiveDataTime.tv_sec > hintTimeInSeconds && now.tv_sec - lastHintUserTime.tv_sec > hintTimeInSeconds)
                {
                    lastHintUserTime = now;
                    MyLock lock(&This->m_sendqueue_mutex);
                }
            }
            else
            {
                lastHintUserTime = now;
                lastReceiveDataTime= now;
            }
        }
        else if (ret > 0)
        {
            if (FD_ISSET(This->m_hSocket,&fdRead))
            {
                int iRetCode = 0;
                printf(" recv data %d \n", recvBuff->remaining());
                if(recvBuff->remaining() > 0)
                {
                    iRetCode = recv(This->m_hSocket,recvBuff->getBuffer()+recvBuff->getPosition(), recvBuff->remaining(),0);
                }
                
                printf(" recv data later  %d   %d \n", recvBuff->remaining(), iRetCode);
                if (iRetCode == -1)
                {
                    This->m_iState = SocketClient_DESTROY;
                    MyLock lock(&This->m_sendqueue_mutex);
                    
                    while( This->m_receivedNewMessageQueue.size()>0)
                    {
                        NewMessage* msg = This->m_receivedNewMessageQueue.front();
                        This->m_receivedNewMessageQueue.pop();
//                        CCLog("删除消息");
                        delete msg;
                    }
                    
                    std::string tmp("网络连接中断！");
                    return ((void *)0);
                }
                else if(iRetCode == 0 && recvBuff->remaining() > 0)
                {
                    This->m_iState = SocketClient_DESTROY;
                    MyLock lock(&This->m_sendqueue_mutex);
                    while( This->m_receivedNewMessageQueue.size()>0)
                    {
                        NewMessage* msg = This->m_receivedNewMessageQueue.front();
                        This->m_receivedNewMessageQueue.pop();
//                        CCLog("删除消息");
                        delete msg;
                    }
                    
                    return ((void *)0);
                }
                else
                {
                    gettimeofday(&lastReceiveDataTime, NULL);
                    
                    recvBuff->setPosition(recvBuff->getPosition()+ iRetCode);
                    recvBuff->flip();
                    
                    unsigned short messageLength = 0;
                    void *pLen = &messageLength;
                    
                    byte high = recvBuff->getByte();
                    memcpy(static_cast<char*>(pLen) + 1, &high, sizeof(char));
                    
                    byte low = recvBuff->getByte();
                    memcpy(pLen, &low, sizeof(char));
                    
//                    CCLog("receive message %d", messageLength);

                    char *pstrMessage = new char(messageLength);
                    recvBuff->get(pstrMessage, 0, messageLength);
                    
//                    CCLog("message--- %s", pstrMessage);
                    
                    Json::Reader read;
                    Json::Value root;
                    read.parse(pstrMessage, root);
                    Json::Value data=root["ResRegMsg"];
                    int ret = data["ResState"].asInt();   //0: 成功  1:server error   2:已注册
//                    CCLog("%d", ret);
                    
                    
                    int tmpOffset = 17;
                    while(recvBuff->remaining() > tmpOffset)
                    {
                        int pos = recvBuff->getPosition();
                        int length= recvBuff->getLength(9);
                        
                        if(recvBuff->remaining()+tmpOffset >= length)
                        {
                            NewMessage* message = new NewMessage();
                            
                            unsigned short messageLength = 0;
                            void *pLen = &messageLength;
                            
                            byte low = recvBuff->getByte();
                            memcpy(pLen, &low, sizeof(char));
                            byte high = recvBuff->getByte();
                            memcpy(static_cast<char*>(pLen) + 1, &high, sizeof(char));

                            
//                            CCLog("receive message %d", messageLength);
                            
                            char* tmp = new char[length-3];
                            recvBuff->get(tmp,0,length-4);
                            tmp[length-4] = '\0';
                            message->data = tmp;
                            printf("%s",tmp);
                            MyLock lock(&This->m_sendqueue_mutex);
                            
                            This->m_receivedNewMessageQueue.push(message);

//                            CData::getCData()->m_dictionary->setObject(message,bytesToInt(message->commandId));
                            
                        }
                        else if(length>recvBuff->getCapacity())
                        {
                            This->m_iState = SocketClient_DESTROY;
                            
                            MyLock lock(&This->m_sendqueue_mutex);
                            
                            while( This->m_receivedNewMessageQueue.size()>0)
                            {
                                NewMessage* msg = This->m_receivedNewMessageQueue.front();
                                This->m_receivedNewMessageQueue.pop();
//                                CCLog("删除消息");
                                delete msg;
                            }
                            
                            This->m_receivedMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CONNECT_TERMINATE,0,"数据包太大，连接中断！"));
                            return ((void *)0);
                        }
                        else
                        {
                            recvBuff->setPosition(pos);
                            break;
                        }
                    }
                    
                    recvBuff->compact();
                }
                
            }
        }
    }
    
    return (void*)0;
}

//获取服务器包
NewMessage* SocketClient::pickReceivedMessage()
{
	NewMessage* msg = NULL;
	MyLock lock(&m_sendqueue_mutex);
//	if( m_receivedMessageQueue.size()>0)
//    {
//		msg = m_receivedMessageQueue.front();
//	}
    
    if( m_receivedNewMessageQueue.size()>0)
    {
        msg = m_receivedNewMessageQueue.front();
    }

	
	return msg;
}

NewMessage* SocketClient::popReceivedMessage()
{
	NewMessage* msg = NULL;
	MyLock lock(&m_sendqueue_mutex);
//	if( m_receivedMessageQueue.size()>0)
//    {
//		msg = m_receivedMessageQueue.front();
//		m_receivedMessageQueue.pop();
//	}
    
    if( m_receivedNewMessageQueue.size()>0)
    {
        msg = m_receivedNewMessageQueue.front();
        m_receivedNewMessageQueue.pop();
    }
	
	return msg;
}

void SocketClient::pushReceivedMessage(NewMessage* msg)
{
	MyLock lock(&m_sendqueue_mutex);
//	m_receivedMessageQueue.push(msg);
    m_receivedNewMessageQueue.push(msg);
}

long long SocketClient::getSeq()
{
	if( m_isvalidSeq )
    {
		long long a = m_sabcde[1];
		long long b = m_sabcde[2];
		long long c = m_sabcde[3];
		long long d = m_sabcde[4];
		m_sabcde[0] = (long long)(a*2+b+c*3+d);
		m_sabcde[1] = a^b+b|c+3+d;
		m_sabcde[2] = b-a+d*123;
		m_sabcde[3] = (c%123456)+a*b+(long long)sqrt((double)abs(d));
		m_sabcde[4] = (long long)(a*1.233f+b*0.45456f+c*d+9);
	}
    
	return m_sabcde[0];
}

void SocketClient::swhlie(int commandId)
{
    
    
}
