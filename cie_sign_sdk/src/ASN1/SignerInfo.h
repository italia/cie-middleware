// SignerInfo.h: interface for the CSignerInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIGNERINFO_H__ED6FFA3F_0A25_4A42_A3E5_BC704B9C25B3__INCLUDED_)
#define AFX_SIGNERINFO_H__ED6FFA3F_0A25_4A42_A3E5_BC704B9C25B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ASN1Sequence.h"
#include "IssuerAndSerialNumber.h"
#include "AlgorithmIdentifier.h"
#include "ASN1Octetstring.h"
#include "ASN1UTCTime.h"
#include "TimeStampToken.h"
#include "disigonsdk.h"

class CSignerInfo : public CASN1Sequence  
{
public:
	CSignerInfo(UUCBufferedReader& reader);
	
	CSignerInfo(const CASN1Object& signerInfo);

	CSignerInfo(const CIssuerAndSerialNumber& issuer, const CAlgorithmIdentifier& digestAlgo, const CAlgorithmIdentifier& encAlgo, const CASN1OctetString& encDigest);	
	
	void addAuthenticatedAttributes(const CASN1SetOf& attributes);
	void addUnauthenticatedAttributes(const CASN1SetOf& attributes);

	CASN1OctetString getEncryptedDigest();
	CAlgorithmIdentifier getDigestAlgorithn();
	CIssuerAndSerialNumber getIssuerAndSerialNumber();
	CASN1SetOf getAuthenticatedAttributes();
	CASN1SetOf getUnauthenticatedAttributes();
	
	CTimeStampToken getTimeStampToken();
	CASN1UTCTime getSigningTime();
	//CASN1ObjectIdentifier getSigningCertificateV2();
	CASN1OctetString getContentHash();
	
	CASN1SetOf getCountersignatures();
	void setCountersignatures(int index, CSignerInfo& countersignature);
	void addCountersignatures(CSignerInfo& countersignature);
	int getCountersignatureCount();
	bool hasTimeStampToken();

	int verifyCountersignature(int i, CASN1SetOf& certificates);
	int verifyCountersignature(int i, CASN1SetOf& certificates, const char* szDateTime, REVOCATION_INFO* pRevocationInfo);
	
	void setTimeStampToken(CTimeStampToken& tst);
	
	virtual ~CSignerInfo();
	
	static CCertificate getSignatureCertificate(CSignerInfo& signature, CASN1SetOf& certificates);
	
	static int verifySignature(CASN1OctetString& source, CSignerInfo& sinfo, CASN1SetOf& certificates, const char* date, REVOCATION_INFO* pRevocationInfo);

};

#endif // !defined(AFX_SIGNERINFO_H__ED6FFA3F_0A25_4A42_A3E5_BC704B9C25B3__INCLUDED_)
