#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

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
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;
#pragma region TCP
	//SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	//if (listenSocket == INVALID_SOCKET)
	//{
	//	int32 errCode = ::WSAGetLastError();
	//	cout << "Socket ErrorCode : " << errCode << endl;
	//	return 0;
	//}

	//// 서버 주소
	//SOCKADDR_IN serverAddr; // IPv4
	//::memset(&serverAddr, 0, sizeof(serverAddr));
	//serverAddr.sin_family = AF_INET;
	//serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//serverAddr.sin_port = ::htons(7777);

	//// 소켓 바인드
	//if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	//{
	//	int32 errCode = ::WSAGetLastError();
	//	cout << "Bind ErrorCode : " << errCode << endl;
	//	return 0;
	//}

	//if (::listen(listenSocket, 10) == SOCKET_ERROR)
	//{
	//	int32 errCode = ::WSAGetLastError();
	//	cout << "Listen ErrorCode : " << errCode << endl;
	//	return 0;
	//}

	//while (true)
	//{
	//	SOCKADDR_IN clientAddr; // IPv4
	//	::memset(&clientAddr, 0, sizeof(clientAddr));
	//	int32 addrlen = sizeof(clientAddr);

	//	SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrlen);
	//	if (clientSocket == INVALID_SOCKET)
	//	{
	//		int32 errCode = ::WSAGetLastError();
	//		cout << "Accept ErrorCode : " << errCode << endl;
	//		return 0;
	//	}

	//	char ipAddress[16];
	//	::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
	//	cout << "Client Connected! IP = " << ipAddress << endl;

	//	while (true)
	//	{
	//		char recvBuffer[1000];


	//		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

	//		if (recvLen <= 0)
	//		{
	//			int errCode = ::WSAGetLastError();
	//			cout << "Recv ErrorCode : " << errCode << endl;
	//			return 0;
	//		}

	//		cout << "Recv Data! Data = " << recvBuffer << endl;
	//		cout << "Recv Data! Len = " << recvLen << endl;

	//		int resultCode = ::send(clientSocket, recvBuffer, recvLen, 0);
	//		if (resultCode == SOCKET_ERROR)
	//		{
	//			int errCode = ::WSAGetLastError();
	//			cout << "Send ErrorCode" << errCode << endl;
	//			return 0;
	//		}
	//	}
	//}
#pragma endregion

#pragma region Non-Blocking
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		return 0;
	}

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
	{
		return 0;
	}

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == INVALID_SOCKET)
	{
		return 0;
	}

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		return 0;
	}

	cout << "Accept" << endl;

	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);
	while (true)
	{
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			//논블로킹으로 해놔서 다시 체크해야댐
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
			{
				continue;
			}
		}
	}
#pragma endregion





	// 윈도우 소켓 종료
	::WSACleanup();
}