#pragma once
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

namespace net
{
    #define PORT 6166
    #define SERVER_PORT (PORT + 1)
    #define IP "192.168.43.255"

    struct User
    {
        // int score;
        // int lifes;
        // int joypadId;
        // int tankId;
        sockaddr_in addr;
        char buf[255];
    };


    enum EUserState
    {
        USER_IDLE,
        USER_SEND
    };

    class Server
    {
    public:
        Server();
        ~Server();

        void link();

        // 
        void pack();
        void resetState();

        void mainLoop();

        void sendMsgToOther(const uint8_t id);


        inline void start() { isFinished = true; }

    private:
        SOCKET serverSock;
        sockaddr_in name;
        bool isFinished;
        std::vector<User> userList;
        std::vector<EUserState> stateList;
        bool isfinished; 
    };

    void Server::sendMsgToOther(const uint8_t id)
    {
        User* user = &userList[id];
        for(size_t i = 0; i < userList.size(); ++i)
        {
            if(i == id) continue;
            sendto(serverSock, user->buf, sizeof(user->buf), 0, (sockaddr*)&user->addr, sizeof(sockaddr_in));
        }
    }
    void Server::resetState()
    {
        for(auto& state : stateList)
            state = USER_IDLE;
    }

    void Server::mainLoop()
    {
        while(!isfinished)
        {
            char buf[255];
            recvfrom(serverSock, buf, 255, 0, NULL, NULL);
            uint8_t id = static_cast<uint8_t>(buf[0]);
            if(stateList[id] == USER_IDLE)
                strcpy(userList[id].buf, buf);
            stateList[id] = USER_SEND;
        }
    }

    //
    void Server::pack()
    {
        std::string buf;
        char numOfUser = static_cast<char>(userList.size());
        buf += (numOfUser);
        for(auto& user : userList)
        {
            buf += user.buf;
        }
        for(auto& user : userList)
        {
            sendto(serverSock, buf.c_str(), buf.length(), 0, (sockaddr*)&user.addr, sizeof(sockaddr_in));
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
    }
    Server::~Server()
    {
        closesocket(serverSock);
    }

    void Server::link()
    {
        std::cout << "link: \n";
        while(!isFinished)
        {
            sockaddr_in client;
            int size = sizeof(client);
            char buf[255];
            recvfrom(serverSock, buf, 255, 0, (sockaddr*)&client, &size);
            std::cout << buf;
            userList.emplace_back(User());
            char id = userList.size() -1;
            userList[id].addr = client;
            int n = sendto(serverSock, &id, sizeof(id), 0, (sockaddr*)&client, size);
            if(n < 0)
                abort();
        }
        
    }

  
    class Client
    {
    private:

    };


    class NetManager 
    {
    public:
 //       NetManager();

        void createServer(std::string& name);
        bool tryGetServer();

		inline sockaddr_in& getServerAddr() { return server; }
    private:
        sockaddr_in server;
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
            recvfrom(sock, buf, 255, 0, (sockaddr*)&server, NULL);
            return true;
        }
        else
        {
            abort();
        }
        closesocket(sock);
    }

}