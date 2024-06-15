#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include <BufferrWriter.h>

void ServerPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br.Peek(&header);

	switch (header.id)
	{
	default:
		break;
	}
}

SendBufferRef ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 attack, vector<BuffData>buffs)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	// id((uint64), HP(uint32), Attack(uint16)
	bw << id << hp << attack;

	struct ListHeader
	{
		uint16 offset;
		uint16 count;
	};

	//���� ������
	ListHeader* buffsHeader = bw.Reserve<ListHeader>();

	buffsHeader->offset = bw.WriteSize();
	buffsHeader->count = buffs.size();

	for (auto& buff : buffs)
		bw << buff.buffId << buff.remainTime;

	header->size = bw.WriteSize();
	header->id = S_TEST;

	sendBuffer->Close(bw.WriteSize());
	return 	sendBuffer;
}