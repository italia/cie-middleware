

#include "DigestInfo.h"
	
CDigestInfo::CDigestInfo(UUCBufferedReader& reader)
:CASN1Sequence(reader)
{  
									
}

CDigestInfo::CDigestInfo(const CAlgorithmIdentifier& algoId, const CASN1OctetString& digest)
{		
	addElement(algoId);
	addElement(digest);
}	

CDigestInfo::CDigestInfo(const CASN1Object& digestInfo)
: CASN1Sequence(digestInfo)
{

}

CAlgorithmIdentifier CDigestInfo::getDigestAlgorithm()
{
	return elementAt(0);
}

CASN1OctetString CDigestInfo::getDigest()
{
	return elementAt(1);
}


