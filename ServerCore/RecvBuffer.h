#pragma once
class RecvBuffer
{
	enum { BUFFER_COUNT = 10 };
public:
	RecvBuffer(int32 bufferSize);
	~RecvBuffer();

	void Clean();
	bool OnRead(int32 numOfBytes);
	bool OnWrite(int32 numOfBytes);

	BYTE* ReadPos() { return &_buffer[_readPos]; }
	BYTE* WritePos() { return &_buffer[_writePos]; }
	int32 DataSize() { return _writePos - _readPos; }
	int32 FreeSize() { return _capacity - _writePos; }
private:
	int32 _capacity = 0; // 최대 버퍼 사이즈 = 단일 버퍼 사이즈 * 버퍼 갯수
	int32 _bufferSize = 0; // 단일 버퍼의 사이즈
	int32 _readPos = 0;
	int32 _writePos = 0;
	Vector<BYTE> _buffer;
};

