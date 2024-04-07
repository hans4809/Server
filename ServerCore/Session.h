#pragma once
#include "IOCPCore.h"
#include "IOCPEvent.h"
#include "NetAddress.h"

class Session : public IOCPObject
{
public:
	Session();
	virtual ~Session();

public:
	// 정보 관련
	void SetNetAddress(NetAddress netAddress) { _netAddress = netAddress; }
	NetAddress GetNetAddress() { return _netAddress; }
	SOCKET GetSocket() { return _socket; }
public:
	// 인터페이스 구현
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(IOCPEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	// Temp
	char _recvBuffer[1000];

private:
	SOCKET _socket = INVALID_SOCKET;
	NetAddress _netAddress = {};
	Atomic<bool> _isConnected = false;
};


