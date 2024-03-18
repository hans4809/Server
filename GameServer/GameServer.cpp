#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"
#include "CoreMacro.h"
#include "ThreadManager.h"
#include "Memory.h"



LockQueue<int32> q;
LockFreeStack<int32> s;

void Push()
{
	while (true)
	{
		int32 value = rand() % 100;
		s.Push(value);

		this_thread::sleep_for(1ms);
	}
}

void Pop()
{
	while (true)
	{
		auto data = s.TryPop();
		if (data != nullptr)
			cout << (*data) << endl;
	}
}

class TestLock
{
	USE_LOCK;
public:
	int32 TestRead()
	{
		READ_LOCK;
		
		if (_queue.empty())
		{
			return -1;
		}

		return _queue.front();
	}

	void TestPsuh()
	{
		WRITE_LOCK;
		
		_queue.push(rand() % 100);
	}

	void TestPop()
	{
		WRITE_LOCK;
		
		if (_queue.empty() == false)
		{
			_queue.pop();
		}
	}
private:
	queue<int32> _queue;
};

TestLock testLock;

void ThreadWrite()
{
	while (true)
	{
		testLock.TestPsuh();
		this_thread::sleep_for(1ms);
		testLock.TestPop();
	}
}

void ThreadRead()
{
	while (true)
	{
		int32 value = testLock.TestRead();
		cout << value << endl;
		this_thread::sleep_for(1ms);
	}
}


int main()
{
	thread t1(Push);
	thread t2(Pop);
	thread t3(Pop);

	t1.join();
	t2.join();
	t3.join();

	for(int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadWrite);
	}

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadRead);
	}

	GThreadManager->Join();
}