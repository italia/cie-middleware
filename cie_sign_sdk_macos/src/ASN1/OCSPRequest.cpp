/*
 *  OCSPRequest.cpp
 *  iDigitalSApp
 *
 *  Created by svp on 14/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

/*
 OCSPRequest     ::=     SEQUENCE {
 tbsRequest                  TBSRequest,
 optionalSignature   [0]     EXPLICIT Signature OPTIONAL }
 
 TBSRequest      ::=     SEQUENCE {
 version             [0] EXPLICIT Version DEFAULT v1,
 requestorName       [1] EXPLICIT GeneralName OPTIONAL,
 requestList             SEQUENCE OF Request,
 requestExtensions   [2] EXPLICIT Extensions OPTIONAL }
 
 Signature       ::=     SEQUENCE {
 signatureAlgorithm   AlgorithmIdentifier,
 signature            BIT STRING,
 certs                [0] EXPLICIT SEQUENCE OF Certificate OPTIONAL }
 
 Version  ::=  INTEGER  {  v1(0) }
 
 Request ::=     SEQUENCE {
 reqCert                    CertID,
 singleRequestExtensions    [0] EXPLICIT Extensions OPTIONAL }
 
 CertID ::= SEQUENCE {
 hashAlgorithm            AlgorithmIdentifier,
 issuerNameHash     OCTET STRING, -- Hash of Issuer's DN
 issuerKeyHash      OCTET STRING, -- Hash of Issuers public key
 serialNumber       CertificateSerialNumber }
 
 issuerNameHash is the hash of the Issuer's distinguished name. The
 hash shall be calculated over the DER encoding of the issuer's name
 field in the certificate being checked. issuerKeyHash is the hash of
 the Issuer's public key. The hash shall be calculated over the value
 (excluding tag and length) of the subject public key field in the
 issuer's certificate. The hash algorithm used for both these hashes,
 is identified in hashAlgorithm. serialNumber is the serial number of
 the certificate for which status is being requested.
 */

#include "OCSPRequest.h"
#include "ASN1OptionalField.h"
#include "AlgorithmIdentifier.h"
#include "sha1.h"
#include <CertStore.h>

//#import <UIKit/UIKit.h>

COCSPRequest::COCSPRequest(UUCBufferedReader& reader)
: CASN1Sequence(reader)
{
	
}

COCSPRequest::COCSPRequest(const CASN1Object& ocspRequest)
: CASN1Sequence(ocspRequest)
{
}

COCSPRequest::COCSPRequest(CCertificate& certificate)
{
	CASN1Sequence tbsRequest;
	
	CASN1Sequence requestList;
	
	CASN1Sequence request;
	
	CASN1Sequence certId;
	
	CASN1Integer serialNumber(certificate.getSerialNumber());
	
	CAlgorithmIdentifier hashAlgorithm(szSHA1OID);
	
	CName issuerName(certificate.getIssuer());
	
	UUCByteArray baIssuerName;
	issuerName.toByteArray(baIssuerName);
	
	
	// calcola l'hash SHA1
	SHA1Context sha;
	
	// hash issuername
	SHA1Reset(&sha);
	
	SHA1Input(&sha, baIssuerName.getContent(), baIssuerName.getLength());
	
	SHA1Result(&sha);
	
	char szAux[100];
	sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
	
	UUCByteArray baIssuerNameHash(szAux);
	UUCByteArray baIssuerKeyHash;
	
	// hash public key
	UUCByteArray baPubKey;

	/*
	// trova il cert di CA
	// estrae l'issuer cert
	UUCByteArray issuer;
	issuerName.getNameAsString(issuer);//getField(OID_COMMON_NAME);
	CCertificate* pCACert = CCertStore::GetCertificate((char*)issuer.getContent());
	if(pCACert)
	{		
		CSubjectPublicKeyInfo spki(pCACert->getCertificateInfo().getSubjectPublicKeyInfo());
		
		CASN1BitString pubKey(spki.getPublicKey());
		
		UUCByteArray *pbaPubKey = (UUCByteArray*)pubKey.getValue();
		if(pbaPubKey->getContent()[0] == 0) // PAD bits
			baPubKey.append(pbaPubKey->getContent() + 1, pbaPubKey->getLength() - 1);	
		else 
			baPubKey.append(*pbaPubKey);

		//pubKey.toByteArray(baPubKey);
		
		//ßNSLog(@"pubkey: %s", baPubKey.toHexString());
		
		SHA1Reset(&sha);
		
		SHA1Input(&sha, baPubKey.getContent(), baPubKey.getLength());
		
		SHA1Result(&sha);
		
		sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
		
		UUCByteArray baPubKeyHash(szAux);
		baIssuerKeyHash.append(baPubKeyHash);
	}
	else */
	{
		//NSLog(@"WARNING! CA Cert not found %s", issuer.c_str());
		
		CASN1Sequence authorityKeyIdentifier(certificate.getAuthorithyKeyIdentifier());
		CASN1OctetString keyIdentifier(authorityKeyIdentifier.elementAt(0));
		keyIdentifier.setTag(0x04); // set the correct tag
		
		baIssuerKeyHash.append(*keyIdentifier.getValue());
	}

		
	certId.addElement(hashAlgorithm);
	certId.addElement(CASN1OctetString(baIssuerNameHash));
	certId.addElement(CASN1OctetString(baIssuerKeyHash));
	certId.addElement(serialNumber);
	
	request.addElement(certId);
	
	requestList.addElement(request);
	
	tbsRequest.addElement(requestList);
	
	addElement(tbsRequest);
}






/*
COCSPRequest::COCSPRequest(CCertificate& certificate)
{
	CASN1Sequence tbsRequest;
	//CASN1Integer version(0);
	
	//tbsRequest.addElement(CASN1OptionalField(version, 0xA0));
	
	CASN1Sequence requestList;
	
	CASN1Sequence request;
	
	CASN1Sequence certId;
	
	CASN1Integer serialNumber(certificate.getSerialNumber());
	
	CAlgorithmIdentifier hashAlgorithm(szSHA1OID);
	
	CName issuerName(certificate.getIssuer());
	//CSubjectPublicKeyInfo pubKeyInfo(caCertificate.getCertificateInfo().getSubjectPublicKeyInfo());
	
	UUCByteArray baIssuerName;
	issuerName.toByteArray(baIssuerName);
	
	//UUCByteArray baPubKey;
	//pubKeyInfo.getPublicKey().toByteArray(baPubKey);
	
	// calcola l'hash SHA1
	SHA1Context sha;
	
	// hash issuername
	SHA1Reset(&sha);
	
	SHA1Input(&sha, baIssuerName.getContent(), baIssuerName.getLength());
	
	SHA1Result(&sha);
		
	char szAux[100];
	sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
	
	UUCByteArray baIssuerNameHash(szAux);
	
	// hash public key
	
	CASN1Sequence authorityKeyIdentifier(certificate.getAuthorithyKeyIdentifier());
	CASN1OctetString keyIdentifier(authorityKeyIdentifier.elementAt(0));
	keyIdentifier.setTag(0x04);
	//const UUCByteArray* pbaKeyIdentifier = authorityKeyIdentifier.elementAt(0).getValue();
	//CASN1OctetString keyIdentifier(*pbaKeyIdentifier);
	
	/*
	SHA1Reset(&sha);
	
	SHA1Input(&sha, baPubKey.getContent(), baPubKey.getLength());
	
	SHA1Result(&sha);
	
	for(i = 0; i < 5 ; i++)
	{
		sprintf(szAux, "%08X ", sha.Message_Digest[i]);
	}
	
	UUCByteArray baPubKeyHash(szAux);
	*/
/*
	certId.addElement(hashAlgorithm);
	certId.addElement(CASN1OctetString(baIssuerNameHash));
	certId.addElement(keyIdentifier);
	certId.addElement(serialNumber);
	
	request.addElement(certId);
	
	requestList.addElement(request);
	
	tbsRequest.addElement(requestList);
	
	addElement(tbsRequest);
}
*/
/*
 // estrae l'issuer cert
 string issuer = issuerName.getField(OID_COMMON_NAME);
 
 UUCByteArray baIssuerKeyHash;
 if(g_certMap.find(issuer) != g_certMap.end())
 {
 CCertificate* pCACert = g_certMap[issuer];
 
 CSubjectPublicKeyInfo spki(pCACert->getCertificateInfo().getSubjectPublicKeyInfo());
 
 CASN1BitString pubKey(spki.getPublicKey());
 UUCByteArray *baPubKey = (UUCByteArray*)pubKey.getValue();
 //spki.toByteArray(baPubKey);
 
 SHA1Reset(&sha);
 
 SHA1Input(&sha, baPubKey->getContent(), baPubKey->getLength());
 
 SHA1Result(&sha);
 
 sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
 
 UUCByteArray baPubKeyHash(szAux);
 baIssuerKeyHash.append(baPubKeyHash);
 }
 else 
 {
 //NSLog(@"Warning CA Cert not found");
 
 CASN1Sequence authorityKeyIdentifier(certificate.getAuthorithyKeyIdentifier());
 CASN1OctetString keyIdentifier(authorityKeyIdentifier.elementAt(0));
 keyIdentifier.setTag(0x04);
 
 keyIdentifier.toByteArray(baIssuerKeyHash);
 }
*/ 
