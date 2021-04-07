// SignedData.cpp: implementation of the CSignedData class.
//
//////////////////////////////////////////////////////////////////////

#include "SignedData.h"
#include "ASN1OptionalField.h"
#include "ASN1Exception.h"
#include "SignerInfo.h"
#include "RSAPublicKey.h"
#include "../RSA/rsaeuro.h"
#include "../RSA/rsa.h"
#include "../RSA/sha2.h"
#include "DigestInfo.h"
#include <sys/types.h>
#include "Certificate.h"
#include "Crl.h"
#include <map>
#include "../RSA/sha1.h"

//#import <UIKit/UIKit.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSignedData::CSignedData(UUCBufferedReader& reader)
: CASN1Sequence(reader)
{
	/*
	addElement(new CVersion(reader));
	addElement(new CDigestAlgorithmIdentifiers(reader));
	addElement(new CContentInfo(reader));
	// Optional Fields

	//reader.mark();

	// Optional fields
	//CExtendedCertificateAndCertificates* pCertificates = NULL;
	CASN1Ignored* pCertificates = NULL;
	if (CASN1OptionalField::isOptionaField(0x00, reader))
	{
		//reader.reset();
		try
		{
			//pCertificates = new CExtendedCertificateAndCertificates(reader); 
			//pCertificates = new CASN1OptionalField(new CExtendedCertificateAndCertificates(reader), 0x00); 
			//pCertificates = new CASN1OptionalField(new CCertificate(reader), 0x00); 
			//pCertificates = new CASN1OptionalField(new CASN1Ignored(reader), 0x00); 
			pCertificates = new CASN1Ignored(reader); 
		}
		catch(CASN1ObjectNotFoundException* ex)
		{
			delete ex;			
		}
	}

	addElement(pCertificates); // ExtendedCertificateAndCertificates

	// Optional fields
	CASN1OptionalField* pCRL = NULL;
	if (CASN1OptionalField::isOptionaField(0x01, reader))
	{
		try
		{
			pCRL = new CASN1OptionalField(new CASN1Ignored(reader), 0x01); 
		}
		catch(CASN1ObjectNotFoundException* ex)
		{			
			delete ex;			
		}
	}
	
	addElement(pCRL);  // CertificateRevocationLists

	// Signer Info
	addElement(new CSignerInfos(reader));
	 */
}

	
CSignedData::CSignedData(const CASN1Object& signedData)
: CASN1Sequence(signedData)
{

}

CSignedData::CSignedData(const CASN1SetOf& algos, const CContentInfo& contentInfo, const CASN1SetOf& signerInfos, const CASN1SetOf& certificates)
{
	addElement(CASN1Integer(1));
	addElement(algos);
	addElement(contentInfo);

	// Optional fields
	addElement(CASN1OptionalField(certificates, 0));  // ExtendedCertificateAndCertificates
	//addElement(NULL);  // CertificateRevocationLists

	// Signer Infos
	addElement(signerInfos);
}

CSignedData::~CSignedData()
{

}
/*
void CSignedData::addExtendedCertificates(const CASN1SetOf& certificates)
{
	addElementAt(certificate, 3);	
}
*/
CASN1SetOf CSignedData::getDigestAlgorithmIdentifiers()
{
	return (CASN1SetOf)elementAt(1);
}
	
CContentInfo CSignedData::getContentInfo()
{
	return (CContentInfo)elementAt(2);
}
	
CASN1SetOf CSignedData::getSignerInfos()
{
	return (CASN1SetOf)elementAt(4);
}
/*
bool CSignedData::verify(int i)
{
	CASN1SetOf signerInfos(elementAt(4));
	CSignerInfo signerInfo(signerInfo.elementAt(i));
						   
	// Verify
	return true;
}
*/

CCertificate CSignedData::getSignerCertificate(int index)
{
	CASN1SetOf signerInfos = getSignerInfos();
	CASN1SetOf certificates = getCertificates();
	
	CSignerInfo sinfo = signerInfos.elementAt(index);
	
	CIssuerAndSerialNumber issuerAndSerialNumber = 
	sinfo.getIssuerAndSerialNumber();
	
	for(int i = 0; i < certificates.size(); i++)
	{
		CCertificate cert = certificates.elementAt(i); 
		CName issuer = cert.getIssuer();
		CName serialNumber = cert.getSerialNumber();
		
		CIssuerAndSerialNumber issuerAndSerial(issuer, serialNumber, false);
		
		if(issuerAndSerial == issuerAndSerialNumber)
		{
			return cert;
		}
	}
	
	throw -1;
}

int CSignedData::verify(int i)
{
	return verify(i, NULL, NULL);
}

int CSignedData::verify(int i, const char* date, REVOCATION_INFO* pRevocationInfo)
{
	if(getContentInfo().size() < 2) // detached
		return -2;

	CASN1SetOf signerInfos = getSignerInfos();
	CSignerInfo signerInfo(signerInfos.elementAt(i));
	CASN1SetOf certificates = getCertificates();
	CASN1OctetString content = getContentInfo().getContent();
	return CSignerInfo::verifySignature(content, signerInfo, certificates, date, pRevocationInfo);
}

void CSignedData::makeDetached()
{
	if(getContentInfo().size() == 2)
		getContentInfo().removeElementAt(1);
}

void CSignedData::setContent(UUCByteArray& content)
{
	CASN1ObjectIdentifier dataOID(szDataOID);
	CASN1OctetString data(content);
	CContentInfo ci(dataOID, data);
	setElementAt(ci, 2);
}

/*

int CSignedData::verify(int i, char* date)
{
	**
	 * verify that the given certificate successfully handles and confirms
	 * the signature associated with this signer and, if a signingTime
	 * attribute is available, that the certificate was valid at the time the
	 * signature was generated.
	 *
	
	//const char* szHex;
	CASN1SetOf signerInfos = getSignerInfos();
	CSignerInfo signerInfo(signerInfos.elementAt(i));
	
	CCertificate cert = getSignerCertificate(i);
	
	int bitmask = 0;
	
	// verifica il certificato
	if(cert.isValid())
	{
		bitmask |= VERIFIED_CERT_VALIDITY;
	}
	
	
	if(cert.isQualified())
	{
		bitmask |= VERIFIED_CERT_QUALIFIED;
	}
	else 
	{
		//data di emissione posteriore al 30 Agosto 2010
		CASN1UTCTime start = cert.getFrom();
		
		CASN1UTCTime dieciAgosto2010("100830000000Z");
		
		int len = start.getLength() > dieciAgosto2010.getLength() ? dieciAgosto2010.getLength() : start.getLength();
		
		if(memcmp(start.getValue()->getContent(), dieciAgosto2010.getValue()->getContent(), len) > 0)
			bitmask |= VERIFIED_CERT_45_2009;
		
	}

	
	int verifyStatus = cert.verifyStatus();
	
	switch (verifyStatus) 
	{
		case REVOCATION_STATUS_GOOD:
			bitmask |= VERIFIED_CRL;
			bitmask |= VERIFIED_CRL_LOADED;
			break;
			
		case REVOCATION_STATUS_REVOKED:
			bitmask |= VERIFIED_CRL_LOADED;
			bitmask |= VERIFIED_CRL_REVOKED;
			break;
			
		case REVOCATION_STATUS_SUSPENDED:
			bitmask |= VERIFIED_CRL_SUSPENDED;
			bitmask |= VERIFIED_CRL_LOADED;
			break;
			
		case REVOCATION_STATUS_UNKNOWN:
			break;
	}
	
	
	if(date != NULL)
	{
		try 
		{
			// verifica il signing time vs expiration time
			CASN1UTCTime dateTime(date);
		
			CASN1UTCTime expTime = cert.getExpiration();
		
			if(expTime.getLength() == dateTime.getLength())
			{
				if(memcmp(dateTime.getValue()->getContent(), expTime.getValue()->getContent(), expTime.getLength()) > 0)
					bitmask |= VERIFIED_SIGNATURE_EXPIRATION;
			}
		}   
		catch(...)
		{
			//bitmask |= VERIFIED_SIGNATURE_EXPIRATION;
		}
	}
	
	
	
	// verifica la cert chain
	string issuer = cert.getIssuer().getField(OID_COMMON_NAME);
	
	if(g_certMap.find(issuer) != g_certMap.end())
	{
		//NSLog(@"CA Cert found");
		CCertificate* pCACert = g_certMap[issuer];
		//NSLog(@"issuer: %s, SN: %s", issuer.c_str(), serialNumber.toHexString());
		
		if(cert.verify(*pCACert))
		{
			//NSLog(@"CA Cert valid");
			bitmask |= VERIFIED_CERTIFICATE_CHAIN;
		}
		else 
		{
			//NSLog(@"CA Cert not valid");
		}
	}
	else 
	{
		NSLog(@"Warning CA Cert not found");
	}

	
	// verifica la firma
	CSubjectPublicKeyInfo publicKeyInfo = cert.getCertificateInfo().getSubjectPublicKeyInfo();
	
	CASN1OctetString encryptedDigest(signerInfo.getEncryptedDigest());	
	const UUCByteArray* pEncDigest = encryptedDigest.getValue();
	
	//UUCByteArray aux1;
	CASN1BitString publicKey(publicKeyInfo.getPublicKey());
	//publicKey.toByteArray(aux1);
	//szHex = aux1.toHexString();
	
	UUCByteArray* pBitString = (UUCByteArray*)publicKey.getValue(); 
	
	//szHex = pBitString->toHexString();
	pBitString->remove(0);
	//szHex = pBitString->toHexString();
	
	UUCBufferedReader reader(*pBitString);
	CRSAPublicKey pubKey(reader);
	
	//UUCByteArray aux;
	//pubKey.toByteArray(aux);
	//szHex = aux.toHexString();
	
	CASN1Integer modulus = pubKey.getModulus();
	CASN1Integer exp = pubKey.getExponent();
	
	R_RSA_PUBLIC_KEY rsakey;
	
    int retVal=0;
	
	try
	{   
		UUCByteArray* modArr = (UUCByteArray*)modulus.getValue();
		UUCByteArray* expArr = (UUCByteArray*)exp.getValue();
		
		memset(rsakey.exponent,0x00,MAX_RSA_MODULUS_LEN);
		memset(rsakey.modulus,0x00,MAX_RSA_MODULUS_LEN);
		
		// copio l'esponente
		memcpy(rsakey.exponent + MAX_RSA_MODULUS_LEN - expArr->getLength(), expArr->getContent(), expArr->getLength());
		
		//copio il modulo
		memcpy(rsakey.modulus + MAX_RSA_MODULUS_LEN - modArr->getLength(), modArr->getContent(), modArr->getLength());
		
		// calcola il numero di bit effettivo
		int i=0;
		while(rsakey.modulus[i] == 0 && i < MAX_RSA_MODULUS_LEN)
		{
			i++;
		}
		
		rsakey.bits = (MAX_RSA_MODULUS_LEN - i) * 8;
		
		BYTE decrypted[MAX_RSA_MODULUS_LEN];
		unsigned int len = MAX_RSA_MODULUS_LEN;
		
		// ritorna il DigestInfo pulito, senza padding
		retVal = RSAPublicDecrypt(decrypted, &len, (BYTE*)pEncDigest->getContent(), (unsigned int)pEncDigest->getLength(), &rsakey);		
		if(!retVal)
		{
			char szAux[100];
			
			UUCBufferedReader reader(UUCByteArray(decrypted, len));
			CDigestInfo digestInfo(reader);
			//UUCByteArray* pauxdi = (UUCByteArray*)digestInfo.getValue(); 
			//szHex = pauxdi->toHexString();
			
			CASN1OctetString digest = digestInfo.getDigest();
			UUCByteArray* pDigestValue = (UUCByteArray*)digest.getValue();
			//szHex = pDigestValue->toHexString();
			
			// content
			UUCByteArray content;
			CASN1OctetString octetString(getContentInfo().getContent());
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
			}
			else
			{
				// se non ci sono signedattributes l'hash va fatto sul content
				buff = (BYTE*)content.getContent();
				bufflen = content.getLength();
			}

			CAlgorithmIdentifier digestAlgo(digestInfo.getDigestAlgorithm());
			CAlgorithmIdentifier sha256Algo(szSHA256OID);
			CAlgorithmIdentifier sha1Algo(szSHA1OID);
			if(digestAlgo.elementAt(0) == sha256Algo.elementAt(0))
			{				
				bitmask |= VERIFIED_SHA256;
				
				BYTE hash[32];
				BYTE hash2[32];
				
				SHA256_CTX	ctx256;
				
				SHA256_Init(&ctx256);
				
				SHA256_Update(&ctx256, buff, bufflen);
				
				SHA256_Final(hash, &ctx256);
				
				SHA256_Init(&ctx256);
				
				SHA256_Update(&ctx256, content.getContent(), content.getLength());
				
				SHA256_Final(hash2, &ctx256);
				
				if(memcmp(hash, pDigestValue->getContent(), 32) == 0)
				{
					// verifica l'hash del content
					if(messageDigest.getLength() > 0)
					{
						if(memcmp(hash2, messageDigest.getContent(), 32) == 0)
						{
							bitmask |= VERIFIED_SIGNATURE;
						}
					}
					else 
					{
						if(memcmp(hash2, hash, 32) == 0)
						{
							bitmask |= VERIFIED_SIGNATURE;
						}
					}
				}
			}
			else if(digestAlgo.elementAt(0) == sha1Algo.elementAt(0)) //if(digestAlgo == CAlgorithmIdentifier(szSHA1OID))
			{
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
					// verifica l'hash del content
					if(messageDigest.getLength() > 0)
					{
						if(memcmp(contentHash.getContent(), messageDigest.getContent(), contentHash.getLength()) == 0)
						{
							bitmask |= VERIFIED_SIGNATURE;
						}
					}
					else 
					{
						if(memcmp(contentHash.getContent(), hashaux.getContent(), contentHash.getLength()) == 0)
						{
							bitmask |= VERIFIED_SIGNATURE;
						}
					}
				}				
			}
		}
		
	}
	catch(...)
	{
		
	}
	
		
	return bitmask;
}
*/
/*
void CSignedData::addSignerInfo(CASN1Object& si)
{
	CASN1SetOf signerInfos(elementAt(4));
	signerInfos.addElement(si);
	setElementAt(signerInfos, 4);
}
*/
CASN1SetOf CSignedData::getCertificates()
{
	return elementAt(3);
}
/*
void CSignedData::addCertificate(CCertificate& cert)
{
	CASN1SetOf certs(elementAt(3));
	certs.addElement(cert);
	setElementAt(certs, 3);
}
 */

