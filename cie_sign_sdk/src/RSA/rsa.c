/*
	RSA.C - RSA routines for RSAEURO

    Copyright (c) J.S.A.Kapp 1994 - 1996.

	RSAEURO - RSA Library compatible with RSAREF(tm) 2.0.

	All functions prototypes are the Same as for RSAREF(tm).
	To aid compatiblity the source and the files follow the
	same naming comventions that RSAREF(tm) uses.  This should aid
	direct importing to your applications.

	This library is legal everywhere outside the US.  And should
	NOT be imported to the US and used there.

	All Trademarks Acknowledged.

	RSA encryption performed as defined in the PKCS (#1) by RSADSI.

	Revision history
		0.90 First revision, code produced very similar to that
		of RSAREF(tm), still it worked fine.

        0.91 Second revision, code altered to aid speeding up.
		Used pointer accesses to arrays to speed up some parts,
		mainly during the loops.

        1.03 Third revision, Random Structure initialization
        double check, RSAPublicEncrypt can now return RE_NEED_RANDOM.
*/

#include "rsaeuro.h"
//#include "r_random.h"
#include "rsa.h"
#include "nn.h"

	
static int rsapublicfunc PROTO_LIST((unsigned char *, unsigned int *, unsigned char *, unsigned int, R_RSA_PUBLIC_KEY *));
static int rsaprivatefunc PROTO_LIST((unsigned char *, unsigned int *, unsigned char *, unsigned int, R_RSA_PRIVATE_KEY *));

/* RSA decryption, according to RSADSI's PKCS #1. */

int RSAPublicDecrypt(
	unsigned char *output,          /* output block */
	unsigned int *outputLen,        /* length of output block */
	unsigned char *input,           /* input block */
	unsigned int inputLen,          /* length of input block */
	R_RSA_PUBLIC_KEY *publicKey)    /* RSA public key */
{
	int status;
	unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN];
	unsigned int i, modulusLen, pkcsBlockLen;

	modulusLen = (publicKey->bits + 7) / 8;

	if(inputLen > modulusLen)
		return(RE_LEN);

	status = rsapublicfunc(pkcsBlock, &pkcsBlockLen, input, inputLen, publicKey);
	if(status)
		return(status);

	if(pkcsBlockLen != modulusLen)
		return(RE_LEN);

	/* Require block type 1. */

	if((pkcsBlock[0] != 0) || (pkcsBlock[1] != 1))
	 return(RE_DATA + 0x2000);

	for(i = 2; i < modulusLen-1; i++)
		if(*(pkcsBlock+i) != 0xff)
			break;

	/* separator check */

	if(pkcsBlock[i++] != 0)
		return(RE_DATA + + 0x3000);

	*outputLen = modulusLen - i;

	if(*outputLen + 11 > modulusLen)
		return(RE_DATA + 0x4000);

	memcpy((POINTER)output, (POINTER)&pkcsBlock[i], *outputLen);

	/* Clear sensitive information. */

	memset((POINTER)pkcsBlock, 0, sizeof(pkcsBlock));

	return(ID_OK);
}

/* RSA encryption, according to RSADSI's PKCS #1. */

int RSAPrivateEncrypt(
	unsigned char *output,          /* output block */
	unsigned int *outputLen,        /* length of output block */
	unsigned char *input,           /* input block */
	unsigned int inputLen,          /* length of input block */
	R_RSA_PRIVATE_KEY *privateKey)  /* RSA private key */
{
	int status;
	unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN];
	unsigned int i, modulusLen;

	modulusLen = (privateKey->bits + 7) / 8;

	if(inputLen + 11 > modulusLen)
		return (RE_LEN);

	*pkcsBlock = 0;
	/* block type 1 */
	*(pkcsBlock+1) = 1;

	for (i = 2; i < modulusLen - inputLen - 1; i++)
		*(pkcsBlock+i) = 0xff;

	/* separator */
	pkcsBlock[i++] = 0;

	memcpy((POINTER)&pkcsBlock[i], (POINTER)input, inputLen);

	status = rsaprivatefunc(output, outputLen, pkcsBlock, modulusLen, privateKey);

	/* Clear sensitive information. */

	memset((POINTER)pkcsBlock, 0, sizeof(pkcsBlock));

	return(status);
}

/* RSA decryption, according to RSADSI's PKCS #1. */

int RSAPrivateDecrypt(
	unsigned char *output,          /* output block */
	unsigned int *outputLen,        /* length of output block */
	unsigned char *input,           /* input block */
	unsigned int inputLen,          /* length of input block */
	R_RSA_PRIVATE_KEY *privateKey)  /* RSA private key */
{
	int status;
	unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN];
	unsigned int i, modulusLen, pkcsBlockLen;

	modulusLen = (privateKey->bits + 7) / 8;

	if(inputLen > modulusLen)
		return (RE_LEN);

	status = rsaprivatefunc(pkcsBlock, &pkcsBlockLen, input, inputLen, privateKey);
	if(status)
		return (status);

	if(pkcsBlockLen != modulusLen)
		return (RE_LEN);

	/* We require block type 2. */

	if((*pkcsBlock != 0) || (*(pkcsBlock+1) != 2))
	 return (RE_DATA);

	for(i = 2; i < modulusLen-1; i++)
		/* separator */
		if (*(pkcsBlock+i) == 0)
			break;

	i++;
	if(i >= modulusLen)
		return(RE_DATA);

	*outputLen = modulusLen - i;

	if(*outputLen + 11 > modulusLen)
		return(RE_DATA);

	memcpy((POINTER)output, (POINTER)&pkcsBlock[i], *outputLen);

	/* Clear sensitive information. */
	memset((POINTER)pkcsBlock, 0, sizeof(pkcsBlock));

	return(ID_OK);
}

/* Raw RSA public-key operation. Output has same length as modulus.

	 Requires input < modulus.
*/
static int rsapublicfunc(
	unsigned char *output,          /* output block */
	unsigned int *outputLen,        /* length of output block */
	unsigned char *input,           /* input block */
	unsigned int inputLen,          /* length of input block */
	R_RSA_PUBLIC_KEY *publicKey)    /* RSA public key */
{
	NN_DIGIT c[MAX_NN_DIGITS], e[MAX_NN_DIGITS], m[MAX_NN_DIGITS],
		n[MAX_NN_DIGITS];
	unsigned int eDigits, nDigits;


		/* decode the required RSA function input data */
	NN_Decode(m, MAX_NN_DIGITS, input, inputLen);
	NN_Decode(n, MAX_NN_DIGITS, publicKey->modulus, MAX_RSA_MODULUS_LEN);
	NN_Decode(e, MAX_NN_DIGITS, publicKey->exponent, MAX_RSA_MODULUS_LEN);

	nDigits = NN_Digits(n, MAX_NN_DIGITS);
	eDigits = NN_Digits(e, MAX_NN_DIGITS);

	if(NN_Cmp(m, n, nDigits) >= 0)
		return(RE_DATA + 0x1000);

	*outputLen = (publicKey->bits + 7) / 8;

	/* Compute c = m^e mod n.  To perform actual RSA calc.*/

	NN_ModExp (c, m, e, eDigits, n, nDigits);

	/* encode output to standard form */
	NN_Encode (output, *outputLen, c, nDigits);

	/* Clear sensitive information. */

	memset((POINTER)c, 0, sizeof(c));
	memset((POINTER)m, 0, sizeof(m));

	return(ID_OK);
}

/* Raw RSA private-key operation. Output has same length as modulus.

	 Requires input < modulus.
*/

static int rsaprivatefunc(
	unsigned char *output,          /* output block */
	unsigned int *outputLen,        /* length of output block */
	unsigned char *input,           /* input block */
	unsigned int inputLen,          /* length of input block */
	R_RSA_PRIVATE_KEY *privateKey)  /* RSA private key */
{
	NN_DIGIT c[MAX_NN_DIGITS], cP[MAX_NN_DIGITS], cQ[MAX_NN_DIGITS],
		dP[MAX_NN_DIGITS], dQ[MAX_NN_DIGITS], mP[MAX_NN_DIGITS],
		mQ[MAX_NN_DIGITS], n[MAX_NN_DIGITS], p[MAX_NN_DIGITS], q[MAX_NN_DIGITS],
		qInv[MAX_NN_DIGITS], t[MAX_NN_DIGITS];
	unsigned int cDigits, nDigits, pDigits;

	/* decode required input data from standard form */
	NN_Decode(c, MAX_NN_DIGITS, input, inputLen);           /* input */

					/* private key data */
	NN_Decode(p, MAX_NN_DIGITS, privateKey->prime[0], MAX_RSA_PRIME_LEN);
	NN_Decode(q, MAX_NN_DIGITS, privateKey->prime[1], MAX_RSA_PRIME_LEN);
	NN_Decode(dP, MAX_NN_DIGITS, privateKey->primeExponent[0], MAX_RSA_PRIME_LEN);
	NN_Decode(dQ, MAX_NN_DIGITS, privateKey->primeExponent[1], MAX_RSA_PRIME_LEN);
	NN_Decode(n, MAX_NN_DIGITS, privateKey->modulus, MAX_RSA_MODULUS_LEN);
	NN_Decode(qInv, MAX_NN_DIGITS, privateKey->coefficient, MAX_RSA_PRIME_LEN);
		/* work out lengths of input components */

    cDigits = NN_Digits(c, MAX_NN_DIGITS);
    pDigits = NN_Digits(p, MAX_NN_DIGITS);
	nDigits = NN_Digits(n, MAX_NN_DIGITS);


	if(NN_Cmp(c, n, nDigits) >= 0)
		return(RE_DATA);

	*outputLen = (privateKey->bits + 7) / 8;

	/* Compute mP = cP^dP mod p  and  mQ = cQ^dQ mod q. (Assumes q has
		 length at most pDigits, i.e., p > q.)
	*/

	NN_Mod(cP, c, cDigits, p, pDigits);
	NN_Mod(cQ, c, cDigits, q, pDigits);

	NN_AssignZero(mP, nDigits);
	NN_ModExp(mP, cP, dP, pDigits, p, pDigits);

	NN_AssignZero(mQ, nDigits);
	NN_ModExp(mQ, cQ, dQ, pDigits, q, pDigits);

	/* Chinese Remainder Theorem:
			m = ((((mP - mQ) mod p) * qInv) mod p) * q + mQ.
	*/

	if(NN_Cmp(mP, mQ, pDigits) >= 0) {
		NN_Sub(t, mP, mQ, pDigits);
	}else{
		NN_Sub(t, mQ, mP, pDigits);
		NN_Sub(t, p, t, pDigits);
	}

	NN_ModMult(t, t, qInv, p, pDigits);
	NN_Mult(t, t, q, pDigits);
	NN_Add(t, t, mQ, nDigits);

	/* encode output to standard form */
	NN_Encode (output, *outputLen, t, nDigits);

	/* Clear sensitive information. */
	memset((POINTER)c, 0, sizeof(c));
	memset((POINTER)cP, 0, sizeof(cP));
	memset((POINTER)cQ, 0, sizeof(cQ));
	memset((POINTER)dP, 0, sizeof(dP));
	memset((POINTER)dQ, 0, sizeof(dQ));
	memset((POINTER)mP, 0, sizeof(mP));
	memset((POINTER)mQ, 0, sizeof(mQ));
	memset((POINTER)p, 0, sizeof(p));
	memset((POINTER)q, 0, sizeof(q));
	memset((POINTER)qInv, 0, sizeof(qInv));
	memset((POINTER)t, 0, sizeof(t));
	return(ID_OK);
}
