/*
 *  SignerInfoGenerator.cpp
 *  iDigitalSApp
 *
 *  Created by svp on 11/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "SignerInfoGenerator.h"
#include "ASN1/ASN1Setof.h"
#include "ASN1/ASN1Octetstring.h"
#include <time.h>
#include "ASN1/ASN1Sequence.h"
#include "ASN1/ASN1ObjectIdentifier.h"
#include "ASN1/ASN1UTCTime.h"
#include "ASN1/AlgorithmIdentifier.h"
#include "Certificate.h"
#include "ASN1/IssuerAndSerialNumber.h"
#include "ASN1/SignerInfo.h"


CSignerInfoGenerator::CSignerInfoGenerator()
: m_pIssuer(NULL), m_pSerialNumber(NULL)//, m_pCounterSignature(NULL)
{
}

CSignerInfoGenerator::~CSignerInfoGenerator()
{
	if(m_pIssuer)
		delete m_pIssuer;
	
	if(m_pSerialNumber)
		delete m_pSerialNumber;
	
	//if(m_pCounterSignature)
	//	delete m_pCounterSignature;
}

void CSignerInfoGenerator::setContentHash(const BYTE* hash, int hashlen)
{
	m_contentHash.append(hash, hashlen);
}

void CSignerInfoGenerator::setSigningCertificate(const BYTE* certificate, int certlen, const BYTE* certHash, int certHashLen)
{
	m_signingCertificate.append(certificate, certlen);
	m_certificateHash.append(certHash, certHashLen);
    
    UUCBufferedReader reader(m_signingCertificate);
    CCertificate cert(reader);
    
    CCertificateInfo certInfo = cert.getCertificateInfo();
    
    m_pIssuer = new CName(certInfo.getIssuer());
    m_pSerialNumber = new CASN1Integer(certInfo.getSerialNumber());
}

void CSignerInfoGenerator::setSignature(const BYTE* signature, int siglen)
{
	m_signature.append(signature, siglen);
}

void CSignerInfoGenerator::setTimestampToken(const BYTE* timestampToken, int tstlen)
{
	m_timeStampToken.append(timestampToken, tstlen);
}

void CSignerInfoGenerator::setTimestampToken(const CTimeStampToken* pTimestampToken)
{
	pTimestampToken->toByteArray(m_timeStampToken);
}

void CSignerInfoGenerator::getSignedAttributes(UUCByteArray& signedAttribute, bool counterSignature, bool signingTime)
{
	CASN1SetOf authAttributes;	
	
	//char* szhex;
	if(!counterSignature) // nella countersignature il content type non deve essere presente
	{
		CASN1SetOf attrValues;
		CASN1Sequence attr;
	
		// content type
		CASN1ObjectIdentifier contentTypeOID(szContentTypeOID);
		attr.addElement(contentTypeOID);
		attrValues.addElement(CASN1ObjectIdentifier(szDataOID));
		attr.addElement(attrValues);
		authAttributes.addElement(attr);
	
		//szhex = (char*)authAttributes.toHexString();
		//NSLog([NSString stringWithCString:szhex]);
	}
		
    if(signingTime)
    {
        // Signing Time
        CASN1SetOf attrValues2;
        CASN1Sequence attr2;
            
        //CASN1SetOf attrValues;
        //CASN1Sequence attr;

        attr2.removeAll();
        char szTime[20];
        time_t now = time(NULL);
        strftime(szTime, 20, "%y%m%d%H%M%SZ", gmtime(&now));
        
        attr2.addElement(CASN1ObjectIdentifier(szSigningTimeOID));
        attrValues2.addElement(CASN1UTCTime(szTime));
        attr2.addElement(attrValues2);
        authAttributes.addElement(attr2);
    }
    
	// message digest
	CASN1SetOf attrValues1;
	CASN1Sequence attr1;
	
	attr1.addElement(CASN1ObjectIdentifier(szMessageDigestOID));
	attrValues1.addElement(CASN1OctetString(m_contentHash));
	attr1.addElement(attrValues1);
	authAttributes.addElement(attr1);
	
	//szhex = (char*)authAttributes.toHexString();
	//NSLog([NSString stringWithCString:szhex]);
	
	
	//szhex = (char*)authAttributes.toHexString();
	
	//NSLog([NSString stringWithCString:szhex]);
	
	// CaDES-BES
	
	// ESS-signing-certificate-v2
	CASN1SetOf attrValues3;
	CASN1Sequence attr3;
	
	attr3.addElement(CASN1ObjectIdentifier(szIdAASigningCertificateV2OID));
	
	CASN1Sequence certidv2s;
		
	certidv2s.addElement(CAlgorithmIdentifier(szSHA256OID));
	certidv2s.addElement(CASN1OctetString(m_certificateHash));
	
	//CASN1Sequence sequence;
	//sequence.addElement(CIssuerAndSerialNumber(*m_pIssuer, *m_pSerialNumber, true));
	certidv2s.addElement(CIssuerAndSerialNumber(*m_pIssuer, *m_pSerialNumber, false));
	
	CASN1Sequence signingCertificateV2;
	
	signingCertificateV2.addElement(certidv2s);
	
	CASN1Sequence innerSequence;
	innerSequence.addElement(signingCertificateV2);
	attrValues3.addElement(innerSequence);
	attr3.addElement(attrValues3);
	authAttributes.addElement(attr3);
	
	//szhex = (char*)authAttributes.toHexString();
	//NSLog([NSString stringWithCString:szhex]);
	
	m_signedAttributes.removeAll();
	
	authAttributes.toByteArray(m_signedAttributes);
	authAttributes.toByteArray(signedAttribute);
}



void CSignerInfoGenerator::toByteArray(UUCByteArray& signerInfoArray)
{
	// crea il SignerInfo
	CSignerInfo signerInfo = getSignerInfo();
	
	signerInfo.toByteArray(signerInfoArray);
}

CSignerInfo CSignerInfoGenerator::getSignerInfo()
{
	// crea il SignerInfo
	CSignerInfo signerInfo(CIssuerAndSerialNumber(*m_pIssuer, *m_pSerialNumber, false), CAlgorithmIdentifier(szSHA256OID), CAlgorithmIdentifier(szSha256WithRsaEncryptionOID), CASN1OctetString(m_signature));
    
    if(m_signedAttributes.getLength() > 0)
        signerInfo.addAuthenticatedAttributes(CASN1SetOf(m_signedAttributes));
	
	buildUnsignedAttributes();
	
	if(m_unsignedAttributes.getLength() != 0)
		signerInfo.addUnauthenticatedAttributes(m_unsignedAttributes);
	
	return signerInfo; 
}	

void CSignerInfoGenerator::buildUnsignedAttributes()
{
	m_unsignedAttributes.removeAll();

	if(m_timeStampToken.getLength() > 0)
	{
		CASN1Sequence v;
		v.addElement(CASN1ObjectIdentifier(szTimestampTokenOID)); // id-aa-timeStampToken
		CASN1SetOf tst;
		tst.addElement(m_timeStampToken);
		v.addElement(tst);
	
		m_unsignedAttributes.addElement(v);
	}
	
}
	
	
