#include "mlib_socket.h"

#include <sys/socket.h>
#include "mlib_log.h"
#include "mlib_utils.h"
#include "mlib_thread.h"
#include "mlib_buffer.h"
#include "SocketConst.h"

MLIB_NS_BEGIN

static std::recursive_mutex g_mutex;

const std::string MSocketRequest::EVENT_FINISHED = "event_finished";
const std::string MSocketRequest::EVENT_CANCELLED = "event_cancelled";
const std::string MSocketRequest::EVENT_DELETE = "event_delete";

static MSharedQueue<MSocketRequest *> g_requests_low;
static MSharedQueue<MSocketRequest *> g_requests_normal;
static MSharedQueue<MSocketRequest *> g_requests_high;

static uint32_t g_num_of_threads_low = 0;
static uint32_t g_num_of_threads_normal = 0;
static uint32_t g_num_of_threads_high = 0;




#pragma mark MSocketRequest

MSocketRequest::MSocketRequest(std::string host, int port, byte clientid, byte serverid): m_iState(SOCKET_CLIENT_WAIT_CONNECT), m_cbRecvBuf(LIMIT_BUFFER), m_cbSendBuf(1024*60)
{
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

MSocketRequest::~MSocketRequest()
{
    m_iState = SOCKET_CLIENT_DESTROY;
    if( m_hSocket != -1)
    {
        close(m_hSocket);
    }
    
    pthread_mutex_destroy(&m_sendqueue_mutex);
    pthread_mutex_destroy(&m_thread_cond_mutex);
    pthread_cond_destroy(&m_threadCond);
    
    while (!m_receivedNewMessageQueue.empty())
    {
        SocketMessage* msg = m_receivedNewMessageQueue.front();
        m_receivedNewMessageQueue.pop();
        SK_SAFE_DELETE(msg);
    }
    
    while (!m_sendNewMessageQueue.empty())
    {
        SocketMessage* msg = m_sendNewMessageQueue.front();
        m_sendNewMessageQueue.pop();
        SK_SAFE_DELETE(msg);
    }
}

void MSocketRequest::send()
{
    if(!m_bThreadSendCreated)
    {
        pthread_create(&pthread_t_send, NULL, ThreadSendMessage, this);
        m_bThreadSendCreated = true;
    }
}

void* MSocketRequest::ThreadSendMessage(void *p)
{
    MSocketRequest* This = static_cast<MSocketRequest*>(p) ;
    
    while(This->m_iState == SOCKET_CLIENT_WAIT_CONNECT && !This->connectServer())
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
            This->m_iState = SOCKET_CLIENT_DESTROY;
            std::string error("连网失败,请检查网络设置");
            
            {
                MyLock lock(&This->m_sendqueue_mutex);
                
                This->m_receivedNewMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CONNECT_FAIL, errno, error));
            }
            
            return ((void *)0);
        }
    }
    
    SocketBuffer& sendBuff = This->m_cbSendBuf;
    int socket = This->m_hSocket;
    
    while (This->m_iState != SOCKET_CLIENT_DESTROY)
    {
        if( This->m_iState == SOCKET_CLIENT_OK)
        {
            //发送缓冲器有遗留的数据要发送
            if(sendBuff.getPosition() > 0)
            {
                sendBuff.flip();
                int ret = send(socket, (char *)sendBuff.getBuffer(), sendBuff.getLimit(),0);
                if(ret == -1)
                {
                    This->m_iState = SOCKET_CLIENT_DESTROY ;
                    
                    std::string err("发送数据，网络异常！");
                    
                    MyLock lock(&This->m_sendqueue_mutex);
                    This->m_receivedNewMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CANNOT_SEND_MESSAGE, errno, err));
                    return ((void *)0);
                }
                else
                {
                    //printf("SocketClient::ThreadSendMessage(), send message to server, size = %d\n",ret);
                }
                
                sendBuff.setPosition(sendBuff.getPosition()+ret);
                sendBuff.compact();
            }
            
            SocketMessage* msg = NULL;
            while( This->m_iState != SOCKET_CLIENT_DESTROY && This->m_sendNewMessageQueue.size()> 0)
            {
                {
                    MyLock lock(&This->m_sendqueue_mutex);
                    
                    msg = This->m_sendNewMessageQueue.front();
                    This->m_sendNewMessageQueue.pop();
                }
                
                printf(" sendData length: %d  %ld" ,  msg->dataLength(), sizeof(char));
                if(msg->dataLength() + sendBuff.getPosition() > sendBuff.getLimit())
                {
                    This->m_iState = SOCKET_CLIENT_DESTROY;
                    printf("send buffer is full, send thread stop!");
                    MyLock lock(&This->m_sendqueue_mutex);
                    This->m_receivedNewMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CANNOT_SEND_MESSAGE,0,"发送缓冲器已满，您的网络环境好像出现了问题！"));
                    return ((void *)0);
                }
                
                sendBuff.put(msg->data(), 0, msg->dataLength());
                sendBuff.flip();
                
                int ret = send(socket,(char *)sendBuff.getBuffer(),sendBuff.getLimit(),0);
                if(ret == -1)
                {
                    This->m_iState = SOCKET_CLIENT_DESTROY;
                    std::string err("发送数据，网络异常！");
                    MyLock lock(&This->m_sendqueue_mutex);
                    This->m_receivedNewMessageQueue.push(constructErrorMessage(TYPE_SELF_DEINE_MESSAGE_CANNOT_SEND_MESSAGE, errno, err));
                    return ((void *)0);
                }
                
                sendBuff.setPosition(sendBuff.getPosition()+ret);
                sendBuff.compact();
                
                delete msg;
            }
        }
        
        if(This->m_iState != SOCKET_CLIENT_DESTROY && This->m_sendNewMessageQueue.size() == 0)
        {
            //sleep
            struct timeval tv;
            struct timespec ts;
            gettimeofday(&tv, NULL);
            ts.tv_sec = tv.tv_sec + 5;
            ts.tv_nsec = 0;
            
            MyLock lock(&(This->m_thread_cond_mutex));
            
            if(This->m_iState != SOCKET_CLIENT_DESTROY && This->m_sendNewMessageQueue.size() == 0)
            {
                pthread_cond_timedwait(&(This->m_threadCond),&(This->m_thread_cond_mutex),&ts);
            }
        }
    }
    
    return (void*)0;
}

bool MSocketRequest::connectServer()
{
    if(_host.length() < 1 || _iport == 0)
    {
        return false;
    }
    
    if(_hSocket != -1)
    {
        close(_hSocket);
    }
    
    _hSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_hSocket == -1)
    {
        return false;
    }
    
    sockaddr_in socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr));
    
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(m_iport);
    socketAddr.sin_addr.s_addr = inet_addr(_host.c_str());
    
    memset(&(socketAddr.sin_zero), 0, sizeof(socketAddr.sin_zero));
    
    int iErrorCode = connect(m_hSocket, (sockaddr*)&socketAddr, sizeof(socketAddr));
    if (iErrorCode == -1)
    {
        printf("socket connect error:%d\n",errno);
        return false;
    }
    
    
    if(pthread_create(&pthread_t_receive, NULL, SocketClient::ThreadReceiveMessage, this) != 0)
    {
        return false;
    }
    
    
    return true;
}

void* MSocketRequest::ThreadReceiveMessage(void *p)
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
    
    MSocketRequest* This = static_cast<MSocketRequest*>(p) ;
    
    SocketBuffer* recvBuff = &This->m_cbRecvBuf;
    
    while (This->m_iState != SOCKET_CLIENT_DESTROY)
    {
        if(This->m_iState != SOCKET_CLIENT_OK)
        {
            usleep(1000);
            continue;
        }
        FD_ZERO(&fdRead);
        
        FD_SET(This->m_hSocket,&fdRead);
        
        aTime.tv_sec = 1;
        aTime.tv_usec = 0;
        
        int ret = select(This->m_hSocket+1, &fdRead, NULL, NULL, &aTime);
        if (ret == -1 )
        {
            if(errno == EINTR)
            {
                printf("======   收到中断信号，什么都不处理＝＝＝＝＝＝＝＝＝");
            }
            else
            {
                This->m_iState = SOCKET_CLIENT_DESTROY;
                MyLock lock(&This->m_sendqueue_mutex);
                return ((void *)0);
            }
        }
        else if(ret == 0)
        {
            gettimeofday(&now, NULL);
            if( g_bcheckReceivedMessage )
            {
                if(now.tv_sec - lastReceiveDataTime.tv_sec > maxIdleTimeInSeconds && now.tv_sec - lastHintUserTime.tv_sec > hintTimeInSeconds)
                {
                    lastHintUserTime = now;
                    
                    MyLock lock(&This->m_sendqueue_mutex);
                    
                    while( This->m_receivedNewMessageQueue.size() > 0)
                    {
                        SocketMessage* msg = This->m_receivedNewMessageQueue.front();
                        This->m_receivedNewMessageQueue.pop();
                        printf("删除消息");
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
            if (FD_ISSET(This->m_hSocket, &fdRead))
            {
                int iRetCode = 0;
                printf(" recv data %d \n", recvBuff->remaining());
                if(recvBuff->remaining() > 0)
                {
                    iRetCode = recv(This->m_hSocket, recvBuff->getBuffer()+recvBuff->getPosition(), recvBuff->remaining(), 0);
                }
                
                printf(" recv data later  %d   %d \n", recvBuff->remaining(), iRetCode);
                if (iRetCode == -1)
                {
                    This->m_iState = SOCKET_CLIENT_DESTROY;
                    MyLock lock(&This->m_sendqueue_mutex);
                    
                    while( This->m_receivedNewMessageQueue.size()>0)
                    {
                        SocketMessage* msg = This->m_receivedNewMessageQueue.front();
                        This->m_receivedNewMessageQueue.pop();
                        printf("删除消息");
                        delete msg;
                    }
                    
                    std::string tmp("网络连接中断！");
                    return ((void *)0);
                }
                else if(iRetCode == 0 && recvBuff->remaining() > 0)
                {
                    This->m_iState = SOCKET_CLIENT_DESTROY;
                    MyLock lock(&This->m_sendqueue_mutex);
                    while( This->m_receivedNewMessageQueue.size()>0)
                    {
                        SocketMessage* msg = This->m_receivedNewMessageQueue.front();
                        This->m_receivedNewMessageQueue.pop();
                        printf("删除消息");
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
                    
                    printf("receive message %d", messageLength);
                    
                    char *pstrMessage = new char(messageLength);
                    recvBuff->get(pstrMessage, 0, messageLength);
                    
                    printf("message--- %s", pstrMessage);
                    SocketMessage* message = new SocketMessage();
                    message->data(pstrMessage);
                    message->dataLength(messageLength);
                    
                    {
                        MyLock lock(&This->m_sendqueue_mutex);
                        
                        This->m_receivedNewMessageQueue.push(message);
                        CData::getCData()->m_dictionary->setObject(message, bytesToInt(message->commandId));
                        
                        
                        
                        //event listener
                        This->addEventListener(MHttpRequest::EVENT_FINISHED, [req] (mlib::MEvent * evt) {
                            if (This->_isSuccess)
                            {
                                if (This->_successHandler)
                                {
                                    This->_successHandler(req);
                                }
                            }
                            else
                            {
                                if (This->_errorHandler)
                                {
                                    This->_errorHandler(req);
                                }
                            }
                        }, &g_mutex);
                        
                        This->dispatchEvent(MEvent(MHttpRequest::EVENT_FINISHED));
                        This->removeEventListenerFor(&g_mutex);
                    }
                    
                    recvBuff->compact();
                }
                
            }
        }
    }
    
    return (void*)0;
}

void MSocketRequest::onSuccess(std::function<void (MSocketRequest *)> handler)
{
    _successHandler = handler;
}

void MSocketRequest::onError(std::function<void (MSocketRequest *)> handler)
{
    _errorHandler = handler;
}


#pragma mark MHttpResponse

MHttpResponse * MSocketRequest::createResponse(unsigned short statusCode, const char *data, size_t size)
{
    return new MHttpResponse(statusCode, data, size);
}

MHttpResponse::MHttpResponse(unsigned short statusCode, const char * data, size_t size) :
_statusCode(statusCode),
_responseData(data, size),
_isErrorHandled(false)
{
    
}



MLIB_NS_END
