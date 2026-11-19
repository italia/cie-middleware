#pragma once

#include "IAS.h"
#include "disigonsdk.h"

class CIESign
{
private:
	IAS* ias;

public:
	CIESign(IAS *ias);
	~CIESign() = default;

	uint16_t sign(const char* inFilePath, const char* type, const char* pin,
	              int page, float x, float y, float w, float h,
	              const char* imagePathFile, const char* outFilePath);
};
