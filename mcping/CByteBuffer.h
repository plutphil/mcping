#pragma once
#include<stdio.h>
#include<SFML/Network.hpp>

#include <WinSock2.h>
#include <Ws2tcpip.h>
using namespace sf;
#define UInt8 Uint8
#define UInt16 Uint16
#define UInt32 Uint32
#define UInt64 Uint64
#define  int i=
#define PUTBYTES


class ByteBuffer {
	//char* buffer;
	//size_t size;
	//size_t writepos;
	sf::TcpSocket* socket;
	ByteBuffer(sf::TcpSocket* psocket) {
		this->socket = psocket;
	}
	bool write(const void* ptr, size_t size) {
		this->socket->send(ptr, size);
	}
	
#undef  ntohll
#define ntohll(x) (((static_cast<UInt64>(ntohl(static_cast<UInt32>(x)))) << 32) + ntohl(x >> 32))

	inline UInt64 HostToNetwork8(const void* a_Value)
	{
		UInt64 buf;
		memcpy(&buf, a_Value, sizeof(buf));
		buf = (((static_cast<UInt64>(htonl(static_cast<UInt32>(buf)))) << 32) + htonl(buf >> 32));
		return buf;
	}
	inline UInt32 HostToNetwork4(const void* a_Value)
	{
		UInt32 buf;
		memcpy(&buf, a_Value, sizeof(buf));
		buf = ntohl(buf);
		return buf;
	}
	inline double NetworkToHostDouble8(const void* a_Value)
	{
		UInt64 buf = 0;
		memcpy(&buf, a_Value, 8);
		buf = ntohll(buf);
		double x;
		memcpy(&x, &buf, sizeof(double));
		return x;
	}
	inline Int64 NetworkToHostLong8(const void* a_Value)
	{
		UInt64 buf;
		memcpy(&buf, a_Value, 8);
		buf = ntohll(buf);
		return *reinterpret_cast<Int64*>(&buf);
	}
	inline UInt64 NetworkToHostULong8(const void* a_Value)
	{
		UInt64 buf;
		memcpy(&buf, a_Value, 8);
		buf = ntohll(buf);
		return buf;
	}
	inline float NetworkToHostFloat4(const void* a_Value)
	{
		UInt32 buf;
		float x;
		memcpy(&buf, a_Value, 4);
		buf = ntohl(buf);
		memcpy(&x, &buf, sizeof(float));
		return x;
	}
#define WriteBuf write
#define Write write
	bool WriteVarInt32(UInt32 a_Value)
	{
		// A 32-bit integer can be encoded by at most 5 bytes:
		unsigned char b[5];
		size_t idx = 0;
		do
		{
			b[idx] = (a_Value & 0x7f) | ((a_Value > 0x7f) ? 0x80 : 0x00);
			a_Value = a_Value >> 7;
			idx++;
		} while (a_Value > 0);

		return WriteBuf(b, idx);
	}

	bool WriteBEInt8(Int8 a_Value)
	{
		
			
		
		return WriteBuf(&a_Value, 1);
	}





	bool WriteBEUInt8(UInt8 a_Value)
	{
		
		
			
		
		return WriteBuf(&a_Value, 1);
	}





	bool WriteBEInt16(Int16 a_Value)
	{
		
			
		PUTBYTES(2);
		UInt16 val;
		memcpy(&val, &a_Value, 2);
		val = htons(val);
		return WriteBuf(&val, 2);
	}





	bool WriteBEUInt16(UInt16 a_Value)
	{
		
			
		PUTBYTES(2);
		a_Value = htons(a_Value);
		return WriteBuf(&a_Value, 2);
	}





	bool WriteBEInt32(Int32 a_Value)
	{
		
			
		PUTBYTES(4);
		UInt32 Converted = HostToNetwork4(&a_Value);
		return WriteBuf(&Converted, 4);
	}





	bool WriteBEUInt32(UInt32 a_Value)
	{
		
			
		PUTBYTES(4);
		UInt32 Converted = HostToNetwork4(&a_Value);
		return WriteBuf(&Converted, 4);
	}





	bool WriteBEInt64(Int64 a_Value)
	{
		
			
		PUTBYTES(8);
		UInt64 Converted = HostToNetwork8(&a_Value);
		return WriteBuf(&Converted, 8);
	}





	bool WriteBEUInt64(UInt64 a_Value)
	{
		
			
		PUTBYTES(8);
		UInt64 Converted = HostToNetwork8(&a_Value);
		return WriteBuf(&Converted, 8);
	}





	bool WriteBEFloat(float a_Value)
	{
		
			
		PUTBYTES(4);
		UInt32 Converted = HostToNetwork4(&a_Value);
		return WriteBuf(&Converted, 4);
	}





	bool WriteBEDouble(double a_Value)
	{
		
			
		PUTBYTES(8);
		UInt64 Converted = HostToNetwork8(&a_Value);
		return WriteBuf(&Converted, 8);
	}





	bool WriteBool(bool a_Value)
	{
		
			
		UInt8 val = a_Value ? 1 : 0;
		return Write(&val, 1);
	}





	bool WriteVarInt32(UInt32 a_Value)
	{
		
			

		// A 32-bit integer can be encoded by at most 5 bytes:
		unsigned char b[5];
		size_t idx = 0;
		do
		{
			b[idx] = (a_Value & 0x7f) | ((a_Value > 0x7f) ? 0x80 : 0x00);
			a_Value = a_Value >> 7;
			idx++;
		} while (a_Value > 0);

		return WriteBuf(b, idx);
	}





	bool WriteVarInt64(UInt64 a_Value)
	{
		
			

		// A 64-bit integer can be encoded by at most 10 bytes:
		unsigned char b[10];
		size_t idx = 0;
		do
		{
			b[idx] = (a_Value & 0x7f) | ((a_Value > 0x7f) ? 0x80 : 0x00);
			a_Value = a_Value >> 7;
			idx++;
		} while (a_Value > 0);

		return WriteBuf(b, idx);
	}





	bool WriteVarUTF8String(const std::string& a_Value)
	{
		
			
		PUTBYTES(a_Value.size() + 1);  // This is a lower-bound on the bytes that will be actually written. Fail early.
		bool res = WriteVarInt32(static_cast<UInt32>(a_Value.size()));
		if (!res)
		{
			return false;
		}
		return WriteBuf(a_Value.data(), a_Value.size());
	}





	bool WritePosition64(Int32 a_BlockX, Int32 a_BlockY, Int32 a_BlockZ)
	{
		
			
		return WriteBEInt64(
			(static_cast<Int64>(a_BlockX & 0x3FFFFFF) << 38) |
			(static_cast<Int64>(a_BlockY & 0xFFF) << 26) |
			(static_cast<Int64>(a_BlockZ & 0x3FFFFFF))
		);
	}


};