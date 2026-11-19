/*
 *  SignerInfoGenerator.cpp
 *  iDigitalSApp
 *
 *  Created by svp on 11/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "SignedDataGeneratorEx.h"
#include "ASN1/ASN1Setof.h"
#include "ASN1/ASN1Octetstring.h"
#include <time.h>
#include "ASN1/ASN1Sequence.h"
#include "ASN1/ASN1ObjectIdentifier.h"
#include "ASN1/ASN1UTCTime.h"
#include "ASN1/AlgorithmIdentifier.h"
#include "Certificate.h"
#include "ASN1/IssuerAndSerialNumber.h"

SignedDataGeneratorEx::SignedDataGeneratorEx(CSignedDocument& sd)
{
	m_signerInfos = sd.getSignerInfos();
	m_certificates = sd.getCertificates();
	m_digestAlgos = sd.getDigestAlgos();
	
	if(!sd.isDetached())
		sd.getContent(m_content);
}

SignedDataGeneratorEx::~SignedDataGeneratorEx()
{
}

bool SignedDataGeneratorEx::isDetached()
{
	return m_content.getLength() == 0;
}

void SignedDataGeneratorEx::setContent(const BYTE* content, int len)
{
	m_content.append(content, len);
}

	
void SignedDataGeneratorEx::addSigners(CSignedDocument& sd)
{
	CASN1SetOf signerInfos = sd.getSignerInfos();
	int size = signerInfos.size();
	for(int i = 0; i <  size; i++)
	{
		m_signerInfos.addElement(signerInfos.elementAt(i));
	}
	
	CASN1SetOf certificates = sd.getCertificates();
	size = certificates.size();
	for(int i = 0; i <  size; i++)
	{
		m_certificates.addElement(certificates.elementAt(i));
	}
	
}

void SignedDataGeneratorEx::addCounterSignature(CSignerInfo& signerInfoRef, CSignedDocument& counterSignature)
{
	// il signeddocument contiene solo un signerinfo ritornato dal webservice
	CSignerInfo signerInfoToAdd = counterSignature.getSignerInfos().elementAt(0);
	
	int size = m_signerInfos.size();
	
	
	for(int i = 0; i < size; i++)
	{
		CSignerInfo si = m_signerInfos.elementAt(i);
		bool res = addCounterSignature(si, signerInfoRef, signerInfoToAdd);
		if (res) 
		{
			m_signerInfos.setElementAt(si, i);
			
			CASN1SetOf certificates = counterSignature.getCertificates();
			size = certificates.size();
			for(int i = 0; i <  size; i++)
			{
				m_certificates.addElement(certificates.elementAt(i));
			}
			
			return;
		}
	}
	
//	NSLog(@"Warning: signerInfo reference not found when adding a countersignature");
}

void SignedDataGeneratorEx::addCounterSignature(CSignerInfo& signerInfoRef, CSignedDocument& counterSignature, CTimeStampResponse& tsr)
{
	// il signeddocument contiene solo un signerinfo ritornato dal webservice
	CSignerInfo signerInfoToAdd = counterSignature.getSignerInfos().elementAt(0);
	
	CTimeStampToken tst(tsr.getTimeStampToken());
	signerInfoToAdd.setTimeStampToken(tst);
	
	int size = m_signerInfos.size();
	
	
	for(int i = 0; i < size; i++)
	{
		CSignerInfo si = m_signerInfos.elementAt(i);
		bool res = addCounterSignature(si, signerInfoRef, signerInfoToAdd);
		if (res) 
		{
			m_signerInfos.setElementAt(si, i);
			
			CASN1SetOf certificates = counterSignature.getCertificates();
			size = certificates.size();
			for(int i = 0; i <  size; i++)
			{
				m_certificates.addElement(certificates.elementAt(i));
			}
			
			return;
		}
	}
	
//	NSLog(@"Warning: signerInfo reference not found when adding a countersignature");
}

bool SignedDataGeneratorEx::addCounterSignature(CSignerInfo& signerInfo, CSignerInfo& signerInfoRef, CSignerInfo& counterSignature)
{
	if(signerInfo == signerInfoRef)
	{
		signerInfo.addCountersignatures(counterSignature);
		return true;
	}
	
	CASN1SetOf countersignatures(signerInfo.getCountersignatures());
	
	int size = countersignatures.size();
	
	for(int i = 0; i < size; i++)
	{
		CSignerInfo si = countersignatures.elementAt(i);
		if(addCounterSignature(si, signerInfoRef, counterSignature))
		{
			signerInfo.setCountersignatures(i, si);
			return true;
		}
	}
	
	return false;	
}

void SignedDataGeneratorEx::setTimestamp(CTimeStampResponse& tsr, int signerInfoIndex)
{
	CSignerInfo si = m_signerInfos.elementAt(signerInfoIndex);
	CTimeStampToken tst = tsr.getTimeStampToken();
	si.setTimeStampToken(tst);
	m_signerInfos.setElementAt(si, signerInfoIndex);
}


void SignedDataGeneratorEx::toByteArray(UUCByteArray& pkcs7SignedData)
{
	// Crea signedData
	CSignedData* pSignedData;
	if(m_content.getLength() == 0) // detached
	{
		CContentType contentType(szDataOID);
		pSignedData = new CSignedData(m_digestAlgos,
									  CContentInfo(contentType),
									  m_signerInfos,
									  m_certificates);
	}
	else
	{
		// Crea signedData
		pSignedData = new CSignedData(m_digestAlgos,
									  CContentInfo(CASN1ObjectIdentifier(szDataOID), CASN1OctetString(m_content)),
									  m_signerInfos,
									  m_certificates);
		
	}

	// Infine crea ContentInfo
	CContentInfo contentInfo(szSignedDataOID, *pSignedData);
	
	contentInfo.toByteArray(pkcs7SignedData);	
	
	delete pSignedData;
}