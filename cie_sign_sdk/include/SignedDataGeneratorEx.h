/*
 *  SignedDataGenerator.h
 *  iDigitalS
 *
 *  Created by svp on 05/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//#include "ASN1/utiltypes.h"
#include "ASN1/UUCByteArray.h"
#include "SignedDocument.h"
#include "ASN1/SignerInfo.h"
#include "ASN1/ASN1Setof.h"
#include "ASN1/TimeStampResponse.h"

#include <vector>

class SignedDataGeneratorEx
{
public:
	SignedDataGeneratorEx(CSignedDocument& sd);

	virtual ~SignedDataGeneratorEx();

	bool isDetached();
	
	void setContent(const BYTE* content, int len);

	void addSigners(CSignedDocument& sd);
	
	void addCounterSignature(CSignerInfo& signerInfoRef, CSignedDocument& countersignature);
	
	void addCounterSignature(CSignerInfo& signerInfoRef, CSignedDocument& counterSignature, CTimeStampResponse& tsr);

	void setTimestamp(CTimeStampResponse& tsr, int signerInfoIndex);
	
		
	void toByteArray(UUCByteArray& pkcs7SignedData);
	
private:
	UUCByteArray m_content;
				
	CASN1SetOf m_signerInfos;
	CASN1SetOf m_certificates;
	CASN1SetOf m_digestAlgos;

	bool addCounterSignature(CSignerInfo& signerInfo, CSignerInfo& signerInfoRef, CSignerInfo& counterSignature);
};
