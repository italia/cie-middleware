// RelativeDistinguishedName.h: interface for the CRelativeDistinguishedName class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RELATIVEDISTINGUISHEDNAME_H__6344EC52_BE52_4C57_AA1D_3D2790EDDD7E__INCLUDED_)
#define AFX_RELATIVEDISTINGUISHEDNAME_H__6344EC52_BE52_4C57_AA1D_3D2790EDDD7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ASN1Setof.h"
//#include "AttributeValueAssertion.h"

class CRelativeDistinguishedName : public CASN1SetOf  
{
public:

	CRelativeDistinguishedName(UUCBufferedReader& reader);	

	CRelativeDistinguishedName();	
	
	CRelativeDistinguishedName(const CASN1Object& rname);	
	
	//void addAttributeValue(const CAttributeValueAssertion& algos);

	virtual ~CRelativeDistinguishedName();

};

#endif // !defined(AFX_RELATIVEDISTINGUISHEDNAME_H__6344EC52_BE52_4C57_AA1D_3D2790EDDD7E__INCLUDED_)
