#define SDL_MAIN_HANDLED
#include <iostream>

#include "UI.h"
#include "System.h"
#include "CmdWindow.h"
#include "net.h"

#include <thread>
#include <mutex>
using namespace::UI;
using namespace::tank_war;
using namespace::net;


// so 我们需要发送和接收的信息全部存储在这里面
struct SMSG
{
	char sendBuf[8];			// 需要发送的字节
	char recvBuf[32];			// 需要接收的字节
	//std::mutex recvMutex;		// 

	SMSG() : sendBuf{}, recvBuf{} 
	{}

	void reset()
	{
		sendBuf[2] = sendBuf[3] = sendBuf[4] = sendBuf[5] = sendBuf[6] = sendBuf[7];
		for (size_t i = 0; i < 32; ++i)
		{
			recvBuf[i] = 0;
		}
	}
};




// 一些线程的启动函数，因为太琐碎，所以把他们放到一块。
// PS：编译器实际上还是会分开。。。。
struct StartUpOfThread
{
	static void serverLink(Server* s)	{	s->link(); }
	static void serverMainLoop(Server* s) { s->mainLoop(); }
	static void serverPack(Server* s) { s->pack();}

	static void clientLink(Client* c) { c->link();}
	
	// 注意，这也是一个死循环，我们会在循环中修改指针指向的值
	// 这是一个危险的操作，for me。
	static void clientRecv(Client* c, SMSG* buf, int len, bool* isFinished)
	{
		while (!*isFinished)
		{
//			buf->recvMutex.lock();
			c->recvMsg(buf->recvBuf, 32);
//			buf->recvMutex.unlock();
		}
	}
};


int main(int argc, char** argv)
{
	
	WSAData data;
	WSAStartup(MAKEWORD(2, 2), &data);

	SDL_Init(SDL_INIT_EVENTS);

	bool isFinished = false;

	// 是否已经创建过的标记
	bool isCreateServer = false;
	bool isCreateClient = false;
	bool isGameStart = false;

	SMSG msg;
	msg.sendBuf[0] = 3;
	msg.sendBuf[1] = 3;

	SDL_Event e;

	Server server;
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(IP);
	Client client(addr);

	std::vector<std::thread> threads(8);

	auto oldTime = steady_clock::now();

	while (!isFinished)
	{
		if (SDL_PollEvent(&e))
		{

			if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_w:
					msg.sendBuf[2] = 1;
					break;
				case SDLK_a:
					msg.sendBuf[4] = 1;
					break;
				case SDLK_s:
					msg.sendBuf[3] = 1;
					break;
				case SDLK_d:
					msg.sendBuf[5] = 1;
					break;

				case SDLK_4:
					if (!isCreateServer)
					{
						isCreateServer = true;
						std::thread t{ StartUpOfThread::serverLink, &server };
						threads[0].swap(t);
					}
					break;
				case SDLK_5:
					if (!isCreateClient)
					{
						isCreateClient = true;
						std::thread t{ StartUpOfThread::clientLink, &client };
						threads[3].swap(t);
					}
					break;
				case SDLK_6:
					if (!isGameStart)
					{
						isGameStart = true;
						std::thread t2{ StartUpOfThread::serverMainLoop, &server };
						std::thread t3{ StartUpOfThread::serverPack, &server };


						std::thread t{ StartUpOfThread::clientRecv, &client, &msg, 32, &isFinished };
						threads[4].swap(t);
						threads[1].swap(t2);
						threads[2].swap(t3);
					}
					break;
				default:
					break;
				}
			}
			else if (e.type == SDL_QUIT)
			{
				isFinished = true;
				break;
			}
				
		}
		else
		{
			auto newTime = steady_clock::now();
			auto d = newTime - oldTime;
			if (duration_cast<milliseconds>(d).count() >= 30)
			{
				oldTime = newTime;
				if (isGameStart)
				{
					client.sendMsg(msg.sendBuf, 8);
					tank_war::staticSystem.control(msg.recvBuf);
					msg.reset();
					tank_war::staticSystem.update();
				}

				for (int i = 0; i < MAP_HEIGHT; ++i)
				{
					for (int j = 0; j < MAP_WIDTH; ++j)
					{
						cmdLine.drawChar(i, j, map[i][j]);
					}
				}
				
			}
		}
		cmdLine.update();
	}

	//for (auto& thread : threads)
	//	thread.join();

	WSACleanup();
	return 0;
}
