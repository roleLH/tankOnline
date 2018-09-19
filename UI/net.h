#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <thread>
#include <mutex>
#include <iostream>

#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <chrono>
#include "util.h"


#define PORT 6166
#define SERVER_PORT (PORT + 1)
#define CLIENT_PORT (PORT + 2)
#define IP "192.168.2.115"
#define INTERVAL_OF_FRAME 30


using namespace std::chrono;

namespace net
{
   
    enum EUserState
    {
        USER_IDLE,
        USER_SEND
    };

    struct UserBuf
    {
		bool isRegistered;		// 用以记录是否注册的表记
		bool logOfServerAddr;	// 记录该用户套接字地址是否已经记录过的标记
		EUserState state;
        sockaddr_in linkAddr;
		sockaddr_in serverAddr;
        char buf[8];
		std::mutex mutexOfBuf;

		UserBuf() : isRegistered(false), logOfServerAddr(false), state(USER_IDLE), linkAddr{}, serverAddr{},  buf{} { }
    };


    class Server
    {
    public:
        Server();
        ~Server();
	public:
        void link();
        void pack();
        void resetState();
        void mainLoop();
        void sendMsgToOther(const char id, SOCKET sock);
        inline void finish() { isFinished = true; }
		inline sockaddr_in& DEBUG_getServer() { return name; }
    private:
		

        SOCKET serverSock;
        sockaddr_in name;
        std::atomic_bool isFinished;
		List<UserBuf> userList;
		int numofUser;
    };

  
    class Client
    {
	public:
		Client(sockaddr_in& server);
//		~Client();
		int sendMsg(const char* buf, int len);
		int recvMsg(char* buf, int len);
		int link();
    private:
		sockaddr_in linkAddr;
		sockaddr_in serverAddr;
		SOCKET sockOfLink, sockOfTrans;
    };


    class NetManager 
    {
    public:
 //       NetManager();

        void createServer(std::string& name);
        bool tryGetServer();

		inline sockaddr_in* getServerAddr() 
		{ 
			sockaddr_in* tmp = NULL;
			// 这里加锁的原因：我们总是在循环中尝试获取server的信息，
			// 在另外的线程，试图去写server 信息
			if (mutexOfServer.try_lock())
			{
				tmp = &server;
				mutexOfServer.unlock();
			}
			return tmp;
		}
    private:
        sockaddr_in server;
		std::mutex mutexOfServer;
    };

}
/*
 Link 数据包 重设计。 time 18-09-19 
 之前只是简单地发送userId 发现还有很多逻辑需要处理。出现的问题就是不同步。
 现，重新设计数据包。
 第一个字节表示后面有多少个ueserId
 第一个字节为 0 第二个字节 表示为该用户初次连接时该用户的id
 第一个字节为 1 第二个字节 表示为其他用户需要同步到该客户端的id
 第一个字节为 2+ 第二个字节 表示为该用户初次连接时该用户的id
				 剩下的字节 表示为其他用户需要同步到该客户端的id
 原因如下：
 当一个客户端初次在系统注册时，返回其id 同时返回当前其他已注册客户的id（服务器向该客户端）
		同时服务器需要向其他客户端同步该客户端的id
 只有第一个用户注册时，数据包第一个字节为0，这也是为了区别第二条规则。

 以上
*/