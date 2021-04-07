// ASN1UTCTime.h: interface for the CASN1UTCTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASN1UTCTIME_H__794C3E2C_270F_44D5_8A0F_40E56ED13484__INCLUDED_)
#define AFX_ASN1UTCTIME_H__794C3E2C_270F_44D5_8A0F_40E56ED13484__INCLUDED_

#pragma once

#include "ASN1Object.h"

class CASN1UTCTime : public CASN1Object
{
public:
	private:
	
	static const BYTE TAG;

public:

	//Costruttori
	CASN1UTCTime(UUCBufferedReader& reader);
	
	CASN1UTCTime(const char* szUTCTime);

	CASN1UTCTime(const CASN1Object& obj);
	
	// Distruttore
	virtual ~CASN1UTCTime();

	void getUTCTime(char* szTime);
};

#endif // !defined(AFX_ASN1UTCTIME_H__794C3E2C_270F_44D5_8A0F_40E56ED13484__INCLUDED_)
