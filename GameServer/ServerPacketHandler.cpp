#include "pch.h"
#include "ServerPacketHandler.h"

PacketHandleFunc GPacketHandler[UINT16_MAX];

// 직접 컨텐츠 작업자가 해야될 작업
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	//LOG(L"Invalid PacketID %d", header->id);
	return false;
}

bool Handle_S_TEST(PacketSessionRef& session, Protocol::S_TEST& pkt)
{
	//TODO

	return true;
}
