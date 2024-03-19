#include "pch.h"
#include "MemoryPool.h"

MemoryPool::MemoryPool(int32 allocSize)
{
}

MemoryPool::~MemoryPool()
{
	while(_queue.empty() == false)
	{
		MemoryHeader* header = _queue.front();
		_queue.pop();
		::free(header);
	}
}

void* MemoryPool::Alloc()
{
	return nullptr;
}

void MemoryPool::Free(void* ptr)
{
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	WRITE_LOCK;
	ptr->allocSize = 0;

	_queue.push(ptr);

	_allocCount.fetch_sub(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* header = nullptr;

	{
		WRITE_LOCK;

		//Pool에 여분의 데이터가 있는지 확인
		if(_queue.empty() == false)
		{
			header = _queue.front();
			_queue.pop();
		}
	}

	//없으면 새로 만듬
	if(header == nullptr)
	{
		header = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize));
	}
	else 
	{
		ASSERT_CRASH(header->allocSize == 0);
	}

	_allocCount.fetch_add(1);

	return nullptr;
}
