#pragma once

// Functor

class IJob
{
public:
	virtual void Excute() {}
};


template<typename Ret, typename... Args>
class FuncJob : public IJob
{
	using FuncType = Ret(*)(Args...);

public:
	FuncJob(FuncType func, Args... args) : _func(func), _tuple(args...) {}

	virtual void Excute() override
	{
		std::apply(_func, _tuple);
	}

private:
	FuncType _func;
	std::tuple<Args...> _tuple;
};

template<typename T, typename Ret, typename... Args>
class MemberJob : public IJob
{
	using FuncType = Ret(T::*)(Args...);

public:
	MemberJob(T* obj, FuncType func, Args... args) : _obj(obj), _func(func), _tuple(args...) {}

	virtual void Excute() override
	{
		//std::apply(_obj, _func, _tuple);
	}

private:
	T* _obj;
	FuncType _func;
	std::tuple<Args...> _tuple;
};

class HealJob : public IJob
{
public:
	virtual void Excute() override
	{
		// Heal Logic
	}
public:
	uint64 _target = 0;
	uint32 _heal = 0;
};

using JobRef = shared_ptr<IJob>;

class JobQueue
{
public:
	void Push(JobRef job)
	{
		WRITE_LOCK;
		_jobs.push(job);
	}

	JobRef Pop()
	{
		WRITE_LOCK;
		if (_jobs.empty())
			return nullptr;

		JobRef ret = _jobs.front();
		_jobs.pop();
		return ret;
	}
private:
	USE_LOCK;
	queue<JobRef> _jobs;
};