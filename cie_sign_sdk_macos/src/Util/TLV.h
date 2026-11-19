#pragma once
#include "Array.h"
#include <map>

typedef std::map<uint8_t, ByteArray> tlvMap;
typedef std::map<uint8_t, ByteDynArray> tlvCreateMap;

class  CTLV
{
	tlvMap map;
public:
	CTLV(ByteArray &data);
	~CTLV();
	ByteArray getValue(uint8_t Tag);
	ByteArray *getTAG(uint8_t Tag);
};

class  CTLVCreate
{
public:
	tlvCreateMap map;

	CTLVCreate();
	~CTLVCreate();
	ByteDynArray* addValue(uint8_t Tag);
	ByteDynArray* getValue(uint8_t Tag);
	void setValue(uint8_t Tag, ByteArray &Value);
	ByteDynArray getBuffer();
};
