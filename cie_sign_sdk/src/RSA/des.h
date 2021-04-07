/*
	DES.H - header file for DESC.C

    Copyright (c) J.S.A.Kapp 1994 - 1996.

	RSAEURO - RSA Library compatible with RSAREF 2.0.

	All functions prototypes are the Same as for RSAREF.
	To aid compatiblity the source and the files follow the
	same naming comventions that RSAREF uses.  This should aid
	direct importing to your applications.

	This library is legal everywhere outside the US.  And should
	NOT be imported to the US and used there.

	DES Code header file.

	Revision 1.00 - JSAK.
*/

#ifndef _DES_H_
#define _DES_H_

/* Error codes. */

#define RE_CONTENT_ENCODING 0x0400
#define RE_DATA 0x0401
#define RE_DIGEST_ALGORITHM 0x0402
#define RE_ENCODING 0x0403
#define RE_KEY 0x0404
#define RE_KEY_ENCODING 0x0405
#define RE_LEN 0x0406
#define RE_MODULUS_LEN 0x0407
#define RE_NEED_RANDOM 0x0408
#define RE_PRIVATE_KEY 0x0409
#define RE_PUBLIC_KEY 0x040a
#define RE_SIGNATURE 0x040b
#define RE_SIGNATURE_ENCODING 0x040c
#define RE_ENCRYPTION_ALGORITHM 0x040d
#define RE_FILE 0x040e


#define ID_OK    0
#define ID_ERROR 1

/* Internal defs. */

#define TRUE    1
#define FALSE   0

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

//#if PROTOTYPES
#define PROTO_LIST(list) list
//#else
//#define PROTO_LIST(list) ()
//#endif

#define R_memset(x, y, z) memset(x, y, z)
#define R_memcpy(x, y, z) memcpy(x, y, z)
#define R_memcmp(x, y, z) memcmp(x, y, z)


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  UINT4 subkeys[32];                                             /* subkeys */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                               /* encrypt flag */
} DES_CBC_CTX;

typedef struct {
  UINT4 subkeys[32];                                             /* subkeys */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 inputWhitener[2];                                 /* input whitener */
  UINT4 outputWhitener[2];                               /* output whitener */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                              /* encrypt flag */
} DESX_CBC_CTX;

typedef struct {
  UINT4 subkeys[3][32];                     /* subkeys for three operations */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                              /* encrypt flag */
} DES3_CBC_CTX;

void DES_CBCInit PROTO_LIST 
  ((DES_CBC_CTX *, unsigned char *, unsigned char *, int));
int DES_CBCUpdate PROTO_LIST
  ((DES_CBC_CTX *, unsigned char *, unsigned char *, unsigned int));
void DES_CBCRestart PROTO_LIST ((DES_CBC_CTX *));

void DESX_CBCInit PROTO_LIST
  ((DESX_CBC_CTX *, unsigned char *, unsigned char *, int));
int DESX_CBCUpdate PROTO_LIST
  ((DESX_CBC_CTX *, unsigned char *, unsigned char *, unsigned int));
void DESX_CBCRestart PROTO_LIST ((DESX_CBC_CTX *));

void DES3_CBCInit PROTO_LIST 
  ((DES3_CBC_CTX *, unsigned char *, unsigned char *, int));
int DES3_CBCUpdate PROTO_LIST
  ((DES3_CBC_CTX *, unsigned char *, unsigned char *, unsigned int));
void DES3_CBCRestart PROTO_LIST ((DES3_CBC_CTX *));

#ifdef __cplusplus
}
#endif
#endif /* _DES_H_ */
