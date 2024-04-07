#include "pch.h"
#include "IOCPCore.h"
#include "IOCPEvent.h"

IOCPCore GIocpCore;

IOCPCore::IOCPCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IOCPCore::~IOCPCore()
{
	::CloseHandle(_iocpHandle);
}

bool IOCPCore::Register(IOCPObject* iocpObject)
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, reinterpret_cast<ULONG_PTR>(iocpObject), 0);
}

bool IOCPCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	IOCPObject* iocpObject = nullptr;
	IOCPEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT reinterpret_cast<PULONG_PTR>(&iocpObject), OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		iocpObject->Dispatch(iocpEvent, numOfBytes);
		return true;
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}
	return false;
}
