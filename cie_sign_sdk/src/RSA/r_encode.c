/*
	R_ENCODE.C - RFC 1113 encoding and decoding routines

    Copyright (c) J.S.A.Kapp 1994 - 1996.

	RSAEURO - RSA Library compatible with RSAREF(tm) 2.0.

	All functions prototypes are the Same as for RSAREF(tm).
	To aid compatiblity the source and the files follow the
	same naming comventions that RSAREF(tm) uses.  This should aid
	direct importing to your applications.

	This library is legal everywhere outside the US.  And should
	NOT be imported to the US and used there.

	All Trademarks Acknowledged.

	Based in a similar form to the PGP RFC 1113 coding routines.

	Revision history
		0.90 First revision, based purely on using macros to
		en/decode data to RFC 1113 standard.

		0.91 Initial revision using current system.

        0.92 Third revision, fixed minor bugs in rev. 0.91
		and better comments updated, plus code layout.

        1.02 Fourth revision, fixed bug in R_EncodePEMBlock
        reported by Wang Wei Jun <wangw@iti.gov.sg>.
*/

#include "rsaeuro.h"

/* RFC 1113 encoding:

	 Value Encoding  Value Encoding  Value Encoding  Value Encoding
			 0 A            17 R            34 i            51 z
			 1 B            18 S            35 j            52 0
			 2 C            19 T            36 k            53 1
			 3 D            20 U            37 l            54 2
			 4 E            21 V            38 m            55 3
			 5 F            22 W            39 n            56 4
			 6 G            23 X            40 o            57 5
			 7 H            24 Y            41 p            58 6
			 8 I            25 Z            42 q            59 7
			 9 J            26 a            43 r            60 8
			10 K            27 b            44 s            61 9
			11 L            28 c            45 t            62 +
			12 M            29 d            46 u            63 /
			13 N            30 e            47 v
			14 O            31 f            48 w         (pad) =
			15 P            32 g            49 x
			16 Q            33 h            50 y
*/

/*
	PEM routines.
	This converts a binary file into printable ASCII characters, in a
	radix-64 form mostly compatible with the PEM RFC 1113 format.
	This makes it easier to send encrypted files over a 7-bit channel.
*/

/* ENC is the basic 1 character encoding function to make a char printing */

#define ENC(c) ((int)bintoasc[((c) & 077)])
#define PAD             '='

/* Index this array by a 6 bit value to get the character corresponding
	 to that value. */

static unsigned char bintoasc[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\
abcdefghijklmnopqrstuvwxyz0123456789+/";

/* Index this array by a 7 bit value to get the 6-bit binary field
	 corresponding to that value.  Any illegal characters return high bit set. */

static unsigned char asctobin[] = {
	0200,0200,0200,0200,0200,0200,0200,0200,
	0200,0200,0200,0200,0200,0200,0200,0200,
	0200,0200,0200,0200,0200,0200,0200,0200,
	0200,0200,0200,0200,0200,0200,0200,0200,
	0200,0200,0200,0200,0200,0200,0200,0200,
	0200,0200,0200,0076,0200,0200,0200,0077,
	0064,0065,0066,0067,0070,0071,0072,0073,
	0074,0075,0200,0200,0200,0200,0200,0200,
	0200,0000,0001,0002,0003,0004,0005,0006,
	0007,0010,0011,0012,0013,0014,0015,0016,
	0017,0020,0021,0022,0023,0024,0025,0026,
	0027,0030,0031,0200,0200,0200,0200,0200,
	0200,0032,0033,0034,0035,0036,0037,0040,
	0041,0042,0043,0044,0045,0046,0047,0050,
	0051,0052,0053,0054,0055,0056,0057,0060,
	0061,0062,0063,0200,0200,0200,0200,0200
};

static void encode PROTO_LIST((unsigned char [4], unsigned char [3], int ));

/* This will always returns 0.  It is an int function for future compatibility,
	 done to provide compatibility with RSAREF(tm). */

int R_EncodePEMBlock(encodedBlock, encodedBlockLen, block, blockLen)
unsigned char *encodedBlock;    /* encoded block */
unsigned int *encodedBlockLen;  /* length of encoded block */
unsigned char *block;           /* block */
unsigned int blockLen;          /* length of block */
{
	unsigned int i, lastLen;

	if(blockLen < 1) {
		*encodedBlockLen = 0;
		return (ID_OK);
	}

	*encodedBlockLen = 0;

        /* Bug Fixed By SK 02/09/95 */
    for (i = 0, lastLen = blockLen; i < blockLen; i += 3) {
        encode(&encodedBlock[4*i/3], &block[i], (lastLen >= 3 ? 3 : lastLen));
        lastLen -= 3;
		*encodedBlockLen += 4;
	}

	return(ID_OK);
}

int R_DecodePEMBlock (outbuf, outlength, inbuf, inlength)
unsigned char *outbuf;          /* block */
unsigned int *outlength;        /* length of block */
unsigned char *inbuf;           /* encoded block */
unsigned int inlength;          /* length of encoded block */
{
	unsigned char *bp;
	int     length;
	unsigned int c1,c2,c3,c4;
	register int j;

	if(inlength % 4)
		return (RE_ENCODING);

	if(inlength < 1) {
		*outlength = 0;
		return (ID_OK);
	}


	length = 0;
	bp = (unsigned char *)inbuf;

	/* FOUR input characters go into each THREE output charcters */

	while(inlength > 0) {
		if(*bp&0x80 || (c1=asctobin[*bp])&0x80)
			return(RE_ENCODING);
		++bp;
		if(*bp&0x80 || (c2=asctobin[*bp])&0x80)
			return(RE_ENCODING);
		if(*++bp == PAD){
			c3 = c4 = 0;
			length += 1;
			if(*++bp != PAD)
				return(RE_ENCODING);
		}
		else if(*bp&0x80 || (c3=asctobin[*bp])&0x80)
				return(RE_ENCODING);
		else {
			if(*++bp == PAD) {
				c4 = 0;
				length += 2;
			}
			else if (*bp&0x80 || (c4=asctobin[*bp])&0x80)
				return(RE_ENCODING);
			else
				length += 3;
		}
		++bp;
		j = (c1 << 2) | (c2 >> 4);
		*outbuf++=j;
		j = (c2 << 4) | (c3 >> 2);
		*outbuf++=j;
		j = (c3 << 6) | c4;
		*outbuf++=j;
		inlength -= 4;
	}

	*outlength = length;
	return(ID_OK);   /* normal return */

}

/*
	Output one group of up to 3 bytes, pointed at by p, to buffer at f.
	If fewer than 3 are present, the 1 or two extras must be zeros.
*/

static void encode(f, p, count)
unsigned char *f;               /* output buffer */
unsigned char *p;               /* input buffer */
int count;                      /* count */
{
	int c1, c2, c3, c4;

	c1 = *p >> 2;
	c2 = ((*p << 4) & 060) | ((p[1] >> 4) & 017);
	c3 = ((p[1] << 2) & 074) | ((p[2] >> 6) & 03);
	c4 = p[2] & 077;
	*f++ = ENC(c1);
	*f++ = ENC(c2);
	if (count == 1)
	{
		*f++ = PAD;
		*f++ = PAD;
	}else{
		*f++ = ENC(c3);
		if(count == 2) *f++ = PAD;
		else *f++ = ENC(c4);
	}
}

