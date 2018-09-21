#include "Controller.h"


namespace tank_war
{

	void StartUpOfThread::clientRecv(net::Client* c, CMSG* buf, volatile bool* isFinished, Controller* controller)
	{
		while (!*isFinished)
		{
			//buf->recvMutex.lock();
			c->recvMsg(buf->recvBuf, 33);
			//buf->recvMutex.unlock();
			controller->setStart();
		}
	}





	void Controller::createServer()
	{
		if (!isCreateServer)
		{
			isCreateServer = true;
			server = new net::Server();
			std::thread t{ StartUpOfThread::serverLink, server };
			
			threads[0].swap(t);
		}
		// 广播该地址
		manager.broadCastServer();
	}

	void Controller::clientLink()
	{
		if (!isClientLink)
		{
			isClientLink = true;

			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(PORT);
			addr.sin_addr = manager.getServerAddr()->sin_addr;
			client = new Client(addr);

			std::thread t{ StartUpOfThread::clientLink, client };

			uint8_t flag = 255;
			client->sendMsg((char*)(&flag), 1);
			// 放到这儿的原因：我们好像必须在这里等待游戏开始的消息。
			// 游戏开始信息由服务端用户发出。
			// 开始标记 client.recvfrom()堵塞返回。
			// 深层原因：服务器端首先开启serverPack()把当前的情况打包发送给客户端，说明已经开始了主循环。
			// 此时还未任何实用数据，正好作为游戏开始的标记
			std::thread t1{ StartUpOfThread::clientRecv, client, &msg, &isFinished, this };
			threads[1].swap(t);
			threads[4].swap(t1);
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
//				std::thread t1{ StartUpOfThread::clientRecv, client, &msg, &isFinished, this };
//				threads[4].swap(t1);
			}
		}
	}

	Controller::Controller(const char* ip)
	{
		
		client = NULL;
		server = NULL;
		isCreateServer = false;
		isClientLink = false;
		isGameStart = false;
		isFinished = false;

		std::thread tp{ StartUpOfThread::tryGetServer, &manager };
		threads[7].swap(tp);
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