// ASN1BitString.h: interface for the CASN1BitString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASN1BITSTRING_H__C1B0BC02_DBEF_4919_902C_5C6C91AE9D25__INCLUDED_)
#define AFX_ASN1BITSTRING_H__C1B0BC02_DBEF_4919_902C_5C6C91AE9D25__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ASN1Object.h"
//#include "ASN1IntegerSequence.h"
#include "ASN1Integer.h"

class CASN1BitString : public CASN1Object  
{
public:
	//Costruttori
	CASN1BitString(UUCBufferedReader& reader);	

	CASN1BitString(const CASN1Object& obj);		
	
	// Distruttore
	virtual ~CASN1BitString();

private: 
	static const BYTE TAG;
};

#endif // !defined(AFX_ASN1BITSTRING_H__C1B0BC02_DBEF_4919_902C_5C6C91AE9D25__INCLUDED_)
