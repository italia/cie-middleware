#ifndef _DIGESTINFO_H
#define _DIGESTINFO_H


#include "ASN1Sequence.h"
#include "AlgorithmIdentifier.h"
#include "ASN1Octetstring.h"


class CDigestInfo : public CASN1Sequence
{
	// Defined as 
	// DigestInfo ::= SEQUENCE {
	// 		digestAlgorithm DigestAlgorithmIdentifier,
	//		digest Digest}
	
public:
	CDigestInfo(UUCBufferedReader& reader);
	
	CDigestInfo(const CAlgorithmIdentifier& algoId, const CASN1OctetString& digest);

	CDigestInfo(const CASN1Object& digestInfo);
	
	CAlgorithmIdentifier getDigestAlgorithm();
	
	CASN1OctetString getDigest();
	
};

#endif //_DIGESTINFO_H