/*
 *  PdfVerifier.h
 *  SignPoDoFo
 *
 *  Created by svp on 26/05/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PDFVERIFIER_H_
#define _PDFVERIFIER_H_


#include "podofo/podofo.h"
#include "podofo/doc/PdfSignOutputDevice.h"
#include "podofo/doc/PdfSignatureField.h"
#include "ASN1/UUCByteArray.h"
#include "disigonsdk.h"

using namespace PoDoFo;
using namespace std;

typedef struct _SignatureAppearanceInfo
{
	int left;
	int bottom;
	int width;
	int heigth;
} SignatureAppearanceInfo;

class PDFVerifier 
{
public:
	PDFVerifier();
	
	virtual ~PDFVerifier();
	
	int Load(const char* pdf, int len);
    int Load(const char* szFilePath);
	
	int GetNumberOfSignatures();
	
	int VerifySignature(int index, const char* szDate, char* signatureType, REVOCATION_INFO* pRevocationInfo);
	
	
	int GetSignature(int index, UUCByteArray& signedDocument, SignatureAppearanceInfo& appearanceInfo);
	
	static int GetNumberOfSignatures(PdfMemDocument* pPdfDocument);
    static int GetNumberOfSignatures(const char* szFilePath);
	
private:
	UUCByteArray m_data;
	static bool IsSignatureField(const PdfMemDocument* pDoc, const PdfObject *const pObj);
	
	int VerifySignature(const PdfMemDocument* pDoc, const PdfObject *const pObj, const char* szDate, char* signatureType, REVOCATION_INFO* pRevocationInfo);

	int GetSignature(const PdfMemDocument* pDoc, const PdfObject *const pObj, UUCByteArray& signedDocument, SignatureAppearanceInfo& appearanceInfo);

	PdfMemDocument* m_pPdfDocument;
	
	int m_actualLen;
	
	char* m_szDocBuffer;
	
};

#endif //_PDFVERIFIER_H




