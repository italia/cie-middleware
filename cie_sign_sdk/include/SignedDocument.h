/*
 *  SignedDocument.h
 *  iDigitalS
 *
 *  Created by svp on 05/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "definitions.h"
#include "ASN1/UUCByteArray.h"
#include "ASN1/ContentInfo.h"
#include "ASN1/SignedData.h"
#include "ASN1/SignerInfo.h"
#include <map>
#include "disigonsdk.h"


class CSignedDocument 
{
public:
	CSignedDocument(const BYTE* content, int len);
	CSignedDocument(const CSignedDocument& signedDocument);
	
	virtual ~CSignedDocument();
	
	int verify();
	int verify(const char* dateTime);
	int verify(int index, REVOCATION_INFO* pRevocationInfo);
	int verify(int index, const char* dateTime, REVOCATION_INFO* pRevocationInfo);
	
	int getSignerCount();
	CASN1SetOf getSignerInfos();
	CASN1SetOf getCertificates();
	CASN1SetOf getDigestAlgos();
	
	CSignerInfo getSignerInfo(int index);
	CCertificate getSignerCertificate(int index);
	void getContent(UUCByteArray& content);

	//static void init(map<string, CCertificate*> certMap);
	
	void makeDetached();
	
	void toByteArray(UUCByteArray& signedData);
	
	CSignedData getSignedData();
	
	bool isDetached();
	void setContent(UUCByteArray& content);


	// 0 successivo al 30 Giugno 2011, 1 successivo al 30 agosto 2010, 2 precedente al 30 agosto 2010
	static int get452009Range(char* szDateTime);
	
private:
	CContentInfo* m_pCMSSignedData;
	CSignedData* m_pSignedData;
	CASN1SetOf m_signerInfos;
	CASN1SetOf m_certificates;
	
};
