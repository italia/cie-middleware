#pragma once

#include "../CSP/IAS.h"
#include "disigonsdk.h"

class CIESign
{
private:
	IAS* ias;



public:
	CIESign(IAS *ias);

	~CIESign();

	//sign: (NSString*)nis url : (NSURL*)url type : (NSString*)type pin : (NSString*)pin page : (int)page x : (float)x y : (float)y w : (float)w h : (float)h response : (NSMutableString*)response
	
	uint16_t sign(const char* inFilePath, const char* type, const char* pin, int page, float x, float y, float w, float h, const char* imagePathFile, const char* outFilePath);


};

