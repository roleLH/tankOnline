#pragma once
#include "net.h"
#include "System.h"

using namespace::net;


namespace tank_war
{

#define SDLK_w 119
#define SDLK_s 115
#define SDLK_a 97
#define SDLK_d 100
#define SDLK_j 106
#define SDLK_k 107

#define SDLK_4 52
#define SDLK_5 53
#define SDLK_6 54


	// so 我们需要发送和接收的信息全部存储在这里面
	class CMSG
	{
	public:

		char sendBuf[8];			// 需要发送的字节
		char recvBuf[33];			// 需要接收的字节
		std::mutex recvMutex;		// 

		CMSG() : sendBuf{}, recvBuf{}
		{}

		void reset()
		{
			sendBuf[2] = sendBuf[3] = sendBuf[4] = sendBuf[5] = sendBuf[6] = sendBuf[7] = 0;
			recvMutex.lock();
			memset(recvBuf, 0, 33);
			recvMutex.unlock();
		}
	};

	class Controller;

	// 一些线程的启动函数，因为太琐碎，所以把他们放到一块。
	// PS：编译器实际上还是会分开。。。。
	struct StartUpOfThread
	{
		static void serverLink(net::Server* s) { s->link(); }
		static void serverMainLoop(net::Server* s) { s->mainLoop(); }
		static void serverPack(net::Server* s) { s->pack(); }

		static void clientLink(net::Client* c) { c->link(); }

		// 注意，这也是一个死循环，我们会在循环中修改指针指向的值
		// 这是一个危险的操作，for me。
		static void clientRecv(net::Client* c, CMSG* buf, volatile bool* isFinished, Controller* controller);
	};


	// 网络传送的数据与游戏逻辑在这里控制
	class Controller
	{
	public:
		// 参数ip 是客户端尝试与该ip建立连接
		Controller(const char* ip = IP);
		~Controller();
		inline char* getSendBuf() const { return (char*)(msg.sendBuf); }
		inline char* getRecvBuf() const { return (char*)(msg.recvBuf); }
		inline void resetMsg() { msg.reset(); }

		/* \note 非常神奇的操作。 尝试了一些方法。
		 * 因为发送数据包需要playerid 信息,我们不得不把它记录下来
		 * 但是我们接收信息的线程暂时没有办法返回。。。。
		 * 在这里给出的解决方案就是这样：把controller设置为全局变量，然后在接收线程（link）中直接设置对应域
		 * 可参见 net::Client::link()
		*/
		inline void setPlayerId(const char playerId) { msg.sendBuf[1] = playerId; }
		inline void setUserId(const char userId) { msg.sendBuf[0] = userId; }
		// 获取本用户的playerId
		inline const int getPlayerId() const { return msg.sendBuf[1]; }
		// 获取本用户的userId
		inline const int getUserId() const { return msg.sendBuf[0]; }

		inline void sendMsg() { client->sendMsg(msg.sendBuf, 8); }

		inline bool isStart() const { return isGameStart.load(); }
		inline void setStart() { isGameStart = true; }
		// 服务器控制
		void createServer();
		// 服务器控制 游戏开始时建立整个消息（数据包）循环
		void gameLoop();

		// 客户端控制
		void clientLink();
		inline void finish() { isFinished = true; server->finish(); }

		// 我们把来自键盘输入的判断放在这里
		inline void keySwitch(int keyCode)
		{
			char* buf = msg.sendBuf;
			switch (keyCode)
			{
			case SDLK_w: buf[2] = 1; break;
			case SDLK_a: buf[4] = 1; break;
			case SDLK_s: buf[3] = 1; break;
			case SDLK_d: buf[5] = 1; break;

			case SDLK_j: buf[6] = 1; break;
			case SDLK_k: buf[7] = 1; break;

			case SDLK_4: createServer(); break;
			case SDLK_5: clientLink(); break;
			case SDLK_6: gameLoop(); break;
			default: break;
			}
		}

	private:
		net::Server* server;
		net::Client* client;
		bool isCreateServer;	// 是否创建服务器的标记
		bool isClientLink;		// 客户端是否尝试连接服务器。warning 这里没有失败检查
		std::atomic_bool isGameStart;		// 游戏是否开始的标记
		std::thread threads[8]; // 线程统一管理
		CMSG msg;
		volatile bool isFinished;	// 标识游戏是否结束的标记
		 
	};


	Controller& staticControllerHandle();
}