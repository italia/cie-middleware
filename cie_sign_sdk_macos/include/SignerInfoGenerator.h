/*
 *  SignerInfoGenerator.h
 *  iDigitalSApp
 *
 *  Created by svp on 11/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "definitions.h"
#include "UUCByteArray.h"
#include "IssuerAndSerialNumber.h"
#include "ASN1/SignerInfo.h"

class CSignerInfoGenerator 
{
public:
	CSignerInfoGenerator();
	
	virtual ~CSignerInfoGenerator();
	
	
	void setContent(const BYTE* content, int len);
	
	void setContentHash(const BYTE* hash, int hashlen);
	
	void setSigningCertificate(const BYTE* certificate, int certlen, const BYTE* certHash, int certHashLen);
	
	void setSignature(const BYTE* signature, int siglen);
	
	void setTimestampToken(const BYTE* timestampToken, int tstlen);

	void setTimestampToken(const CTimeStampToken* pTimestampToken);
	
	void getSignedAttributes(UUCByteArray& signedAttribute, bool countersignature, bool signingTime);
	
	void toByteArray(UUCByteArray& signerInfo);
	
	CSignerInfo getSignerInfo();
	
private:
	UUCByteArray m_content;
	UUCByteArray m_contentHash;
	UUCByteArray m_signingCertificate;
	UUCByteArray m_signature;
	UUCByteArray m_signedAttributes;
	CASN1SetOf m_unsignedAttributes;
	UUCByteArray m_certificateHash;
	UUCByteArray m_timeStampToken;
	CASN1SetOf m_counterSignatures;
	void buildUnsignedAttributes();
	CName* m_pIssuer;
	CASN1Integer* m_pSerialNumber;
	
};
