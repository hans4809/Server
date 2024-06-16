#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}	
}

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	Protocol::S_TEST pkt;

	ASSERT_CRASH(pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)));

	cout << "ID : " << pkt.id() << " HP : " << pkt.hp() << " MP : " << pkt.mp() << endl;

	cout << "Buff Count: " << pkt.buffs_size() << endl;

	for (auto& buff : pkt.buffs())
	{
		cout << "BUFF ID : " << buff.buffid() << " BUFF TIME  : " << buff.remaintime() << endl;

		cout << "Victims: " << buff.victims_size() << endl;
		for (auto& victim : buff.victims())
		{
			cout << victim << " ";
		}
		cout << endl;
	}
}
