#pragma once
#include "Job.h"
class Room
{
public:
	// �̱� ������ ȯ���� ���� �ڵ�
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

public:
	// ��Ƽ ������ ȯ�濡���� Job���� ����
	void PushJob(JobRef job) { _jobs.Push(job); }
	void FlushJobs();

	template<typename T, typename Ret, typename... Args>
	void PushJob(Ret(T::*memFunc)(Args...), Args... args)
	{
		JobRef job = make_shared<MemberJob<T, Ret, Args...>>(static_cast<T*>(this), memFunc, args...);
		_jobs.Push(job);
	}

private:
	map<uint64, PlayerRef> _players;

	JobQueue _jobs;
};

extern Room GRoom;
