// SignedData.h: interface for the CSignedData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIGNEDDATA_H__C408FDA9_5C26_4F85_8073_EA7278527011__INCLUDED_)
#define AFX_SIGNEDDATA_H__C408FDA9_5C26_4F85_8073_EA7278527011__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ASN1Sequence.h"
#include "AlgorithmIdentifier.h"
#include "ContentInfo.h"
#include "Certificate.h"
#include "disigonsdk.h"

class CSignedData : public CASN1Sequence  
{
public:
	CSignedData(UUCBufferedReader& reader);
	
	CSignedData(const CASN1Object& signedData);

	CSignedData(const CASN1SetOf& algos, const CContentInfo& contentInfo, const CASN1SetOf& signerInfos, const CASN1SetOf& certificates);

	virtual ~CSignedData();

	CASN1SetOf getDigestAlgorithmIdentifiers();
	
	CContentInfo getContentInfo();
	
	CASN1SetOf getSignerInfos();
	
	CASN1SetOf getCertificates();

	CCertificate getSignerCertificate(int index);

	void makeDetached();

	void setContent(UUCByteArray& content);

	int verify(int i);
	
	int verify(int i, const char* dateTime, REVOCATION_INFO* pRevocationInfo);
	
};

#endif // !defined(AFX_SIGNEDDATA_H__C408FDA9_5C26_4F85_8073_EA7278527011__INCLUDED_)
