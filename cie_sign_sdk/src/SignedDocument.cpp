/*
 *  CSignedDocument.cpp
 *  iDigitalS
 *
 *  Created by svp on 05/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "SignedDocument.h"
#include "ASN1/ASN1Object.h"
#include "ASN1/UUCBufferedReader.h"
#include "ASN1/RSAPublicKey.h"
#include "RSA/rsaeuro.h"
#include "RSA/rsa.h"
#include "RSA/sha2.h"
#include "ASN1/DigestInfo.h"
#include <sys/types.h>
#include "Certificate.h"
#include "ASN1/Crl.h"
#include "Base64.h"
#include "UUCLogger.h"

USE_LOG;

CSignedDocument::CSignedDocument(const BYTE* content, int len)
: m_pCMSSignedData(NULL), m_pSignedData(NULL)
{
	LOG_DBG((0, "--> CSignedDocument", "CSignedDocument: %d", len));

	UUCByteArray c;
	
	if(content[0] == 'M' || content[0] == '-' ) // base64
	{
		try
		{
			LOG_DBG((0, "CSignedDocument", "base64"));

			char* szBase64Content;
			char* szContent = new char[len + 1];
			char* szEncoded = new char[len + 1];

			memcpy(szContent, content, len);

			//LOG_DBG((0, "CSignedDocument", "1"));

			szContent[len] = 0;
			char* pos = strstr(szContent, "--");
			
			if(pos != NULL) // header PEM da eliminare
			{
				szBase64Content = strtok(szContent, "\r\n");
				szBase64Content = strtok(NULL, "----");
			}
			else 
			{
				szBase64Content = szContent;
			}
			
			strcpy(szEncoded, "");

			//LOG_DBG((0, "CSignedDocument", "2"));

			char* szPart = strtok(szBase64Content, "\r\n");
			//LOG_DBG((0, "CSignedDocument", "3: %s", szPart));

			while(szPart)
			{
				if(strlen(szEncoded) + strlen(szPart) > len)
					throw -1;

				strcat(szEncoded, szPart);
				szPart = strtok(NULL, "\r\n");
			}

			size_t decLen = base64_decoded_size(strlen(szEncoded));
			char* szDecoded = base64_decode(szEncoded);

			c.append((BYTE*)szDecoded, decLen);

			//LOG_DBG((0, "CSignedDocument", "append"));
			delete szContent;
			delete szEncoded;
		}
		catch(...)
		{
			//LOG_DBG((0, "CSignedDocument", "No Base64"));
			throw 1;
		}
	}
	else 
	{
		//LOG_DBG((0, "CSignedDocument", "DER"));

		c.append(content, len);
	}

	//LOG_DBG((0, "CSignedDocument", "Create Buffered Reader"));

	if(!(c.get(0) == 0x30 && (c.get(1) & 0x80)))
	{
		//LOG_ERR((0, "CSignedDocument", "No PKCS#7"));
		throw -6;
	}

	UUCBufferedReader r(c);

	//LOG_DBG((0, "CSignedDocument", "Buffered Reader OK"));

	m_pCMSSignedData = new CContentInfo(r);
	//LOG_DBG((0, "CSignedDocument", "SignedData OK"));
	if(!m_pCMSSignedData->getContentType().equals(CASN1ObjectIdentifier(szSignedDataOID)))
	{
		//LOG_DBG((0, "CSignedDocument", "No Signed Data"));
		delete m_pCMSSignedData; 
		throw -1;
	}
	
	//LOG_DBG((0, "CSignedDocument", "SignedData"));

	m_pSignedData = new CSignedData(m_pCMSSignedData->getContent());

	//LOG_DBG((0, "CSignedDocument", "CreateBuffered Reader"));

	//int count = m_pSignedData->size();
	m_signerInfos = m_pSignedData->getSignerInfos();
	
	//LOG_DBG((0, "CSignedDocument", "SignerInfos OK"));

	m_certificates = m_pSignedData->getCertificates();
	
	//LOG_DBG((0, "CSignedDocument", "Certificates OK"));	
}

CSignedData CSignedDocument::getSignedData()
{
	return *m_pSignedData;
}

void CSignedDocument::toByteArray(UUCByteArray& signedData)
{
	m_pCMSSignedData->toByteArray(signedData);
}

CSignedDocument::CSignedDocument(const CSignedDocument& CSignedDocument)
{
	m_pCMSSignedData = new CContentInfo(*CSignedDocument.m_pCMSSignedData);	
	m_pSignedData = new CSignedData(m_pCMSSignedData->getContent());
	//int count = m_pSignedData->size();
	m_signerInfos = m_pSignedData->getSignerInfos();
	m_certificates = m_pSignedData->getCertificates();
}

CSignedDocument::~CSignedDocument()
{
	
	if(m_pCMSSignedData)
		delete m_pCMSSignedData;
	
	//if(m_pSignerInfos)
	//	delete m_pSignerInfos;
	
	if(m_pSignedData)
		delete m_pSignedData;
	
	/*if(m_pCertificates)
		delete m_pCertificates;
	*/
	
}
/*
void CSignedDocument::init(map<string, CCertificate*> certMap)
{
	g_certMap = certMap;
}
*/

CASN1SetOf CSignedDocument::getSignerInfos()
{
	return m_signerInfos;
}

CASN1SetOf CSignedDocument::getCertificates()
{
	return m_certificates;
}


CASN1SetOf CSignedDocument::getDigestAlgos()
{
	return m_pSignedData->getDigestAlgorithmIdentifiers();
}

void CSignedDocument::makeDetached()
{
	m_pSignedData->makeDetached();
}

void CSignedDocument::setContent(UUCByteArray& content)
{
	m_pSignedData->setContent(content);
}

int CSignedDocument::verify()
{
	return verify(NULL);
}

int CSignedDocument::verify(const char* dateTime)
{
	
	int bitmask = 0;
	// verifica la firma per ciascun signer
	for(int i = 0; i < m_signerInfos.size(); i++)
	{
		bitmask |= verify(i, dateTime, NULL);
	}
	
	return bitmask;
}

int CSignedDocument::verify(int i, REVOCATION_INFO* pRevocationInfo)
{
	return verify(i, NULL, pRevocationInfo);
}

int CSignedDocument::verify(int i, const char* dateTime, REVOCATION_INFO* pRevocationInfo)
{
	/**
	 * verify that the given certificate successfully handles and confirms
	 * the signature associated with this signer and, if a signingTime
	 * attribute is available, that the certificate was valid at the time the
	 * signature was generated.
	 */
	
	return m_pSignedData->verify(i, dateTime, pRevocationInfo);
}

int CSignedDocument::getSignerCount()
{
	return m_signerInfos.size();
}

CSignerInfo CSignedDocument::getSignerInfo(int index)
{
	return m_signerInfos.elementAt(index);
}

void CSignedDocument::getContent(UUCByteArray& content)
{
	CASN1OctetString octetString(m_pSignedData->getContentInfo().getContent());
	
	// content
	if(octetString.getTag() == 0x24) // contructed octet string
	{
		CASN1Sequence contentArray(octetString);
		int size = contentArray.size();
		for(int i = 0; i < size; i++)
		{
			CASN1OctetString part(contentArray.elementAt(i));
			content.append(part.getValue()->getContent(), part.getLength());
		}
	}
	else	
	{
		content.append(octetString.getValue()->getContent(), octetString.getLength());
	}
}

CCertificate CSignedDocument::getSignerCertificate(int index)
{
	CSignerInfo sinfo = m_signerInfos.elementAt(index);
	
	CIssuerAndSerialNumber issuerAndSerialNumber = 
		sinfo.getIssuerAndSerialNumber();

	for(int i = 0; i < m_certificates.size(); i++)
	{
		CCertificate cert = m_certificates.elementAt(i); 
		CName issuer = cert.getIssuer();
		CASN1Integer serialNumber = cert.getSerialNumber();
		
		CIssuerAndSerialNumber issuerAndSerial(issuer, serialNumber, false);
		
		if(issuerAndSerial == issuerAndSerialNumber)
		{
			return cert;
		}
	}
	
	throw -1;
}

bool CSignedDocument::isDetached()
{
	return m_pSignedData->getContentInfo().size() == 1;
	//CContentInfo ci(m_pSignedData->getContentInfo());
	//return ci.size() == 1;
}

// 0 successivo al 30 Giugno 2011, 1 successivo al 30 agosto 2010, 2 precedente al 30 agosto 2010
int CSignedDocument::get452009Range(char* szDateTime)
{
	int flag452009 = 0;  

	if(szDateTime)
	{
		try 
		{
			CASN1UTCTime trentaGiugno2011("110630000000Z");
		
			CASN1UTCTime dateTime(szDateTime);
		
			// verifica 30 giugno 2011
			int len = dateTime.getLength() > trentaGiugno2011.getLength() ? trentaGiugno2011.getLength() : dateTime.getLength();
		
			if(memcmp(dateTime.getValue()->getContent(), trentaGiugno2011.getValue()->getContent(), len) > 0)
			{
				flag452009 = 0;
			}
			else 
			{
				CASN1UTCTime trentaAgosto2010("100830000000Z");
				len = dateTime.getLength() > trentaAgosto2010.getLength() ? trentaAgosto2010.getLength() : dateTime.getLength();
			
				if(memcmp(dateTime.getValue()->getContent(), trentaAgosto2010.getValue()->getContent(), len) > 0)
				{
					flag452009 = 1;
				}	
				else 
				{
					flag452009 = 2;
				}
			}
		}
		catch (...) 
		{
			//NSLog(@"DateTime parsing exception %s", szDateTime);
		}
	}
	
	return flag452009;
}



