#pragma once


#include "Certificate.h"
#include "BaseSigner.h"
#include "RSAPrivateKey.h"
#include "IAS.h"

class CCIESigner : public CBaseSigner
{
public:
	CCIESigner(IAS* pIAS);
	virtual ~CCIESigner(void);

	long Init(const char* szPIN);

	virtual long GetCertificate(const char* alias, CCertificate** ppCertificate, UUCByteArray& id);

	virtual long Sign(UUCByteArray& data, UUCByteArray& id, int algo, UUCByteArray& signature);

	virtual long Close();

private:
    IAS* m_pIAS;
    char m_szPIN[9];
	CCertificate*   m_pCertificate;
};
