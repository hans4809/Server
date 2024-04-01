#include "pch.h"
#include <iostream>

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << "ErrorCode : " << errCode << endl;
}

int main()
{
	this_thread::sleep_for(1s);
	// 윈도우 소켓 초기화 (ws2_32, 라이브러리 초기화)
	// 관련 정보가 wsaData에 채워짐
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	// ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
	// type : TCP(SOCK_STREAM) vs UDP(SOCK_DGRAM)
	// protocol : 0
	//return : descriptor
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		int32 errCode = ::WSAGetLastError();
		cout << "Socket ErrorCode : " << errCode << endl;
		return 0;
	}
#pragma region TCP
	//// 연결할 목적지는? (IP 주소 + Port)
	//SOCKADDR_IN serverAddr; // IPv4
	//::memset(&serverAddr, 0, sizeof(serverAddr));
	//serverAddr.sin_family = AF_INET;
	////serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
	//::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	//serverAddr.sin_port = ::htons(7777);

	//if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	//{
	//	int32 errCode = ::WSAGetLastError();
	//	cout << "Connect ErrorCode : " << errCode << endl;
	//	return 0;
	//}

	//// 연결 성공 TODO : 데이터 송수신
	//cout << "Connected To Server!" << endl;

	//while (true)
	//{
	//	//TODO
	//	char sendBuffer[100] = "Hello World!";
	//	int resultCode = ::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
	//	if (resultCode == SOCKET_ERROR)
	//	{
	//		int errCode = ::WSAGetLastError();
	//		cout << "Send ErrorCode" << errCode << endl;
	//		return 0;
	//	}

	//	cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;

	//	char recvBuffer[1000];

	//	int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

	//	if (recvLen <= 0)
	//	{
	//		int errCode = ::WSAGetLastError();
	//		cout << "Recv ErrorCode : " << errCode << endl;
	//		return 0;
	//	}

	//	cout << "Recv Data! Data = " << recvBuffer << endl;
	//	cout << "Recv Data! Len = " << recvLen << endl;

	//	this_thread::sleep_for(1s);
	//}
#pragma endregion

#pragma region Non-Blocking
	u_long on = 1;
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
	{
		return 0;
	}

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	//Connect
	while (true)
	{
		if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			// 논블로킹이라 바로 에러가 나지 않음
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
			// 이미 연결된 상태
			if (WSAGetLastError() == WSAEISCONN)
			{
				cout << "Connected To Server!" << endl;
				break;
			}
			// 그 외의 에러
			break;
		}
	}

	cout << "Connected To Server!" << endl;

	char sendBuffer[100] = "Hello World!";

#pragma region Non-Overlapped
	// Send
	//while (true)
	//{
	//	if (::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
	//	{
	//		// 논블로킹으로 해놔서 다시 체크해야댐
	//		if (::WSAGetLastError() == WSAEWOULDBLOCK)
	//		{
	//			continue;
	//		}

	//		// 에러
	//		break;
	//	}

	//	cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;
	//	
	//	// Recv
	//	while (true)
	//	{
	//		char recvBuffer[1000];
	//		int recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

	//		if (recvLen == SOCKET_ERROR)
	//		{
	//			// 논블로킹으로 해놔서 다시 체크해야댐
	//			if (::WSAGetLastError() == WSAEWOULDBLOCK)
	//			{
	//				continue;
	//			}

	//			// 에러
	//			break;
	//		}
	//		else if (recvLen == 0)
	//		{
	//			// 연결 끊김
	//			break;
	//		}

	//		cout<< "Recv Data! Len = " << recvLen << endl;
	//		break;
	//	}


	//	this_thread::sleep_for(1s);
	//}
#pragma endregion

#pragma region Overlapped

	WSAEVENT wsaEvent = ::WSACreateEvent();
	WSAOVERLAPPED overlapped = {};
	overlapped.hEvent = wsaEvent;
	while (true)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = sendBuffer;
		wsaBuf.len = 100;

		DWORD sendLen = 0;
		DWORD flags = 0;
		if (::WSASend(clientSocket, &wsaBuf, 1, &sendLen, flags, &overlapped, nullptr) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSA_IO_PENDING)
			{
				//Pending
				::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
				::WSAGetOverlappedResult(clientSocket, &overlapped, &sendLen, FALSE, &flags);
			}
			else
			{
				// 진짜 문제
				break;
			}
		}

		cout << "Send Data! Len = " << sendLen << endl;

		this_thread::sleep_for(1s);	
	}
#pragma endregion

#pragma endregion

	
	
	// 소켓 리소스 반환
	::closesocket(clientSocket);

	// 윈도우 소켓 종료
	::WSACleanup();
}