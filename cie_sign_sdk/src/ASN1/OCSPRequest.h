/*
 *  OCSPRequest.h
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
*/

#ifndef _OCSPRequest_H
#define _OCSPRequest_H


#include "ASN1Sequence.h"
#include "ASN1Integer.h"
#include "Certificate.h"

class COCSPRequest : public CASN1Sequence
{
	
public:	
	
	COCSPRequest(UUCBufferedReader& reader);
	
	COCSPRequest(const CASN1Object& contentInfo);
	
	COCSPRequest(CCertificate& certificate);

};

#endif //_OCSPRequest