/*
 *  PdfSignatureGenerator.cpp
 *  SignPoDoFo
 *
 *  Created by svp on 26/05/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "PdfSignatureGenerator.h"
#include "PdfVerifier.h"
#include "UUCLogger.h"

#define SINGNATURE_SIZE 10000

#ifdef CreateFont
#undef CreateFont
#endif

#ifdef GetObject
#undef GetObject
#endif

int GetNumberOfSignatures(PdfMemDocument* pPdfDocument);

USE_LOG;

PdfSignatureGenerator::PdfSignatureGenerator()
: m_pPdfDocument(NULL), m_pSignatureField(NULL), m_pSignOutputDevice(NULL), m_pFinalOutDevice(NULL),
m_pMainDocbuffer(NULL), m_pSignDocbuffer(NULL)
{
	PoDoFo::PdfError::EnableLogging(false);
}

PdfSignatureGenerator::~PdfSignatureGenerator()
{
	if(m_pPdfDocument)
		delete m_pPdfDocument;
	
	if(m_pSignatureField)
		delete m_pSignatureField;
	
	if(m_pSignOutputDevice)
		delete m_pSignOutputDevice;
	
	if(m_pFinalOutDevice)
		delete m_pFinalOutDevice;
	
	if(m_pMainDocbuffer)
		delete m_pMainDocbuffer;
	
	if(m_pSignDocbuffer)
		delete m_pSignDocbuffer;
	
}

int PdfSignatureGenerator::Load(const char* pdf, int len)
{
	if(m_pPdfDocument)
		delete m_pPdfDocument;
	
	try
	{
        printf("PDF");
        //printf("%s", (char *)pdf);
        printf("LENGTH");
        printf("%i", len);
        printf("STOP");

		m_pPdfDocument = new PdfMemDocument();
		m_pPdfDocument->Load(pdf, len);
		printf("OK m_pPdfDocument");
		int nSigns = PDFVerifier::GetNumberOfSignatures(m_pPdfDocument);
		printf("OK nSigns: %d", nSigns);

		if(nSigns > 0)
		{
			/* Stock PoDoFo 0.9.8: no SetIncrementalUpdates */
		}
		m_actualLen = len;
		
		return nSigns;
	}
    catch(::PoDoFo::PdfError& err)
    {
        return -2;
    }
	catch (...)
	{
		return -1;
	}
}

void PdfSignatureGenerator::AddFont(const char* szFontName, const char* szFontPath)
{
	(void)szFontPath;
	try {
		m_pPdfDocument->CreateFont(szFontName, false, false, false,
			PdfEncodingFactory::GlobalWinAnsiEncodingInstance(),
			PdfFontCache::eFontCreationFlags_AutoSelectBase14, true);
		m_pPdfDocument->CreateFont(szFontName, true, false, false,
			PdfEncodingFactory::GlobalWinAnsiEncodingInstance(),
			PdfFontCache::eFontCreationFlags_AutoSelectBase14, true);
	} catch (...) {
		/* font optional for invisible PAdES */
	}
}

void PdfSignatureGenerator::InitSignature(int pageIndex, const char* szReason, const char* szReasonLabel, const char* szName, const char* szNameLabel, const char* szLocation, const char* szLocationLabel, const char* szFieldName, const char* szSubFilter)
{
	LOG_DBG((0, "quella con tutti 0\n", ""));
	InitSignature(pageIndex, 0, 0, 0, 0, szReason, szReasonLabel, szName, szNameLabel, szLocation, szLocationLabel, szFieldName, szSubFilter);
}

void PdfSignatureGenerator::InitSignature(int pageIndex, float left, float bottom, float width, float height, const char* szReason, const char* szReasonLabel, const char* szName, const char* szNameLabel, const char* szLocation, const char* szLocationLabel, const char* szFieldName, const char* szSubFilter)
{
	LOG_DBG((0,"quella senza tutti 0\n", ""));
	InitSignature(pageIndex, left, bottom, width,  height, szReason, szReasonLabel, szName, szNameLabel, szLocation, szLocationLabel, szFieldName, szSubFilter, NULL, NULL, NULL, NULL);
}

void PdfSignatureGenerator::InitSignature(int pageIndex, float left, float bottom, float width, float height, const char* szReason, const char* szReasonLabel, const char* szName, const char* szNameLabel, const char* szLocation, const char* szLocationLabel, const char* szFieldName, const char* szSubFilter, const char* szImagePath, const char* szDescription, const char* szGraphometricData, const char* szVersion)
{
	LOG_DBG((0, "--> InitSignature", "%d, %f, %f, %f, %f, %s, %s, %s, %s, %s, %s, %s, %s", pageIndex, left, bottom, width, height, szReason, szName, szLocation, szFieldName, szSubFilter, szImagePath, szGraphometricData, szVersion));
    //LOG_DBG((0, "--> InitSignature", ""));
    
             
	if(m_pSignatureField)
		delete m_pSignatureField;

	PdfPage* pPage = m_pPdfDocument->GetPage(pageIndex);
    PdfRect cropBox = pPage->GetCropBox();
    
    float left0 = left * cropBox.GetWidth();
    float bottom0 = cropBox.GetHeight() - (bottom * cropBox.GetHeight());
    
    float width0 = width * cropBox.GetWidth();
    float height0 = height * cropBox.GetHeight();
    
    printf("pdf rect: %f, %f, %f, %f\n", left0, bottom0, width0, height0);
    
	PdfRect rect(left0, bottom0, width0, height0);
	
	LOG_DBG((0, "InitSignature", "PdfSignatureField"));

	/* Stock PoDoFo 0.9.8 API (no IPZS-patched SetAppearance/SetSignatureSize on field) */
	(void)szReasonLabel;
	(void)szNameLabel;
	(void)szLocationLabel;
	(void)szSubFilter;
	(void)szImagePath;
	(void)szDescription;
	(void)szGraphometricData;
	(void)szVersion;

	m_pSignatureField = new PdfSignatureField(pPage, rect, m_pPdfDocument);
	if (szFieldName && szFieldName[0])
		m_pSignatureField->SetFieldName(szFieldName);

	LOG_DBG((0, "InitSignature", "PdfSignatureField OK"));

	if(szReason && szReason[0])
	{
		PdfString reason(szReason);
		m_pSignatureField->SetSignatureReason(reason);
	}

	LOG_DBG((0, "InitSignature", "szReason OK"));

	if(szLocation && szLocation[0])
	{
		PdfString location(szLocation);
		m_pSignatureField->SetSignatureLocation(location);
	}

	LOG_DBG((0, "InitSignature", "szLocation OK"));

	PdfDate now;
	m_pSignatureField->SetSignatureDate(now);

	LOG_DBG((0, "InitSignature", "Date OK"));

	if(szName && szName[0])
	{
		/* Creator used as signer display name on stock PoDoFo */
		m_pSignatureField->SetSignatureCreator(PdfName(szName));
	}

	LOG_DBG((0, "InitSignature", "szName OK"));

	LOG_DBG((0, "InitSignature", "m_actualLen %d", m_actualLen));
	int fulllen = m_actualLen * 2 + SINGNATURE_SIZE * 2 + 1024;

	/* Prepare signature beacon before writing the PDF (stock PoDoFo). */
	m_pSignDocbuffer = new char[fulllen];
	m_pFinalOutDevice = new PdfOutputDevice(m_pSignDocbuffer, fulllen);
	m_pSignOutputDevice = new PdfSignOutputDevice(m_pFinalOutDevice);
	m_pSignOutputDevice->SetSignatureSize(SINGNATURE_SIZE);
	m_pSignatureField->EnsureSignatureObject();
	m_pSignatureField->SetSignature(*m_pSignOutputDevice->GetSignatureBeacon());

	int mainDoclen = 0;
	m_pMainDocbuffer = NULL;
	while (!m_pMainDocbuffer) {
		try{
			LOG_DBG((0, "InitSignature", "fulllen %d", fulllen));
			m_pMainDocbuffer = new char[fulllen];
			PdfOutputDevice pdfOutDevice(m_pMainDocbuffer, fulllen);
			m_pPdfDocument->Write(&pdfOutDevice);
			mainDoclen = pdfOutDevice.GetLength();
		}
		catch (::PoDoFo::PdfError err) {
			if(m_pMainDocbuffer) {
				delete m_pMainDocbuffer;
				m_pMainDocbuffer = NULL;
			}
			LOG_DBG((0, "PdfError", "what %s", err.what()));
			fulllen *= 2;
			delete m_pSignOutputDevice;
			delete m_pFinalOutDevice;
			delete [] m_pSignDocbuffer;
			m_pSignDocbuffer = new char[fulllen];
			m_pFinalOutDevice = new PdfOutputDevice(m_pSignDocbuffer, fulllen);
			m_pSignOutputDevice = new PdfSignOutputDevice(m_pFinalOutDevice);
			m_pSignOutputDevice->SetSignatureSize(SINGNATURE_SIZE);
			m_pSignatureField->SetSignature(*m_pSignOutputDevice->GetSignatureBeacon());
		}
	}

	LOG_DBG((0, "InitSignature", "buffers OK %d", fulllen));
	m_pSignOutputDevice->Write(m_pMainDocbuffer, mainDoclen);
	LOG_DBG((0, "InitSignature", "Write OK %d", mainDoclen));
	m_pSignOutputDevice->AdjustByteRange();
	LOG_DBG((0, "InitSignature", "AdjustByteRange OK"));
}

void PdfSignatureGenerator::GetBufferForSignature(UUCByteArray& toSign)
{
	//int fulllen = m_actualLen * 2 + SINGNATURE_SIZE * 2;
	int len = m_pSignOutputDevice->GetLength() * 2;
	
	char* buffer = new char[len];
	
	m_pSignOutputDevice->Seek(0);
	
	int nRead = m_pSignOutputDevice->ReadForSignature(buffer, len);
	if(nRead == -1)
		throw nRead;
	
	toSign.append((BYTE*)buffer, nRead);
	
	delete buffer;
}

void PdfSignatureGenerator::SetSignature(const char* signature, int len)
{
	PdfData signatureData(signature, len);
	m_pSignOutputDevice->SetSignature(signatureData);
}

void PdfSignatureGenerator::GetSignedPdf(UUCByteArray& signedPdf)
{
	int finalLength = m_pSignOutputDevice->GetLength();
	char* szSignedPdf = new char[finalLength];
	
	m_pSignOutputDevice->Seek(0);
	int nRead = m_pSignOutputDevice->Read(szSignedPdf, finalLength);
	
	signedPdf.append((BYTE*)szSignedPdf, nRead);
	
	delete szSignedPdf;
}

const double PdfSignatureGenerator::getWidth(int pageIndex) {
	if (m_pPdfDocument) {
		PdfPage* pPage = m_pPdfDocument->GetPage(pageIndex);
		return pPage->GetPageSize().GetWidth();
	}
	return 0;
}

const double PdfSignatureGenerator::getHeight(int pageIndex) {
	if (m_pPdfDocument) {
		PdfPage* pPage = m_pPdfDocument->GetPage(pageIndex);
		return pPage->GetPageSize().GetHeight();
	}
	return 0;
}

const double PdfSignatureGenerator::lastSignatureY(int left, int bottom) {
	if(!m_pPdfDocument)
		return -1;
	/// Find the document catalog dictionary
	const PdfObject *const trailer = m_pPdfDocument->GetTrailer();
	if (! trailer->IsDictionary())
		return -1;
	const PdfObject *const catalogRef =	trailer->GetDictionary().GetKey(PdfName("Root"));
	if (catalogRef==0 || ! catalogRef->IsReference())
		return -2;//throw std::invalid_argument("Invalid /Root entry");
	const PdfObject *const catalog =
		m_pPdfDocument->GetObjects().GetObject(catalogRef->GetReference());
	if (catalog==0 || !catalog->IsDictionary())
		return -3;//throw std::invalid_argument("Invalid or non-dictionary
	//referenced by /Root entry");
	
	/// Find the Fields array in catalog dictionary
	const PdfObject *acroFormValue = catalog->GetDictionary().GetKey(PdfName("AcroForm"));
	if (acroFormValue == 0)
		return bottom;
	if (acroFormValue->IsReference())
		acroFormValue = m_pPdfDocument->GetObjects().GetObject(acroFormValue->GetReference());
	
	if (!acroFormValue->IsDictionary())
		return bottom;
	
	const PdfObject *fieldsValue = acroFormValue->GetDictionary().GetKey(PdfName("Fields"));
	if (fieldsValue == 0)
		return bottom;
	
	if (fieldsValue->IsReference())
		fieldsValue = m_pPdfDocument->GetObjects().GetObject(acroFormValue->GetReference());
	
	if (!fieldsValue->IsArray())
		return bottom;
	
	vector<const PdfObject*> signatureVector;
	
	/// Verify if each object of the array is a signature field
	const PdfArray &array = fieldsValue->GetArray();
	
	int minY = bottom;
	
	for (unsigned int i=0; i<array.size(); i++) {
		const PdfObject * pObj = m_pPdfDocument->GetObjects().GetObject(array[i].GetReference());
		if (IsSignatureField(m_pPdfDocument, pObj)) {
			const PdfObject *const keyRect = pObj->GetDictionary().GetKey(PdfName("Rect"));
			if (keyRect == 0) {
				return bottom;
			}
			PdfArray rectArray = keyRect->GetArray();
			PdfRect rect;
			rect.FromArray(rectArray);
			
			if (rect.GetLeft() == left) {
				minY = (rect.GetBottom() <= minY && rect.GetBottom()!=0) ? rect.GetBottom()-85 : minY;
			}
		}
	}
	return minY;
}

bool PdfSignatureGenerator::IsSignatureField(const PdfMemDocument* pDoc, const PdfObject *const pObj)
{
	if (pObj == 0) return false;
	
	if (!pObj->IsDictionary())
		return false;
	
	const PdfObject *const keyFTValue = pObj->GetDictionary().GetKey(PdfName("FT"));
	if (keyFTValue == 0)
		return false;
	
	string value;
	keyFTValue->ToString(value);
	if (value != "/Sig")
		return false;
	
	const PdfObject *const keyVValue = pObj->GetDictionary().GetKey(PdfName("V"));
	if (keyVValue == 0)
		return false;
	
	const PdfObject *const signature = pDoc->GetObjects().GetObject(keyVValue->GetReference());
	if (signature->IsDictionary())
		return true;
	else
		return false;
}
