#define SDL_MAIN_HANDLED
#include <iostream>

#include "UI.h"
#include "System.h"
#include "CmdWindow.h"
#include "net.h"

#include <thread>
#include <atomic>
using namespace::UI;
using namespace::tank_war;
using namespace::net;


// so 我们需要发送和接收的信息全部存储在这里面
struct SMSG
{
	char sendBuf[8];			// 需要发送的字节
	char recvBuf[32];			// 需要接收的字节
	std::atomic_bool recvFlag;  // 仅当标记为true时，我们才去读取recvBuf，并把标记设为false
};




// 一些线程的启动函数，因为太琐碎，所以把他们放到一块。
// PS：编译器实际上还是会分开。。。。
struct StartUpOfThread
{
	static void serverLink(Server* s)	{	s->link(); }
	static void serverMainLoop(Server* s){  s->mainLoop(); }
	static void serverMainLoop(Server* s) { s->mainLoop(); }
	static void serverPack(Server* s) { s->pack();}

	static void clientLink(Client* c) { c->link();}

	// 注意，这也是一个死循环，我们会在循环中修改指针指向的值
	// 这是一个危险的操作，for me。
	static void clientRecv(Client* c, SMSG* buf, int len, std::atomic<bool*> isFinished)
	{
		while (!*isFinished)
		{
			c->recvMsg(buf->recvBuf, 32);
			buf->recvFlag = true;
		}
	}
};


int main(int argc, char** argv)
{
	
	WSAData data;
	WSAStartup(MAKEWORD(2, 2), &data);

	SDL_Init(SDL_INIT_EVENTS);

	bool isFinished = false;

	char buf[8] = {0, };
	char s = 0, c = 0, ss = 0;
	SDL_Event e;

	Server server;
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(IP);
	Client client(addr);


	while (!isFinished)
	{
		if (SDL_PollEvent(&e))
		{
			if (e.type == SDL_KEYUP)
			{
				
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_w :
					buf[2] = 1;	break;
				case SDLK_a :
					buf[4] = 1; break;
				case SDLK_s : 
					buf[3] = 1; break;
				case SDLK_d :
					buf[5] = 1; break;
				case SDLK_z:
					s = 1; break;
				case SDLK_x:
					c = 1;	break;
				case SDLK_c:
					ss = 1;

				default:
					break;
				}
			}
			else if (e.type == SDL_QUIT)
				break;
		}
		
		cmdLine.update();

	//	resetMap();
	//	SDL_Delay(33);
	}

	WSACleanup();
	return 0;
}
