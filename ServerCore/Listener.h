#pragma once
#include "IOCPCore.h"
#include "NetAddress.h"

class AcceptEvent;


class Listener : public IOCPObject
{
public:
	Listener() = default;
	~Listener();

public:
	/*�ܺο��� ���*/
	bool StartAccept(NetAddress netAddress);
	void CloseSocket();

public:
	// �������̽� ����
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(IOCPEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	/*���� ����*/
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);
protected:
	SOCKET _socket = INVALID_SOCKET;
	Vector<AcceptEvent*> _acceptEvents;
};

