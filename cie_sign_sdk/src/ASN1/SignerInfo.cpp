// SignerInfo.cpp: implementation of the CSignerInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "SignerInfo.h"
#include "ASN1OptionalField.h"
#include "ASN1Exception.h"
#include "RSAPublicKey.h"
#include "../RSA/rsaeuro.h"
#include "../RSA/rsa.h"
#include "CertStore.h"
#include "DigestInfo.h"
#include <sys/types.h>
#include "Certificate.h"
#include "Crl.h"
#include <map>
#include "../RSA/sha1.h"
#include "../RSA/sha2.h"
#include "UUCLogger.h"

#include <stdio.h>

#include <openssl/bio.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

//#import <UIKit/UIKit.h>

USE_LOG;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSignerInfo::~CSignerInfo()
{

}

CSignerInfo::CSignerInfo(UUCBufferedReader& reader)
: CASN1Sequence(reader)
{
}
	
CSignerInfo::CSignerInfo(const CASN1Object& signerInfo)
: CASN1Sequence(signerInfo)
{
}

CSignerInfo::CSignerInfo(const CIssuerAndSerialNumber& issuer, const CAlgorithmIdentifier& digestAlgo, const CAlgorithmIdentifier& encAlgo, const CASN1OctetString& encDigest)	
{
	addElement(CASN1Integer(1));
	addElement(issuer);
	addElement(digestAlgo);

	// Optional Field
	//addElement(authAttributes);

	addElement(encAlgo);
	addElement(encDigest);
	
	// Optional Field
	//addElement(NULL);
}

	
void CSignerInfo::addAuthenticatedAttributes(const CASN1SetOf& attributes)
{
	addElementAt(CASN1OptionalField(attributes, 0x00), 3);
}

void CSignerInfo::addUnauthenticatedAttributes(const CASN1SetOf& attributes)
{
	if(size() < 7)
		addElement(CASN1OptionalField(attributes, 0x01));
	else
		setElementAt(CASN1OptionalField(attributes, 0x01), 6);
}

CASN1OctetString CSignerInfo::getEncryptedDigest()
{
	CASN1Object obj(elementAt(3));
	
	if(obj.getTag() == 0xA0) // optional auth attributes present
	{
		return (CASN1OctetString)elementAt(5);
	}
	else 
	{
		return (CASN1OctetString)elementAt(4);
	}

		
}

CAlgorithmIdentifier CSignerInfo::getDigestAlgorithn()
{
	//CASN1Object obj(elementAt(3));
	
	//if(obj.getTag() == 0xA0) // optional auth attributes present
	//{
		return (CAlgorithmIdentifier)elementAt(2);
	//}
	//else 
	//{
	//	return (CAlgorithmIdentifier)elementAt(5);
	//}
}

CIssuerAndSerialNumber CSignerInfo::getIssuerAndSerialNumber()
{
	return (CIssuerAndSerialNumber)elementAt(1);
}

CASN1SetOf CSignerInfo::getAuthenticatedAttributes()
{
	CASN1Object obj(elementAt(3));
	
	if(obj.getTag() == 0xA0) // optional auth attributes present
	{
		return obj;
	}
	else 
	{
		CASN1SetOf empty;
		return empty;
	}
}

CASN1SetOf CSignerInfo::getUnauthenticatedAttributes()
{
	if(size() > 6)
	{
		CASN1Object obj(elementAt(3));
		
		if(obj.getTag() == 0xA0) // optional auth attributes present
			return elementAt(6);
		else
			return elementAt(5);
	}
	else 
	{
		return CASN1SetOf();
	}

}

CASN1UTCTime CSignerInfo::getSigningTime()
{
	CASN1SetOf attrs = getAuthenticatedAttributes();
	
	int size = attrs.size();
	for(int j = 0; j < size; j++)
	{
		CASN1Sequence attr = attrs.elementAt(j);
		CASN1ObjectIdentifier objId(attr.elementAt(0));
		if(objId.equals(CASN1ObjectIdentifier(szSigningTimeOID)))
		   return (CASN1SetOf(attr.elementAt(1))).elementAt(0);
	}
	
	throw -1L;
}
/* NON CORRETTO
CASN1ObjectIdentifier CSignerInfo::getSigningCertificateV2()
{
	CASN1SetOf attrs = getAuthenticatedAttributes();
	
	for(int j = 0; j < attrs.size(); j++)
	{
		CASN1Sequence attr = attrs.elementAt(j);
		CASN1ObjectIdentifier objId(attr.elementAt(0));
		if(objId.equals(CASN1ObjectIdentifier(szIdAASigningCertificateV2OID)))
		   return (CASN1SetOf(attr.elementAt(1))).elementAt(0);
	}
	
	throw -1L;
}
*/
CASN1OctetString CSignerInfo::getContentHash()
{
	CASN1SetOf attrs = getAuthenticatedAttributes();
	int size = attrs.size();
	for(int j = 0; j < size; j++)
	{
		CASN1Sequence attr = attrs.elementAt(j);
		CASN1ObjectIdentifier objId(attr.elementAt(0));
		if(objId.equals(CASN1ObjectIdentifier(szMessageDigestOID)))
			return (CASN1SetOf(attr.elementAt(1))).elementAt(0);
	}
	
	throw -1L;
}

CTimeStampToken CSignerInfo::getTimeStampToken()
{
	CASN1SetOf attributes = getUnauthenticatedAttributes();
	CASN1ObjectIdentifier oidTimestampToken(szTimestampTokenOID);
	
	// search for timestamp oid
	int nSize = attributes.size();
	for(int i = 0; i < nSize; i++)
	{
		CASN1Sequence attribute = attributes.elementAt(i);
		CASN1ObjectIdentifier oid = attribute.elementAt(0);
		
		if(oid.equals(oidTimestampToken))
		{
			CASN1SetOf values(attribute.elementAt(1));
			return values.elementAt(0);
		}
	}
	
	return CTimeStampToken(CASN1Sequence());
}

bool CSignerInfo::hasTimeStampToken()
{
	CASN1SetOf tst = getTimeStampToken();
	return tst.size() > 0;
}

CASN1SetOf CSignerInfo::getCountersignatures()
{
	CASN1SetOf counterSignatures;
	CASN1SetOf attributes = getUnauthenticatedAttributes();
	CASN1ObjectIdentifier oid(szCounterSignatureOID);
	
	// search for countersignature oid
	int nSize = attributes.size();
	for(int i = 0; i < nSize; i++)
	{
		CASN1Sequence attribute = attributes.elementAt(i);
		CASN1ObjectIdentifier oid1 = attribute.elementAt(0);
		
		if(oid.equals(oid1))
		{
			CASN1SetOf set(attribute.elementAt(1));
			counterSignatures.addElement(set.elementAt(0));
			//return attribute.elementAt(1);
		}
	}
	
	return counterSignatures;
}

void CSignerInfo::setCountersignatures(int index, CSignerInfo& countersignature)
{
	CASN1SetOf attributes = getUnauthenticatedAttributes();
	CASN1ObjectIdentifier oid(szCounterSignatureOID);
	int counter = 0;
	// search for countersignature oid
	int nSize = attributes.size();
	for(int i = 0; i < nSize; i++)
	{
		CASN1Sequence attribute = attributes.elementAt(i);
		CASN1ObjectIdentifier oid1 = attribute.elementAt(0);
		
		if(oid.equals(oid1))
		{
			if(counter == index)
			{
				CASN1Sequence v;
				v.addElement(oid); 
				CASN1SetOf cs;
				cs.addElement(countersignature);
				v.addElement(cs);
				
				attributes.setElementAt(v, i);
				
				addUnauthenticatedAttributes(attributes);
				return;
			}
			
			counter++;
		}
	}
}

void CSignerInfo::addCountersignatures(CSignerInfo& countersignature)
{
	CASN1SetOf attributes = getUnauthenticatedAttributes();
	CASN1ObjectIdentifier oid(szCounterSignatureOID);
	
	CASN1Sequence v;
	v.addElement(oid); 	
	CASN1SetOf cs;
	cs.addElement(countersignature);
	v.addElement(cs);				
	attributes.addElement(v);
				
	addUnauthenticatedAttributes(attributes);
}

void CSignerInfo::setTimeStampToken(CTimeStampToken& tst)
{
	CASN1SetOf attributes = getUnauthenticatedAttributes();
	CASN1ObjectIdentifier oid(szTimestampTokenOID);
	
	CASN1Sequence v;
	v.addElement(oid); 	
	CASN1SetOf cs;
	cs.addElement(tst);
	v.addElement(cs);				
	attributes.addElement(v);
	
	addUnauthenticatedAttributes(attributes);
}

int CSignerInfo::getCountersignatureCount()
{
	CASN1SetOf countersignatures(getCountersignatures());
	return countersignatures.size();
}

int CSignerInfo::verifyCountersignature(int i, CASN1SetOf& certificates)
{
	return verifyCountersignature(i, certificates, NULL, NULL);
}

int CSignerInfo::verifyCountersignature(int i, CASN1SetOf& certificates, const char* szDateTime, REVOCATION_INFO* pRevocationInfo)
{
	CASN1SetOf countersignatures(getCountersignatures());
	CSignerInfo countersignature(countersignatures.elementAt(i));
	CASN1OctetString source(getEncryptedDigest());
	return verifySignature(source, countersignature, certificates, szDateTime, pRevocationInfo);
}


int CSignerInfo::verifySignature(CASN1OctetString& source, CSignerInfo& signerInfo, CASN1SetOf& certificates, const char* szDateTime, REVOCATION_INFO* pRevocationInfo)
{
	LOG_DBG((0, "--> CSignerInfo::verifySignature", "Verify Revocation: %d", (pRevocationInfo != NULL)));

	CCertificate cert = getSignatureCertificate(signerInfo, certificates);
	
	int bitmask = 0;
	
	// verifica il certificato
	if(cert.isValid(szDateTime))
	{
		bitmask |= VERIFIED_CERT_VALIDITY;
	}
	
	if(cert.isQualified())
	{
		bitmask |= VERIFIED_CERT_QUALIFIED;
	}

	if(cert.isNonRepudiation())
	{
		bitmask |= VERIFIED_KEY_USAGE;
	}
	
	if(cert.isSHA256())
	{
		bitmask |= VERIFIED_CERT_SHA256;
	}
	

	if(pRevocationInfo)
	{
		pRevocationInfo->nRevocationStatus = REVOCATION_STATUS_UNKNOWN;

		// verify revocation status only if the certificate is valid
		if(bitmask & VERIFIED_CERT_VALIDITY)
		{
			int verifyStatus = cert.verifyStatus(szDateTime, pRevocationInfo);
			
			switch (verifyStatus) 
			{
				case REVOCATION_STATUS_GOOD:
					bitmask |= VERIFIED_CERT_GOOD;
					bitmask |= VERIFIED_CRL_LOADED;
					break;
					
				case REVOCATION_STATUS_REVOKED:
					bitmask |= VERIFIED_CRL_LOADED;
					bitmask |= VERIFIED_CERT_REVOKED;
					break;
					
				case REVOCATION_STATUS_SUSPENDED:
					bitmask |= VERIFIED_CERT_SUSPENDED;
					bitmask |= VERIFIED_CRL_LOADED;
					break;
					
				case REVOCATION_STATUS_UNKNOWN:
					bitmask |= VERIFIED_CRL_LOADED;
					break;

				default:
					break;
			}
		}		
	}

	
	// verifica la cert chain
//	CName issuerName(cert.getIssuer());
//	UUCByteArray issuer;
//	issuerName.getNameAsString(issuer);//getField(OID_COMMON_NAME);
//
    CCertificate* pCert = &cert;
    CCertificate* pCACert = CCertStore::GetCertificate(cert);
    while(pCACert && pCert->verifySignature(*pCACert))
	{
        bitmask |= VERIFIED_CACERT_FOUND;

		//NSLog(@"issuer: %s, SN: %s", issuer.c_str(), serialNumber.toHexString());
		
		if(pCACert->isValid(szDateTime))
		{
			bitmask |= VERIFIED_CACERT_VALIDITY;
			if(pRevocationInfo)
			{
				int verifyStatus = pCACert->verifyStatus(szDateTime, NULL);
		
				switch (verifyStatus) 
				{
					case REVOCATION_STATUS_GOOD:
						bitmask |= VERIFIED_CACERT_GOOD;
						bitmask |= VERIFIED_CACRL_LOADED;
						break;
						
					case REVOCATION_STATUS_REVOKED:
						bitmask |= VERIFIED_CACRL_LOADED;
						bitmask |= VERIFIED_CACERT_REVOKED;
						break;
						
					case REVOCATION_STATUS_SUSPENDED:
						bitmask |= VERIFIED_CACERT_SUSPENDED;
						bitmask |= VERIFIED_CACRL_LOADED;
						break;
						
					case REVOCATION_STATUS_UNKNOWN:
						break;
				}	
			}
		}
        
        pCert = pCACert;
        pCACert = CCertStore::GetCertificate(*pCACert);
	}
	
    if(!pCACert)
    {
        bitmask |= VERIFIED_CERT_CHAIN;
    }
    else
    {
        //NSLog(@"CA Cert not valid");
    }
	
	
	// verifica la firma
    
    // OpenSSL
    UUCByteArray baCert;
    cert.toByteArray(baCert);
    
//    int result;
//    BIO* bio;
    X509 *x509 = NULL;
    
    const BYTE* content = baCert.getContent();
    x509 = d2i_X509(NULL, &content, baCert.getLength());
        
    EVP_PKEY *evp_pubkey;
    RSA *rsa_pubkey;

    evp_pubkey  = X509_get_pubkey(x509);
    rsa_pubkey  = EVP_PKEY_get1_RSA(evp_pubkey);

    //////////////
    
//	CSubjectPublicKeyInfo publicKeyInfo = cert.getCertificateInfo().getSubjectPublicKeyInfo();
	
	CASN1OctetString encryptedDigest(signerInfo.getEncryptedDigest());	
	const UUCByteArray* pEncDigest = encryptedDigest.getValue();
	
//	//UUCByteArray aux1;
//	CASN1BitString publicKey(publicKeyInfo.getPublicKey());
//	//publicKey.toByteArray(aux1);
//	//szHex = aux1.toHexString();
//
//	UUCByteArray* pBitString = (UUCByteArray*)publicKey.getValue();
//
//	//szHex = pBitString->toHexString();
//	pBitString->remove(0);
//	//szHex = pBitString->toHexString();
//
//	UUCBufferedReader reader(*pBitString);
//	CRSAPublicKey pubKey(reader);
//
//	//UUCByteArray aux;
//	//pubKey.toByteArray(aux);
//	//szHex = aux.toHexString();
//
//	CASN1Integer modulus = pubKey.getModulus();
//	CASN1Integer exp = pubKey.getExponent();
//
//	R_RSA_PUBLIC_KEY rsakey;
//
//    int retVal=0;
	
	try
	{   
//		UUCByteArray* modArr = (UUCByteArray*)modulus.getValue();
//		UUCByteArray* expArr = (UUCByteArray*)exp.getValue();
//		
//		memset(rsakey.exponent,0x00,MAX_RSA_MODULUS_LEN);
//		memset(rsakey.modulus,0x00,MAX_RSA_MODULUS_LEN);
//		
//		// copio l'esponente
//		memcpy(rsakey.exponent + MAX_RSA_MODULUS_LEN - expArr->getLength(), expArr->getContent(), expArr->getLength());
//		
//		//copio il modulo
//		memcpy(rsakey.modulus + MAX_RSA_MODULUS_LEN - modArr->getLength(), modArr->getContent(), modArr->getLength());
//		
//		// calcola il numero di bit effettivo
//		int i=0;
//		while(rsakey.modulus[i] == 0 && i < MAX_RSA_MODULUS_LEN)
//			i++;
//		
//		rsakey.bits = (MAX_RSA_MODULUS_LEN - i) * 8;
//		
		BYTE decrypted[MAX_RSA_MODULUS_LEN];
		unsigned int len = MAX_RSA_MODULUS_LEN;
		
        const BYTE* encrypted = pEncDigest->getContent();
        const int encrypted_len = (int)pEncDigest->getLength();
        
        len = RSA_public_decrypt(encrypted_len, (BYTE*)encrypted, decrypted, rsa_pubkey,RSA_PKCS1_PADDING);
        
        RSA_free(rsa_pubkey);
        EVP_PKEY_free(evp_pubkey);
        X509_free(x509);
        
		// ritorna il DigestInfo pulito, senza padding
//		retVal = RSAPublicDecrypt(decrypted, &len, (BYTE*)pEncDigest->getContent(), (unsigned int)pEncDigest->getLength(), &rsakey);
		if(len)
		{
			LOG_DBG((0, "CSignerInfo::verifySignature", "RSAPublicDecrypt OK"));

			char szAux[100];
			
			UUCByteArray dec(decrypted, len);
			UUCBufferedReader reader(dec);
			CDigestInfo digestInfo(reader);
			//UUCByteArray* pauxdi = (UUCByteArray*)digestInfo.getValue(); 
			//szHex = pauxdi->toHexString();
			
			CASN1OctetString digest = digestInfo.getDigest();
			UUCByteArray* pDigestValue = (UUCByteArray*)digest.getValue();
			//szHex = pDigestValue->toHexString();
			
			// content
			UUCByteArray content;
			CASN1OctetString octetString(source);
			//UUCByteArray baoctetstring;
			//octetString.toByteArray(baoctetstring);
			//NSLog([NSString stringWithCString:baoctetstring.toHexString()]);
			
			if(octetString.getTag() == 0x24) // contructed octet string
			{
				CASN1Sequence contentArray(octetString);
				int size = contentArray.size();
				for(int i = 0; i < size; i++)
				{
					content.append(contentArray.elementAt(i).getValue()->getContent(), contentArray.elementAt(i).getLength());
				}
			}
			else	
			{
				content.append(octetString.getValue()->getContent(), octetString.getLength());
			}
			
			
			BYTE* buff;
			int bufflen = 0;
			
			UUCByteArray messageDigest;
			
			// estra i signedattributes
			UUCByteArray signedAttr;
			CASN1SetOf authAttr(signerInfo.getAuthenticatedAttributes());
			int authAttrSize = authAttr.size(); 

			LOG_DBG((0, "CSignerInfo::verifySignature", "Attrsize: %d", authAttrSize));

			if(authAttrSize > 0)
			{
				CASN1ObjectIdentifier oid(szMessageDigestOID);
				CASN1ObjectIdentifier oid1(szContentTypeOID);
				CASN1ObjectIdentifier oid2(szIdAASigningCertificateV2OID);
				for(int i = 0; i < authAttrSize; i++)
				{
					CASN1Sequence attr(authAttr.elementAt(i));
					
					if(oid.equals(attr.elementAt(0)))
					{
						bitmask |= VERIFIED_SIGNED_ATTRIBUTE_MD;
						CASN1SetOf values(attr.elementAt(1));
						CASN1OctetString val(values.elementAt(0));
						const UUCByteArray* pval = val.getValue();
						messageDigest.append(*pval);
					}
					else if(oid1.equals(attr.elementAt(0)))
					{
						bitmask |= VERIFIED_SIGNED_ATTRIBUTE_CT;
					}
					else if(oid2.equals(attr.elementAt(0)))
					{
						bitmask |= VERIFIED_SIGNED_ATTRIBUTE_SC;
					}
					
				}
				
				
				authAttr.toByteArray(signedAttr);
				//szHex = signedAttr.toHexString();
				//NSLog([NSString stringWithCString:szHex]);
				
				buff = (BYTE*)signedAttr.getContent();
				bufflen = signedAttr.getLength();

				//LOG_DBG((0, "CSignerInfo::verifySignature", "Buf: %s, %d", signedAttr.toHexString(), bufflen));
			}
			else
			{
				// se non ci sono signedattributes l'hash va fatto sul content
				buff = (BYTE*)content.getContent();
				bufflen = content.getLength();

				//LOG_DBG((0, "CSignerInfo::verifySignature", "Buf2: %s, %d", content.toHexString(), bufflen));	
			}
			
			CAlgorithmIdentifier digestAlgo(digestInfo.getDigestAlgorithm());
			CAlgorithmIdentifier sha256Algo(szSHA256OID);
			CAlgorithmIdentifier sha1Algo(szSHA1OID);
			if(digestAlgo.elementAt(0) == sha256Algo.elementAt(0))
			{			
				LOG_DBG((0, "CSignerInfo::verifySignature", "SHA256 OK"));

				bitmask |= VERIFIED_SHA256;
				
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

				UUCByteArray bahash((BYTE*)hash, 32);
				LOG_DBG((0, "CSignerInfo::verifySignature", "DigestValue: %s, %s", pDigestValue->toHexString(), bahash.toHexString()));

				if(memcmp(hash, pDigestValue->getContent(), 32) == 0)
				{
					LOG_DBG((0, "CSignerInfo::verifySignature", "SHA256 Len OK"));

					// verifica l'hash del content
					if(messageDigest.getLength() > 0)
					{
						if(memcmp(hash2, messageDigest.getContent(), 32) == 0)
						{							
							bitmask |= VERIFIED_SIGNATURE;
							LOG_DBG((0, "CSignerInfo::verifySignature", "VERIFIED: %x", bitmask));
						}
						else
						{
							LOG_DBG((0, "CSignerInfo::verifySignature", "Not verified"));
						}
					}
					else 
					{
						if(memcmp(hash2, hash, 32) == 0)
						{
							bitmask |= VERIFIED_SIGNATURE;
							LOG_DBG((0, "CSignerInfo::verifySignature", "VERIFIED 2: %x", bitmask));
						}
						else
						{
							LOG_DBG((0, "CSignerInfo::verifySignature", "Not verified 2"));
						}
					}
				}
				else
				{
					LOG_DBG((0, "CSignerInfo::verifySignature", "Not verified 3"));
				}
			}
			else if(digestAlgo.elementAt(0) == sha1Algo.elementAt(0)) //if(digestAlgo == CAlgorithmIdentifier(szSHA1OID))
			{
				LOG_DBG((0, "CSignerInfo::verifySignature", "SHA1"));
				//BYTE hash[20];
				
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
					LOG_DBG((0, "CSignerInfo::verifySignature", "length 1"));

					// verifica l'hash del content
					if(messageDigest.getLength() > 0)
					{
						LOG_DBG((0, "CSignerInfo::verifySignature", "length 2"));
						if(memcmp(contentHash.getContent(), messageDigest.getContent(), contentHash.getLength()) == 0)
						{
							bitmask |= VERIFIED_SIGNATURE;
						}
						else
						{
							LOG_DBG((0, "CSignerInfo::verifySignature", "Not verified 2"));
						}
					}
					else 
					{
						if(memcmp(contentHash.getContent(), hashaux.getContent(), contentHash.getLength()) == 0)
						{
							bitmask |= VERIFIED_SIGNATURE;
						}
						else
						{
							LOG_DBG((0, "CSignerInfo::verifySignature", "Not verified 3"));
						}
					}
				}				
			}
		}
		else
        {
            LOG_ERR((0, "CSignerInfo::verifySignature", "RSA Signature not verified"));
        }
	}
	catch(...)
	{
		LOG_ERR((0, "CSignerInfo::verifySignature", "Unexpected Exception"));
	}
	
	
	return bitmask;
}

CCertificate CSignerInfo::getSignatureCertificate(CSignerInfo& signature, CASN1SetOf& certificates)
{	
	CIssuerAndSerialNumber issuerAndSerialNumber = 
		signature.getIssuerAndSerialNumber();
	
	for(int i = 0; i < certificates.size(); i++)
	{
		CCertificate cert = certificates.elementAt(i); 
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

