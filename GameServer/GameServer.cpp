#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include <tchar.h>
#include "Protocol.pb.h"
#include "Job.h"
#include "Room.h"
#include "Player.h"
#include "DBConnectionPool.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDB;Trusted_Connection=Yes;"));

	// CREATE TABLE
	{
		auto query = L"									\
			DROP TABLE IF EXISTS [dbo].[Gold]			\
			CREATE TABLE [dbo].[Gold]					\
			(											\
				[id] INT NOT NULL PRIMARY KEY IDENTITY,	\
				[gold] INT NULL							\
			);";

		DBConnection* dbConnection = GDBConnectionPool->Pop();
		ASSERT_CRASH(dbConnection->Execute(query));
		GDBConnectionPool->Push(dbConnection);
	}

	//ADD DATA
	for (int32 i = 0; i < 3; i++)
	{
		DBConnection* dbConnection = GDBConnectionPool->Pop();
		dbConnection->Unbind();

		int32 gold = 100;
		SQLLEN len = 0;

		ASSERT_CRASH(dbConnection->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(gold), &gold, &len));

		ASSERT_CRASH(dbConnection->Execute(L"INSERT INTO [dbo].[Gold] ([gold]) VALUES (?)"));
		GDBConnectionPool->Push(dbConnection);
	}

	//READ
	{
		DBConnection* dbConnection = GDBConnectionPool->Pop();
		dbConnection->Unbind();

		int32 gold = 100;
		SQLLEN len = 0;
		ASSERT_CRASH(dbConnection->BindParam(1, SQL_C_LONG, SQL_INTEGER, sizeof(gold), &gold, &len));

		int32 outID = 0;
		SQLLEN outIDLen = 0;
		ASSERT_CRASH(dbConnection->BindColumn(1, SQL_C_LONG, sizeof(outID), &outID, &outIDLen));

		int32 outGold = 0;
		SQLLEN outGoldLen = 0;
		ASSERT_CRASH(dbConnection->BindColumn(2, SQL_C_LONG, sizeof(outGold), &outGold, &outGoldLen));

		ASSERT_CRASH(dbConnection->Execute(L"SELECT * FROM [dbo].[Gold] WHERE gold = (?)"));
	
		while (dbConnection->Fetch())
		{
			cout << "ID : " << outID << " Gold : " << outGold << endl;
		}
		GDBConnectionPool->Push(dbConnection);
	}


	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100);

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();
}