#include "net.h"
#include "Controller.h"

namespace net
{
	void Server::sendMsgToOther(const char id, SOCKET sock)
	{
		char buff[5] = {0, };
		if (numofUser == 1)		// ���ϵͳ��ǰֻ�и��û�
		{
			buff[0] = 0;
			buff[1] = id;
			sendto(sock, buff, 5, 0, (sockaddr*)&(userList[id].linkAddr), sizeof(sockaddr_in));
			return;
		}
		else					// �����¼��ǰ���������û�id
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
		// �����û�ͬ���������ͻ���
		for (size_t i = 0; i < userList.size(); ++i)
		{
			if (userList[i].isRegistered && i != buff[1])
				sendto(sock, buff, 5, 0, (sockaddr*)&userList[i].linkAddr, sizeof(sockaddr_in));
		}

	}

	void Server::resetState()
	{

	}



	// �������˵���ѭ����
	// �������ǽ������Կͻ��˷��͵���Ϣ�����俽������Ӧ�ͻ��˵�buf��
	// �������5sû�н��յ��κ���Ϣ����ô���˳���ѭ��
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
			if (n <= 0)	// �������5��û�н��յ��κ���Ϣ����ô��ֹͣ��
			{
				isFinished = true;
				break;
			}

			if (FD_ISSET(serverSock, &rset))
			{
				recvfrom(serverSock, buf, 8, 0, (sockaddr*)&client, &size);

				uint8_t id = static_cast<uint8_t>(buf[0]);	// ��һ���ֽڱ�ʶ�û�id

				if (id == 255) continue;

															// ע�⣬����ֻ���������¼�ͻ������ڴ�����׽��ֵ�ַ��Ϣ
															// ������û���δ��¼���׽��ֵ�ַ��Ϣ����ô�ͽ��м�¼
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

	// ������������ã�ÿ��30ms��ÿ֡���������û�����������Ȼ���͸����пͻ���
	// ���ڴ�������ݣ�����������Ե�ÿ��user.buf��ĵ�һ���ֽڣ�����ֻ�����7���ֽ�
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

	// ������������������Ҫȷ�������û��������ӡ�
	// ���⣬�ڴ˽׶Σ�����Ҳ��Ҫͬ�����������ӵ��û���
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
			if (n < 0) break;	// ��������κν��մ����˳�ѭ��

			std::cout << "server link cur user port: [ " << client.sin_port << " ]\n";

			char id = userList.newObject();
			numofUser++;

			userList[id].isRegistered = true;
			userList[id].linkAddr = client;

			sendMsgToOther(id, linkSock);	// �����û���Ϣͬ���������ͻ���
			// �����߼����ԡ�
			// ÿ�����û�����ʱ���ѵ�ǰ�����û���Ϣͬ�������û���
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

	// �����˽������ӣ�������ӽ�������ô�÷���˷��ص�id������Ϸplayer��
	// ����Ŀǰ���ֻ�ܽ���4��player
	// �����Ϸ�Ѿ���ʼ����ô�����ٽ�������
	int Client::link()
	{
		std::string buf = "i want to link.\n";
		sendto(sockOfLink, buf.c_str(), buf.size(), 0, (sockaddr*)&linkAddr, sizeof(linkAddr));
		
		char buff[5];
		int cnt = 4;
		while (cnt--)
		{
			int n = recvfrom(sockOfLink, buff, 5, 0, NULL, NULL);
			if (n < 0)	// ��������κν��մ�����ô�˳�ѭ��
				break;
			// �߼����⡣�ȴ��޸� 
			if (buff[0] == 0)
			{
				// ��������ͻ���ϵͳ��ע����û�
				tank_war::staticControllerHandle().setUserId(buff[1]);
				tank_war::staticControllerHandle().setPlayerId(tank_war::staticSysyemHandle().addPlayer(buff[1]));
			}
			else if(buff[0] == 1)	// ˵����idΪ�����û���ͬ��
			{ 
				tank_war::staticSysyemHandle().addPlayer(buff[1]);
			}
			else	// ˵��Ϊ����ע�ᣬ���������û���֮ǰ�Ѿ�ע����ˣ�ͬ�������û� 
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
	* ������������ã���ʱ25s Ȼ��ȴ����ݱ��ĵ��������д�� ���ͷ�����ϸ��Ϣ
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

		int n = select(sock + 1, &rset, NULL, NULL, &timeout);
		if (n == 0)
		{
			return false;
		}
		else if (n > 0)
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
		return closesocket(sock);
	}

	static inline void tryGetServer(NetManager* m)
	{
		m->tryGetServer();
	}



}