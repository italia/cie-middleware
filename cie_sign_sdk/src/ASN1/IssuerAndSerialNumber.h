// IssuerAndSerialNumber.h: interface for the CIssuerAndSerialNumber class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISSUERANDSERIALNUMBER_H__0AE18C2B_4100_4D86_98D3_167D05277E42__INCLUDED_)
#define AFX_ISSUERANDSERIALNUMBER_H__0AE18C2B_4100_4D86_98D3_167D05277E42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ASN1Sequence.h"
#include "ASN1Integer.h"
#include "Name.h"


class CIssuerAndSerialNumber : public CASN1Sequence  
{
public:
	CIssuerAndSerialNumber(UUCBufferedReader& reader);
	
	CIssuerAndSerialNumber(const CASN1Object& issuerAndSerNum);

	CIssuerAndSerialNumber(const CName& issuer, const CASN1Integer& serNum, bool contextSpecific);
	
	virtual ~CIssuerAndSerialNumber();
};

#endif // !defined(AFX_ISSUERANDSERIALNUMBER_H__0AE18C2B_4100_4D86_98D3_167D05277E42__INCLUDED_)
