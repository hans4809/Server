#pragma once

#define OUT
/*------------------
		lock
-------------------*/

#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard##idx(_locks[idx], typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx)		WriteLockGuard writelockGuard##idx(_locks[idx], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0)
/*-------------------
		Memory
-------------------*/
#ifdef _DEBUG
#define xalloc(size) BaseAllocator::Alloc(size)
#define xrelease(ptr) BaseAllocator::Release(ptr)
#else
#define xalloc(size) BaseAllocator::Alloc(size)
#define xrelease(ptr) BaseAllocator::Release(ptr)
#endif


/*-------------------
		Crash
-------------------*/
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}	