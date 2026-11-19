
#include "CIESigner.h"
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include "pkcs11.h"
/*
#pragma pack(push, cryptoki, 1)
#include "pkcs11.h"
#pragma pack(pop, cryptoki)
*/
#else
#include "cryptoki.h"
#endif

#include "UUCLogger.h"
USE_LOG;
#include <stdlib.h>
#include <openssl/obj_mac.h>

extern "C" {
void makeDigestInfo(int algid, unsigned char* pbtDigest, size_t btDigestLen, unsigned char* pbtDigestInfo, size_t* pbtDigestInfoLen);
}

CCIESigner::CCIESigner(IAS* pIAS)
: m_pIAS(pIAS), m_pCertificate(NULL)
{
}

CCIESigner::~CCIESigner(void)
{
	if (m_pCertificate)
		delete m_pCertificate;
}

long CCIESigner::Init(const char* szPIN)
{
    strcpy(m_szPIN, szPIN);
    
    LOG_DBG((0, "Init CIESigner\n", ""));
    
    try
    {
        m_pIAS->SelectAID_IAS();
        m_pIAS->SelectAID_CIE();
        m_pIAS->InitDHParam();
        
//        ByteDynArray IdServizi;
//        m_pIAS->ReadIdServizi(IdServizi);
        
        ByteDynArray data;
        m_pIAS->ReadDappPubKey(data);
        m_pIAS->InitExtAuthKeyParam();
        m_pIAS->DHKeyExchange();
        m_pIAS->DAPP();
        
        ByteArray baPIN((BYTE*)szPIN, (size_t)strlen(szPIN));
        StatusWord sw = m_pIAS->VerifyPIN(baPIN);
        
        if(sw != 0x9000)
        {
            LOG_DBG((0, "<-- CCIESigner::Init", "VerifyPIN failed: %x", sw));
            printf("Init CIESigner OK\n");
            return sw;
        }
    
        LOG_DBG((0, "<-- CCIESigner::Init", "OK"));
        
        return 0;
    }
    catch (scard_error err)
    {
        LOG_ERR((0, "<-- CCIESigner::Init", "failed: %x", err.sw));

        return err.sw;
    }
    catch (scard_error* err)
    {
        LOG_ERR((0, "<-- CCIESigner::Init", "failed*: %x", err->sw));

        return err->sw;
    }
    catch(...)
    {
        LOG_ERR((0, "<-- CCIESigner::Init", "unexpected failure"));
        return -1;
    }
	
	return 0;
}

long CCIESigner::GetCertificate(const char* szAlias, CCertificate** ppCertificate, UUCByteArray& id)
{
	id.append((BYTE)'1');

    LOG_DBG((0, "--> CCIESigner::GetCertificate", "Alias: %s", szAlias));
    
    ByteDynArray c;
    m_pIAS->ReadCertCIE(c);
    
    *ppCertificate = new CCertificate(c.data(), c.size());
    
    LOG_DBG((0, "<-- CCIESigner::GetCertificate", "OK"));
    
    
	return CKR_OK;
}

long CCIESigner::Sign(UUCByteArray& data, UUCByteArray& id, int algo, UUCByteArray& signature)
{
	LOG_DBG((0, "--> CCIESigner::Sign", "algo: %d", algo));

    // DigestInfo
    unsigned char digestinfo[256];
    size_t digestinfolen = 256;
        
    try
    {
        // TODO digest info
        switch(algo)
        {
            case CKM_SHA256_RSA_PKCS:
                makeDigestInfo(NID_sha256, (unsigned char*)data.getContent(), (size_t)data.getLength(), (unsigned char*)digestinfo, &digestinfolen);
                
                break;
                
            case CKM_SHA1_RSA_PKCS:
                makeDigestInfo(NID_sha1, (unsigned char*)data.getContent(), (size_t)data.getLength(), (unsigned char*)digestinfo, &digestinfolen);
                break;
                
            case CKM_RSA_PKCS:
                digestinfolen = data.getLength();
                memcpy(digestinfo, data.getContent(), digestinfolen);
                break;
        }
        
        ByteArray baDigestInfo(digestinfo, digestinfolen);
        ByteDynArray baSignature;
        
        m_pIAS->Sign(baDigestInfo, baSignature);
        
        signature.append(baSignature.data(), (int)baSignature.size());
	}
    catch (scard_error err)
    {
        return err.sw;
    }
	catch (...)
	{
		return -1;
	}

	return CKR_OK;
}

long CCIESigner::Close()
{
	return 0;
}
