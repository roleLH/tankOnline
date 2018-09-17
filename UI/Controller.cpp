#include "Controller.h"

namespace tank_war
{
	void Controller::createServer()
	{
		if (!isCreateServer)
		{
			isCreateServer = true;
			server = new net::Server();
			std::thread t{ StartUpOfThread::serverLink, server };
			threads[0].swap(t);
		}
	}

	void Controller::clientLink()
	{
		if (!isClientLink)
		{
			isClientLink = true;
			std::thread t{ StartUpOfThread::clientLink, client };
			threads[1].swap(t);
		}
	}

	void Controller::gameLoop()
	{
		if (!isGameStart)
		{
			isGameStart = true;
			if (isCreateServer)	// 如果开启服务器
			{
				std::thread t1{ StartUpOfThread::serverMainLoop, server };
				std::thread t2{ StartUpOfThread::serverPack, server };
				threads[2].swap(t1);
				threads[3].swap(t2);
			}
			if (isClientLink) // 如果客户端已连接
			{
				std::thread t{ StartUpOfThread::clientRecv, client, &msg, 32, &isFinished };
				threads[4].swap(t);
			}
		}
	}

	Controller::Controller(const char* ip)
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(PORT);
		addr.sin_addr.s_addr = inet_addr(ip);
		client = new net::Client(addr);
		server = NULL;
		isCreateServer = false;
		isClientLink = false;
		isGameStart = false;
		isFinished = false;

	}

	Controller::~Controller()
	{
		for (auto& thread : threads)
		{
			if (thread.joinable())
				thread.join();
		}
		if (client)
			delete client;
		if (server)
			delete server;
	}

	Controller& staticControllerHandle()
	{
		static Controller staticController;
		return staticController;
	}

}