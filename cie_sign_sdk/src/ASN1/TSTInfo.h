// TSTInfo.h: interface for the CTSTInfo class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _TSTINFO_H
#define _TSTINFO_H

#if !defined(AFX_TSTINFO_H__7B1086C5_9AEE_4973_8D52_FBB757D01E2A__INCLUDED_)
#define AFX_TSTINFO_H__7B1086C5_9AEE_4973_8D52_FBB757D01E2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ASN1Sequence.h"
#include "ASN1Integer.h"
#include "ASN1UTCTime.h"
#include "Name.h"
#include "AlgorithmIdentifier.h"

class CTSTInfo : public CASN1Sequence  
{
public:

	CTSTInfo(UUCBufferedReader& reader);

	CTSTInfo(const CASN1Object& tstInfo);

	virtual ~CTSTInfo();

	CASN1UTCTime getUTCTime();

	CASN1Integer getSerialNumber();
	
	CAlgorithmIdentifier getDigestAlgorithn();

	CASN1Sequence getMessageImprint();

	// N.B. il campo TSAName è opzionale. se non presente nel tstoken torna eccezione
	CName getTSAName();
	
};

#endif // !defined(AFX_TSTINFO_H__7B1086C5_9AEE_4973_8D52_FBB757D01E2A__INCLUDED_)
#endif //_TSTINFO_H