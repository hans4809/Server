#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ThreadManager.h"

#include "SocketUtils.h"
#include "Listener.h"
#pragma region ServerTest
	//void HandleError(const char* cause)
	//{
	//	int32 errCode = ::WSAGetLastError();
	//	cout << cause << "ErrorCode : " << errCode << endl;
	//}

	//const int BUFFER_SIZE = 1000;
	//struct Session
	//{
	//	SOCKET socket;
	//	char recvBuffer[BUFFER_SIZE] = {};
	//	int32 recvBytes = 0;
	//	int32 sendBytes = 0;
	//};
	//enum IO_TYPE
	//{
	//	READ,
	//	WRITE,
	//	ACCEPT,
	//	CONNECT,
	//};
	//struct OverlappedEx
	//{
	//	WSAOVERLAPPED overlapped = {};
	//	int32 type = 0; // read, write, accept, connect
	//};

	//void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags)
	//{
	//	cout << "Data Recv Len Callback = " << recvLen << endl;
	//}

	//void WorkerThreadMain(HANDLE iocpHandle)
	//{
	//	while (true)
	//	{
	//		DWORD bytesTransferred = 0;
	//		Session* session = nullptr;
	//		OverlappedEx* overlappedEx = nullptr;
	//		BOOL ret = ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred,
	//			(ULONG_PTR*)&session, (LPOVERLAPPED*)&overlappedEx, INFINITE);

	//		if (ret = FALSE || bytesTransferred == 0)
	//		{
	//			continue;
	//		}

	//		ASSERT_CRASH(overlappedEx->type == IO_TYPE::READ);

	//		cout << "Recv Data IOCP = " << bytesTransferred << endl;

	//		WSABUF wsaBuf;
	//		wsaBuf.buf = session->recvBuffer;
	//		wsaBuf.len = BUFFER_SIZE;

	//		DWORD recvLen = 0;
	//		DWORD flags = 0;
	//		::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
	//	}
	//}
	//int main()
	//{
	//	WSAData wsaData;
	//	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	//		return 0;
	//#pragma region TCP
	//	//SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	//	//if (listenSocket == INVALID_SOCKET)
	//	//{
	//	//	int32 errCode = ::WSAGetLastError();
	//	//	cout << "Socket ErrorCode : " << errCode << endl;
	//	//	return 0;
	//	//}

	//	//// 서버 주소
	//	//SOCKADDR_IN serverAddr; // IPv4
	//	//::memset(&serverAddr, 0, sizeof(serverAddr));
	//	//serverAddr.sin_family = AF_INET;
	//	//serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//	//serverAddr.sin_port = ::htons(7777);

	//	//// 소켓 바인드
	//	//if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	//	//{
	//	//	int32 errCode = ::WSAGetLastError();
	//	//	cout << "Bind ErrorCode : " << errCode << endl;
	//	//	return 0;
	//	//}

	//	//if (::listen(listenSocket, 10) == SOCKET_ERROR)
	//	//{
	//	//	int32 errCode = ::WSAGetLastError();
	//	//	cout << "Listen ErrorCode : " << errCode << endl;
	//	//	return 0;
	//	//}

	//	//while (true)
	//	//{
	//	//	SOCKADDR_IN clientAddr; // IPv4
	//	//	::memset(&clientAddr, 0, sizeof(clientAddr));
	//	//	int32 addrlen = sizeof(clientAddr);

	//	//	SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrlen);
	//	//	if (clientSocket == INVALID_SOCKET)
	//	//	{
	//	//		int32 errCode = ::WSAGetLastError();
	//	//		cout << "Accept ErrorCode : " << errCode << endl;
	//	//		return 0;
	//	//	}

	//	//	char ipAddress[16];
	//	//	::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
	//	//	cout << "Client Connected! IP = " << ipAddress << endl;

	//	//	while (true)
	//	//	{
	//	//		char recvBuffer[1000];


	//	//		int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);

	//	//		if (recvLen <= 0)
	//	//		{
	//	//			int errCode = ::WSAGetLastError();
	//	//			cout << "Recv ErrorCode : " << errCode << endl;
	//	//			return 0;
	//	//		}

	//	//		cout << "Recv Data! Data = " << recvBuffer << endl;
	//	//		cout << "Recv Data! Len = " << recvLen << endl;

	//	//		int resultCode = ::send(clientSocket, recvBuffer, recvLen, 0);
	//	//		if (resultCode == SOCKET_ERROR)
	//	//		{
	//	//			int errCode = ::WSAGetLastError();
	//	//			cout << "Send ErrorCode" << errCode << endl;
	//	//			return 0;
	//	//		}
	//	//	}
	//	//}
	//#pragma endregion

	//#pragma region Non-Blocking
	//	// 블로킹 소켓
	//	// accept -> 접속한 클라가 있을 때
	//	// connect -> 서버에 접속 성공했을 때
	//	// send, sendto -> 요청한 데이터를 송신 버퍼에 복사했을 때
	//	// recv, recvfrom -> 수신 버퍼에 데이터가 있고, 이를 유저 레벨 버퍼에서 복사했을 때

	//	// 논블로킹 소켓
	//	/*SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	//	if (listenSocket == INVALID_SOCKET)
	//	{
	//		return 0;
	//	}

	//	u_long on = 1;
	//	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
	//	{
	//		return 0;
	//	}

	//	SOCKADDR_IN serverAddr;
	//	::memset(&serverAddr, 0, sizeof(serverAddr));
	//	serverAddr.sin_family = AF_INET;
	//	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	//	serverAddr.sin_port = ::htons(7777);

	//	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == INVALID_SOCKET)
	//	{
	//		return 0;
	//	}

	//	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	//	{
	//		return 0;
	//	}

	//	cout << "Accept" << endl;*/
	//#pragma region SelectModel
	//	// Select 모델 = (select 함수가 핵심이 됨)
	//	// 소켓 함수 호출이 성공할 시점을 미리 알 수 잇다.
	//	// 문제 상황)
	//	// 수신 버퍼에 데이터가 없는데, read 한다거나
	//	// 송신 버퍼가 꽉 차있는데 write 한다거나
	//	// - 블로킹 소켓 : 조건이 만족되지 않아서 블로킹 되는 상황 예방
	//	// - 논블로킹 소켓 : 조건이 만족되지 않아서 불필요하게 반복 체크하는 상황을 예방

	//	// socket set
	//	// 1) 읽기[ ] 쓰기[ ] 예외[ ] 관찰 대상 등록
	//	// OutOfBand는 send() 마지막 인자 MSG_OOB로 보내는 특별한 데이터
	//	// 받는 쪽에서도 recv OOB 세팅을 해야 읽을 수 있음
	//	// 2) select(readSet, writeSet, exceptSet) : -> 관찰 시작
	//	// 3) 적어도 하나의 소켓이 준비되면 리턴 -> 낙오자는 알아서 제거됨
	//	// 4) 준비된 소켓을 찾아서 처리

	//	// fd_set set;
	//	// FD_ZERO : 비운다
	//	// ex) FD_ZERO(&set);
	//	// FD_SET : 소켓 s를 넣는다.
	//	// ex) FD_SET(s, &set);
	//	// FD_CLR : 소켓 s를 제거
	//	// ex) FD_CLR(s, &set);
	//	// FD_ISSET : 소켓 s가 set에 있으면 0이 아닌 값을 리턴

	//	//vector<Session> sessions;
	//	//sessions.reserve(100);

	//	//fd_set reads;
	//	//fd_set writes;

	//	//while (true)
	//	//{
	//	//	// 소켓 set 초기화
	//	//	FD_ZERO(&reads);
	//	//	FD_ZERO(&writes);

	//	//	// ListenSocket을 등록
	//	//	FD_SET(listenSocket, &reads);

	//	//	// 소켓 등록
	//	//	for (Session& s : sessions)
	//	//	{
	//	//		if (s.recvBytes <= s.sendBytes)
	//	//			FD_SET(s.socket, &reads);
	//	//		else
	//	//			FD_SET(s.socket, &writes);
	//	//	}

	//	//	// [옵션] 마지막 timeout 인자 설정 가능
	//	//	int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);
	//	//	if (retVal == SOCKET_ERROR)
	//	//	{
	//	//		break;
	//	//	}

	//	//	// Listener 소켓 체크
	//	//	if (FD_ISSET(listenSocket, &reads))
	//	//	{
	//	//		SOCKADDR_IN clientAddr;
	//	//		int32 addrLen = sizeof(clientAddr);
	//	//		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

	//	//		if (clientSocket != INVALID_SOCKET)
	//	//		{
	//	//			cout << "Client Connected!" << endl;
	//	//			sessions.push_back(Session{ clientSocket });
	//	//		}
	//	//	}

	//	//	// 나머지 소켓 체크
	//	//	for (Session& s : sessions)
	//	//	{
	//	//		// Read
	//	//		if (FD_ISSET(s.socket, &reads))
	//	//		{
	//	//			int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFFER_SIZE, 0);
	//	//			if (recvLen <= 0)
	//	//			{
	//	//				::closesocket(s.socket);
	//	//				s = sessions.back();
	//	//				sessions.pop_back();
	//	//				continue;
	//	//			}
	//	//			else
	//	//			{
	//	//				s.recvBytes = recvLen;
	//	//			}
	//	//		}

	//	//		// Write
	//	//		if (FD_ISSET(s.socket, &writes))
	//	//		{
	//	//			// 블로킹 모드 -> 모든 데이터 다 보냄
	//	//			// 논브로킹 모드 -> 일부만 보낼 수 있음 (상대방 수신 버퍼 상황에 따라)
	//	//			int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes, 0);
	//	//			if (sendLen == SOCKET_ERROR)
	//	//			{
	//	//				::closesocket(s.socket);
	//	//				s = sessions.back();
	//	//				sessions.pop_back();
	//	//				continue;
	//	//			}
	//	//			s.sendBytes += sendLen;
	//	//			if (s.recvBytes == s.sendBytes)
	//	//			{
	//	//				s.recvBytes = 0;
	//	//				s.sendBytes = 0;
	//	//			}
	//	//		}
	//	//	}
	//	//}
	//#pragma endregion

	//#pragma region WSAEventSelect
	//	// WSAEventSelect = (WSAEventSelect 함수가 핵심이 됨)
	//	// 소켓과 관련된 네트워크 이벤트를 [이벤트 객체]를 통해 감지

	//	// 이벤트 객체 관련 함수들
	//	// 생성 : WSACreateEvent (수동 리셋 Manual-Reset + Non-Signaled 상태 시작)
	//	// 삭제 : WSACloseEvent
	//	// 신호 상태 감지 : WSAWaitForMultipleEvents
	//	// 구체적인 네트워크 이벤트 알아내기 : WSAEnumNetworkEvents

	//	// 소켓 <-> 이벤트 객체 연동
	//	// WSAEventSelect(socket, event, networkEvents);
	//	// - 관심 있는 네트워크 이벤트
	//	// FD_ACCEPT : 접속한 클라가 있으면 accpet
	//	// FD_READ : 데이터 수신 가능 recv, recvfrom
	//	// FD_WRITE : 데이터 송신 가능 send, sendto
	//	// FD_CLOSE : 상대가 접속 종료
	//	// FD_CONNECT : 통신을 위한 연결 절차 완료
	//	// FD_OOB

	//	// 주의 사항
	//	// WSAEventSelect를 호출하면 소켓이 자동으로 Non-블로킹 모드로 동작
	//	// accept() 함수가 리턴하는 소켓은 listenSocket과 동일한 속성을 가진다.
	//	// - 따라서 clientSocket은 FD_READ, FD_WRITE 등을 다시 등록해야 된다.
	//	// 드물게 WSAEWOULDBLOCK 오류가 뜰 수 있으니 예외 처리 필요
	//	// 중요)
	//	// - 이벤트 발생 시, 적절한 소켓 함수 호출해야 한다.
	//	// - 아니면 다음 번에는 동일 네트워크 이벤트가 발생하지 않는다.
	//	// ex) FD_READ 발생 시, recv 함수 호출해야 하고, 하지 않으면 FD_READ가 두번 다시 발생하지 않는다.

	//	// 1) count, event
	//	// 2) waitAll : 모두 기다림?  하나만 완료 되어도 OK?
	//	// 3) timeout : 타임아웃
	//	// 4) 지금은 false
	//	// return : 완료된 첫 번째 인덱스
	//	// WSAWaitForMultipleEvents


	//	// 1) socket
	//	// 2) eventObject : socket과 연동된 이벤트 객체 핸들을 넘겨주면, 이벤트 객체를 non-signaled
	//	// 3) networkEvents : 네트워크 이벤트 / 오류 정보가 저장
	//	// WSAEnumNetworkEvents

	//	//vector<WSAEVENT> wsaEvents;
	//	//vector<Session> sessions;
	//	//sessions.reserve(100);

	//	//WSAEVENT listenEvent = ::WSACreateEvent();
	//	//wsaEvents.push_back(listenEvent);
	//	//sessions.push_back(Session{ listenSocket });
	//	//if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
	//	//{
	//	//	HandleError("WSAEventSelect");
	//	//	return 0;
	//	//}

	//	//while (true)
	//	//{
	//	//	int32 index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);
	//	//	if (index == WSA_WAIT_FAILED)
	//	//	{
	//	//		HandleError("WSAWaitForMultipleEvents");
	//	//		continue;
	//	//	}

	//	//	index -= WSA_WAIT_EVENT_0;

	//	//	WSANETWORKEVENTS networkEvents;
	//	//	if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)
	//	//	{
	//	//		HandleError("WSAEnumNetworkEvents");
	//	//		continue;
	//	//	}

	//	//	// Listner 소켓 체크
	//	//	if (networkEvents.lNetworkEvents && FD_ACCEPT)
	//	//	{
	//	//		// Error-Check
	//	//		if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
	//	//		{
	//	//			HandleError("NOT_ACCEPT");
	//	//			continue;
	//	//		}

	//	//		SOCKADDR_IN clientAddr;
	//	//		int32 addrLen = sizeof(clientAddr);

	//	//		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
	//	//		if (clientSocket == INVALID_SOCKET)
	//	//		{
	//	//			HandleError("Accept");
	//	//			continue;
	//	//		}
	//	//		else
	//	//		{
	//	//			cout << "Client Connected!" << endl;
	//	//			WSAEVENT clientEvent = ::WSACreateEvent();
	//	//			wsaEvents.push_back(clientEvent);
	//	//			sessions.push_back(Session{ clientSocket });
	//	//			if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
	//	//			{
	//	//				HandleError("WSAEventSelect");
	//	//				return 0;
	//	//			}
	//	//		}
	//	//	}

	//	//	// Client Session 소켓 체크
	//	//	if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
	//	//	{
	//	//		//Error-Check
	//	//		if ((networkEvents.lNetworkEvents & FD_READ) && (networkEvents.iErrorCode[FD_READ_BIT] != 0))
	//	//		{
	//	//			HandleError("WRITE-ERROR");
	//	//			continue;
	//	//		}
	//	//		if ((networkEvents.lNetworkEvents & FD_WRITE) && (networkEvents.iErrorCode[FD_WRITE_BIT] != 0))
	//	//		{
	//	//			HandleError("WRITE-ERROR");
	//	//			continue;
	//	//		}

	//	//		Session& s = sessions[index];

	//	//		// Read
	//	//		if (s.recvBytes == 0)
	//	//		{
	//	//			int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFFER_SIZE, 0);
	//	//			if (recvLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
	//	//			{
	//	//				continue;
	//	//			}
	//	//			s.recvBytes = recvLen;
	//	//			cout << "Recv Data! Data = " << recvLen << endl;
	//	//		}

	//	//		// Write
	//	//		if (s.recvBytes > s.sendBytes)
	//	//		{
	//	//			int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
	//	//			if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
	//	//			{
	//	//				continue;
	//	//			}
	//	//			s.sendBytes += sendLen;
	//	//			if (s.recvBytes == s.sendBytes)
	//	//			{
	//	//				s.recvBytes = 0;
	//	//				s.sendBytes = 0;
	//	//			}

	//	//			cout << "Send Data! Data = " << sendLen << endl;
	//	//		}

	//	//		// FD_CLOSE 처리
	//	//		if (networkEvents.lNetworkEvents & FD_CLOSE)
	//	//		{
	//	//			continue;
	//	//		}
	//	//	}
	//	//}
	//#pragma endregion

	//#pragma region Overlapped(Event)
	//	// Overlapped IO (비동기 + 논블로킹)
	//	// - Overlapped 함수를 건다 (WSASend, WSARecv)
	//	// - Overlapped 함수가 성공했는지 확이 후
	//	// -> 성공했으면 결과를 얻어서 처리
	//	// -> 실패했으면 WSAGetLastError로 에러 코드 확인 후, 다시 시도

	//	// 1) 비동기 입출력 소켓
	//	// 2) WSABUF의 배열의 시작 주소 + 개수 	// Scatter-Gather
	//	// 3) 보내고/받은 바이트 수
	//	// 4) 플래그
	//	// 5) WSAOVERLAPPED 구조체의 주소값
	//	// 6) 입출력이 완료되면 OS가 호출할 콜백 함수
	//	// WSASend
	//	// WSARecv

	//	// Overlapped 모델 (이벤트 기반)
	//	// - 비동기 입출력 지원하는 소켓 생성 + 통지 받기 위한 이벤트 객체 생성
	//	// - 비동기 입출력 함수 호출 (1에서 만든 이벤트 객체를 같이 넘겨줌)
	//	// - 비동기 입출력 함수가 완료되지 않으면, WSA_IO_PENDING 리턴
	//	// - 비동기 입출력 함수가 완료되면, 이벤트 객체가 signaled 상태로 만들어서 완료 상태를 알려줌
	//	// - WSAWaitForMultipleEvents 함수로 이벤트 객체의 signal 판별
	//	// - WSAGetOverlappedResult 함수로 비동기 입출력 결과 확인 및 데이터 처리

	//	// 1) 비동기 소켓
	//	// 2) 넘겨준 Overlapped 구조체
	//	// 3) 전송된 바이트 수
	//	// 4) 비동기 입출력 작업이 끝날 때 까지 대기 할지?
	//	// 5) 비동기 입출력 작업 관련 부가 정보. 거의 사용 안함
	//	// WSAGetOverlappedResult

	//	//while (true)
	//	//{
	//	//	SOCKADDR_IN clientAddr;
	//	//	int32 addrLen = sizeof(clientAddr);

	//	//	SOCKET clientSocket;
	//	//	while (true)
	//	//	{
	//	//		clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
	//	//		if (clientSocket != INVALID_SOCKET)
	//	//		{
	//	//			break;
	//	//		}
	//	//		if (::WSAGetLastError() == WSAEWOULDBLOCK)
	//	//		{
	//	//			continue;
	//	//		}
	//	//		 
	//	//		// 문제 상황
	//	//		return 0;
	//	//	}

	//	//	Session session = Session{ clientSocket };
	//	//	WSAEVENT wsaEvent = ::WSACreateEvent();
	//	//	session.overlapped.hEvent = wsaEvent;

	//	//	cout << "Client Connected!" << endl;

	//	//	while (true)
	//	//	{
	//	//		WSABUF wsaBuf;
	//	//		wsaBuf.buf = session.recvBuffer;
	//	//		wsaBuf.len = BUFFER_SIZE;

	//	//		DWORD recvLen = 0;
	//	//		DWORD flags = 0;
	//	//		if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, nullptr) == SOCKET_ERROR)
	//	//		{
	//	//			if (::WSAGetLastError() == WSA_IO_PENDING)
	//	//			{
	//	//				// Pending
	//	//				::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
	//	//				::WSAGetOverlappedResult(session.socket, &session.overlapped, &recvLen, FALSE, &flags);
	//	//			}
	//	//			else
	//	//			{
	//	//				// TODO : 문제 상황
	//	//				break;
	//	//			}
	//	//		}

	//	//		cout << "Data Recv! Len = " << recvLen << endl;	
	//	//	}

	//	//	::closesocket(session.socket);
	//	//	::WSACloseEvent(wsaEvent);
	//	//}

	//#pragma endregion

	//#pragma region OverlappedCallback
	//	//Overelapped 모델 (Completion Routine 콜백 기반)
	//	// - 비동기 입출력 지원하는 소켓 생성
	//	// - 비동기 입출력 함수 호출 (완료 루틴의 시작 주소를 넘겨준다.)
	//	// - 비동기 작업이 바로 완료되지 않으면, WSA_ID_PENDING 오류 코드
	//	// - 비동기 입출력 함수 호출한 쓰레드를 -> Alertalbe Wait 상태로 만든다.
	//	// ex) WaitForSingleObjectEx , WaitForMultipleObjectsEx, SleepEx, WSAWaitForMultipleEvents
	//	// - 비동기 IO가 완료되면, 운영체제는 완료 루틴 호출
	//	// - 완료 루틴 호출이 모두 ㅡㄲ타면, 쓰레드는 Alertable Wait 상태에서 빠져나온다.

	//	// 1) 오류 발생 시 0이 아닌 값
	//	// 2) 전송 바이트 수
	//	// 3) 비동기 입출력 함수 호출 시, 넘겨준 WSAOVERLAPPED 구조체의 주소값
	//	// 4) 0

	//	//while (true)
	//	//{
	//	//	SOCKADDR_IN clientAddr;
	//	//	int32 addrLen = sizeof(clientAddr);

	//	//	SOCKET clientSocket;
	//	//	while (true)
	//	//	{
	//	//		clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
	//	//		if (clientSocket != INVALID_SOCKET)
	//	//		{
	//	//			break;
	//	//		}
	//	//		if (::WSAGetLastError() == WSAEWOULDBLOCK)
	//	//		{
	//	//			continue;
	//	//		}

	//	//		// 문제 상황
	//	//		return 0;
	//	//	}

	//	//	Session session = Session{ clientSocket };
	//	//	//WSAEVENT wsaEvent = ::WSACreateEvent();

	//	//	cout << "Client Connected!" << endl;
	//	//}

	//	//while (true)
	//	//{
	//	//	WSABUF wsaBuf;
	//	//	wsaBuf.buf = session.recvBuffer;
	//	//	wsaBuf.len = BUFFER_SIZE;

	//	//	DWORD recvLen = 0;
	//	//	DWORD flags = 0;
	//	//	if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped, RecvCallback) == SOCKET_ERROR)
	//	//	{
	//	//		if (::WSAGetLastError() == WSA_IO_PENDING)
	//	//		{
	//	//			// Pending
	//	//			// Alertable Wait
	//	//			::SleepEx(INFINITE, TRUE);
	//	//			//::WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, TRUE);
	//	//			
	//	//		}
	//	//		else
	//	//		{
	//	//			// TODO : 문제 상황
	//	//			break;
	//	//		}
	//	//	}
	//	//	else 
	//	//	{
	//	//		cout << "Data Recv! Len = " << recvLen << endl;
	//	//	}
	//	//	
	//	//}

	//	//::closesocket(session.socket);
	//	//::WSACloseEvent(wsaEvent);
	//#pragma endregion

	//#pragma endregion

	//#pragma region IOCP
	//	// Overlapped 모델 (Completion Routine 콜백 기반)
	//	// - 비동기 입출력 함수 완료되면, 쓰레드마다 있는 APC 쿠에 일감이 쌓임
	//	// - Alertable Wait 상태로 들어가서 APC 큐 비우기 (콜백 함수)
	//	// 단점) APC 큐 쓰레드마다 있다. Alertable Wait 자체도 조금 부담!
	//	// 단점) 이벤트 방식 소켓 이벤트 1:1 대응

	//	// IOCP (Completin Port) 모델
	//	// - ACP -> Completion Port (쓰레드마다 있는 건 아니고 1개, 중앙에서 관리하는 APC 큐?)
	//	// - Alertable Wait -> CP 결과 처리를 GetQueuedCompletionStauts
	//	// 쓰레드랑 궁합이 굉장히 좋다.

	//	// CreateCompletionPort
	//	// GetQueuedCompletionStatus

	//	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	//	if (listenSocket == INVALID_SOCKET)
	//	{
	//		return 0;
	//	}

	//	SOCKADDR_IN serverAddr;
	//	::memset(&serverAddr, 0, sizeof(serverAddr));
	//	serverAddr.sin_family = AF_INET;
	//	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	//	serverAddr.sin_port = ::htons(7777);

	//	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == INVALID_SOCKET)
	//	{
	//		return 0;
	//	}

	//	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	//	{
	//		return 0;
	//	}

	//	cout << "Accept" << endl;

	//	vector<Session*> sessionManager;
	//	// CP 생성
	//	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	//	// WorkerThreads
	//	for (int32 i = 0; i < 5; i++)
	//	{
	//		GThreadManager->Launch([=]() {WorkerThreadMain(iocpHandle); });
	//	}

	//	//Main Thread = Accept 담당
	//	while (true)
	//	{
	//		SOCKADDR_IN clientAddr;
	//		int32 addrLen = sizeof(clientAddr);

	//		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
	//		if (clientSocket == INVALID_SOCKET)
	//		{
	//			return 0;
	//		}

	//		Session* session = new Session();
	//		session->socket = clientSocket;
	//		sessionManager.push_back(session);
	//		//WSAEVENT wsaEvent = ::WSACreateEvent();

	//		cout << "Client Connected!" << endl;

	//		//소켓에 CP 등록
	//		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, /*Key*/ (ULONG_PTR)session, 0);

	//		WSABUF wsaBuf;
	//		wsaBuf.buf = session->recvBuffer;
	//		wsaBuf.len = BUFFER_SIZE;

	//		OverlappedEx* overlappedEx = new OverlappedEx();
	//		overlappedEx->type = IO_TYPE::READ;

	//		DWORD recvLen = 0;
	//		DWORD flags = 0;
	//		::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
	//	}

	//	GThreadManager->Join();
	//	// MainThread = Accept 담당
	//#pragma endregion
	////Select 모델
	//// - 장점) 윈도우/리눅스 공통
	//// - 단점) 성능 최하 (매번 등록 비용), 64개 제한
	//// WSAEventSelect모델
	//// - 장점) 비교적 뛰어난 성능
	//// - 단점) 64개 제한
	//// Overlapped (이벤트 기반)
	//// - 장점) 성능
	//// - 단점) 64개 제한
	//// Overlapped (콜백 기반)
	//// - 장점) 성능
	//// - 단점) 모든 모든 비동기 소켓 함수에서 사용 가능하진 않음 (accept), 빈번한 Alertable Wait로 인한 성능 저하
	//// IOCP

	//// Reacter Pattern (- 뒤늦게 논브로킹 소켓, 소켓 상태 확인 후-> 뒤늦게 recv send 호출)
	//// Proactor Pattern (-미리 Overlapped WSA-)


	//	// 윈도우 소켓 종료
	//	::WSACleanup();
	//}
#pragma endregion

int main()
{
	Listener listener;
	listener.StartAccept(NetAddress(L"127.0.0.1", 7777));

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]() 
			{
				while (true)
				{
					GIocpCore.Dispatch();
				}
			});
	}


}
