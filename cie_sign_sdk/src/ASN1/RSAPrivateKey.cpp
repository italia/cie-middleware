
#include "RSAPrivateKey.h"

#undef _WIN32_WINNT
#define _WIN32_WINNT  0x400
//#include <wincrypt.h>

//RSAPrivateKey :: = SEQUENCE{
//	version           Version,
//	modulus           INTEGER, --n
//	publicExponent    INTEGER, --e
//	privateExponent   INTEGER, --d
//	prime1            INTEGER, --p
//	prime2            INTEGER, --q
//	exponent1         INTEGER, --d mod(p - 1)
//	exponent2         INTEGER, --d mod(q - 1)
//	coefficient       INTEGER, --(inverse of q) mod p
//	otherPrimeInfos   OtherPrimeInfos OPTIONAL
//}

// Macro per trasformazione da little-endian a big-endian e viceversa
#define REVERSE(in, out, len)	\
{										\
	for(int i = 0; i < len; i++)		\
	{									\
		out[i] = in[len - i - 1];			\
	}									\
}							

CRSAPrivateKey::CRSAPrivateKey(UUCBufferedReader& reader)
: CASN1Sequence(reader)
{
}
	
CRSAPrivateKey::CRSAPrivateKey(const CASN1Object& obj)
: CASN1Sequence(obj)
{

}
	
CRSAPrivateKey::CRSAPrivateKey(const UUCByteArray& content)
: CASN1Sequence(content)
{

}

CRSAPrivateKey::CRSAPrivateKey(const CASN1Integer& modulus, const CASN1Integer& pubExp, const CASN1Integer& priExp)
{
	addElement(modulus);
	addElement(pubExp);
	addElement(priExp);
}
/*
CRSAPrivateKey::CRSAPrivateKey(const BYTE* pbtRSAKey_MS, const DWORD dwLen)
{
	BLOBHEADER header;
	header.bType = pbtRSAKey_MS[0];
	header.bVersion = pbtRSAKey_MS[1];
	header.reserved = 0x0000;
	header.aiKeyAlg = pbtRSAKey_MS[4] + pbtRSAKey_MS[5] * 0x100 + pbtRSAKey_MS[6] * 0x10000 + pbtRSAKey_MS[7] * 0x1000000;

	char szMagic[5];

	memcpy((BYTE*)szMagic, pbtRSAKey_MS + 8, 4);
	szMagic[4] = '\0';
	if(strcmp((char*)szMagic, "RSA2") != 0)
	{
		throw(-100);
	}

	BYTE* pbtAux;

	// Lunghezza modulo
	DWORD dwModLen = (pbtRSAKey_MS[12] + pbtRSAKey_MS[13] * 0x100 + pbtRSAKey_MS[14] * 0x10000 + pbtRSAKey_MS[15] * 0x1000000) / 8;
	
	// Esponente pubblico
	BYTE pbtPubExp[4];
	pbtAux = (BYTE*)pbtRSAKey_MS + 16; 
	REVERSE(pbtAux, pbtPubExp, 4);
	DWORD dwPubExpLen = 4;

	// Modulo
	BYTE pbtMod[512];
	pbtAux = (BYTE*)pbtRSAKey_MS + 20; 
	REVERSE(pbtAux, pbtMod, dwModLen);

	// Esponente privato
	BYTE pbtPriExp[512];
	pbtAux = (BYTE*)pbtRSAKey_MS + dwLen - dwModLen;
	REVERSE(pbtAux, pbtPriExp, dwModLen);
	DWORD dwPriExpLen = dwModLen;

	addElement(CASN1Integer(pbtMod, dwModLen));
	addElement(CASN1Integer(pbtPubExp, dwPubExpLen));
	addElement(CASN1Integer(pbtPriExp, dwPriExpLen));
}
 */
CRSAPrivateKey::~CRSAPrivateKey(void)
{

}

CASN1Integer CRSAPrivateKey::getModulus() 
{
	return CASN1Integer(elementAt(1));
}

CASN1Integer CRSAPrivateKey::getPublicExponent() 
{
	return CASN1Integer(elementAt(2));
}

CASN1Integer CRSAPrivateKey::getPrivateExponent() 
{
	return CASN1Integer(elementAt(3));
}

