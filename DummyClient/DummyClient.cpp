#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
//#include <winsock2.h>
//#include <mswsock.h>
//#include <ws2tcpip.h>
//#pragma comment(lib, "ws2_32.lib")

//void HandleError(const char* cause)
//{
//	int32 errCode = ::WSAGetLastError();
//	cout << cause << " ErrorCode : " << errCode << endl;
//}

char sendBuffer[] = "Hello World";

class ServerSession : public Session
{
public:
	~ServerSession()
	{
		cout << "~ServerSession" << endl;
	}

	virtual void OnConnected() override
	{
		cout << "Connected to Server" << endl;
		Send((BYTE*)sendBuffer, sizeof(sendBuffer));
	}

	virtual int32 OnRecv(BYTE* buffer, int32 len) override
	{
		cout << "OnRecv Len = " << len << endl;

		this_thread::sleep_for(1s);
		Send((BYTE*)sendBuffer, sizeof(sendBuffer));
		return len;
	}

	virtual void OnSend(int32 len) override
	{
		cout << "OnSend Len = " << len << endl;
	}

	virtual void OnDisconnected() override
	{
		cout << "Disconnected to Server" << endl;
	}
};
int main()
{
#pragma region NonBlockingClient
	//WSAData wsaData;
//if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//	return 0;

//SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
//if (clientSocket == INVALID_SOCKET)
//	return 0;

//u_long on = 1;
//if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
//	return 0;

//SOCKADDR_IN serverAddr;
//::memset(&serverAddr, 0, sizeof(serverAddr));
//serverAddr.sin_family = AF_INET;
//::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
//serverAddr.sin_port = ::htons(7777);

//// Connect
//while (true)
//{
//	if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
//	{
//		// 원래 블록했어야 했는데... 너가 논블로킹으로 하라며?
//		if (::WSAGetLastError() == WSAEWOULDBLOCK)
//			continue;
//		// 이미 연결된 상태라면 break
//		if (::WSAGetLastError() == WSAEISCONN)
//			break;
//		// Error
//		break;
//	}
//}

//cout << "Connected to Server!" << endl;

//char sendBuffer[100] = "Hello World";
//WSAEVENT wsaEvent = ::WSACreateEvent();
//WSAOVERLAPPED overlapped = {};
//overlapped.hEvent = wsaEvent;

//// Send
//while (true)
//{
//	WSABUF wsaBuf;
//	wsaBuf.buf = sendBuffer;
//	wsaBuf.len = 100;

//	DWORD sendLen = 0;
//	DWORD flags = 0;
//	if (::WSASend(clientSocket, &wsaBuf, 1, &sendLen, flags, &overlapped, nullptr) == SOCKET_ERROR)
//	{
//		if (::WSAGetLastError() == WSA_IO_PENDING)
//		{
//			// Pending
//			::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
//			::WSAGetOverlappedResult(clientSocket, &overlapped, &sendLen, FALSE, &flags);
//		}
//		else
//		{
//			// 진짜 문제 있는 상황
//			break;
//		}
//	}

//	cout << "Send Data ! Len = " << sizeof(sendBuffer) << endl;

//	this_thread::sleep_for(1s);
//}

//// 소켓 리소스 반환
//::closesocket(clientSocket);

//// 윈속 종료
//::WSACleanup();
#pragma endregion

	this_thread::sleep_for(1s);

	ClientServiceRef service = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IOCPCore>(),
		MakeShared<ServerSession>,
		1
	);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIOCPCore()->Dispatch();
				}
			});
	}

	GThreadManager->Join();
}