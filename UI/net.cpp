#include "net.h"
#include "Controller.h"

namespace net
{
	void Server::sendMsgToOther(const char id, SOCKET sock)
	{
		char buff[5] = {0, };
		if (numofUser == 1)		// 如果系统当前只有该用户
		{
			buff[0] = 0;
			buff[1] = id;
			sendto(sock, buff, 5, 0, (sockaddr*)&(userList[id].linkAddr), sizeof(sockaddr_in));
			return;
		}
		else					// 否则记录当前所有其他用户id
		{
			buff[0] = numofUser;
			buff[1] = id;
			int index = 2;
			for (size_t i = 0; i < userList.size(); ++i)
			{
				if (userList[i].isRegistered && i != buff[1])
				{
					buff[index++] = i;
				}
			}
			sendto(sock, buff, 5, 0, (sockaddr*)&(userList[id].linkAddr), sizeof(sockaddr_in));
		}

		buff[0] = 1;
		// 将该用户同步到其他客户端
		for (size_t i = 0; i < userList.size(); ++i)
		{
			if (userList[i].isRegistered && i != buff[1])
				sendto(sock, buff, 5, 0, (sockaddr*)&userList[i].linkAddr, sizeof(sockaddr_in));
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
		timeout.tv_sec = 500;
		timeout.tv_usec = 0;
		fd_set rset;
		FD_ZERO(&rset);
		while (!isFinished)
		{
			sockaddr_in client;
			int size = sizeof(client);
			char buf[8] = {};

			FD_SET(serverSock, &rset);
			int  n = select(serverSock + 1, &rset, NULL, NULL, &timeout);
			if (n <= 0)	// 如果超过5秒没有接收到任何消息，那么就停止。
			{
				isFinished = true;
				break;
			}

			if (FD_ISSET(serverSock, &rset))
			{
				recvfrom(serverSock, buf, 8, 0, (sockaddr*)&client, &size);

				uint8_t id = static_cast<uint8_t>(buf[0]);	// 第一个字节标识用户id

				if (id == 255) continue;

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
					for (int i = 0; i < 8; ++i)
						userList[id].buf[i] = buf[i];
					userList[id].mutexOfBuf.unlock();
				}
				FD_CLR(serverSock, &rset);
			}
			else
			{
				isFinished = true;
				break;
			}

			//	userList[id].state = USER_SEND;
		}
		std::cout << "main loop quit ??! \n";
	}

	// 这个函数的作用：每隔30ms（每帧）将所有用户的输入打包，然后发送给所有客户端
	// 关于打包的数据，我们这里忽略掉每个user.buf域的第一个字节，我们只传输后7个字节
	void Server::pack()
	{

		while (!isFinished)
		{
			char buf[33] = { 0, };
			steady_clock::time_point start = steady_clock::now();

			for (size_t i = 0; i < userList.size(); ++i)
			{
				if (userList[i].isRegistered)
				{

					userList[i].mutexOfBuf.lock();
					for (size_t k = 0; k < 8; ++k)
					{
						buf[buf[0] * 8 + k +1] = userList[i].buf[k];
					}
					userList[i].mutexOfBuf.unlock();
					buf[0]++;

				}
			}

			for (size_t i = 0; i < userList.size(); ++i)
			{
				if (userList[i].isRegistered)
				{
					sendto(serverSock, buf, (buf[0] * 8 + 1), 0, (sockaddr*)&userList[i].serverAddr, sizeof(sockaddr_in));
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

		numofUser = 0;

	}
	Server::~Server()
	{
		closesocket(serverSock);
	}

	// 建立服务器后，首先需要确定多少用户与其连接。
	// 另外，在此阶段，我们也需要同步所有已连接的用户。
	void Server::link()
	{

		SOCKET linkSock = socket(AF_INET, SOCK_DGRAM, 0);
		sockaddr_in tmpAddr;
		memset(&tmpAddr, 0, sizeof(tmpAddr));
		tmpAddr.sin_family = AF_INET;
		tmpAddr.sin_port = htons(PORT);
		tmpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		bind(linkSock, (sockaddr*)&tmpAddr, sizeof(tmpAddr));

		while (!isFinished)
		{
			sockaddr_in client;
			int size = sizeof(client);
			char buf[255] = {};
			int n = recvfrom(linkSock, buf, 255, 0, (sockaddr*)&client, &size);
			if (n < 0) break;	// 如果出现任何接收错误，退出循环

			std::cout << "server link cur user port: [ " << client.sin_port << " ]\n";

			char id = userList.newObject();
			numofUser++;

			userList[id].isRegistered = true;
			userList[id].linkAddr = client;

			sendMsgToOther(id, linkSock);	// 将该用户信息同步到其他客户端
			// 这里逻辑不对。
			// 每当有用户连接时，把当前所有用户信息同步到该用户。
		}
		closesocket(linkSock);

	}

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
			int n = recvfrom(sockOfLink, buff, 5, 0, NULL, NULL);
			if (n < 0)	// 如果出现任何接收错误，那么退出循环
				break;
			// 逻辑问题。等待修改 
			if (buff[0] == 0)
			{
				// 在这里向客户端系统中注册该用户
				tank_war::staticControllerHandle().setUserId(buff[1]);
				tank_war::staticControllerHandle().setPlayerId(tank_war::staticSysyemHandle().addPlayer(buff[1]));
			}
			else if(buff[0] == 1)	// 说明此id为其他用户的同步
			{ 
				tank_war::staticSysyemHandle().addPlayer(buff[1]);
			}
			else	// 说明为初次注册，但有其他用户在之前已经注册过了，同步其他用户 
			{
				tank_war::staticControllerHandle().setUserId(buff[1]);
				tank_war::staticControllerHandle().setPlayerId(tank_war::staticSysyemHandle().addPlayer(buff[1]));
				int index = 2;
				for (int i = buff[0] - 1; i > 0; --i)
				{
					tank_war::staticSysyemHandle().addPlayer(buff[index++]);
				}
			}
		}
		return 0;
	}



	// 此函数的作用：向局域网内的其他机器广播该机器的ip地址，
	// 以说明该机器已经创建服务器并且等待连接。
	// PS: 广播只有一次。
	void NetManager::broadCastServer()
	{

		std::string buf = "i had create the server.\n";
		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(8887);
		addr.sin_addr.s_addr = inet_addr("255.255.255.255");

		bool flag = true;
		setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&flag, sizeof(flag));

		int n = sendto(sock, buf.c_str(), buf.size(), 0, (sockaddr*)&addr, sizeof(addr));
		closesocket(sock);
		if (n < 0)
			abort();
	}


	/**
	* 这个函数的作用：定时255s 然后等待数据报的到来，最后写入 发送方的详细信息
	*/
	bool NetManager::tryGetServer()
	{
		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
		sockaddr_in name;
		memset(&name, 0, sizeof(name));
		name.sin_family = AF_INET;
		name.sin_port = htons(8887);
		name.sin_addr.s_addr = htonl(INADDR_ANY);
		bind(sock, (sockaddr*)&name, sizeof(name));
		char buf[255];

		fd_set rset;
		timeval timeout;
		timeout.tv_sec = 255;
		timeout.tv_usec = 0;
		FD_ZERO(&rset);
		FD_SET(sock, &rset);

		int n = select(sock + 1, &rset, NULL, NULL, &timeout);
		if (n == 0)
		{
			return false;
		}
		else if (n > 0)
		{
			memset(&server, 0, sizeof(sockaddr_in));
			int size = sizeof(server);
			mutexOfServer.lock();
			recvfrom(sock, buf, 255, 0, (sockaddr*)&server, &size);
			mutexOfServer.unlock();
			return true;
		}
		else
		{
			abort();
		}
		std::cout << "i get the server addr !!!\n";
		return closesocket(sock);
	}


}