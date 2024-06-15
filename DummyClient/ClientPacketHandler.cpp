#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

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

#pragma pack(1)
//[ PKT_S_TEST ][ 가변 데이터 ]
struct PKT_S_TEST
{
	struct BuffListItem
	{
		uint64 buffId;
		float remainTime;
	};

	uint16 packetSize; // 2Byte
	uint16 packetId; // 2Byte
	uint64 id; // 8Byte
	uint32 hp; // 4Byte
	uint16 attack; // 2Byte

	uint16 buffOffset; // 2Byte
	uint16 buffCount; // 2Byte

	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(PKT_S_TEST);
		size += buffCount * sizeof(BuffListItem);
		if(size != packetSize)
			return false;

		if(buffOffset + buffCount * sizeof(BuffListItem) > packetSize)
			return false;

		return true;
	}
	//vector<BuffData> buffs;
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	if(len <sizeof(PKT_S_TEST))
		return;

	PKT_S_TEST pkt;
	br >> pkt;

	if (pkt.Validate() == false)
		return;

	//cout << "ID = " << id << " HP : " << hp << " Attack : " << attack << endl;

	vector<PKT_S_TEST::BuffListItem> buffs;

	buffs.resize(pkt.buffCount);
	for (int i = 0; i < pkt.buffCount; i++)
	{
		br >> buffs[i];
	}

	cout << "Buff Count : " << pkt.buffCount << endl;
	for (int i = 0; i < pkt.buffCount; i++)
	{
		cout << "Buff ID : " << buffs[i].buffId << " Remain Time : " << buffs[i].remainTime << endl;
	}
}
