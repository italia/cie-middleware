#pragma once

#include "definitions.h"
#include "cryptoki.h"

#include "ASN1/SignerInfo.h"
#include "SignedDocument.h"
#include "SignerInfoGenerator.h"
#include "TSAClient.h"
#include "BaseSigner.h"

#define ALGO_SHA1 1
#define ALGO_SHA256 2

class CSignatureGeneratorBase
{
protected:
	CSignatureGeneratorBase(CBaseSigner* pCryptoki);
	CSignatureGeneratorBase(CSignatureGeneratorBase* pGenerator);
	virtual ~CSignatureGeneratorBase(void);

public:

	virtual void SetData(const UUCByteArray& data);

	virtual void SetAlias(char* alias);

	virtual void SetHashAlgo(int hashAlgo);

	virtual void SetTSA(char* szUrl, char* szUsername, char* szPassword);
	virtual void SetTSAUsername(char* szUsername);
	virtual void SetTSAPassword(char* szPassword);

	virtual long Generate(UUCByteArray& pkcs7SignedData, BOOL bDetached = FALSE, BOOL bVerifyRevocation = FALSE) = 0;

protected:
	CBaseSigner*	m_pSigner;
	UUCByteArray	m_data;
	int				m_nHashAlgo;
	char			m_szAlias[MAX_PATH];
	CTSAClient*		m_pTSAClient;

	
};


class CSignatureGenerator : public CSignatureGeneratorBase
{
public:
	CSignatureGenerator(CBaseSigner* pSigner, bool bRemote = false);
	virtual ~CSignatureGenerator(void);

	void SetPKCS7Data(const UUCByteArray& pkcs7Data);

	void SetCAdES(bool cades);
	bool GetCAdES();

    long GetCertificate(CCertificate** ppCertificate);
	virtual long Generate(UUCByteArray& pkcs7SignedData, BOOL bDetached = FALSE, BOOL bVerifyRevocation = FALSE);

private:
	bool m_bCAdES;
	bool m_bRemote;


	CASN1SetOf m_signerInfos;// = new DerSet(signerInfos);
	CASN1SetOf m_certificates;// = new DerSet(certificates);
	CASN1SetOf m_digestAlgos;// = new DerSet(certificates);

	CSignerInfoGenerator m_signerInfoGenerator;
};
