#pragma once
#include <WinSock2.h>
#include <vector>

#include "net.h"
#include "UI.h"
#include "CmdWindow.h"


namespace UI
{

	struct STransition
	{
		bool isValid;
		CState* nextState;
	};

	class CState
	{
	public:
		virtual void control(const char* joypadState);
		virtual void update();

		virtual void enter();
		virtual void exit();
		std::vector<STransition> transitions;
	};

	class CStartState : public CState
	{
	public:
		CStartState();
		void control(const char* joypadState);
		void update();
	private:
		int selectId;
		GameUI* gameui;
	};

	class CExitState : public CState
	{

	};

	class CServerListState : public CState
	{
	public:
		CServerListState();
		~CServerListState();
		void control(const char* joypadState);
		void update();
		void enter();
	private:
		std::vector<sockaddr_in> serverList;
		GameUI* gameui;
		net::NetManager* manager;
	};

	class CRoomState : public CState
	{

	};

	class CGameState : public CState
	{

	};



	CStartState::CStartState()
		: selectId(0), gameui(&staticGameUI)
	{	}
	void CStartState::control(const char* joypadState)
	{
		if (joypadState[1] == 1 || joypadState[3] == 1)
		{
			selectId = (selectId - 1) % 3;
		}
		else if (joypadState[2] == 1 || joypadState[4] == 1)
		{
			selectId = (selectId + 1) % 3;
		}
		else if (joypadState[5] == 1)
		{
			this->transitions[selectId].isValid = true;
		}
		else
		{
			this->transitions[2].isValid = true;
		}
	}
	void CStartState::update()
	{
		gameui->onChange(selectId);
		char* bmp = gameui->getBmp();
		for (size_t i = 0; i < 20; ++i)
			for (size_t j = 0; j < 20; ++j)
				staticWindowHandle().drawChar(20 + j, 10 + i, bmp[i * 20 + j]);
	}


	CServerListState::CServerListState()
		: serverList(), gameui(&staticGameUI)
	{
		manager = new net::NetManager();
	}
	CServerListState::~CServerListState()
	{
		if (manager)
			delete manager;
	}
	// 选择列表中的服务ip并且尝试连接
	// 但是目前我们只有一个服务ip。。。
	// 所以，没得选。
	void CServerListState::control(const char* joypadState)
	{
		
	}

	void CServerListState::enter()
	{
		manager->tryGetServer();
	}
	
	class CFSM
	{
	public:
		CFSM();
		~CFSM();
		void update();
	private:
		std::vector<CState* > states;
		CState* curState;
		CState* initState;
	};

	CFSM::CFSM()
	{
		states.emplace_back(new CStartState());
		states.emplace_back(new CRoomState());
		states.emplace_back(new CServerListState());
		states.emplace_back(new CExitState());
		states.emplace_back(new CGameState());

		states[0]->transitions.emplace_back(STransition{ false, states[1] });
		states[0]->transitions.emplace_back(STransition{ false, states[2] });
		states[0]->transitions.emplace_back(STransition{ false, states[3] });

		states[1]->transitions.emplace_back(STransition{ false, states[4] });
		states[1]->transitions.emplace_back(STransition{ false, states[0] });
		
		states[2]->transitions.emplace_back(STransition{ false, states[1] });
		states[2]->transitions.emplace_back(STransition{ false, states[0] });
		
		states[4]->transitions.emplace_back(STransition{ false, states[1] });

		initState = states[0];
	}
	CFSM::~CFSM()
	{
		for (auto& state : states)
		{
			if (state)
				delete state;
		}
	}

	void CFSM::update()
	{

	}

}
