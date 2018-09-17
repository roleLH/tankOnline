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
#include "static.h"
#include "util.h"


#define PORT 6166
#define SERVER_PORT (PORT + 1)
#define CLIENT_PORT (PORT + 2)
#define IP "10.251.224.3"
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
        void sendMsgToOther(const char* buf, int len, SOCKET sock);
        inline void start() { isFinished = true; }
		inline sockaddr_in& DEBUG_getServer() { return name; }
    private:
		

        SOCKET serverSock;
        sockaddr_in name;
        std::atomic_bool isFinished;
		List<UserBuf> userList;
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