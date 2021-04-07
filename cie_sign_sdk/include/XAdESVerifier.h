#pragma once

#include "ASN1/UUCByteArray.h"
#include "Certificate.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>


typedef struct RefDataFile_st {
  char* szId;			// data file id
  char* szFileName;	// signed doc file name
  char* szMimeType;	// date file mime type
  char* szContentType;	// DETATCHED, EMBEDDED or EMBEDDED_BASE64
  long  nSize;		// file size (unencoded)
  char* szDigestType;	// digest type
  UUCByteArray mbufDigest;  // real DataFile digest value
  UUCByteArray mbufDetachedDigest; // detached file digest
  int nAttributes;		// number of other attributes
  char* szCharset;		// datafile initial codepage
  char** pAttNames;		// other attribute names
  char** pAttValues;		// other attribute values
  UUCByteArray mbufContent;
} RefDataFile;

// signature info for each user signature
typedef struct SignatureInfo_st {
  char* szId;			// signature id
  int nDocs;			// number of separate documents signed
//  DocInfo** pDocs;	// info for each signed document	
  char* szTimeStamp;	// signature timestamp in format "YYYY-MM-DDTHH:MM:SSZ"
  UUCByteArray sigPropDigest;
  UUCByteArray sigPropRealDigest;
  UUCByteArray sigInfoRealDigest;
  UUCByteArray sigValue;    // RSA+SHA1 signature value
  CCertificate* pX509Cert;	// X509Cert certificate (used internally for data during loading)
  UUCByteArray mbufOrigContent;
  int nDigestAlgo;
  bool bCAdES;
} SignatureInfo;

typedef struct XAdESDoc_st {
	char* szFormat;		// data format name
	char* szFormatVer;	// data format version
	int nDataFiles;
	RefDataFile** pRefDataFiles;
	int nSignatures;
	SignatureInfo** ppSignatures;
} XAdESDoc;


class CXAdESVerifier
{
public:
	CXAdESVerifier(void);
	virtual ~CXAdESVerifier(void);

	long Load(BYTE* buf, int len);
	long Load(char *szFilename);
	
	CCertificate* GetCertificate(int index);
	CASN1ObjectIdentifier GetDigestAlgorithm(int index);

	int verifySignature(int index, const char* szDateTime, REVOCATION_INFO* pRevocationInfo);

private:
	static XAdESDoc* parseXAdESFile(char *szFilename);
	static void parseSignatureNode(xmlXPathContextPtr xpathCtx, xmlNodeSetPtr signatureNodes, XAdESDoc* pXAdESDoc);
	static bool m_bLibXmlInitialized;
	XAdESDoc* m_pXAdESDoc;
};
