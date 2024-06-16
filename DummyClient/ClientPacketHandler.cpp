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

		uint16 victimsOffset;
		uint16 victimsCount;

		bool Validate(BYTE* packetStart, uint16 packetSize, OUT uint32& size)
		{
			if (victimsOffset + victimsCount * sizeof(uint64) > packetSize)
				return false;

			size += victimsCount * sizeof(uint64);

			return true;
		}
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
		if(packetSize < size)
			return false;

		if (buffOffset + buffCount * sizeof(BuffListItem) > packetSize)
			return false;

		size += buffCount * sizeof(BuffListItem);

		BuffsList buffList = GetBuffsList();
		for (int i = 0; i < buffList.Count(); i++)
		{
			if (buffList[i].Validate((BYTE*)this, packetSize, OUT size) == false)
				return false;
		}

		// 최종 크기 비교
		if(size != packetSize)
			return false;

		return true;
	}
	//vector<BuffData> buffs;

	using BuffsList = PacketList<PKT_S_TEST::BuffListItem>;
	using BuffVictimsList = PacketList<uint64>;
	
	BuffsList GetBuffsList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffOffset;
		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffListItem*>(data), buffCount);
	}

	BuffVictimsList GetBuffVictimsList(BuffListItem* buffItem)
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffItem->victimsOffset;
		return BuffVictimsList(reinterpret_cast<uint64*>(data), buffItem->victimsCount);
	}
}; 
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);

	if (pkt->Validate() == false)
		return;

	//cout << "ID = " << id << " HP : " << hp << " Attack : " << attack << endl;

	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();

	cout << "Buff Count : " << buffs.Count() << endl;

	for (int i = 0; i < buffs.Count(); i++)
	{
		cout << "Buff ID : " << buffs[i].buffId << " Remain Time : " << buffs[i].remainTime << endl;
	
		PKT_S_TEST::BuffVictimsList victims = pkt->GetBuffVictimsList(&buffs[i]);
		
		cout << "Victim Count : " << victims.Count() << endl;

		for (auto& victim : victims)
		{
			cout << "Victim ID : " << victim << endl;
		}
	}
}
