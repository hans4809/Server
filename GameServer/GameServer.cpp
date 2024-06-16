#include "pch.h"
//#include "CorePch.h"
//#include <atomic>
//#include <mutex>
//#include <windows.h>
//#include <future>
#include "ThreadManager.h"

#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferrWriter.h"
#include "ServerPacketHandler.h"
#include "Protocol.pb.h"

// 패킷 직렬화(Serialization)

int main()
{
	ServerPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IOCPCore>(),
		MakeShared<GameSession>,
		100
	);
	
	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]() 
			{
				while (true)
				{
					service->GetIOCPCore()->Dispatch();
				}
			});
	}

	char sendData[1000] = "Hello World";

	while (true)
	{
		Protocol::S_TEST pkt;
		pkt.set_id(1001);
		pkt.set_hp(100);
		pkt.set_mp(50);

		{
			Protocol::BuffData* data = pkt.add_buffs();
			data->set_buffid(100);
			data->set_remaintime(1.2f);
			data->add_victims(1000);
		}

		{
			Protocol::BuffData* data = pkt.add_buffs();
			data->set_buffid(200);
			data->set_remaintime(2.5f);
			data->add_victims(2000);
			data->add_victims(3000);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);

		GSessionManager.BroadCast(sendBuffer);

		this_thread::sleep_for(250ms);
	}

	GThreadManager->Join();
}
