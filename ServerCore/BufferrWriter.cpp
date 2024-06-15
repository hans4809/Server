#include "pch.h"
#include "BufferrWriter.h"

BufferWriter::BufferWriter()
{
}

BufferWriter::BufferWriter(BYTE* buffer, int32 size, uint32 pos)
	: _buffer(buffer), _size(size), _pos(pos)
{
}

BufferWriter::~BufferWriter()
{
}

bool BufferWriter::Write(void* src, uint32 len)
{
	if(FreeSize() < len)
		return false;

	::memcpy(&_buffer[_pos], src, len);
	_pos += len;
	return true;
}