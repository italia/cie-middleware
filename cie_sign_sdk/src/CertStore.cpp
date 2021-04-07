
#include "CertStore.h"
#include <stdio.h>
#include "UUCLogger.h"
#include <map>

using namespace std;

USE_LOG;

unsigned long getHash(const char* szKey);

#ifdef WIN32

#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/param.h>
#endif

map<unsigned long, CCertificate*> CCertStore::m_certMap;


void CCertStore::AddCertificate(CCertificate& certificate)
{
    //LOG_DBG((0, "--> CertStore::AddCertificate", ""));

    unsigned long nHash;

    try
    {
        CASN1OctetString subjectKeyIdentifier = certificate.getSubjectKeyIdentifier();
        
        if(subjectKeyIdentifier.getLength() > 0)
        {
            UUCByteArray* pValue = (UUCByteArray*)subjectKeyIdentifier.getValue();
            const char* szSki = pValue->toHexString();
            
            //LOG_DBG((0, "AddCertificate", "szSki: %s", szSki));
            
            nHash = getHash(szSki);
        }
        else
        {
            UUCByteArray baSubject;
            certificate.getSubject().getNameAsString(baSubject);
            nHash = getHash((char*)baSubject.getContent());
        }
        
        CCertificate* pCert = new CCertificate(certificate);
        
        m_certMap[nHash] = pCert;
    }
    catch(...)
    {
        LOG_ERR((0, "CertStore::AddCertificate Exception", ""));
    }

    //LOG_DBG((0, "<-- CertStore::AddCertificate", ""));
}

CCertificate* CCertStore::GetCertificate(CCertificate& certificate)
{
    unsigned long nHash;

    try
    {
        CASN1OctetString autorityKeyIdentifier = certificate.getAuthorithyKeyIdentifier();
        
        if(autorityKeyIdentifier.getLength() > 0)
        {
            UUCByteArray* pValue = (UUCByteArray*)autorityKeyIdentifier.getValue();
            pValue->set(0, 0x04);
            
            const char* szAki = pValue->toHexString();
            
            LOG_DBG((0, "GetCertificate", "szAki: %s", szAki));
            
            nHash = getHash(szAki);
        }
        else
        {
            UUCByteArray baIssuer;
            certificate.getIssuer().getNameAsString(baIssuer);
            nHash = getHash((char*)baIssuer.getContent());
        }
        
        CCertificate* pCert = m_certMap[nHash];
        if(pCert != NULL && pCert->getSerialNumber() == certificate.getSerialNumber())
            return NULL;
        
        return pCert;
    }
    catch(...)
    {
        LOG_ERR((0, "CertStore::AddCertificate Exception", ""));
    }

    return NULL;
}

void CCertStore::CleanUp()
{
	for (map<unsigned long, CCertificate*>::iterator it = m_certMap.begin(); it != m_certMap.end(); ++it)
	{
		CCertificate* pCert = (CCertificate*)it->second;
		SAFEDELETE(pCert)
	}
}

unsigned long getHash(const char* szKey)
{
	int h = 0;
	int off = 0;
	char* val = (char*)szKey;
	int len = strlen((char*)szKey);

	if (len < 16) 
	{
 	    for (int i = len ; i > 0; i--) 
		{
 			h = (h * 37) + val[off++];
 	    }
 	} 
	else 
	{
 	    // only sample some characters
 	    int skip = len / 8;
 	    for (int i = len ; i > 0; i -= skip, off += skip) 
		{
 			h = (h * 39) + val[off];
 	    }
 	}

	return h;	
}
