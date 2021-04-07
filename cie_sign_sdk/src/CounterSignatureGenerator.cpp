/*
 *  CounterSignatureGenerator.cpp
 *  iDigitalSApp
 *
 *  Created by svp on 13/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "CounterSignatureGenerator.h"


CounterSignatureGenerator::CounterSignatureGenerator(CSignedDocument& signedDoc, int signerInfoIndex)
: m_signedDoc(signedDoc), m_signerInfoIndex(signerInfoIndex), m_signerInfo(m_signedDoc.getSignerInfo(m_signerInfoIndex))
{
	m_signerInfos = signedDoc.getSignerInfos();
	m_certificates = signedDoc.getCertificates();
	m_digestAlgos = signedDoc.getDigestAlgos();
}

CounterSignatureGenerator::~CounterSignatureGenerator()
{
}

void CounterSignatureGenerator::setSigningCertificate(const BYTE* certificate, int certlen, const BYTE* certHash, int certHashLen)
{
	m_signingCertificate.append(certificate, certlen);
	m_signerInfoGenerator.setSigningCertificate(certificate, certlen, certHash, certHashLen);
}

void CounterSignatureGenerator::getContent(UUCByteArray& content)
{
	CASN1OctetString signatureValue(m_signerInfo.getEncryptedDigest());
	const UUCByteArray* pValue = signatureValue.getValue();
	
	content.append(pValue->getContent(), pValue->getLength());
}

void CounterSignatureGenerator::setContentHash(const BYTE* hash, int hashlen)
{
	m_signerInfoGenerator.setContentHash(hash, hashlen);
}

void CounterSignatureGenerator::setSignature(const BYTE* signature, int siglen)
{
	m_signerInfoGenerator.setSignature(signature, siglen);
}

void CounterSignatureGenerator::setTimestampToken(const BYTE* timestampToken, int tstlen)
{
	m_signerInfoGenerator.setTimestampToken(timestampToken, tstlen);
}

void CounterSignatureGenerator::getSignedAttributes(UUCByteArray& signedAttribute)
{
	m_signerInfoGenerator.getSignedAttributes(signedAttribute, true, true);
}

void CounterSignatureGenerator::toByteArray(UUCByteArray& pkcs7SignedData)
{	
	CSignerInfo signerInfoCounterSignature(m_signerInfoGenerator.getSignerInfo());
	
	CASN1Sequence v;
	v.addElement(CASN1ObjectIdentifier(szCounterSignatureOID)); // id-aa-timeStampToken
	CASN1SetOf countersignature;
	countersignature.addElement(signerInfoCounterSignature);
	v.addElement(countersignature);
		
	CASN1SetOf unauthAttributes;
	
	if(m_signerInfo.size() == 7)
	{
		unauthAttributes = m_signerInfo.getUnauthenticatedAttributes();
	}

	unauthAttributes.addElement(v);
	m_signerInfo.addUnauthenticatedAttributes(unauthAttributes);
	
	m_signerInfos.setElementAt(m_signerInfo, m_signerInfoIndex);
	
	CCertificate cert(m_signingCertificate);
	int i = 0;
	for(i = 0; i < m_certificates.size(); i++)
	{
		if(m_certificates.elementAt(i) == cert)
			break;
	}
	
	if(i == m_certificates.size())
		m_certificates.addElement(cert);
	
	UUCByteArray content;
	m_signedDoc.getContent(content);
	
	// Crea signedData
	CSignedData signedData(m_digestAlgos,
						   CContentInfo(CAlgorithmIdentifier(szDataOID), CASN1OctetString(content)),
						   m_signerInfos,
						   m_certificates);
	
	// Infine crea ContentInfo
	CContentInfo contentInfo(szSignedDataOID, signedData);
	
	contentInfo.toByteArray(pkcs7SignedData);
	

}



