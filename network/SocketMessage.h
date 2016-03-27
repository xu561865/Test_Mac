#ifndef SOCKET_MESSAGE_H
#define SOCKET_MESSAGE_H

//连接失败
static const unsigned int TYPE_SELF_DEINE_MESSAGE_CONNECT_FAIL = 0xfffffA01;
//网络异常中断
static const unsigned int TYPE_SELF_DEINE_MESSAGE_CONNECT_TERMINATE = 0xfffffA02;
//服务器关闭连接
static const unsigned int TYPE_SELF_DEINE_MESSAGE_SERVER_CLOSE_CONNECTION = 0xfffffA03;
//消息发布出去（信号很差）
static const unsigned int TYPE_SELF_DEINE_MESSAGE_CANNOT_SEND_MESSAGE = 0xfffffA04;
//提示网络可能出问题了
static const unsigned int TYPE_SELF_DEINE_MESSAGE_IDLE_TIMEOUT = 0xfffffA05;
//提示网络可能出问题了,可以退出到登录
static const unsigned int TYPE_SELF_DEINE_MESSAGE_RECONNECT_HINT = 0xfffffA06;
//提示网络可能出问题了,必须退出到登录
static const unsigned int TYPE_SELF_DEINE_MESSAGE_RECONNECT_FORCE = 0xfffffA07;
//错误提示
static const unsigned int TYPE_SELF_DEINE_MESSAGE_ERROR_MESSAGE = 0xfffffA08;

static const unsigned int TYPE_SELF_DEINE_MESSAGE_CLIENT_KILL_MESSAGE = 0xfffffA09;


class SocketMessage
{
public:
    SocketMessage();
    ~SocketMessage();
    char* data() const;
    void data(char*);
    unsigned short dataLength();
    void dataLength(unsigned short);
    
    
private:
    char* _data;
    unsigned short _dataLength;
    
};

#endif /*SOCKET_MESSAGE_H*/

