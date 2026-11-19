// Certificate.cpp: implementation of the CCertificate class.
//
//////////////////////////////////////////////////////////////////////


#include "Certificate.h"
#include "ASN1Octetstring.h"
#include "../RSA/sha1.h"
#include "OCSPRequest.h"
#include "ASN1OptionalField.h"
#include "Crl.h"
#include "ASN1Exception.h"
#include "RSAPublicKey.h"
#include "../RSA/rsaeuro.h"
#include "../RSA/rsa.h"
#include "../RSA/sha2.h"
#include "DigestInfo.h"
#include <sys/types.h>
#include <time.h>
#include "RSAPublicKey.h"
#include "Base64.h"
#include <curl/curl.h>
#include "LdapCrl.h"
#include "UUCLogger.h"
#include "CertStore.h"
#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#define PROXY_AUTHENTICATION_REQUIRED	407

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
long HTTPRequest(UUCByteArray& data, const char* szUrl, const char* szContentType, UUCByteArray& response);

extern char g_szVerifyProxy[MAX_PATH];
extern char* g_szVerifyProxyUsrPass;
extern int g_nVerifyProxyPort;

USE_LOG;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCertificate* CCertificate::createCertificate(UUCByteArray& contentArray)
{
	const BYTE* content = contentArray.getContent();
	int len = contentArray.getLength();
	//UUCByteArray c;
	
	if(content[0] != 0x30) // base64
	{
		char* szBase64Content;
		char* szContent = new char[len + 1];
		char* szEncoded = new char[len + 1];

		memcpy(szContent, content, len);
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

		char* szPart = strtok(szBase64Content, "\r\n");
		while(szPart)
		{
			strcat(szEncoded, szPart);
			szPart = strtok(NULL, "\r\n");
		}

		size_t decLen = base64_decoded_size(strlen(szEncoded));
		char* szDecoded = base64_decode(szEncoded);
		
		if(!(szDecoded[0] == 0x30 && (szDecoded[1] & 0x80)))
		{
			free(szDecoded);
			//LOG_ERR((0, "CSignedDocument", "No X509 Cert"));
			throw -6;
		}

		UUCBufferedReader reader((BYTE*)szDecoded, decLen);

		delete szContent;
		
		CCertificate* pCert = new CCertificate(reader);
	
		free(szDecoded);

		return pCert;

	}
	else 
	{
//		if(!(content[0] == 0x30 && (content[1] & 0x80)))
//		{
//			//LOG_ERR((0, "CSignedDocument", "No X509 Cert"));
//			throw -6;
//		}

		//c.append(content, len);
		UUCBufferedReader reader((BYTE*)content, len);
		//c.append();
		
		//delete raw;
		//delete szContent;
		
		return new CCertificate(reader);
	}
	

}

CCertificate::CCertificate(const BYTE* value, long len)
: CASN1Sequence(value, len)
{	
	
}

CCertificate::CCertificate(UUCBufferedReader& reader)
:CASN1Sequence(reader)
{
	
}

/*
CCertificate::CCertificate(const CCertificateInfo& certInfo, const CAlgorithmIdentifier& algoId, const CASN1BitString& signature)
{		
	addElement(new CCertificateInfo(certInfo));
	addElement(new CAlgorithmIdentifier(algoId));
	addElement(new CASN1BitString(signature));
}	
*/

CCertificate::CCertificate(const CASN1Object& cert)
: CASN1Sequence(cert)
{

}

CCertificate::~CCertificate()
{

}

CCertificateInfo CCertificate::getCertificateInfo()
{
	return elementAt(0);
}

CAlgorithmIdentifier CCertificate::getAlgorithmIdentifier()
{
	return elementAt(1);	
}

CName CCertificate::getIssuer()
{
	return getCertificateInfo().getIssuer();
}

CName CCertificate::getSubject()
{
	return getCertificateInfo().getSubject();
}

CASN1Integer CCertificate::getSerialNumber()
{
	return getCertificateInfo().getSerialNumber();
}

CASN1UTCTime CCertificate::getExpiration()
{
	return getCertificateInfo().getExpiration();
}	

CASN1UTCTime CCertificate::getFrom()
{
	return getCertificateInfo().getFrom();
}	


CASN1Sequence CCertificate::getExtensions()
{
	return getCertificateInfo().getExtensions();
}	

CASN1Sequence CCertificate::getQCStatements()
{
	return getExtension(CASN1ObjectIdentifier("1.3.6.1.5.5.7.1.3"));
}

bool CCertificate::isNonRepudiation()
{
	CASN1Sequence keyUsage(getExtension(CASN1ObjectIdentifier("2.5.29.15")));
	int n = keyUsage.size(); 
	if(n == 0) // not found
		return false;
	

	CASN1OctetString octetString(keyUsage.elementAt(n-1));

	UUCByteArray content;

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

	UUCBufferedReader reader(content);
	CASN1BitString val(reader);

	const BYTE* pKeyUsageFlags = val.getValue()->getContent();
	BYTE unusedbits = pKeyUsageFlags[0];
	BYTE flags = pKeyUsageFlags[1];
	if(unusedbits < 7)
	{
		if((flags & 0x40) == 0x40)// non repudiation and digital signature
			return true;
	}

	return false;
}

bool CCertificate::isQualified()
{
	
	CASN1Sequence keyUsage(getExtension(CASN1ObjectIdentifier("2.5.29.15")));
	if(keyUsage.size() == 0) // not found
		return false;
	
	
	CASN1OctetString val(keyUsage.elementAt(1));
	const BYTE* pKeyUsageFlags = val.getValue()->getContent(); 
	if(!(pKeyUsageFlags[0] & 0x01)) // non repudiation
		return false;
	
	CASN1Sequence qcStatement(getExtension(CASN1ObjectIdentifier("1.3.6.1.5.5.7.1.3")));
	if(qcStatement.size() == 0) // not found
		return false;
	
	/* spostato in basso nell'apposito metodo
	 CAlgorithmIdentifier sha256Algo(szSHA256OID);
	CAlgorithmIdentifier digestAlgo(elementAt(1));
	if(!(digestAlgo.elementAt(0) == sha256Algo.elementAt(0)))
		return false;
	*/
	
	return true;
}

bool CCertificate::isSHA256()
{
		
	CAlgorithmIdentifier sha256Algo(szSHA256OID);
	CAlgorithmIdentifier digestAlgo(elementAt(1));
	if(!(digestAlgo.elementAt(0) == sha256Algo.elementAt(0)))
		return false;
	
	return true;
}
bool CCertificate::isValid()
{
	char szTime[20];
	time_t now = time(NULL);
	strftime(szTime, 20, "%y%m%d%H%M%SZ", localtime(&now));
	
	return isValid(szTime);
}

bool CCertificate::isValid(const char* szDateTime)
{
	if(!szDateTime)
		return isValid();
	
	CASN1UTCTime utcnow(szDateTime);
	
	CASN1UTCTime expiration(getExpiration());

	BYTE* btExpiration;
	
	if(expiration.getValue()->getLength() > 13)
	{
		btExpiration = (BYTE*)expiration.getValue()->getContent() + expiration.getValue()->getLength() - 13;
	}
	else 
	{
		btExpiration = (BYTE*)expiration.getValue()->getContent();
	}
	
	if(memcmp(utcnow.getValue()->getContent(), btExpiration, 13) > 0)
		return false;
	
	CASN1UTCTime from(getFrom());
	
	BYTE* btFrom;
	
	if(from.getValue()->getLength() > 13)
	{
		btFrom = (BYTE*)from.getValue()->getContent() + from.getValue()->getLength() - 13;
	}
	else 
	{
		btFrom = (BYTE*)from.getValue()->getContent();
	}
	
	if(memcmp(utcnow.getValue()->getContent(), btFrom, 13) < 0)
		return false;
	
	return true;
}


CASN1Sequence CCertificate::getExtension(const CASN1ObjectIdentifier& oid)
{
	CASN1Sequence certExtensions = getExtensions();
	CASN1Sequence extensions = certExtensions.elementAt(0);
	CASN1Sequence requestedExtension;
	int count = extensions.size();
	for(int i = 0; i < count; i++)
	{
		CASN1Sequence extension = extensions.elementAt(i);
		//int n = extension.size();
		//const char* szHex = ((UUCByteArray*)(extension.getValue()))->toHexString();
		CASN1ObjectIdentifier extoid = extension.elementAt(0);
		if(extoid.equals(oid))
		{
			return extension;
		
		}
	}
	
	return requestedExtension; 
}


CASN1Sequence CCertificate::getCertificatePolicies()
{
	CASN1Sequence certPolicies(getExtension(szCertificatePolicies));
	
	if(certPolicies.size() > 0)
	{
		CASN1OctetString val(certPolicies.elementAt(1));
	
		UUCBufferedReader reader(*val.getValue());
		return CASN1Sequence(reader);
	}
	else
	{
		return CASN1Sequence();
	}
}

CASN1OctetString CCertificate::getAuthorithyKeyIdentifier()
{
	CASN1Sequence keyIdentifier(getExtension(szAuthorityKeyIdentifier));
	
	CASN1OctetString val(keyIdentifier.elementAt(1));
	
	UUCBufferedReader reader(*val.getValue());
	return CASN1Sequence(reader);
}


CASN1OctetString CCertificate::getSubjectKeyIdentifier()
{
    
    CASN1Sequence keyIdentifier = getExtension(szSubjectKeyIdentifier);
    
    if(keyIdentifier.size() > 0)
    {
        CASN1OctetString val(keyIdentifier.elementAt(1));
        return val;
    }
    else
    {
        CASN1OctetString val("");
        return val;
    }
}

/*
 A.1 OCSP over HTTP
 
 
 This section describes the formatting that will be done to the
 request and response to support HTTP.
 
 A.1.1 Request
 
 
 HTTP based OCSP requests can use either the GET or the POST method to
 submit their requests. To enable HTTP caching, small requests (that
 after encoding are less than 255 bytes), MAY be submitted using GET.
 If HTTP caching is not important, or the request is greater than 255
 bytes, the request SHOULD be submitted using POST.  Where privacy is
 a requirement, OCSP transactions exchanged using HTTP MAY be
 protected using either TLS/SSL or some other lower layer protocol.
 
 An OCSP request using the GET method is constructed as follows:
 
 GET {url}/{url-encoding of base-64 encoding of the DER encoding of
 the OCSPRequest}
 
 where {url} may be derived from the value of AuthorityInfoAccess or
 other local configuration of the OCSP client.
 
 An OCSP request using the POST method is constructed as follows: The
 Content-Type header has the value "application/ocsp-request" while
 the body of the message is the binary value of the DER encoding of
 the OCSPRequest.
 
 A.1.2 Response
 
 
 An HTTP-based OCSP response is composed of the appropriate HTTP
 headers, followed by the binary value of the DER encoding of the
 OCSPResponse. The Content-Type header has the value
 "application/ocsp-response". The Content-Length header SHOULD specify
 the length of the response. Other HTTP headers MAY be present and MAY
 be ignored if not understood by the requestor.
*/

int CCertificate::verifyStatus(REVOCATION_INFO* pRevocationInfo)
{
	return verifyStatus(NULL, pRevocationInfo);
}

int CCertificate::verifyStatus(const char* szTime, REVOCATION_INFO* pRevocationInfo)
{
	LOG_DBG((0, "--> CCertificate::verifyStatus", "Time: %s", szTime));

	int status = REVOCATION_STATUS_UNKNOWN;
	
	CASN1Integer serialNumber(getSerialNumber());

	try 
	{
		// verifica la presenza di OCSP
		CASN1Sequence ocsp(getExtension(szAuthorityInfoAccess));
		if(ocsp.size() == 0)
			throw 1L;

		CASN1OctetString val(ocsp.elementAt(1));
		UUCByteArray* pbaVal = (UUCByteArray*)val.getValue();
		UUCBufferedReader reader(*pbaVal);
		//CASN1Sequence sequ(reader);
		
		CASN1Sequence authorityInfoAccess(reader);//sequ.elementAt(0));
		
		//AuthorityInfoAccessSyntax  ::=
		//SEQUENCE SIZE (1..MAX) OF AccessDescription
		//
		//AccessDescription  ::=  SEQUENCE {
		//	accessMethod          OBJECT IDENTIFIER,
		//	accessLocation        GeneralName  }

		// cerca il metodo OCSP
		CASN1ObjectIdentifier oid(szMethodOCSP);		
		int size = authorityInfoAccess.size();
		for(int i = 0; i < size; i++)
		{
			CASN1Sequence accessDescription(authorityInfoAccess.elementAt(i));
		
			if(oid.equals(accessDescription.elementAt(0)))
			{
				LOG_DBG((0, "CCertificate::verifyStatus", "createOCSP request"));

				CASN1Object accessLocation(accessDescription.elementAt(1));				
				UUCByteArray *pValue = (UUCByteArray*)accessLocation.getValue();
				pValue->append((BYTE)'\0');
				
				LOG_DBG((0, "CCertificate::verifyStatus", "OCSP Url: %s", (char*)pValue->getContent()));

				// prepara la OCSP request
				COCSPRequest ocspRequest(*this);
				UUCByteArray baOcspRequest;
				ocspRequest.toByteArray(baOcspRequest);
				//const char* sz1 = baOcspRequest.toHexString();
				//NSLog([NSString stringWithCString:sz1]);
				
				//string ocspurl;
				//ocspurl.append((char*)accessLocation.getValue()->getContent(), accessLocation.getLength());

				LOG_DBG((0, "CCertificate::verifyStatus", "POST OCSP Request"));

				UUCByteArray response;
				long nRet = HTTPRequest(baOcspRequest, (char*)pValue->getContent(), "application/ocsp-request", response);
				if(nRet)
				{
					LOG_ERR((0, "CCertificate::verifyStatus", "OCSP not available. Error: %x", nRet));
				}

				if(response.getLength() == 0)
				{
					LOG_ERR((0, "CCertificate::verifyStatus", "Empty OCSP response"));
					throw -1;
				}

				LOG_DBG((0, "CCertificate::verifyStatus", "OCSP OK"));

				// il response data va in un oggetto OCSResponse
				UUCBufferedReader reader(response);
				CASN1Sequence ocspResponse(reader);
				
				CASN1Integer responseStatus(ocspResponse.elementAt(0));
				
				int status = responseStatus.getIntValue(); 
				LOG_DBG((0, "CCertificate::verifyStatus", "OCSP responseStatus: %d", status));
				if(status == 0) // successfull
				{				
					CASN1Sequence responseBytes1(ocspResponse.elementAt(1));
					CASN1Sequence responseBytes(responseBytes1.elementAt(0));
					
					CASN1ObjectIdentifier responseType(responseBytes.elementAt(0));
					CASN1OctetString response(responseBytes.elementAt(1));
					
					const UUCByteArray* pVal = response.getValue();
					
					UUCBufferedReader reader1(*pVal);
					CASN1Sequence basicOCSPResponse(reader1);
					
					CASN1Sequence responseData(basicOCSPResponse.elementAt(0));
					
					CASN1Sequence responses(responseData.elementAt(2));
					
					CASN1Sequence singleResponse(responses.elementAt(0));
					
					//NSLog(@"%s", ((UUCByteArray*)singleResponse.getValue())->toHexString());
					
					
					CASN1Object certStatus(singleResponse.elementAt(1));
					CASN1UTCTime thisUpdate(singleResponse.elementAt(2));

					if(pRevocationInfo)
					{
						pRevocationInfo->nType = TYPE_OCSP;
						thisUpdate.getUTCTime(pRevocationInfo->szThisUpdate);
					}
					
					BYTE tag = certStatus.getTag();
					
					LOG_DBG((0, "CCertificate::verifyStatus", "certStatus: %d", tag));

					switch (tag & 0x0F) 
					{
						case 0:
							// good
							LOG_DBG((0, "CCertificate::verifyStatus", "Status GOOD"));
							status = REVOCATION_STATUS_GOOD;
							if(pRevocationInfo)
								pRevocationInfo->nRevocationStatus = REVOCATION_STATUS_GOOD;
							break;
						
						case 1:
							// revoked
							
							
							// verifica CRLReason
							{
								//CASN1Sequence certStatusSeq(certStatus);	
								//const UUCByteArray* pValue = certStatus.getValue();
								//UUCBufferedReader reader(*pValue);
								//CASN1Sequence crlReason(reader);
								CASN1Sequence clrReason(certStatus);
								
							try
							{
									// verifica la data rispetto al revocation time
									CASN1Object revocationTime(clrReason.elementAt(0));
								
									BYTE* btRevocationTime;
								
									if(revocationTime.getValue()->getLength() > 13)
									{
										btRevocationTime = (BYTE*)revocationTime.getValue()->getContent() + revocationTime.getValue()->getLength() -	13;
									}
									else 
									{
										btRevocationTime = (BYTE*)revocationTime.getValue()->getContent();
									}

									if(pRevocationInfo)
									{
										strncpy(pRevocationInfo->szRevocationDate, (char*)btRevocationTime, 13);
										pRevocationInfo->szRevocationDate[13] = 0;
									}

									if(szTime != NULL)
										if(memcmp(szTime, btRevocationTime, 13) < 0)
										{
											if(pRevocationInfo)
												pRevocationInfo->nRevocationStatus = REVOCATION_STATUS_GOOD;
											return REVOCATION_STATUS_GOOD;
										}
									
								
									CASN1OctetString reasonCode(clrReason.elementAt(1));
									const UUCByteArray *pVal = reasonCode.getValue();
								
									BYTE reason = pVal->getContent()[2];//reasonCode.getTag() & 0x0F;
									if(reason == 6) //Certificate HOLD
									{
										LOG_DBG((0, "CCertificate::verifyStatus", "Status SUSPENDED"));
										status = REVOCATION_STATUS_SUSPENDED;
									}
									else 
									{
										LOG_DBG((0, "CCertificate::verifyStatus", "Status REVOKED"));
										status = REVOCATION_STATUS_REVOKED;
									}
									if(pRevocationInfo)
										pRevocationInfo->nRevocationStatus = status;

				/*				
								CASN1Object reasonCode(crlReason.elementAt(0));
								BYTE reason = reasonCode.getTag() & 0x0F;
								if(reason == 6) //Certificate HOLD
									status = REVOCATION_STATUS_SUSPENDED;
								else 
									status = REVOCATION_STATUS_REVOKED;
				*/
							}
							catch(CASN1Exception* ex)
							{
								LOG_DBG((0, "CCertificate::verifyStatus", "Unexpected Exception"));
								delete ex;
								status = REVOCATION_STATUS_REVOKED;
							}
							}
							break;
							
						case 2:
							// unknown
							LOG_DBG((0, "CCertificate::verifyStatus", "Status UNKNONWN"));
							status = REVOCATION_STATUS_UNKNOWN;
							break;
							
						default:
							break;
					}
					
					if(status != REVOCATION_STATUS_UNKNOWN)
						return status;
				}
				
			}
		}					
	}
	catch(CASN1Exception* ex)
	{
		LOG_ERR((0, "CCertificate::verifyStatus", "Unexpected ASN1 Exception"));
		delete ex;
	}
	catch(long r)
	{
		LOG_MSG((0, "CCertificate::verifyStatus", "authorityInfoAccess OCSP not present. Error: %x", r));
	}
	catch(...)
	{
		LOG_ERR((0, "CCertificate::verifyStatus", "Unexpected Exception"));
	}

	try 
	{
		//char* sz;
		
		LOG_DBG((0, "CCertificate::verifyStatus", "Try CRL"));

		// verifica la crl
		CASN1Sequence crlDP1(getExtension(szCrlDistributionPointsOID));
		
		//sz = (char*)((UUCByteArray*)crlDP1.getValue())->toHexString();
		//NSLog([NSString stringWithCString:sz]);
		
		CASN1OctetString crlDPValue(crlDP1.elementAt(1));
		
		UUCBufferedReader reader(*(crlDPValue.getValue()));
		
		CASN1Sequence crlDP(reader);
		
		//sz = (char*)((UUCByteArray*)crlDP.getValue())->toHexString();
		//NSLog([NSString stringWithCString:sz]);
		
		int size = crlDP.size();
		if(size > 0)
		{
			for(int i = 0; i < size; i++)
			{
			
				CASN1Sequence dp(crlDP.elementAt(i));
			
				CASN1Sequence distributionPointName(dp.elementAt(0));
				
				CASN1Sequence fullName(distributionPointName.elementAt(0));
				
				CASN1Object name3(fullName.elementAt(0));
				
				UUCByteArray *pValue = (UUCByteArray *)name3.getValue();

				pValue->append((BYTE)'\0');

				char*  szcrlurl = (char*)pValue->getContent();

				LOG_DBG((0, "CCertificate::verifyStatus", "CRL Url: %s", szcrlurl));

				//crlurl.append((char*)name3.getValue()->getContent(), name3.getLength());
				
				UUCByteArray response;
/*				
				if(strstr(szcrlurl, "ldap") > 0)
	 			{
					long nRet = 1;
#ifdef WIN32
					nRet = getCRLFromLDAP(szcrlurl, response);
#else
					LOG_DBG((0, "CCertificate::verifyStatus", "CRL not available. Error: %x", 500));
					return status = REVOCATION_STATUS_NOTLOADED;//REVOCATION_STATUS_UNKNOWN;//REVOCATION_STATUS_GOOD;
#endif
					if(nRet)
					{
						LOG_ERR((0, "CCertificate::verifyStatus", "CRL not available. Error: %x", nRet));
						return REVOCATION_STATUS_NOTLOADED;
					}
					else
					{
						LOG_DBG((0, "CCertificate::verifyStatus", "CRL OK"));
					}

				}
				else 
				{*/
					UUCByteArray data;
					long nRet = HTTPRequest(data, szcrlurl, NULL, response);
					if(nRet)
					{
						LOG_ERR((0, "CCertificate::verifyStatus", "CRL not available. Error: %x", nRet));
						return REVOCATION_STATUS_NOTLOADED;
					}
					else
					{
						LOG_DBG((0, "CCertificate::verifyStatus", "CRL OK, nRet: %d", nRet));
					}
			//	}

			    if(response.getLength() > 0)
			    {				
					UUCBufferedReader reader(response.getContent(), response.getLength());
					//LOG_MSG((0, "CCertificate::verifyStatus", "proxy response: %s", response.getContent()));
					CCrl crl(reader);

                    int revstatus = 0;
					if(!crl.isRevoked(serialNumber, szTime, &revstatus, pRevocationInfo))
					{
						LOG_MSG((0, "CCertificate::verifyStatus", "Cert Status: GOOD: %d", revstatus));
						status = REVOCATION_STATUS_GOOD;
					}
                    else
                    {
                        LOG_MSG((0, "CCertificate::verifyStatus", "Cert Status: REVOKED: %d", revstatus));
                        status = REVOCATION_STATUS_REVOKED;
                    }
				}			
				else
				{
					LOG_ERR((0, "CCertificate::verifyStatus", "CRL Empty"));
					return REVOCATION_STATUS_NOTLOADED;
				}
			}
		}
	}
	catch(CASN1Exception* ex)
	{
		LOG_ERR((0, "CCertificate::verifyStatus", "Unexpected ASN1 Exception"));
		delete ex;
	}
	catch(...)
	{
		LOG_ERR((0, "CCertificate::verifyStatus", "Unexpected Exception"));
	}
	
	LOG_DBG((0, "<-- CCertificate::verifyStatus", "exit"));

	return status;
}

int CCertificate::verify()
{
	int bitmask = 0;

	// verifica la cert chain
    CCertificate* pCert = this;
	CCertificate* pCACert = CCertStore::GetCertificate(*pCert);
	while(pCACert && pCert->verifySignature(*pCACert) )
	{
		//NSLog(@"issuer: %s, SN: %s", issuer.c_str(), serialNumber.toHexString());
		
		bitmask |= VERIFIED_CACERT_FOUND;
        pCert = pCACert;
        pCACert = CCertStore::GetCertificate(*pCACert);
	}

    if(!pCACert)
    {
        //NSLog(@"CA Cert valid");
        bitmask |= VERIFIED_CERT_CHAIN;
    }
    else
    {
        //NSLog(@"CA Cert not valid");
    }
    
	return bitmask;
}

bool CCertificate::verifySignature(CCertificate& cert)
{
	//NSLog(@"Verify CERT signature");
	
    // OpenSSL
    UUCByteArray baCert;
    cert.toByteArray(baCert);
        
    X509 *x509 = NULL;
    
    const BYTE* content = baCert.getContent();
    x509 = d2i_X509(NULL, &content, baCert.getLength());
            
    EVP_PKEY *evp_pubkey;
    RSA *rsa_pubkey;

    evp_pubkey  = X509_get_pubkey(x509);
    rsa_pubkey  = EVP_PKEY_get1_RSA(evp_pubkey);
    
	CASN1BitString encryptedDigest(elementAt(2));	
	UUCByteArray encDigest(*(UUCByteArray*)encryptedDigest.getValue());
	encDigest.remove(0);
	
    BYTE decrypted[MAX_RSA_MODULUS_LEN];
    unsigned int len = MAX_RSA_MODULUS_LEN;
    
    const BYTE* encrypted = encDigest.getContent();
    const int encrypted_len = (int)encDigest.getLength();
    
    len = RSA_public_decrypt(encrypted_len, (BYTE*)encrypted, decrypted, rsa_pubkey,RSA_PKCS1_PADDING);
    
    RSA_free(rsa_pubkey);
    EVP_PKEY_free(evp_pubkey);
    X509_free(x509);
    
    
    if(len)
    {
        //NSLog(@"RSA OK");
        try
        {
            
			char szAux[100];

			UUCBufferedReader reader(decrypted, len);
			CDigestInfo digestInfo(reader);
			//UUCByteArray* pauxdi = (UUCByteArray*)digestInfo.getValue(); 
			//szHex = pauxdi->toHexString();
			
			CASN1OctetString digest = digestInfo.getDigest();
			UUCByteArray* pDigestValue = (UUCByteArray*)digest.getValue();
			//szHex = pDigestValue->toHexString();
			
			// content
			UUCByteArray content;
			getCertificateInfo().toByteArray(content);
			
			
			BYTE* buff;
			int bufflen = 0;
			
			buff = (BYTE*)content.getContent();
			bufflen = content.getLength();
			
			CAlgorithmIdentifier digestAlgo(digestInfo.getDigestAlgorithm());
			CAlgorithmIdentifier sha256Algo(szSHA256OID);
			CAlgorithmIdentifier sha1Algo(szSHA1OID);
			if(digestAlgo.elementAt(0) == sha256Algo.elementAt(0))
			{				
				//bitmask |= VERIFIED_SHA256;
				
				//NSLog(@"SHA256");
				
				
				BYTE hash[32];
				BYTE hash2[32];
	/*
				sha256_context	ctx256;
				sha256_starts(&ctx256);
				sha256_update(&ctx256, buff, bufflen);	
				sha256_finish(&ctx256, hash);

				sha256_context	ctx2561;
				sha256_starts(&ctx2561);
				sha256_update(&ctx2561, content.getContent(), content.getLength());	
				sha256_finish(&ctx2561, hash2);
*/
				sha2(buff, bufflen, hash, 0);
				sha2(content.getContent(), content.getLength(), hash2, 0);
/*
				SHA256_CTX	ctx256;				
				SHA256_Init(&ctx256);				
				SHA256_Update(&ctx256, buff, bufflen);				
				SHA256_Final(hash, &ctx256);
				
				SHA256_CTX	ctx2561;			
				SHA256_Init(&ctx2561);				
				SHA256_Update(&ctx2561, content.getContent(), content.getLength());				
				SHA256_Final(hash2, &ctx2561);
*/				
				if(memcmp(hash, pDigestValue->getContent(), 32) == 0)
				{
					// verifica l'hash del content
					if(memcmp(hash2, hash, 32) == 0)
					{
						//NSLog(@"Verify OK");
						
						return true;
					}
				}
				else 
				{
					//NSLog(@"Digest not match");
				}

			}
			else if(digestAlgo.elementAt(0) == sha1Algo.elementAt(0)) //if(digestAlgo == CAlgorithmIdentifier(szSHA1OID))
			{
				//NSLog(@"SHA1");
				
				// calcola l'hash SHA1
				SHA1Context sha;
				
				SHA1Reset(&sha);
				
				SHA1Input(&sha, buff, bufflen);
				
				SHA1Result(&sha);
				
				sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
				
				UUCByteArray hashaux(szAux);
				
				SHA1Reset(&sha);
				SHA1Input(&sha, content.getContent(), content.getLength());
				SHA1Result(&sha);
				sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
				UUCByteArray contentHash(szAux);
				
				
				if(memcmp(hashaux.getContent(), pDigestValue->getContent(), hashaux.getLength()) == 0)
				{
					// verifica l'hash del content
					if(memcmp(contentHash.getContent(), hashaux.getContent(), contentHash.getLength()) == 0)
					{
						//NSLog(@"Verify OK");
						
						return true;
					}
				}
				else 
				{
					//NSLog(@"Digest not match");
				}
			}
			else 
			{
//				NSLog(@"Warning HASH Algo not found");
			}

        }
        catch(CASN1Exception* ex)
        {
            delete ex;
            //NSLog(@"certificate:verify - ASN1Exception");
            return false;
        }
        catch(...)
        {
            //NSLog(@"certificate:verify - Generic exception");
            return false;
        }
    }
	
	return false;
}


/*
CASN1BitString& CCertificate::getSubjectPublicKeyInfo()
{
	return *(CASN1BitString*)getElementAt(2);	
}
*/											 
/*
CASN1BitString& CCertificate::getSignature()
{
	return *(CASN1BitString*)getElementAt(2);	
}
 */


long HTTPRequest(UUCByteArray& data, const char* szUrl, const char* szContentType, UUCByteArray& response)
{    
	//general initilization
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
	CURL* ctx = curl_easy_init();

	//set URL
	curl_easy_setopt(ctx, CURLOPT_URL, szUrl);
	
	curl_easy_setopt(ctx, CURLOPT_SSL_VERIFYPEER, false);

	if(data.getLength() > 0)
	{
		//set POST method
		curl_easy_setopt(ctx, CURLOPT_POST, 1);

		//give the data you want to post
		curl_easy_setopt(ctx, CURLOPT_POSTFIELDS, data.getContent());
		LOG_ERR((0, "HTTPRequest", "POST data content: %s", data.getContent()));
		//give the data lenght
		curl_easy_setopt(ctx, CURLOPT_POSTFIELDSIZE, data.getLength());
	}

	if(g_nVerifyProxyPort != -1)
	{
		LOG_MSG((0, "HTTPRequest", "Proxy: %s, %d", g_szVerifyProxy, g_nVerifyProxyPort));

		//set the proxy
		curl_easy_setopt(ctx, CURLOPT_PROXY, g_szVerifyProxy);

		//set the proxy type
		curl_easy_setopt(ctx, CURLOPT_PROXYTYPE,  CURLPROXY_HTTP);

		if(g_nVerifyProxyPort != 0)
		{
			LOG_MSG((0, "HTTPRequest", "Proxy Port: %d", g_nVerifyProxyPort));

			curl_easy_setopt(ctx, CURLOPT_PROXYPORT, g_nVerifyProxyPort);			
		}
	
		if(g_szVerifyProxyUsrPass != NULL)
		{
			LOG_MSG((0, "HTTPRequest", "Proxy UserPass: %s", g_szVerifyProxyUsrPass));
			curl_easy_setopt(ctx, CURLOPT_PROXYUSERPWD, g_szVerifyProxyUsrPass);			
		}
	}
    
	//set the callback function that handle the data return from server
	//if you don't set this, the return data just show up on the screen
	//size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userp)
	curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, WriteCallback);

	/* we pass our 'chunk' struct to the callback function */ 
	curl_easy_setopt(ctx, CURLOPT_WRITEDATA, (void *)&response);

	struct curl_slist *headers = NULL;

	if(szContentType)
	{
		char szAux[256];
		sprintf(szAux, "Content-Type: %s", szContentType);

		headers = curl_slist_append(headers, szAux);

		curl_easy_setopt(ctx, CURLOPT_HTTPHEADER, headers);
	}

	//let's do it...
	CURLcode ret = curl_easy_perform(ctx);

	LONG responseCode;

	/* Check for errors */
	if (ret != CURLE_OK)
	{
		//fprintf(stderr, "curl_easy_perform() failed: %s\n",
		//      curl_easy_strerror(ret));

		LOG_ERR((0, "HTTPRequest", "Unable to connect to: %s", szUrl));

		return ret;
	}

	CURLcode httpCode = curl_easy_getinfo(ctx, CURLINFO_RESPONSE_CODE, &responseCode);

	LOG_ERR((0, "HTTPRequest", "HttpCode: %d", responseCode));
	
	if (responseCode == PROXY_AUTHENTICATION_REQUIRED)
	{
		LOG_ERR((0, "HTTPRequest", "Unable to connect to: %s. Proxy authentication required", szUrl));
		return responseCode;
	}

	LOG_ERR((0, "HTTPRequest", "connect to: %s OK", szUrl));

	//clean up
	if(headers)
		curl_slist_free_all(headers);

	curl_easy_cleanup(ctx);

	if (response.getLength() == 0)
	{
		LOG_ERR((0, "<-- HTTPRequest", "empty response"));
		return -1;
	}

	return 0;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	UUCByteArray* pResponse = (UUCByteArray*)userp;
	size_t realsize = size * nmemb;
	//LOG_ERR((0, "WriteCallback", "response len: %d", realsize));
	pResponse->append((BYTE*)contents, realsize);

	return realsize;
}
