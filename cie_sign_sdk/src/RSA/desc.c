/*
	DESC.C - Data Encryption Standard routines for RSAEURO

    Copyright (c) J.S.A.Kapp 1994 - 1996.

	RSAEURO - RSA Library compatible with RSAREF(tm) 2.0.

	All functions prototypes are the Same as for RSAREF(tm).
	To aid compatiblity the source and the files follow the
	same naming comventions that RSAREF(tm) uses.  This should aid
	direct importing to your applications.

	This library is legal everywhere outside the US.  And should
	NOT be imported to the US and used there.

	Based on Outerbridge's D3DES (V5.09) 1992 Vintage.

	DESX(tm) - RSA Data Security.

	DES386 to be define ONLY in conjunction with 386 compiled
	code.

	All Trademarks Acknowledged.

	Revision history
	0.90 First revision, this was the original retrofitted D3DES
	version.

	0.91 Second revision, retrofitted new S-box array and new desfunc
	routine.  Marginally quicker code improves DES throughput.

	0.92 Current revision, added support for 386 assembler desfunc
	routine, with altered S boxes and key generation to support easier
	S box look up.  Code that uses 386 desfunc is about 80K per sec
	faster than RSAREF(tm) code.
*/


#include "des.h"
#include <string.h>


static UINT2 bytebit[8] = {
	0200, 0100, 040, 020, 010, 04, 02, 01
};

static UINT4 bigbyte[24] = {
	0x800000L, 0x400000L, 0x200000L, 0x100000L,
	0x80000L,  0x40000L,  0x20000L,  0x10000L,
	0x8000L,   0x4000L,   0x2000L,   0x1000L,
	0x800L,    0x400L,    0x200L,    0x100L,
	0x80L,     0x40L,     0x20L,     0x10L,
	0x8L,      0x4L,      0x2L,      0x1L
};

static unsigned char totrot[16] = {
	1, 2, 4, 6, 8, 10, 12, 14, 15, 17, 19, 21, 23, 25, 27, 28
};

static unsigned char pc1[56] = {
	56, 48, 40, 32, 24, 16,  8,      0, 57, 49, 41, 33, 25, 17,
	 9,  1, 58, 50, 42, 34, 26,     18, 10,  2, 59, 51, 43, 35,
	62, 54, 46, 38, 30, 22, 14,      6, 61, 53, 45, 37, 29, 21,
	13,  5, 60, 52, 44, 36, 28,     20, 12,  4, 27, 19, 11,  3
};

static unsigned char pc2[48] = {
	13, 16, 10, 23,  0,  4,  2, 27, 14,  5, 20,  9,
	22, 18, 11,  3, 25,  7, 15,  6, 26, 19, 12,  1,
	40, 51, 30, 36, 46, 54, 29, 39, 50, 44, 32, 47,
	43, 48, 38, 55, 33, 52, 45, 41, 49, 35, 28, 31
};

#ifndef DES386

UINT4 Spbox[8][64] = {
	0x01010400L, 0x00000000L, 0x00010000L, 0x01010404L,
	0x01010004L, 0x00010404L, 0x00000004L, 0x00010000L,
	0x00000400L, 0x01010400L, 0x01010404L, 0x00000400L,
	0x01000404L, 0x01010004L, 0x01000000L, 0x00000004L,
	0x00000404L, 0x01000400L, 0x01000400L, 0x00010400L,
	0x00010400L, 0x01010000L, 0x01010000L, 0x01000404L,
	0x00010004L, 0x01000004L, 0x01000004L, 0x00010004L,
	0x00000000L, 0x00000404L, 0x00010404L, 0x01000000L,
	0x00010000L, 0x01010404L, 0x00000004L, 0x01010000L,
	0x01010400L, 0x01000000L, 0x01000000L, 0x00000400L,
	0x01010004L, 0x00010000L, 0x00010400L, 0x01000004L,
	0x00000400L, 0x00000004L, 0x01000404L, 0x00010404L,
	0x01010404L, 0x00010004L, 0x01010000L, 0x01000404L,
	0x01000004L, 0x00000404L, 0x00010404L, 0x01010400L,
	0x00000404L, 0x01000400L, 0x01000400L, 0x00000000L,
	0x00010004L, 0x00010400L, 0x00000000L, 0x01010004L,
	0x80108020L, 0x80008000L, 0x00008000L, 0x00108020L,
	0x00100000L, 0x00000020L, 0x80100020L, 0x80008020L,
	0x80000020L, 0x80108020L, 0x80108000L, 0x80000000L,
	0x80008000L, 0x00100000L, 0x00000020L, 0x80100020L,
	0x00108000L, 0x00100020L, 0x80008020L, 0x00000000L,
	0x80000000L, 0x00008000L, 0x00108020L, 0x80100000L,
	0x00100020L, 0x80000020L, 0x00000000L, 0x00108000L,
	0x00008020L, 0x80108000L, 0x80100000L, 0x00008020L,
	0x00000000L, 0x00108020L, 0x80100020L, 0x00100000L,
	0x80008020L, 0x80100000L, 0x80108000L, 0x00008000L,
	0x80100000L, 0x80008000L, 0x00000020L, 0x80108020L,
	0x00108020L, 0x00000020L, 0x00008000L, 0x80000000L,
	0x00008020L, 0x80108000L, 0x00100000L, 0x80000020L,
	0x00100020L, 0x80008020L, 0x80000020L, 0x00100020L,
	0x00108000L, 0x00000000L, 0x80008000L, 0x00008020L,
	0x80000000L, 0x80100020L, 0x80108020L, 0x00108000L,
	0x00000208L, 0x08020200L, 0x00000000L, 0x08020008L,
	0x08000200L, 0x00000000L, 0x00020208L, 0x08000200L,
	0x00020008L, 0x08000008L, 0x08000008L, 0x00020000L,
	0x08020208L, 0x00020008L, 0x08020000L, 0x00000208L,
	0x08000000L, 0x00000008L, 0x08020200L, 0x00000200L,
	0x00020200L, 0x08020000L, 0x08020008L, 0x00020208L,
	0x08000208L, 0x00020200L, 0x00020000L, 0x08000208L,
	0x00000008L, 0x08020208L, 0x00000200L, 0x08000000L,
	0x08020200L, 0x08000000L, 0x00020008L, 0x00000208L,
	0x00020000L, 0x08020200L, 0x08000200L, 0x00000000L,
	0x00000200L, 0x00020008L, 0x08020208L, 0x08000200L,
	0x08000008L, 0x00000200L, 0x00000000L, 0x08020008L,
	0x08000208L, 0x00020000L, 0x08000000L, 0x08020208L,
	0x00000008L, 0x00020208L, 0x00020200L, 0x08000008L,
	0x08020000L, 0x08000208L, 0x00000208L, 0x08020000L,
	0x00020208L, 0x00000008L, 0x08020008L, 0x00020200L,
	0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
	0x00802080L, 0x00800081L, 0x00800001L, 0x00002001L,
	0x00000000L, 0x00802000L, 0x00802000L, 0x00802081L,
	0x00000081L, 0x00000000L, 0x00800080L, 0x00800001L,
	0x00000001L, 0x00002000L, 0x00800000L, 0x00802001L,
	0x00000080L, 0x00800000L, 0x00002001L, 0x00002080L,
	0x00800081L, 0x00000001L, 0x00002080L, 0x00800080L,
	0x00002000L, 0x00802080L, 0x00802081L, 0x00000081L,
	0x00800080L, 0x00800001L, 0x00802000L, 0x00802081L,
	0x00000081L, 0x00000000L, 0x00000000L, 0x00802000L,
	0x00002080L, 0x00800080L, 0x00800081L, 0x00000001L,
	0x00802001L, 0x00002081L, 0x00002081L, 0x00000080L,
	0x00802081L, 0x00000081L, 0x00000001L, 0x00002000L,
	0x00800001L, 0x00002001L, 0x00802080L, 0x00800081L,
	0x00002001L, 0x00002080L, 0x00800000L, 0x00802001L,
	0x00000080L, 0x00800000L, 0x00002000L, 0x00802080L,
	0x00000100L, 0x02080100L, 0x02080000L, 0x42000100L,
	0x00080000L, 0x00000100L, 0x40000000L, 0x02080000L,
	0x40080100L, 0x00080000L, 0x02000100L, 0x40080100L,
	0x42000100L, 0x42080000L, 0x00080100L, 0x40000000L,
	0x02000000L, 0x40080000L, 0x40080000L, 0x00000000L,
	0x40000100L, 0x42080100L, 0x42080100L, 0x02000100L,
	0x42080000L, 0x40000100L, 0x00000000L, 0x42000000L,
	0x02080100L, 0x02000000L, 0x42000000L, 0x00080100L,
	0x00080000L, 0x42000100L, 0x00000100L, 0x02000000L,
	0x40000000L, 0x02080000L, 0x42000100L, 0x40080100L,
	0x02000100L, 0x40000000L, 0x42080000L, 0x02080100L,
	0x40080100L, 0x00000100L, 0x02000000L, 0x42080000L,
	0x42080100L, 0x00080100L, 0x42000000L, 0x42080100L,
	0x02080000L, 0x00000000L, 0x40080000L, 0x42000000L,
	0x00080100L, 0x02000100L, 0x40000100L, 0x00080000L,
	0x00000000L, 0x40080000L, 0x02080100L, 0x40000100L,
	0x20000010L, 0x20400000L, 0x00004000L, 0x20404010L,
	0x20400000L, 0x00000010L, 0x20404010L, 0x00400000L,
	0x20004000L, 0x00404010L, 0x00400000L, 0x20000010L,
	0x00400010L, 0x20004000L, 0x20000000L, 0x00004010L,
	0x00000000L, 0x00400010L, 0x20004010L, 0x00004000L,
	0x00404000L, 0x20004010L, 0x00000010L, 0x20400010L,
	0x20400010L, 0x00000000L, 0x00404010L, 0x20404000L,
	0x00004010L, 0x00404000L, 0x20404000L, 0x20000000L,
	0x20004000L, 0x00000010L, 0x20400010L, 0x00404000L,
	0x20404010L, 0x00400000L, 0x00004010L, 0x20000010L,
	0x00400000L, 0x20004000L, 0x20000000L, 0x00004010L,
	0x20000010L, 0x20404010L, 0x00404000L, 0x20400000L,
	0x00404010L, 0x20404000L, 0x00000000L, 0x20400010L,
	0x00000010L, 0x00004000L, 0x20400000L, 0x00404010L,
	0x00004000L, 0x00400010L, 0x20004010L, 0x00000000L,
	0x20404000L, 0x20000000L, 0x00400010L, 0x20004010L,
	0x00200000L, 0x04200002L, 0x04000802L, 0x00000000L,
	0x00000800L, 0x04000802L, 0x00200802L, 0x04200800L,
	0x04200802L, 0x00200000L, 0x00000000L, 0x04000002L,
	0x00000002L, 0x04000000L, 0x04200002L, 0x00000802L,
	0x04000800L, 0x00200802L, 0x00200002L, 0x04000800L,
	0x04000002L, 0x04200000L, 0x04200800L, 0x00200002L,
	0x04200000L, 0x00000800L, 0x00000802L, 0x04200802L,
	0x00200800L, 0x00000002L, 0x04000000L, 0x00200800L,
	0x04000000L, 0x00200800L, 0x00200000L, 0x04000802L,
	0x04000802L, 0x04200002L, 0x04200002L, 0x00000002L,
	0x00200002L, 0x04000000L, 0x04000800L, 0x00200000L,
	0x04200800L, 0x00000802L, 0x00200802L, 0x04200800L,
	0x00000802L, 0x04000002L, 0x04200802L, 0x04200000L,
	0x00200800L, 0x00000000L, 0x00000002L, 0x04200802L,
	0x00000000L, 0x00200802L, 0x04200000L, 0x00000800L,
	0x04000002L, 0x04000800L, 0x00000800L, 0x00200002L,
	0x10001040L, 0x00001000L, 0x00040000L, 0x10041040L,
	0x10000000L, 0x10001040L, 0x00000040L, 0x10000000L,
	0x00040040L, 0x10040000L, 0x10041040L, 0x00041000L,
	0x10041000L, 0x00041040L, 0x00001000L, 0x00000040L,
	0x10040000L, 0x10000040L, 0x10001000L, 0x00001040L,
	0x00041000L, 0x00040040L, 0x10040040L, 0x10041000L,
	0x00001040L, 0x00000000L, 0x00000000L, 0x10040040L,
	0x10000040L, 0x10001000L, 0x00041040L, 0x00040000L,
	0x00041040L, 0x00040000L, 0x10041000L, 0x00001000L,
	0x00000040L, 0x10040040L, 0x00001000L, 0x00041040L,
	0x10001000L, 0x00000040L, 0x10000040L, 0x10040000L,
	0x10040040L, 0x10000000L, 0x00040000L, 0x10001040L,
	0x00000000L, 0x10041040L, 0x00040040L, 0x10000040L,
	0x10040000L, 0x10001000L, 0x10001040L, 0x00000000L,
	0x10041040L, 0x00041000L, 0x00041000L, 0x00001040L,
	0x00001040L, 0x00040040L, 0x10000000L, 0x10041000L
};

#else
	/* S box tables for assembler desfunc */

unsigned long Spbox[8][64] = {
	0x04041000,0x00000000,0x00040000,0x04041010,
	0x04040010,0x00041010,0x00000010,0x00040000,
	0x00001000,0x04041000,0x04041010,0x00001000,
	0x04001010,0x04040010,0x04000000,0x00000010,
	0x00001010,0x04001000,0x04001000,0x00041000,
	0x00041000,0x04040000,0x04040000,0x04001010,
	0x00040010,0x04000010,0x04000010,0x00040010,
	0x00000000,0x00001010,0x00041010,0x04000000,
	0x00040000,0x04041010,0x00000010,0x04040000,
	0x04041000,0x04000000,0x04000000,0x00001000,
	0x04040010,0x00040000,0x00041000,0x04000010,
	0x00001000,0x00000010,0x04001010,0x00041010,
	0x04041010,0x00040010,0x04040000,0x04001010,
	0x04000010,0x00001010,0x00041010,0x04041000,
	0x00001010,0x04001000,0x04001000,0x00000000,
	0x00040010,0x00041000,0x00000000,0x04040010,
	0x00420082,0x00020002,0x00020000,0x00420080,
	0x00400000,0x00000080,0x00400082,0x00020082,
	0x00000082,0x00420082,0x00420002,0x00000002,
	0x00020002,0x00400000,0x00000080,0x00400082,
	0x00420000,0x00400080,0x00020082,0x00000000,
	0x00000002,0x00020000,0x00420080,0x00400002,
	0x00400080,0x00000082,0x00000000,0x00420000,
	0x00020080,0x00420002,0x00400002,0x00020080,
	0x00000000,0x00420080,0x00400082,0x00400000,
	0x00020082,0x00400002,0x00420002,0x00020000,
	0x00400002,0x00020002,0x00000080,0x00420082,
	0x00420080,0x00000080,0x00020000,0x00000002,
	0x00020080,0x00420002,0x00400000,0x00000082,
	0x00400080,0x00020082,0x00000082,0x00400080,
	0x00420000,0x00000000,0x00020002,0x00020080,
	0x00000002,0x00400082,0x00420082,0x00420000,
	0x00000820,0x20080800,0x00000000,0x20080020,
	0x20000800,0x00000000,0x00080820,0x20000800,
	0x00080020,0x20000020,0x20000020,0x00080000,
	0x20080820,0x00080020,0x20080000,0x00000820,
	0x20000000,0x00000020,0x20080800,0x00000800,
	0x00080800,0x20080000,0x20080020,0x00080820,
	0x20000820,0x00080800,0x00080000,0x20000820,
	0x00000020,0x20080820,0x00000800,0x20000000,
	0x20080800,0x20000000,0x00080020,0x00000820,
	0x00080000,0x20080800,0x20000800,0x00000000,
	0x00000800,0x00080020,0x20080820,0x20000800,
	0x20000020,0x00000800,0x00000000,0x20080020,
	0x20000820,0x00080000,0x20000000,0x20080820,
	0x00000020,0x00080820,0x00080800,0x20000020,
	0x20080000,0x20000820,0x00000820,0x20080000,
	0x00080820,0x00000020,0x20080020,0x00080800,
	0x02008004,0x00008204,0x00008204,0x00000200,
	0x02008200,0x02000204,0x02000004,0x00008004,
	0x00000000,0x02008000,0x02008000,0x02008204,
	0x00000204,0x00000000,0x02000200,0x02000004,
	0x00000004,0x00008000,0x02000000,0x02008004,
	0x00000200,0x02000000,0x00008004,0x00008200,
	0x02000204,0x00000004,0x00008200,0x02000200,
	0x00008000,0x02008200,0x02008204,0x00000204,
	0x02000200,0x02000004,0x02008000,0x02008204,
	0x00000204,0x00000000,0x00000000,0x02008000,
	0x00008200,0x02000200,0x02000204,0x00000004,
	0x02008004,0x00008204,0x00008204,0x00000200,
	0x02008204,0x00000204,0x00000004,0x00008000,
	0x02000004,0x00008004,0x02008200,0x02000204,
	0x00008004,0x00008200,0x02000000,0x02008004,
	0x00000200,0x02000000,0x00008000,0x02008200,
	0x00000400,0x08200400,0x08200000,0x08000401,
	0x00200000,0x00000400,0x00000001,0x08200000,
	0x00200401,0x00200000,0x08000400,0x00200401,
	0x08000401,0x08200001,0x00200400,0x00000001,
	0x08000000,0x00200001,0x00200001,0x00000000,
	0x00000401,0x08200401,0x08200401,0x08000400,
	0x08200001,0x00000401,0x00000000,0x08000001,
	0x08200400,0x08000000,0x08000001,0x00200400,
	0x00200000,0x08000401,0x00000400,0x08000000,
	0x00000001,0x08200000,0x08000401,0x00200401,
	0x08000400,0x00000001,0x08200001,0x08200400,
	0x00200401,0x00000400,0x08000000,0x08200001,
	0x08200401,0x00200400,0x08000001,0x08200401,
	0x08200000,0x00000000,0x00200001,0x08000001,
	0x00200400,0x08000400,0x00000401,0x00200000,
	0x00000000,0x00200001,0x08200400,0x00000401,
	0x80000040,0x81000000,0x00010000,0x81010040,
	0x81000000,0x00000040,0x81010040,0x01000000,
	0x80010000,0x01010040,0x01000000,0x80000040,
	0x01000040,0x80010000,0x80000000,0x00010040,
	0x00000000,0x01000040,0x80010040,0x00010000,
	0x01010000,0x80010040,0x00000040,0x81000040,
	0x81000040,0x00000000,0x01010040,0x81010000,
	0x00010040,0x01010000,0x81010000,0x80000000,
	0x80010000,0x00000040,0x81000040,0x01010000,
	0x81010040,0x01000000,0x00010040,0x80000040,
	0x01000000,0x80010000,0x80000000,0x00010040,
	0x80000040,0x81010040,0x01010000,0x81000000,
	0x01010040,0x81010000,0x00000000,0x81000040,
	0x00000040,0x00010000,0x81000000,0x01010040,
	0x00010000,0x01000040,0x80010040,0x00000000,
	0x81010000,0x80000000,0x01000040,0x80010040,
	0x00800000,0x10800008,0x10002008,0x00000000,
	0x00002000,0x10002008,0x00802008,0x10802000,
	0x10802008,0x00800000,0x00000000,0x10000008,
	0x00000008,0x10000000,0x10800008,0x00002008,
	0x10002000,0x00802008,0x00800008,0x10002000,
	0x10000008,0x10800000,0x10802000,0x00800008,
	0x10800000,0x00002000,0x00002008,0x10802008,
	0x00802000,0x00000008,0x10000000,0x00802000,
	0x10000000,0x00802000,0x00800000,0x10002008,
	0x10002008,0x10800008,0x10800008,0x00000008,
	0x00800008,0x10000000,0x10002000,0x00800000,
	0x10802000,0x00002008,0x00802008,0x10802000,
	0x00002008,0x10000008,0x10802008,0x10800000,
	0x00802000,0x00000000,0x00000008,0x10802008,
	0x00000000,0x00802008,0x10800000,0x00002000,
	0x10000008,0x10002000,0x00002000,0x00800008,
	0x40004100,0x00004000,0x00100000,0x40104100,
	0x40000000,0x40004100,0x00000100,0x40000000,
	0x00100100,0x40100000,0x40104100,0x00104000,
	0x40104000,0x00104100,0x00004000,0x00000100,
	0x40100000,0x40000100,0x40004000,0x00004100,
	0x00104000,0x00100100,0x40100100,0x40104000,
	0x00004100,0x00000000,0x00000000,0x40100100,
	0x40000100,0x40004000,0x00104100,0x00100000,
	0x00104100,0x00100000,0x40104000,0x00004000,
	0x00000100,0x40100100,0x00004000,0x00104100,
	0x40004000,0x00000100,0x40000100,0x40100000,
	0x40100100,0x40000000,0x00100000,0x40004100,
	0x00000000,0x40104100,0x00100100,0x40000100,
	0x40100000,0x40004000,0x40004100,0x00000000,
	0x40104100,0x00104000,0x00104000,0x00004100,
	0x00004100,0x00100100,0x40000000,0x40104000,
};

#endif



void unscrunch PROTO_LIST ((unsigned char *, UINT4 *));
void scrunch PROTO_LIST ((UINT4 *, unsigned char *));
void deskey PROTO_LIST ((UINT4 *, unsigned char *, int));
static void cookey PROTO_LIST ((UINT4 *, UINT4 *, int));
void desfunc PROTO_LIST ((UINT4 *, UINT4 *));

/* Initialize context.  Caller must zeroize the context when finished. */

void DES_CBCInit(
	DES_CBC_CTX *context,           /* context */
	unsigned char *key,             /* key */
	unsigned char *iv,              /* initializing vector */
	int encrypt)                    /* encrypt flag (1 = encrypt, 0 = decrypt) */
{
	/* Save encrypt flag to context. */
	context->encrypt = encrypt;

	/* Pack initializing vector into context. */

	scrunch(context->iv, iv);
	scrunch(context->originalIV, iv);

	/* Precompute key schedule */

	deskey(context->subkeys, key, encrypt);
}

/* DES-CBC block update operation. Continues a DES-CBC encryption
	 operation, processing eight-byte message blocks, and updating
	 the context.

	 This requires len to be a multiple of 8.
*/
int DES_CBCUpdate(
	DES_CBC_CTX *context,           /* context */
	unsigned char *output,          /* output block */
	unsigned char *input,           /* input block */
	unsigned int len)                                                               /* length of input and output blocks */
{
	UINT4 inputBlock[2], work[2];
	unsigned int i;

	if(len % 8)                                                                             /* block size check */
		return(RE_LEN);

	for(i = 0; i < len/8; i++) {
		scrunch(inputBlock, &input[8*i]);

		/* Chain if encrypting. */

		if(context->encrypt == 0) {
			*work = *inputBlock;
			*(work+1) = *(inputBlock+1);
		}else{
			*work = *inputBlock ^ *context->iv;
			*(work+1) = *(inputBlock+1) ^ *(context->iv+1);
		}

		desfunc(work, context->subkeys);

		/* Chain if decrypting, then update IV. */

		if(context->encrypt == 0) {
			*work ^= *context->iv;
			*(work+1) ^= *(context->iv+1);
			*context->iv = *inputBlock;
			*(context->iv+1) = *(inputBlock+1);
		}else{
			*context->iv = *work;
			*(context->iv+1) = *(work+1);
		}
		unscrunch (&output[8*i], work);
	}

	/* Clear sensitive information. */

	memset((POINTER)inputBlock, 0, sizeof(inputBlock));
	memset((POINTER)work, 0, sizeof(work));

	return(ID_OK);
}

void DES_CBCRestart(DES_CBC_CTX *context)                                                                         /* context */
{
	/* Restore the original IV */

	*context->iv = *context->originalIV;
	*(context->iv+1) = *(context->originalIV+1);
}

/* Initialize context.  Caller should clear the context when finished.
	 The key has the DES key, input whitener and output whitener concatenated.
	 This is the RSADSI special DES implementation.
*/
void DESX_CBCInit(
	DESX_CBC_CTX *context,          /* context */
	unsigned char *key,            /* DES key and whiteners */
	unsigned char *iv,              /* DES initializing vector */
	int encrypt)                    /* encrypt flag (1 = encrypt, 0 = decrypt) */
{
	/* Save encrypt flag to context. */

	context->encrypt = encrypt;

	/* Pack initializing vector and whiteners into context. */

	scrunch(context->iv, iv);
	scrunch(context->inputWhitener, key + 8);
	scrunch(context->outputWhitener, key + 16);
	/* Save the IV for use in Restart */
	scrunch(context->originalIV, iv);

	/* Precompute key schedule. */

	deskey (context->subkeys, key, encrypt);
}

/* DESX-CBC block update operation. Continues a DESX-CBC encryption
	 operation, processing eight-byte message blocks, and updating
	 the context.  This is the RSADSI special DES implementation.

	 Requires len to a multiple of 8.
*/

int DESX_CBCUpdate (
	DESX_CBC_CTX *context,          /* context */
	unsigned char *output,          /* output block */
	unsigned char *input,           /* input block */
	unsigned int len)               /* length of input and output blocks */
{
	UINT4 inputBlock[2], work[2];
	unsigned int i;

	if(len % 8)                                                                             /* Length check */
		return(RE_LEN);

	for(i = 0; i < len/8; i++)  {
		scrunch(inputBlock, &input[8*i]);

		/* Chain if encrypting, and xor with whitener. */

		if(context->encrypt == 0) {
			*work = *inputBlock ^ *context->outputWhitener;
			*(work+1) = *(inputBlock+1) ^ *(context->outputWhitener+1);
		}else{
			*work = *inputBlock ^ *context->iv ^ *context->inputWhitener;
			*(work+1) = *(inputBlock+1) ^ *(context->iv+1) ^ *(context->inputWhitener+1);
		}

		desfunc(work, context->subkeys);

		/* Xor with whitener, chain if decrypting, then update IV. */

		if(context->encrypt == 0) {
			*work ^= *context->iv ^ *context->inputWhitener;
			*(work+1) ^= *(context->iv+1) ^ *(context->inputWhitener+1);
			*(context->iv) = *inputBlock;
			*(context->iv+1) = *(inputBlock+1);
		}else{
			*work ^= *context->outputWhitener;
			*(work+1) ^= *(context->outputWhitener+1);
			*context->iv = *work;
			*(context->iv+1) = *(work+1);
		}
		unscrunch(&output[8*i], work);
	}

	R_memset((POINTER)inputBlock, 0, sizeof(inputBlock));
	R_memset((POINTER)work, 0, sizeof(work));

	return(ID_OK);
}

void DESX_CBCRestart(DESX_CBC_CTX *context)          /* context */
{
	/* Restore the original IV */
	*context->iv = *context->originalIV;
	*(context->iv+1) = *(context->originalIV+1);
}

/* Initialize context.  Caller must zeroize the context when finished. */

void DES3_CBCInit(
	DES3_CBC_CTX *context,          /* context */
	unsigned char *key,             /* key */
	unsigned char *iv,              /* initializing vector */
	int encrypt)                    /* encrypt flag (1 = encrypt, 0 = decrypt) */
{
	/* Copy encrypt flag to context. */
	context->encrypt = encrypt;

	/* Pack initializing vector into context. */

	scrunch(context->iv, iv);

	/* Save the IV for use in Restart */
	scrunch(context->originalIV, iv);

	/* Precompute key schedules. */

	deskey(context->subkeys[0], encrypt ? key : &key[16], encrypt);
	deskey(context->subkeys[1], &key[8], !encrypt);
	deskey(context->subkeys[2], encrypt ? &key[16] : key, encrypt);
}

int DES3_CBCUpdate(
	DES3_CBC_CTX *context,          /* context */
	unsigned char *output,          /* output block */
	unsigned char *input,           /* input block */
	unsigned int len)               /* length of input and output blocks */
{
	UINT4 inputBlock[2], work[2];
	unsigned int i;

	if(len % 8)                  /* length check */
		return(RE_LEN);

	for(i = 0; i < len/8; i++) {
		scrunch(inputBlock, &input[8*i]);

		/* Chain if encrypting. */

		if(context->encrypt == 0) {
			*work = *inputBlock;
			*(work+1) = *(inputBlock+1);
		}
		else {
			*work = *inputBlock ^ *context->iv;
			*(work+1) = *(inputBlock+1) ^ *(context->iv+1);
		}

		desfunc(work, context->subkeys[0]);
		desfunc(work, context->subkeys[1]);
		desfunc(work, context->subkeys[2]);

		/* Chain if decrypting, then update IV. */

		if(context->encrypt == 0) {
			*work ^= *context->iv;
			*(work+1) ^= *(context->iv+1);
			*context->iv = *inputBlock;
			*(context->iv+1) = *(inputBlock+1);
		}
		else {
			*context->iv = *work;
			*(context->iv+1) = *(work+1);
		}
		unscrunch(&output[8*i], work);
	}

	R_memset((POINTER)inputBlock, 0, sizeof(inputBlock));
	R_memset((POINTER)work, 0, sizeof(work));

	return (0);
}

void DES3_CBCRestart (DES3_CBC_CTX *context)          /* context */
{
	/* Restore the original IV */
	*context->iv = *context->originalIV;
	*(context->iv+1) = *(context->originalIV+1);
}

void scrunch (
	UINT4 *into,
	unsigned char *outof)
{
	*into    = (*outof++ & 0xffL) << 24;
	*into   |= (*outof++ & 0xffL) << 16;
	*into   |= (*outof++ & 0xffL) << 8;
	*into++ |= (*outof++ & 0xffL);
	*into    = (*outof++ & 0xffL) << 24;
	*into   |= (*outof++ & 0xffL) << 16;
	*into   |= (*outof++ & 0xffL) << 8;
	*into   |= (*outof   & 0xffL);
}

void unscrunch(
	unsigned char *into,
	UINT4 *outof)
{
	*into++ = (unsigned char)((*outof >> 24) & 0xffL);
	*into++ = (unsigned char)((*outof >> 16) & 0xffL);
	*into++ = (unsigned char)((*outof >>  8) & 0xffL);
	*into++ = (unsigned char)( *outof++      & 0xffL);
	*into++ = (unsigned char)((*outof >> 24) & 0xffL);
	*into++ = (unsigned char)((*outof >> 16) & 0xffL);
	*into++ = (unsigned char)((*outof >>  8) & 0xffL);
	*into   = (unsigned char)( *outof        & 0xffL);
}

/* Compute DES Subkeys */

void deskey(
	UINT4 subkeys[32],
	unsigned char key[8],
	int encrypt)
{
	UINT4 kn[32];
	int i, j, l, m, n;
	unsigned char pc1m[56], pcr[56];

	for(j = 0; j < 56; j++) {
		l = pc1[j];
		m = l & 07;
		pc1m[j] = (unsigned char)((key[l >> 3] & bytebit[m]) ? 1 : 0);
	}
	for(i = 0; i < 16; i++) {
		m = i << 1;
		n = m + 1;
		kn[m] = kn[n] = 0L;
		for(j = 0; j < 28; j++) {
			l = j + totrot[i];
			if(l < 28) pcr[j] = pc1m[l];
			else pcr[j] = pc1m[l - 28];
		}
		for(j = 28; j < 56; j++) {
			l = j + totrot[i];
			if(l < 56) pcr[j] = pc1m[l];
			else pcr[j] = pc1m[l - 28];
		}
		for(j = 0; j < 24; j++) {
			if(pcr[pc2[j]])
				kn[m] |= bigbyte[j];
			if(pcr[pc2[j+24]])
				kn[n] |= bigbyte[j];
		}
	}
	cookey(subkeys, kn, encrypt);

#ifdef DES386
	for(i=0;i < 32;i++)
		subkeys[i] <<= 2;
#endif

	R_memset((POINTER)pc1m, 0, sizeof(pc1m));
	R_memset((POINTER)pcr, 0, sizeof(pcr));
	R_memset((POINTER)kn, 0, sizeof(kn));
}

static void cookey(
	UINT4 *subkeys,
	UINT4 *kn,
	int encrypt)
{
	UINT4 *cooked, *raw0, *raw1;
	int increment;
	unsigned int i;

	raw1 = kn;
	cooked = encrypt ? subkeys : &subkeys[30];
	increment = encrypt ? 1 : -3;

	for (i = 0; i < 16; i++, raw1++) {
		raw0 = raw1++;
		*cooked    = (*raw0 & 0x00fc0000L) << 6;
		*cooked   |= (*raw0 & 0x00000fc0L) << 10;
		*cooked   |= (*raw1 & 0x00fc0000L) >> 10;
		*cooked++ |= (*raw1 & 0x00000fc0L) >> 6;
		*cooked    = (*raw0 & 0x0003f000L) << 12;
		*cooked   |= (*raw0 & 0x0000003fL) << 16;
		*cooked   |= (*raw1 & 0x0003f000L) >> 4;
		*cooked   |= (*raw1 & 0x0000003fL);
		cooked += increment;
	}
}

#ifndef DES386 /* ignore C version in favor of 386 ONLY desfunc */

#define	F(l,r,key){\
	work = ((r >> 4) | (r << 28)) ^ *key;\
	l ^= Spbox[6][work & 0x3f];\
	l ^= Spbox[4][(work >> 8) & 0x3f];\
	l ^= Spbox[2][(work >> 16) & 0x3f];\
	l ^= Spbox[0][(work >> 24) & 0x3f];\
	work = r ^ *(key+1);\
	l ^= Spbox[7][work & 0x3f];\
	l ^= Spbox[5][(work >> 8) & 0x3f];\
	l ^= Spbox[3][(work >> 16) & 0x3f];\
	l ^= Spbox[1][(work >> 24) & 0x3f];\
}

/* This desfunc code is marginally quicker than that uses in
	 RSAREF(tm)
*/

void desfunc(
UINT4 *block,		/* Data block */
UINT4 *ks)	/* Key schedule */
{
	unsigned long left,right,work;

	left = block[0];
	right = block[1];

	work = ((left >> 4) ^ right) & 0x0f0f0f0f;
	right ^= work;
	left ^= work << 4;
	work = ((left >> 16) ^ right) & 0xffff;
	right ^= work;
	left ^= work << 16;
	work = ((right >> 2) ^ left) & 0x33333333;
	left ^= work;
	right ^= (work << 2);
	work = ((right >> 8) ^ left) & 0xff00ff;
	left ^= work;
	right ^= (work << 8);
	right = (right << 1) | (right >> 31);
	work = (left ^ right) & 0xaaaaaaaa;
	left ^= work;
	right ^= work;
	left = (left << 1) | (left >> 31);

	/* Now do the 16 rounds */
	F(left,right,&ks[0]);
	F(right,left,&ks[2]);
	F(left,right,&ks[4]);
	F(right,left,&ks[6]);
	F(left,right,&ks[8]);
	F(right,left,&ks[10]);
	F(left,right,&ks[12]);
	F(right,left,&ks[14]);
	F(left,right,&ks[16]);
	F(right,left,&ks[18]);
	F(left,right,&ks[20]);
	F(right,left,&ks[22]);
	F(left,right,&ks[24]);
	F(right,left,&ks[26]);
	F(left,right,&ks[28]);
	F(right,left,&ks[30]);

	right = (right << 31) | (right >> 1);
	work = (left ^ right) & 0xaaaaaaaa;
	left ^= work;
	right ^= work;
	left = (left >> 1) | (left  << 31);
	work = ((left >> 8) ^ right) & 0xff00ff;
	right ^= work;
	left ^= work << 8;
	work = ((left >> 2) ^ right) & 0x33333333;
	right ^= work;
	left ^= work << 2;
	work = ((right >> 16) ^ left) & 0xffff;
	left ^= work;
	right ^= work << 16;
	work = ((right >> 4) ^ left) & 0x0f0f0f0f;
	left ^= work;
	right ^= work << 4;

	*block++ = right;
	*block = left;
}

#endif /* DES386 endif */
