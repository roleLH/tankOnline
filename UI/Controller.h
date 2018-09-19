#pragma once
#include "net.h"
#include "System.h"

using namespace::net;


namespace tank_war
{
	
	// so ������Ҫ���ͺͽ��յ���Ϣȫ���洢��������
	class CMSG
	{
	public:

		char sendBuf[8];			// ��Ҫ���͵��ֽ�
		char recvBuf[33];			// ��Ҫ���յ��ֽ�
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

	// һЩ�̵߳�������������Ϊ̫���飬���԰����Ƿŵ�һ�顣
	// PS��������ʵ���ϻ��ǻ�ֿ���������
	struct StartUpOfThread
	{
		static void serverLink(net::Server* s) { s->link(); }
		static void serverMainLoop(net::Server* s) { s->mainLoop(); }
		static void serverPack(net::Server* s) { s->pack(); }

		static void clientLink(net::Client* c) { c->link(); }

		// ע�⣬��Ҳ��һ����ѭ�������ǻ���ѭ�����޸�ָ��ָ���ֵ
		// ����һ��Σ�յĲ�����for me��
		static void clientRecv(net::Client* c, CMSG* buf, volatile bool* isFinished, Controller* controller);
	};


	// ���紫�͵���������Ϸ�߼����������
	class Controller
	{
	public:
		// ����ip �ǿͻ��˳������ip��������
		Controller(const char* ip = IP);
		~Controller();
		inline char* getSendBuf() const { return (char*)(msg.sendBuf); }
		inline char* getRecvBuf() const { return (char*)(msg.recvBuf); }
		inline void resetMsg() { msg.reset(); }

		/* \note �ǳ�����Ĳ����� ������һЩ������
		 * ��Ϊ�������ݰ���Ҫplayerid ��Ϣ,���ǲ��ò�������¼����
		 * �������ǽ�����Ϣ���߳���ʱû�а취���ء�������
		 * ����������Ľ������������������controller����Ϊȫ�ֱ�����Ȼ���ڽ����̣߳�link����ֱ�����ö�Ӧ��
		 * �ɲμ� net::Client::link()
		*/
		inline void setPlayerId(const char playerId) { msg.sendBuf[1] = playerId; }
		inline void setUserId(const char userId) { msg.sendBuf[0] = userId; }

		inline void sendMsg() { client->sendMsg(msg.sendBuf, 8); }

		inline bool isStart() const { return isGameStart.load(); }
		inline void setStart() { isGameStart = true; }
		// ����������
		void createServer();
		// ���������� ��Ϸ��ʼʱ����������Ϣ�����ݰ���ѭ��
		void gameLoop();

		// �ͻ��˿���
		void clientLink();
		inline void finish() { isFinished = true; server->finish(); }

	private:
		net::Server* server;
		net::Client* client;
		bool isCreateServer;	// �Ƿ񴴽��������ı��
		bool isClientLink;		// �ͻ����Ƿ������ӷ�������warning ����û��ʧ�ܼ��
		std::atomic_bool isGameStart;		// ��Ϸ�Ƿ�ʼ�ı��
		std::thread threads[8]; // �߳�ͳһ����
		CMSG msg;
		volatile bool isFinished;	// ��ʶ��Ϸ�Ƿ�����ı��
		 
	};


	Controller& staticControllerHandle();
}