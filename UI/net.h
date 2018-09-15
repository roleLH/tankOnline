#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <thread>
#include <mutex>
#include <iostream>
/**
 * 数据包格式 第一个字节表示几个用户 后面每7个字节 第一个字节表示用户id 后六个表示手柄状态
 * 
 *  
 */
#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <chrono>
#include "static.h"
#include "util.h"

using namespace std::chrono;

namespace net
{
    #define PORT 6166
    #define SERVER_PORT (PORT + 1)
	#define CLIENT_PORT (PORT + 2)
    #define IP "10.251.228.146"
	#define INTERVAL_OF_FRAME 30
	
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

	
    void Server::sendMsgToOther(const char* buf, int len, SOCKET sock)
    {
		for (size_t i = 0; i < userList.size(); ++i)
		{
			if (userList[i].isRegistered)
				sendto(sock, buf, len, 0, (sockaddr*)&userList[i].linkAddr, sizeof(sockaddr_in));
		}

    }
	
    void Server::resetState()
    {
        
    }



	// 服务器端的主循环。
	// 其作用是接收来自客户端发送的消息并将其拷贝到相应客户端的buf域
    // 如果超过5s没有接收到任何消息，那么就退出主循环
	void Server::mainLoop()
    {
		timeval timeout;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		fd_set rset;
		FD_ZERO(&rset);
		cmdLine.text(0, 5, "main loop start :");
        while(!isFinished)
        {
			sockaddr_in client;
			int size = sizeof(client);
			char buf[8] = {};

			FD_SET(serverSock, &rset);
			int  n = select(serverSock + 1, &rset, NULL, NULL, &timeout);
			if (n <= 0)	// 如果超过1秒没有接收到任何消息，那么就停止。
			{
				isFinished = true;
				break;
			}

			if (FD_ISSET(serverSock, &rset))
			{
				recvfrom(serverSock, buf, 8, 0, (sockaddr*)&client, &size);
				
				uint8_t id = static_cast<uint8_t>(buf[0]);

				// 注意，我们只能在这里记录客户端用于传输的套接字地址信息
				// 如果该用户还未记录过套接字地址信息，那么就进行记录
				if (!userList[id].logOfServerAddr)
				{
					userList[id].serverAddr = client;
					userList[id].logOfServerAddr = true;
				}

				if (userList[id].state == USER_IDLE)
				{
					userList[id].mutexOfBuf.lock();
					strcpy_s(userList[id].buf, buf);
					userList[id].mutexOfBuf.unlock();
				}
				FD_CLR(serverSock, &rset);
			}
			else
			{
				isFinished = true;
				break;
			}

			for (int i = 0; i < 8; ++i)
				buf[i] += '0';
			buf[7] = 0;
			cmdLine.text(12, 5, buf);


		//	userList[id].state = USER_SEND;
        }
		cmdLine.text(0, 5, "main loop end :");
    }

    // 这个函数的作用：每隔30ms（每帧）将所有用户的输入打包，然后发送给所有客户端
    void Server::pack()
    {
        
		while (!isFinished)
		{
			std::string buf;
			steady_clock::time_point start = steady_clock::now();
			char numOfUser = static_cast<char>(userList.size());
			buf += (numOfUser);
			for (size_t i = 0; i < userList.size(); ++i)
			{
				if (userList[i].isRegistered)
				{
					userList[i].mutexOfBuf.lock();
					buf += userList[i].buf;
					userList[i].mutexOfBuf.unlock();
				}
			}
			for (size_t i = 0; i < userList.size(); ++i)
			{
				if (userList[i].isRegistered)
				{
					sendto(serverSock, buf.c_str(), buf.length(), 0, (sockaddr*)&userList[i].serverAddr, sizeof(sockaddr_in));
				}
			}
			auto d = steady_clock::now() - start;
			Sleep(INTERVAL_OF_FRAME - std::chrono::duration_cast<milliseconds>(d).count());
		}

        
    }

    Server::Server()
    {
        serverSock = socket(AF_INET, SOCK_DGRAM, 0);
        memset(&name, 0, sizeof(name));
        name.sin_family = AF_INET;
        name.sin_port = htons(SERVER_PORT);
        name.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(serverSock, (sockaddr*)&name, sizeof(name));
		isFinished = false;
		
    }
    Server::~Server()
    {
        closesocket(serverSock);
    }

	// 建立服务器后，首先需要确定多少用户与其连接。
	// 另外，在此阶段，我们也需要同步所有已连接的用户。
    void Server::link()
    {
		cmdLine.text(0, 0, "server link:");


		SOCKET linkSock = socket(AF_INET, SOCK_DGRAM, 0);
		sockaddr_in tmpAddr;
		memset(&tmpAddr, 0, sizeof(tmpAddr));
		tmpAddr.sin_family = AF_INET;
		tmpAddr.sin_port = htons(PORT);
		tmpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		bind(linkSock, (sockaddr*)&tmpAddr, sizeof(tmpAddr));
        
		while(!isFinished)
        {
            sockaddr_in client;
			int size = sizeof(client);
			char buf[255] = {};
            recvfrom(linkSock, buf, 255, 0, (sockaddr*)&client, &size);
			std::cout << "server recv: " << inet_ntoa(client.sin_addr) << std::endl;
            std::cout << buf << std::endl;
            
			char id = userList.newObject();
			userList[id].isRegistered = true;
			userList[id].linkAddr = client;
			
			sendMsgToOther(&id, 1, linkSock);	// 将该用户信息同步到其他客户端
        }
		closesocket(linkSock);
		
    }

  
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

	Client::Client(sockaddr_in& server)
	{
		sockOfLink = socket(AF_INET, SOCK_DGRAM, 0);
		sockOfTrans = socket(AF_INET, SOCK_DGRAM, 0);

		memset(&serverAddr, 0, sizeof(serverAddr));
		memset(&linkAddr, 0, sizeof(linkAddr));
		serverAddr = linkAddr = server;
		serverAddr.sin_port = htons(SERVER_PORT);

	}
	int Client::sendMsg(const char* buf, int len)
	{
		return sendto(sockOfTrans, buf, len, 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
	}

	int Client::recvMsg(char* buf, int len)
	{
		return recvfrom(sockOfTrans, buf, len, 0, NULL, NULL);
	}

	// 与服务端建立连接，如果连接建立，那么用服务端返回的id创建游戏player。
	// 这里目前最多只能建立4个player
	// 如果游戏已经开始，那么将不再接收数据
	int Client::link()
	{
		std::string buf = "i want to link.\n";
		sendto(sockOfLink, buf.c_str(), buf.size(), 0, (sockaddr*)&linkAddr, sizeof(linkAddr));
		char buff[5];
		int cnt = 4;
		while (cnt--)
		{
			recvfrom(sockOfLink, buff, 5, 0, NULL, NULL);
			cmdLine.text(0, 20, "C Link:");
			cmdLine.text(9, 20, buff);
			tank_war::staticSystem.addPlayer(buff[0]);
		}
		return 0;
	}

    class NetManager 
    {
    public:
 //       NetManager();

        void createServer(std::string& name);
        bool tryGetServer();

		inline sockaddr_in* getServerAddr() 
		{ 
			sockaddr_in* tmp = NULL;
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


	void NetManager::createServer(std::string& name)
	{
		
		std::string buf = "i had create the server.\n";
		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(PORT);
		addr.sin_addr.s_addr = inet_addr(IP);
		int n = sendto(sock, buf.c_str(), buf.size(), 0, (sockaddr*)&addr, sizeof(addr));
		closesocket(sock);
		if (n < 0)
			abort();
	}


     /**
     * 这个函数的作用：定时25s 然后等待数据报的到来，最后写入 发送方的详细信息
     */ 
    bool NetManager::tryGetServer()
    {
        SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
        sockaddr_in name;
        memset(&name, 0, sizeof(name));
        name.sin_family = AF_INET;
        name.sin_port = htons(PORT);
        name.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(sock, (sockaddr*)&name, sizeof(name));
        char buf[255];

        fd_set rset;
        timeval timeout;
        timeout.tv_sec = 25;
        timeout.tv_usec = 0;
        FD_ZERO(&rset);
        FD_SET(sock, &rset);

        int n = select(sock +1, &rset, NULL, NULL, &timeout);
        if(n == 0)
        {
            return false;
        }
        else if(n > 0)
        {
            memset(&server, 0, sizeof(sockaddr_in));
			mutexOfServer.lock();
            recvfrom(sock, buf, 255, 0, (sockaddr*)&server, NULL);
			mutexOfServer.unlock();
			return true;
        }
        else
        {
            abort();
        }
        closesocket(sock);
    }

	static inline void tryGetServer(NetManager* m)
	{
		m->tryGetServer();
	}


}