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


// so ������Ҫ���ͺͽ��յ���Ϣȫ���洢��������
struct SMSG
{
	char sendBuf[8];			// ��Ҫ���͵��ֽ�
	char recvBuf[32];			// ��Ҫ���յ��ֽ�
	std::atomic_bool recvFlag;  // �������Ϊtrueʱ�����ǲ�ȥ��ȡrecvBuf�����ѱ����Ϊfalse
};




// һЩ�̵߳�������������Ϊ̫���飬���԰����Ƿŵ�һ�顣
// PS��������ʵ���ϻ��ǻ�ֿ���������
struct StartUpOfThread
{
	static void serverLink(Server* s)	{	s->link(); }
	static void serverMainLoop(Server* s){  s->mainLoop(); }
	static void serverMainLoop(Server* s) { s->mainLoop(); }
	static void serverPack(Server* s) { s->pack();}

	static void clientLink(Client* c) { c->link();}

	// ע�⣬��Ҳ��һ����ѭ�������ǻ���ѭ�����޸�ָ��ָ���ֵ
	// ����һ��Σ�յĲ�����for me��
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
