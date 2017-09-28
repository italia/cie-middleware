#pragma once
#include "Array.h"
#include <map>

typedef std::map<BYTE,ByteArray> tlvMap;
typedef std::map<BYTE, ByteDynArray> tlvCreateMap;

class  CTLV
{
	tlvMap map;
public:
	CTLV(ByteArray &data);
	~CTLV(void);
	RESULT getValue(BYTE Tag,ByteArray &Value);
	RESULT getTAG(BYTE Tag,ByteArray *&Value);
};

class  CTLVCreate
{
public:
	tlvCreateMap map;

	CTLVCreate(void);
	~CTLVCreate(void);
	RESULT addValue(BYTE Tag,ByteDynArray *&Value);
	RESULT getValue(BYTE Tag,ByteDynArray *&Value);
	RESULT setValue(BYTE Tag,ByteArray &Value);
	RESULT getBuffer(ByteDynArray &Value);
};
