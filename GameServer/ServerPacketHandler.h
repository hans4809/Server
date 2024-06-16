#pragma once
#include "BufferReader.h"
#include "BufferrWriter.h"

enum
{
	S_TEST = 1,
};

template<typename T, typename C>
class PacketIterator
{
public:
	PacketIterator(C& containter, uint16 index) : _container(containter), _index(index) { }

	bool operator!=(const PacketIterator& other) const { return _index != other._index; }
	const T& operator*() const { return _container[_index]; }
	T& operator*() { return _container[_index]; }
	T* operator->() { return &_container[_index]; }
	PacketIterator& operator++() { _index++; return *this; }
	PacketIterator operator++(int32) { PacketIterator ret = *this; ++_index; return ret; }

private:
	C& _container;
	uint16 _index;
};

template<typename T>
class PacketList
{
public:
	PacketList() : _data(nullptr), _count(0) { }
	PacketList(T* data, uint16 count) : _data(data), _count(count) { }

	T& operator[](uint16 index)
	{
		ASSERT_CRASH(index < _count);
		return _data[index];
	}

	uint16 Count() { return _count; }

	// range-based for loop
	PacketIterator<T, PacketList<T>> begin() { return PacketIterator<T, PacketList<T>>(*this, 0); }
	PacketIterator<T, PacketList<T>> end() { return PacketIterator<T, PacketList<T>>(*this, _count); }

private:
	T* _data;
	uint16 _count;
};

#pragma pack(1)
//[ PKT_S_TEST ][ 가변 데이터 ]
struct PKT_S_TEST
{
	struct BuffListItem
	{
		uint64 buffId;
		float remainTime;

		uint16 victimOffset;
		uint16 victimCount;
	};

	uint16 packetSize; // 2Byte
	uint16 packetId; // 2Byte
	uint64 id; // 8Byte
	uint32 hp; // 4Byte
	uint16 attack; // 2Byte

	uint16 buffOffset; // 2Byte
	uint16 buffCount; // 2Byte
};

class PKT_S_TEST_WRITE
{
public:
	using BuffsListItem = PKT_S_TEST::BuffListItem;
	using BuffsList = PacketList<PKT_S_TEST::BuffListItem>;
	using BuffVictimsList = PacketList<uint64>;

	PKT_S_TEST_WRITE(uint64 id, uint32 hp, uint16 attack)
	{
		_sendBuffer = GSendBufferManager->Open(4096);
		_bw = BufferWriter(_sendBuffer->Buffer(), _sendBuffer->AllocSize());

		_pkt = _bw.Reserve<PKT_S_TEST>();
		_pkt->packetSize = 0; // 채워줘야 함
		_pkt->packetId = S_TEST;
		_pkt->id = id;
		_pkt->hp = hp;
		_pkt->attack = attack;
		_pkt->buffOffset = 0; // 채워줘야 함
		_pkt->buffCount = 0; // 채워줘야 함
	}

	BuffsList ReserveBuffsList(uint16 buffCount)
	{
		BuffsListItem* firstBuffsListitem = _bw.Reserve<BuffsListItem>(buffCount);
		_pkt->buffOffset = (uint64)firstBuffsListitem - (uint64)_pkt;
		_pkt->buffCount = buffCount;
		return BuffsList(firstBuffsListitem, buffCount);
	}

	BuffVictimsList ReserveBuffVictimsList(BuffsListItem* buff, uint16 victimCount)
	{
		uint64* firstVictimListItem = _bw.Reserve<uint64>(victimCount);
		buff->victimOffset = (uint64)firstVictimListItem - (uint64)_pkt;
		buff->victimCount = victimCount;
		return BuffVictimsList(firstVictimListItem, victimCount);
	}

	SendBufferRef CloseAndReturn()
	{
		_pkt->packetSize = _bw.WriteSize();

		_sendBuffer->Close(_bw.WriteSize());
		return _sendBuffer;
	}
private:
	PKT_S_TEST* _pkt = nullptr;
	SendBufferRef _sendBuffer;
	BufferWriter _bw;
};
#pragma pack()

class ServerPacketHandler
{
public:

	static void HandlePacket(BYTE* buffer, int32 len);

};

