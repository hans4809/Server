#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send,
};

class IOCPEvent : public OVERLAPPED
{
public:
	IOCPEvent(EventType type);

	void Init();

public:
	EventType eventType;
	IOCPObjectRef owner;
};

class ConnectEvent : public IOCPEvent
{
public:
	ConnectEvent() : IOCPEvent(EventType::Connect) { };
};

class DisConnectEvent : public IOCPEvent
{
public:
	DisConnectEvent() : IOCPEvent(EventType::Disconnect) { };
};

class AcceptEvent : public IOCPEvent
{
public:
	AcceptEvent() : IOCPEvent(EventType::Accept) { };

public:
	SessionRef session = nullptr;
};

class RecvEvent : public IOCPEvent
{
public:
	RecvEvent() : IOCPEvent(EventType::Recv) { };
};

class SendEvent : public IOCPEvent
{
public:
	SendEvent() : IOCPEvent(EventType::Send) { };

	//Temp
	vector<BYTE> buffer;
};