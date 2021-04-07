// Certificate.h: interface for the CCertificate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CERTIFICATE_H__2DF2B808_9398_479F_9FD2_9A229517EF9D__INCLUDED_)
#define AFX_CERTIFICATE_H__2DF2B808_9398_479F_9FD2_9A229517EF9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ASN1Sequence.h"
#include "CertificateInfo.h"
#include "AlgorithmIdentifier.h"
#include "ASN1Octetstring.h"

//#include "ASN1BitString.h"
#include "ASN1UTCTime.h"
#include "Crl.h"
#include "disigonsdk.h"



class CCertificate : public CASN1Sequence  
{
public:

	CCertificate (UUCBufferedReader& reader);
	
	CCertificate(const BYTE* value, long len);

	//CCertificate (const CCertificateInfo& certInfo, const CAlgorithmIdentifier& algoId, const CASN1BitString& signature);

	CCertificate (const CASN1Object& cert);

	virtual ~CCertificate();

	CCertificateInfo		getCertificateInfo();

	CAlgorithmIdentifier	getAlgorithmIdentifier();
	
	CASN1OctetString		getAuthorithyKeyIdentifier();
    CASN1OctetString      getSubjectKeyIdentifier();
    
	CASN1Sequence			getCertificatePolicies();

	CASN1Sequence			getQCStatements();

	bool isNonRepudiation();

	int verifyStatus(REVOCATION_INFO* pRevocationInfo);
	int verifyStatus(const char* szTime, REVOCATION_INFO* pRevocationInfo);
	bool verifySignature(CCertificate& cert);
	int verify();

	CName getIssuer();
	CASN1Integer getSerialNumber();
	CName getSubject();
	
	CASN1UTCTime getExpiration();	
	CASN1UTCTime getFrom();
	
	CASN1Sequence getExtensions();

	bool isQualified();
	bool isValid();
	bool isValid(const char* szDateTime);
	bool isSHA256();
	CASN1Sequence getExtension(const CASN1ObjectIdentifier& oid);
	
	static CCertificate* createCertificate(UUCByteArray& contentArray);
};

#endif // !defined(AFX_CERTIFICATE_H__2DF2B808_9398_479F_9FD2_9A229517EF9D__INCLUDED_)
