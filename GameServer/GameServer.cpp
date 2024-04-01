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

const int BUFFER_SIZE = 1000;
struct Session
{
	SOCKET socket;
	char recvBuffer[BUFFER_SIZE] = {};
	int32 recvBytes = 0;
	int32 sendBytes = 0;
};
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
	// 블로킹 소켓
	// accept -> 접속한 클라가 있을 때
	// connect -> 서버에 접속 성공했을 때
	// send, sendto -> 요청한 데이터를 송신 버퍼에 복사했을 때
	// recv, recvfrom -> 수신 버퍼에 데이터가 있고, 이를 유저 레벨 버퍼에서 복사했을 때
	
	// 논블로킹 소켓
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

	// Select 모델 = (select 함수가 핵심이 됨)
	// 소켓 함수 호출이 성공할 시점을 미리 알 수 잇다.
	// 문제 상황)
	// 수신 버퍼에 데이터가 없는데, read 한다거나
	// 송신 버퍼가 꽉 차있는데 write 한다거나
	// - 블로킹 소켓 : 조건이 만족되지 않아서 블로킹 되는 상황 예방
	// - 논블로킹 소켓 : 조건이 만족되지 않아서 불필요하게 반복 체크하는 상황을 예방

	// socket set
	// 1) 읽기[ ] 쓰기[ ] 예외[ ] 관찰 대상 등록
	// OutOfBand는 send() 마지막 인자 MSG_OOB로 보내는 특별한 데이터
	// 받는 쪽에서도 recv OOB 세팅을 해야 읽을 수 있음
	// 2) select(readSet, writeSet, exceptSet) : -> 관찰 시작
	// 3) 적어도 하나의 소켓이 준비되면 리턴 -> 낙오자는 알아서 제거됨
	// 4) 준비된 소켓을 찾아서 처리

	// fd_set set;
	// FD_ZERO : 비운다
	// ex) FD_ZERO(&set);
	// FD_SET : 소켓 s를 넣는다.
	// ex) FD_SET(s, &set);
	// FD_CLR : 소켓 s를 제거
	// ex) FD_CLR(s, &set);
	// FD_ISSET : 소켓 s가 set에 있으면 0이 아닌 값을 리턴

	vector<Session> sessions;
	sessions.reserve(100);

	fd_set reads;
	fd_set writes;

	while (true)
	{
		// 소켓 set 초기화
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		// ListenSocket을 등록
		FD_SET(listenSocket, &reads);

		// 소켓 등록
		for (Session& s : sessions)
		{
			if(s.recvBytes <= s.sendBytes)
				FD_SET(s.socket, &reads);
			else
				FD_SET(s.socket, &writes);
		}

		// [옵션] 마지막 timeout 인자 설정 가능
		int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);
		if (retVal == SOCKET_ERROR)
		{
			break;
		}

		// Listener 소켓 체크
		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected!" << endl;
				sessions.push_back(Session{ clientSocket });
			}
		}
		
		// 나머지 소켓 체크
		for (Session& s : sessions)
		{
			// Read
			if (FD_ISSET(s.socket, &reads))
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFFER_SIZE, 0);
				if (recvLen <= 0)
				{
					::closesocket(s.socket);
					s = sessions.back();
					sessions.pop_back();
					continue;
				}
				else
				{
					s.recvBytes = recvLen;
				}
			}

			// Write
			if (FD_ISSET(s.socket, &writes))
			{
				// 블로킹 모드 -> 모든 데이터 다 보냄
				// 논브로킹 모드 -> 일부만 보낼 수 있음 (상대방 수신 버퍼 상황에 따라)
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes, 0);
				if (sendLen == SOCKET_ERROR)
				{
					::closesocket(s.socket);
					s = sessions.back();
					sessions.pop_back();
					continue;
				}
				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}
			}
		}
	}
#pragma endregion





	// 윈도우 소켓 종료
	::WSACleanup();
}