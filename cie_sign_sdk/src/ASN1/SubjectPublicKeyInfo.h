// SubjectPublicKeyInfo.h: interface for the CSubjectPublicKeyInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBJECTPUBLICKEYINFO_H__93E2619B_704A_49A2_8DEA_DCCC521605CE__INCLUDED_)
#define AFX_SUBJECTPUBLICKEYINFO_H__93E2619B_704A_49A2_8DEA_DCCC521605CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ASN1Sequence.h"
#include "AlgorithmIdentifier.h"
#include "ASN1BitString.h"

class CSubjectPublicKeyInfo : public CASN1Sequence  
{
public:
	CSubjectPublicKeyInfo(UUCBufferedReader& reader);
	
	CSubjectPublicKeyInfo(const CASN1Object& obj);
	
	virtual ~CSubjectPublicKeyInfo();
	
	CAlgorithmIdentifier getAlgorithmIdentifier();
	
	CASN1BitString getPublicKey();
	
};

#endif // !defined(AFX_SUBJECTPUBLICKEYINFO_H__93E2619B_704A_49A2_8DEA_DCCC521605CE__INCLUDED_)
