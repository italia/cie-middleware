
#include "SignatureGenerator.h"
#include "ASN1/ASN1Setof.h"
#include "ASN1/ASN1Octetstring.h"
#include <time.h>
#include "ASN1/DigestInfo.h"
#include "ASN1/ASN1Sequence.h"
#include "ASN1/ASN1ObjectIdentifier.h"
#include "ASN1/ASN1UTCTime.h"
#include "ASN1/AlgorithmIdentifier.h"
#include "ASN1/Certificate.h"
#include "ASN1/IssuerAndSerialNumber.h"
#include "RSA/sha1.h"
#include "RSA/sha2.h"
#include "CertStore.h"


#include <string>

#include "UUCLogger.h"
USE_LOG;

using namespace std;

CSignatureGeneratorBase::CSignatureGeneratorBase(CBaseSigner* pSigner)
: m_pSigner(pSigner), m_pTSAClient(NULL), m_nHashAlgo(CKM_SHA1_RSA_PKCS)
{
	m_szAlias[0] = 0;
}

CSignatureGeneratorBase::CSignatureGeneratorBase(CSignatureGeneratorBase* pGenerator)
{
	m_pSigner = pGenerator->m_pSigner;
	m_pTSAClient = pGenerator->m_pTSAClient;
	strcpy(m_szAlias, pGenerator->m_szAlias);
	m_nHashAlgo = pGenerator->m_nHashAlgo;
}

CSignatureGeneratorBase::~CSignatureGeneratorBase(void)
{
}

void CSignatureGeneratorBase::SetData(const UUCByteArray& data)
{
	m_data.append((BYTE*)data.getContent(), data.getLength());
}

void CSignatureGeneratorBase::SetAlias(char* alias)
{
	strcpy(m_szAlias, alias);
}

void CSignatureGeneratorBase::SetHashAlgo(int hashAlgo)
{
	m_nHashAlgo = hashAlgo;
}


void CSignatureGeneratorBase::SetTSA(char* szUrl, char* szUsername = NULL, char* szPassword = NULL)
{
	m_pTSAClient = new CTSAClient();
	m_pTSAClient->SetTSAUrl(szUrl);
	if(szUsername)
	{
		m_pTSAClient->SetCredential(szUsername, szPassword);
	}
}

void CSignatureGeneratorBase::SetTSAUsername(char* szUsername)
{
	m_pTSAClient->SetUsername(szUsername);
}

void CSignatureGeneratorBase::SetTSAPassword(char* szPassword)
{
	m_pTSAClient->SetPassword(szPassword);
}


// CSignatureGenerator
CSignatureGenerator::CSignatureGenerator(CBaseSigner* pSigner, bool bRemote)
: CSignatureGeneratorBase(pSigner), m_bCAdES(false), m_bRemote(bRemote)
{
}

CSignatureGenerator::~CSignatureGenerator(void)
{
}

void CSignatureGenerator::SetPKCS7Data(const UUCByteArray& pkcs7data)
{
	try
	{
		CSignedDocument sd((BYTE*)pkcs7data.getContent(), pkcs7data.getLength());
		
		if(!sd.isDetached())
		{
			sd.getContent(m_data);
		}

		m_signerInfos = sd.getSignerInfos();
		m_certificates = sd.getCertificates();
		m_digestAlgos = sd.getDigestAlgos();
	}
	catch(...)
	{
	}
}

void CSignatureGenerator::SetCAdES(bool cades)
{
	m_bCAdES = cades;
}

bool CSignatureGenerator::GetCAdES()
{
	return m_bCAdES;
}

long CSignatureGenerator::GetCertificate(CCertificate** ppCertificate)
{
    UUCByteArray id;
    long nRes = m_pSigner->GetCertificate(m_szAlias, ppCertificate, id);
    if(nRes)
    {
        LOG_ERR((0, "CSignatureGenerator::Generate", "GetCertificate error: %x", nRes));
        return nRes;
    }
    
    return 0;
}

long CSignatureGenerator::Generate(UUCByteArray& pkcs7SignedData, BOOL bDetached, BOOL bVerifyCertificate)
{
	// get the certificate based on alias
	LOG_DBG((0, "CSignatureGenerator::Generate", ""));

	UUCByteArray id;
	CCertificate* pSignerCertificate;
	long nRes = m_pSigner->GetCertificate(m_szAlias, &pSignerCertificate, id);
	if(nRes)
	{
		LOG_ERR((0, "CSignatureGenerator::Generate", "GetCertificate error: %x", nRes));
		m_pSigner->Close();
		return nRes;
	}
	
	LOG_DBG((0, "CSignatureGenerator::Generate", "bVerifyCertificate: %d", bVerifyCertificate));

	if (bVerifyCertificate)
	{
		if(!pSignerCertificate->isValid())
		{
			SAFEDELETE(pSignerCertificate);
			m_pSigner->Close();
			return DISIGON_ERROR_CERT_EXPIRED;
		}

		/*if (!(pSignerCertificate->isNonRepudiation()))
		{
			SAFEDELETE(pSignerCertificate);
			return DISIGON_ERROR_CERT_NOT_FOR_SIGNATURE;
		}*/

		int bitmask = pSignerCertificate->verify();

		if((bitmask & VERIFIED_CACERT_FOUND) == 0)
		{
			SAFEDELETE(pSignerCertificate);
			m_pSigner->Close();
			return DISIGON_ERROR_CACERT_NOTFOUND;
		}

		if((bitmask & VERIFIED_CERT_CHAIN) == 0)
		{
			SAFEDELETE(pSignerCertificate);
			m_pSigner->Close();
			return DISIGON_ERROR_CERT_INVALID;
		}
		
		if(pSignerCertificate->verifyStatus(NULL) != REVOCATION_STATUS_GOOD)
		{
			SAFEDELETE(pSignerCertificate);
			m_pSigner->Close();
			return DISIGON_ERROR_CERT_REVOKED;
		}
	}

	// extract the cert value
	UUCByteArray certval;
	pSignerCertificate->toByteArray(certval);
	
	// hash certificate
	BYTE certHash[100];

/*
	sha256_context	ctx256;
	sha256_starts(&ctx256);
	sha256_update(&ctx256, certval.getContent(), certval.getLength());	
	sha256_finish(&ctx256, certHash);
*/
	LOG_DBG((0, "CSignatureGenerator::Generate", "HASH"));

	sha2(certval.getContent(), certval.getLength(), certHash, 0);
/*
	SHA256_CTX	ctx256;
	SHA256_Init(&ctx256);
	SHA256_Update(&ctx256, certval.getContent(), certval.getLength());	
	SHA256_Final(certHash, &ctx256);
*/

	LOG_DBG((0, "CSignatureGenerator::Generate", "setSigningCertificate"));

	m_signerInfoGenerator.setSigningCertificate(certval.getContent(), certval.getLength(), certHash, 32); 

	// hash algo
	int mech = m_bCAdES ? CKM_SHA256_RSA_PKCS : m_nHashAlgo;
	CAlgorithmIdentifier hashOID(mech == CKM_SHA256_RSA_PKCS ? szSHA256OID : szSHA1OID);
	int i = 0;
	for(i = 0; i < m_digestAlgos.size(); i++)
	{
		if(m_digestAlgos.elementAt(i) == hashOID)
			break;
	}

	if(i == m_digestAlgos.size())
		m_digestAlgos.addElement(hashOID);


	// content hashing

	LOG_DBG((0, "CSignatureGenerator::Generate", "CertificateHash"));

	BYTE* hash;
	int hashlen;

	switch(mech)
	{
	case CKM_SHA256_RSA_PKCS:
		{
			LOG_DBG((0, "CSignatureGenerator::Generate", "CertificateHash: CKM_SHA256_RSA_PKCS"));

			hash = new BYTE[32];
			hashlen = 32;
			sha2((BYTE*)m_data.getContent(), m_data.getLength(), hash, 0);
			m_signerInfoGenerator.setContentHash(hash, hashlen);
			
			UUCByteArray signedAttributes;
			m_signerInfoGenerator.getSignedAttributes(signedAttributes, false, !bDetached);
			sha2(signedAttributes.getContent(), signedAttributes.getLength(), hash, 0);
		}
		break;

	case CKM_SHA1_RSA_PKCS:
		{
			LOG_DBG((0, "CSignatureGenerator::Generate", "CertificateHash: CKM_SHA1_RSA_PKCS"));

			hash = new BYTE[24];
			hashlen = 24;

			char szAux[50];

			// calcola l'hash SHA1
			SHA1Context sha;				
			SHA1Reset(&sha);				
			SHA1Input(&sha, (BYTE*)m_data.getContent(), m_data.getLength());				
			SHA1Result(&sha);
				
			sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
				
			UUCByteArray hashaux(szAux);

			memcpy(hash, hashaux.getContent(), hashlen);

			m_signerInfoGenerator.setContentHash(hash, hashlen);
			
			UUCByteArray signedAttributes;
			m_signerInfoGenerator.getSignedAttributes(signedAttributes, false, !bDetached);

			// compute total digest			
			SHA1Reset(&sha);
			SHA1Input(&sha, signedAttributes.getContent(), signedAttributes.getLength());
			SHA1Result(&sha);
				
			sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
				
			UUCByteArray hashaux1(szAux);

			memcpy(hash, hashaux1.getContent(), hashlen);
		}
		break;
	}

	UUCByteArray digest;

	if(m_bRemote)
	{
		digest.append(hash, hashlen);		
	}
	else
	{
		CASN1OctetString digestString(hash, hashlen);		
		CDigestInfo digestInfo(hashOID, digestString);

		digestInfo.toByteArray(digest);
	}

	delete hash;
	UUCByteArray signature;

	LOG_DBG((0, "CSignatureGenerator::Generate", "Sign"));

	// make signature on the digest info
	CK_RV rv = m_pSigner->Sign(digest, id, CKM_RSA_PKCS, signature);
	if(rv)
	{
		LOG_DBG((0, "CSignatureGenerator::Generate", "Sign error: %x", rv));
		m_pSigner->Close();
		return rv;
	}

	m_signerInfoGenerator.setSignature((BYTE*)signature.getContent(), signature.getLength());

	// TSA
	if (m_pTSAClient != NULL)
    {
		CSignerInfo signerInfo = m_signerInfoGenerator.getSignerInfo();	
	
		CASN1OctetString octetString = signerInfo.getEncryptedDigest();
		UUCByteArray content;
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

		hash = new BYTE[32];
		hashlen = 32;

		//sha2((BYTE*)m_data.getContent(), m_data.getLength(), hash, 0);
		sha2((BYTE*)content.getContent(), content.getLength(), hash, 0);

		UUCByteArray hashaux(hash, hashlen);

		CTimeStampToken* ptst = NULL;
        nRes = m_pTSAClient->GetTimeStampToken(hashaux, NULL, &ptst);
        if (ptst)
        {
			m_signerInfoGenerator.setTimestampToken(ptst);
        }
		else
		{
			LOG_DBG((0, "CSignatureGenerator::Generate", "TSA error: %x", nRes));
			delete pSignerCertificate;
			delete ptst;
			return DISIGON_ERROR_TSA;
		}

		delete ptst;
    }


	CSignerInfo signerInfo = m_signerInfoGenerator.getSignerInfo();	
	m_signerInfos.addElement(signerInfo);

	/*
	// signer certificate
	for(i = 0; i < m_certificates.size(); i++)
	{
		if(m_certificates.elementAt(i) == cert)
			break;
	}
	 		
	if(i == m_certificates.size())
		m_certificates.addElement(cert);
*/

    // aggiunge i certificati di CA
    
    CCertificate* pCACert = CCertStore::GetCertificate(*pSignerCertificate);
    while(pCACert)
    {
        m_certificates.addElement(*pCACert);
        pCACert = CCertStore::GetCertificate(*pCACert);
    }
        
	m_certificates.addElement(*pSignerCertificate);
	delete pSignerCertificate;

	// Crea signedData
	CSignedData* pSignedData;
	if(m_data.getLength() == 0 || bDetached) // detached
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
		CASN1ObjectIdentifier dataOID(szDataOID);
		CASN1OctetString data(m_data);
		CContentInfo ci(dataOID, data);
		pSignedData = new CSignedData(m_digestAlgos,
									  ci,
									  m_signerInfos,
									  m_certificates);		
	}

	LOG_DBG((0, "CSignatureGenerator::Generate", "ContentInfo"));

	// Infine crea ContentInfo
	CContentInfo contentInfo(szSignedDataOID, *pSignedData);
	
	pkcs7SignedData.removeAll();
	contentInfo.toByteArray(pkcs7SignedData);	
	
	delete pSignedData;	

	m_pSigner->Close();

	LOG_DBG((0, "<-- CSignatureGenerator::Generate", "OK"));

	return CKR_OK;
}

//int nRead = 0;
//BYTE buffer[BUFFERSIZE];
//UUCByteArray content;
//
//FILE* f = fopen("dnie.cer", "rb");
//
//while (((nRead = fread(buffer, 1, BUFFERSIZE, f)) > 0))
//{
//	content.append(buffer, nRead);
//}
//
//fclose(f);
//
//CCertificate* pCertif = CCertificate::createCertificate(content);
//
//content.removeAll();
//
//f = fopen("caclass2.crl", "rb");
//
//while (((nRead = fread(buffer, 1, BUFFERSIZE, f)) > 0))
//{
//	content.append(buffer, nRead);
//}
//
//fclose(f);
//
//REVOCATION_INFO revInfo;
//
//UUCBufferedReader reader(content.getContent(), content.getLength());
//
//CCrl crl(reader);
//int status = REVOCATION_STATUS_UNKNOWN;
//CASN1Integer serialNumber(pCertif->getSerialNumber());
//if (!crl.isRevoked(serialNumber, NULL, &status, &revInfo))
//{
//	LOG_ERR((0, "CCertificate::verifyStatus", "Cert Status: GOOD: %d", status));
//	status = REVOCATION_STATUS_GOOD;
//}
//
//
//long r = pCertif->verifyStatus(&revInfo);
