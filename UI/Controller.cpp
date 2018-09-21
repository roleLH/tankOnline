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
		// �㲥�õ�ַ
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
			// �ŵ������ԭ�����Ǻ������������ȴ���Ϸ��ʼ����Ϣ��
			// ��Ϸ��ʼ��Ϣ�ɷ�����û�������
			// ��ʼ��� client.recvfrom()�������ء�
			// ���ԭ�򣺷����������ȿ���serverPack()�ѵ�ǰ�����������͸��ͻ��ˣ�˵���Ѿ���ʼ����ѭ����
			// ��ʱ��δ�κ�ʵ�����ݣ�������Ϊ��Ϸ��ʼ�ı��
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
			if (isCreateServer)	// �������������
			{
				std::thread t1{ StartUpOfThread::serverMainLoop, server };
				std::thread t2{ StartUpOfThread::serverPack, server };
				threads[2].swap(t1);
				threads[3].swap(t2);
			}
			if (isClientLink) // ����ͻ���������
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