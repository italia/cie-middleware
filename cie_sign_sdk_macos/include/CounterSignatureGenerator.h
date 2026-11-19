/*
 *  CounterSignatureGenerator.h
 *  iDigitalSApp
 *
 *  Created by svp on 13/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//#include "ASN1/utiltypes.h"
#include "UUCByteArray.h"
#include "SignerInfoGenerator.h"
#include "SignedDocument.h"

class CounterSignatureGenerator 
{
public:
	CounterSignatureGenerator(CSignedDocument& signedDoc, int signerInfoIndex);
	
	virtual ~CounterSignatureGenerator();
		
	void getContent(UUCByteArray& content);
	
	void setContentHash(const BYTE* hash, int hashlen);
	
	void setSigningCertificate(const BYTE* certificate, int certlen, const BYTE* certHash, int certHashLen);
	
	void setSignature(const BYTE* signature, int siglen);
	
	void setTimestampToken(const BYTE* timestampToken, int tstlen);
	
	void getSignedAttributes(UUCByteArray& signedAttribute);
	
	void toByteArray(UUCByteArray& signedDoc);
	
private:
	CSignedDocument m_signedDoc;
	CSignerInfo m_signerInfo;	
	int m_signerInfoIndex;
		
	UUCByteArray m_signingCertificate;
	CASN1SetOf m_signerInfos;// = new DerSet(signerInfos);
	CASN1SetOf m_certificates;// = new DerSet(certificates);
	CASN1SetOf m_digestAlgos;// = new DerSet(certificates);
	
	CSignerInfoGenerator m_signerInfoGenerator;
};
