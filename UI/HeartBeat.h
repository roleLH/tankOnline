#pragma once

#include <WinSock2.h>
#include <vector>
#include <atomic>
#include <mutex>
#include <assert.h>
#include "util.h"

namespace net
{
    
    struct UserState
    {
       
        std::atomic<int> cnt;
        UserState* next;
        UserState()
            : cnt{0}, next{NULL}
            {   }
        UserState(int id)
            : cnt{0}, next{NULL}
            {   }
    };

    struct UserStateHead
    {
        UserState* head;
        UserState* tail;
        std::mutex mutexOfHead;

    };

    class HeartBeatManager
    {
    public:
        HeartBeatManager(int sec, int th);
        ~HeartBeatManager();

        void heartBeat();
        inline bool isAlive(int userStateId) { return userStates[userStateId].cnt < threshold; }
		inline void beat(int userStateId) { userStates[userStateId].cnt = 0; }
		inline void finished() { isFinished = true; }
    private:
        void remove(UserState* node);   // 移除此节点的下一个节点
        void linked(int pos1, int pos2);    // 将pos1位置的链表连接至pos2的末尾

        int second;     // 每隔second秒一次心跳
        int threshold;  // 当 cnt >= threshold 时，认为连接断开
        std::atomic_bool isFinished;
        List<UserState> userStates;
        std::vector<UserStateHead> timeWheel;
    };

    HeartBeatManager::HeartBeatManager(int sec, int th)
    {
        second = sec;
        threshold = th;
        isFinished = false;
        timeWheel.resize(second << 1);
    }
	HeartBeatManager::~HeartBeatManager()
	{
		finished();
	}

    void HeartBeatManager::remove(UserState* node)
    {
        assert(node);
        UserState* p = node->next;
        node->next = p->next;
        p->next = NULL;
    }

    void HeartBeatManager::linked(int pos1, int pos2)
    {
        if(timeWheel[pos2].head == NULL)
        {
			timeWheel[pos2].head = timeWheel[pos1].head;
        }
		else
		{
			timeWheel[pos2].tail->next = timeWheel[pos1].head;
		}
		timeWheel[pos2].tail = timeWheel[pos1].tail;
    }

    void HeartBeatManager::heartBeat()
    {
        int curPos = 0;
        while(!isFinished)
        {
            timeval timeout;
            timeout.tv_sec = second;
            select(1, NULL, NULL, NULL, &timeout);
            UserState* cur = timeWheel[curPos].head;

            timeWheel[curPos].mutexOfHead.lock();
            for(cur; cur != NULL; cur = cur->next)
                cur->cnt++;
			timeWheel[curPos].mutexOfHead.unlock();

			// 这里可能会有问题
			linked(curPos, (curPos + second) % (timeWheel.size()));
			timeWheel[curPos].head = NULL;
			timeWheel[curPos].tail = NULL;
			curPos = (curPos + 1) % timeWheel.size();
        }
    }
}