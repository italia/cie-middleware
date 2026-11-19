#pragma once
#include "Array.h"
#include <map>

typedef std::map<uint8_t, ByteArray> tlvMap;
typedef std::map<uint8_t, ByteDynArray> tlvCreateMap;

class  CTLV
{
	tlvMap map;
public:
	CTLV(const ByteArray &data);
	~CTLV(void);
	ByteArray getValue(uint8_t Tag);
	ByteArray *getTAG(uint8_t Tag);
};

class  CTLVCreate
{
public:
	tlvCreateMap map;

	CTLVCreate(void);
	~CTLVCreate(void);
	ByteDynArray* addValue(uint8_t Tag);
	ByteDynArray* getValue(uint8_t Tag);
	void setValue(uint8_t Tag, const ByteArray &Value);
	ByteDynArray getBuffer();
};
