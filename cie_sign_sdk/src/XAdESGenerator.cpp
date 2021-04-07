
#include "XAdESGenerator.h"


#include "Base64.h"
#include "base64-std.h"
#include "RSA/sha1.h"
#include "RSA/sha2.h"
#include "ASN1/UUCByteArray.h"
#include <time.h>
#include "BigIntegerLibrary.h"
#include "ASN1/DigestInfo.h"
#include <map>

#define DIGEST_METHOD_SHA1      "http://www.w3.org/2000/09/xmldsig#sha1"
#define DIGEST_METHOD_RSASHA256    "http://www.w3.org/2001/04/xmldsig-more#rsa-sha256"
#define DIGEST_METHOD_SHA256    "http://www.w3.org/2001/04/xmlenc#sha256"
#define NAMESPACE_XML_DSIG      "http://www.w3.org/2000/09/xmldsig#"
#define NAMESPACE_XADES_111     "http://uri.etsi.org/01903/v1.1.1#"
#define NAMESPACE_XADES_132     "http://uri.etsi.org/01903/v1.3.2#"
#define NAMESPACE_XADES_1410    "http://uri.etsi.org/01903/v1.4.1"
#define NAMESPACE_XADES         "http://uri.etsi.org/01903#"


#define REFTYPE_CONTENT 1
#define REFTYPE_URI 2

typedef unsigned long uint32;
char* Bin128ToDec(const uint32 N[5]);

void printBigInt(const BYTE* buffer, int buflen, string& sDecimalValue);

CXAdESGenerator::CXAdESGenerator(CBaseSigner* pCryptoki)
: CSignatureGeneratorBase(pCryptoki), m_bXAdES(false)
{

}

CXAdESGenerator::CXAdESGenerator(CSignatureGeneratorBase* pGenerator)
: CSignatureGeneratorBase(pGenerator), m_bXAdES(false)
{
	
}

CXAdESGenerator::~CXAdESGenerator(void)
{

}

/*
void CXAdESGenerator::AddContent(UUCByteArray& content)
{
	Reference* pReference = new Reference;
	pReference->nType = REFTYPE_CONTENT;
	pReference->reference.content = content;

	m_refecenceVect.push_back(pReference);
}

void CXAdESGenerator::AddReference(char* szURI)
{
	Reference* pReference = new Reference;
	pReference->nType = REFTYPE_URI;
	pReference->reference.szURI = szURI;

	m_refecenceVect.push_back(pReference);
}

*/
void CXAdESGenerator::SetXAdES(bool xades)
{
	m_bXAdES = xades;
}

void CXAdESGenerator::SetFileName(char* szFileName)
{
	strcpy(m_szFileName, szFileName);
}

static void extractNs(xmlDocPtr pDoc, xmlNode * a_node, map<const xmlChar*, xmlNsPtr*>* pNsPtrMap)
{
	xmlNode *cur_node = NULL;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {

			xmlNsPtr* nsPtr = xmlGetNsList(pDoc, cur_node);
			if (nsPtr)
			{
				pNsPtrMap->insert(pair<const xmlChar*, xmlNsPtr*>(nsPtr[0]->href, nsPtr));
				//string key((char*)nsPtr[0]->href);
				//pNsPtrMap[nsPtr[0]->href] = nsPtr;
			}
		}

		extractNs(pDoc, cur_node->children, pNsPtrMap);
	}
}

long CXAdESGenerator::Generate(UUCByteArray& xadesData, BOOL bDetached, BOOL bVerifyCertificate)
{
	// get the certificate based on alias
	UUCByteArray id;
	CCertificate* pSignerCertificate;
	m_pSigner->GetCertificate(m_szAlias, &pSignerCertificate, id);
	
	if (bVerifyCertificate)
	{
		if (!pSignerCertificate->isValid())
		{
			SAFEDELETE(pSignerCertificate);
			m_pSigner->Close();
			return DISIGON_ERROR_CERT_EXPIRED;
		}

		/*if (!(pSignerCertificate->isNonRepudiation()))
		{
		SAFEDELETE(pSignerCertificate);
		return DISIGON_ERROR_CERT_NOT_FOR_SIGNATURE;
		}*/

		int bitmask = pSignerCertificate->verify();

		if ((bitmask & VERIFIED_CACERT_FOUND) == 0)
		{
			SAFEDELETE(pSignerCertificate);
			m_pSigner->Close();
			return DISIGON_ERROR_CACERT_NOTFOUND;
		}

		if ((bitmask & VERIFIED_CERT_CHAIN) == 0)
		{
			SAFEDELETE(pSignerCertificate);
			m_pSigner->Close();
			return DISIGON_ERROR_CERT_INVALID;
		}

		if (pSignerCertificate->verifyStatus(NULL) != REVOCATION_STATUS_GOOD)
		{
			SAFEDELETE(pSignerCertificate);
			m_pSigner->Close();
			return DISIGON_ERROR_CERT_REVOKED;
		}		
	}

	xmlDocPtr pDoc;
	xmlNodePtr root;	
    /*
     * build an XML tree from the file;
     */
	pDoc = xmlParseMemory((char*)m_data.getContent(), m_data.getLength());
    if (pDoc == NULL) 
		return DISIGON_ERROR_INVALID_FILE;

    /*
     * Check the document is of the right kind
     */
    root = xmlDocGetRootElement(pDoc);
    if (root == NULL) 
	{
        //fprintf(stderr,"empty document\n");
		xmlFreeDoc(pDoc);
		return DISIGON_ERROR_INVALID_FILE;
    }

	map<const xmlChar*, xmlNsPtr*> nsPtrMap;
	
	extractNs(pDoc, root, &nsPtrMap);
		
	// generate ID
	time_t t = time(NULL);
	sprintf(m_szID, "signature_%d", t);

	// hash del documento
	//string strDocHashB64;
	//string strDocCanonicalForm;

	//CanonicalizeAndHashBase64(pDoc, strDocHashB64, strDocCanonicalForm);
	

	// QualifyingProperties
	xmlDocPtr docQualifyingProperties = NULL;
	string strQualifPropsHashB64;
	string strCanonicalForm;
	
	if(m_bXAdES)
	{
		docQualifyingProperties = CreateQualifyingProperties(pDoc, pSignerCertificate);
		
		xmlDocPtr doc0 = xmlCopyDoc(docQualifyingProperties, TRUE);
		xmlDocPtr doc1 = xmlNewDoc((const xmlChar*)"1.0");
		xmlNodePtr rootNode = &(doc0->children[0].children[0]);				

		xmlDocSetRootElement(doc1, rootNode);

		//xmlNsPtr ns0 = xmlNewNs(rootNode, BAD_CAST NAMESPACE_XADES_132, NULL);

		std::map<const xmlChar*, xmlNsPtr*>::iterator it;

		for (it = nsPtrMap.begin(); it != nsPtrMap.end(); ++it)
		{ 
			xmlNsPtr* nsPtr = it->second;
			while (nsPtr)
			{
				xmlNsPtr ns0 = xmlNewNs(rootNode, BAD_CAST nsPtr[0]->href, BAD_CAST nsPtr[0]->prefix);
				nsPtr = nsPtr[0]->next ? &(nsPtr[0]->next) : NULL;
			}
		}

		xmlNsPtr ns = xmlNewNs(rootNode, BAD_CAST NAMESPACE_XML_DSIG, BAD_CAST "ds");
		xmlNsPtr ns1 = xmlNewNs(rootNode, BAD_CAST NAMESPACE_XADES_1410, BAD_CAST "xadesv1410");
		xmlNsPtr ns2 = xmlNewNs(rootNode, BAD_CAST NAMESPACE_XADES_132, BAD_CAST "xades");
		
		
		CanonicalizeAndHashBase64(doc1, strQualifPropsHashB64, strCanonicalForm);				

		xmlFreeDoc(doc0);
		xmlFreeDoc(doc1);
		//xmlFreeNs(ns0);
		//xmlFreeNs(ns);
		//xmlFreeNs(ns1);
		//xmlFreeNs(ns2);
	}

	// SignedInfo
	xmlDocPtr docSignedInfo = CreateSignedInfo(pDoc, strQualifPropsHashB64, bDetached, m_szFileName);
	
	xmlNodePtr rootNode1 = xmlDocGetRootElement(docSignedInfo);

	std::map<const xmlChar*, xmlNsPtr*>::iterator it;
	for (it = nsPtrMap.begin(); it != nsPtrMap.end(); ++it)
	{
		xmlNsPtr* nsPtr = it->second;
		while (nsPtr)
		{
			xmlNsPtr ns0 = xmlNewNs(rootNode1, BAD_CAST nsPtr[0]->href, BAD_CAST nsPtr[0]->prefix);
			nsPtr = nsPtr[0]->next ? &(nsPtr[0]->next) : NULL;
		}
	}

	string strSignedInfoHashB64;
	string strSigInfoCanonicalForm;
	xmlDocPtr doc0 = xmlCopyDoc(docSignedInfo, TRUE);
	xmlNodePtr root0 = xmlDocGetRootElement(doc0);

	//map<const xmlChar*, xmlNsPtr*> nsPtrMap0;
	//extractNs(doc0, root0, &nsPtrMap0);

	xmlDocPtr doc1 = xmlNewDoc((const xmlChar*)"1.0");
	xmlNodePtr rootNode = &(doc0->children[0].children[0]);

	xmlDocSetRootElement(doc1, rootNode);
	//std::map<const xmlChar*, xmlNsPtr*>::iterator it;

	for (it = nsPtrMap.begin(); it != nsPtrMap.end(); ++it)
	{
		xmlNsPtr* nsPtr = it->second;
		while (nsPtr)
		{
			xmlNsPtr ns0 = xmlNewNs(rootNode, BAD_CAST nsPtr[0]->href, BAD_CAST nsPtr[0]->prefix);
			nsPtr = nsPtr[0]->next ? &(nsPtr[0]->next) : NULL;
		}
	}

	/*for (it = nsPtrMap0.begin(); it != nsPtrMap0.end(); ++it)
	{
		xmlNsPtr* nsPtr = it->second;
		while (nsPtr)
		{
			xmlNsPtr ns0 = xmlNewNs(rootNode, BAD_CAST nsPtr[0]->href, BAD_CAST nsPtr[0]->prefix);
			nsPtr = nsPtr[0]->next ? &(nsPtr[0]->next) : NULL;
		}
	}*/

	xmlNsPtr ns = xmlNewNs(rootNode, BAD_CAST NAMESPACE_XML_DSIG, BAD_CAST "ds");
	xmlNsPtr ns1 = xmlNewNs(rootNode, BAD_CAST NAMESPACE_XADES_1410, BAD_CAST "xadesv1410");
	xmlNsPtr ns2 = xmlNewNs(rootNode, BAD_CAST NAMESPACE_XADES_132, BAD_CAST "xades");


	CanonicalizeAndHashBase64(doc1, strSignedInfoHashB64, strSigInfoCanonicalForm);

	//CanonicalizeAndHashBase64(docSignedInfo, strSignedInfoHashB64, strSigInfoCanonicalForm);
	

	// compute signature
	UUCByteArray signature;
	UUCByteArray data((BYTE*)strSigInfoCanonicalForm.c_str(), strSigInfoCanonicalForm.length());

	UUCByteArray hashaux;

//	if(m_bXAdES)
//	{
		BYTE hash[50];
		sha2(data.getContent(), data.getLength(), hash, 0);
		hashaux.append(hash, 32);
/*	}
	else
	{
		// calcola l'hash SHA1
		SHA1Context sha;
		SHA1Reset(&sha);
		SHA1Input(&sha, data.getContent(), data.getLength());
		SHA1Result(&sha);
		
		char szAux[100];
		sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
		
		hashaux.load(szAux);
	}
*/

	CAlgorithmIdentifier hashOID(szSHA256OID);
	UUCByteArray digest;

	CASN1OctetString digestString(hashaux.getContent(), hashaux.getLength());
	CDigestInfo digestInfo(hashOID, digestString);

	digestInfo.toByteArray(digest);

	// make signature on the digest info
	CK_RV rv = m_pSigner->Sign(digest, id, CKM_RSA_PKCS, signature);
	if(rv)
		return rv;

	static xmlChar nl[] = "\n";

	// base64
	const string in2((char*)signature.getContent(), signature.getLength());
	string out2;
	Base64::Encode(in2, &out2);

	string strSignatureB64 = out2.c_str();// base64_encode((char*)signature.getContent(), signature.getLength());

	// New Line
	//xmlNodeAddContent(docSignedInfo->children, nl);

	xmlNodePtr pSignatureRoot;

    pSignatureRoot = xmlDocGetRootElement(docSignedInfo);

	// SignatureValue
	xmlNodePtr pSignatureValue = xmlNewChild(pSignatureRoot, NULL, BAD_CAST "ds:SignatureValue", BAD_CAST strSignatureB64.c_str());

	// new line
	//xmlNodeAddContent(pSignatureRoot, nl);

	// KeyInfo
	xmlNodePtr pKeyInfo = xmlNewChild(pSignatureRoot, NULL, BAD_CAST "ds:KeyInfo", NULL);

	// X509Data
	xmlNodePtr pX509Data = xmlNewChild(pKeyInfo, NULL, BAD_CAST "ds:X509Data", NULL);

	// certificate in B64
	UUCByteArray baCert;
	pSignerCertificate->toByteArray(baCert);

	const string in1((char*)baCert.getContent(), baCert.getLength());
	string out1;
	Base64::Encode(in1, &out1);

	string strCertB64 = out1.c_str();// base64_encode((char*)baCert.getContent(), baCert.getLength());

	//X509Certificate
	xmlNodePtr pX509Certificate = xmlNewChild(pX509Data, NULL, BAD_CAST "ds:X509Certificate", BAD_CAST strCertB64.c_str());	
	//xmlNodeAddContent(pX509Data, nl);
	//xmlNodeAddContent(pKeyInfo, nl);
	//xmlNodeAddContent(pKeyInfo->parent, nl);

	if(docQualifyingProperties)
	{
		//Object
		xmlNodePtr pObject = xmlNewChild(pSignatureRoot, NULL, BAD_CAST "ds:Object", NULL);

		//QualifyingProperties
		xmlNodePtr rootQualifyingProperties = xmlDocGetRootElement(docQualifyingProperties);
		xmlAddChild(pObject, rootQualifyingProperties);
		//xmlNodeAddContent(pObject, nl);
		//xmlNodeAddContent(pObject->parent, nl);
	}

	xmlChar* membuf;
	int nSize;

	if (bDetached)
	{
		xmlDocPtr newdoc = xmlNewDoc((const xmlChar*)"1.0");
		xmlDocSetRootElement(newdoc, pSignatureRoot);
		xmlKeepBlanksDefault(0);
		xmlDocDumpFormatMemory(newdoc, &membuf, &nSize, 1);
	}
	else
	{
		// signature
		xmlAddChild(root, pSignatureRoot);
		xmlKeepBlanksDefault(0);
		xmlDocDumpFormatMemory(pDoc, &membuf, &nSize, 1);

	}

	xadesData.append(membuf, nSize);

	xmlFree(membuf);

/*		
	xmlChar* pCanonicalForm = NULL;
	int docLen = xmlC14NDocDumpMemory(pDoc, NULL, 0, NULL, 0, &pCanonicalForm);
	if(docLen > 0)
		pCanonicalForm[docLen] = 0;

	xadesData.append(pCanonicalForm, docLen);
*/
//	xmlFreeDoc(docQualifyingProperties);
//	xmlFreeDoc(docSignedInfo);
	xmlFreeDoc(pDoc);

	return CKR_OK;
}

void CXAdESGenerator::CanonicalizeAndHashBase64(xmlDocPtr pDoc, string& strDocHashB64, string& strCanonical)
{
	/*xmlChar* nss[] = {
		BAD_CAST "xades", BAD_CAST "ds", NULL
	};*/

	// hash del documento
	xmlChar* pCanonicalDoc = NULL;
	int docLen = xmlC14NDocDumpMemory(pDoc, NULL, XML_C14N_1_0, NULL, 0, &pCanonicalDoc);
	if(docLen > 0)
		pCanonicalDoc[docLen] = 0;
	
	printf("\n%s\n", pCanonicalDoc);

	strCanonical.append((char*)pCanonicalDoc);

	UUCByteArray hashaux;
	if(m_bXAdES)
	{
		BYTE hash[50];
		sha2(pCanonicalDoc, docLen, hash, 0);
		hashaux.append(hash, 32);
	}
	else
	{
		// calcola l'hash SHA1
		SHA1Context sha;
		SHA1Reset(&sha);
		SHA1Input(&sha, pCanonicalDoc, docLen);
		SHA1Result(&sha);
		
		char szAux[100];
		sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
		
		hashaux.load(szAux);
	}

	const char* hex = hashaux.toHexString();

	//strDocHashB64 = szAux;

	/*
	BYTE docHash[32];
				
	SHA256_CTX	ctx256;
	SHA256_Init(&ctx256);
	SHA256_Update(&ctx256, pCanonicalDoc, docLen);
	SHA256_Final(docHash, &ctx256);
	*/				
	const string in((char*)hashaux.getContent(), hashaux.getLength());
	string out;
	Base64::Encode(in, &out);

	strDocHashB64 = out.c_str();
	//strDocHashB64 = base64_encode((char*)hashaux.getContent(), hashaux.getLength());
}


xmlDocPtr CXAdESGenerator::CreateSignedInfo(xmlDocPtr pDocument, string& strQualifyingPropertiesB64Hash, bool bDetached, char* szFileName)
{
	// xml signed doc
	static xmlChar nl[] = "\n";

	// XML doc
	xmlDocPtr doc = xmlNewDoc((const xmlChar*)"1.0");
	doc->children = xmlNewDocNode(doc, NULL, BAD_CAST "ds:Signature", NULL);
  
	xmlNodePtr pSignatureNode = doc->children;// xmlNewChild(doc->children, NULL, BAD_CAST "ds:Signature", nl);
	xmlNewProp(pSignatureNode, BAD_CAST "Id", BAD_CAST m_szID);
	xmlNsPtr ns = xmlNewNs(pSignatureNode, BAD_CAST NAMESPACE_XML_DSIG, BAD_CAST "ds");
	xmlNsPtr ns1 = xmlNewNs(pSignatureNode, BAD_CAST NAMESPACE_XADES_1410, BAD_CAST "xadesv1410");
	xmlNsPtr ns2 = xmlNewNs(pSignatureNode, BAD_CAST NAMESPACE_XADES_132, BAD_CAST "xades");

	// <SignedInfo>
	xmlNodePtr pSignedInfo = xmlNewChild(pSignatureNode, NULL, (const xmlChar*)"ds:SignedInfo", NULL);

	// in ver 1.1 we need this namespace def
	//xmlNewNs(pSignedInfo, (const xmlChar*)NAMESPACE_XML_DSIG, NULL);

	// <CanonicalizationMethod>
	xmlNodePtr pN1 = xmlNewChild(pSignedInfo, NULL, (const xmlChar*)"ds:CanonicalizationMethod", NULL);
	xmlSetProp(pN1, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/TR/2001/REC-xml-c14n-20010315");
	//xmlNodeAddContent(pSignedInfo, nl);

	// <SignatureMethod>
	pN1 = xmlNewChild(pSignedInfo, NULL, (const xmlChar*)"ds:SignatureMethod", NULL);
	if(m_bXAdES)
	{
		xmlSetProp(pN1, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2001/04/xmldsig-more#rsa-sha256");
	}
	else
	{
		xmlSetProp(pN1, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#rsa-sha1");
	}

	//xmlNodeAddContent(pSignedInfo, nl);

	// <Reference>
	pN1 = xmlNewChild(pSignedInfo, NULL, (const xmlChar*)"ds:Reference", NULL);
	if (bDetached)
	{
		xmlSetProp(pN1, (const xmlChar*)"URI", (const xmlChar*)szFileName);
	}
	else
	{
		xmlSetProp(pN1, (const xmlChar*)"URI", (const xmlChar*)"");
	}

	if (!bDetached)
	{
		// Transformation
		xmlNodePtr pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"ds:Transforms", NULL);
		xmlNodePtr pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"ds:Transform", NULL);

		// filter xpath
		xmlSetProp(pN3, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2002/06/xmldsig-filter2");
		
		xmlNodePtr pN31 = xmlNewChild(pN3, NULL, (const xmlChar*)"dsig-xpath:XPath", (const xmlChar*)"/descendant::ds:Signature");
		xmlNsPtr ns1 = xmlNewNs(pN31, BAD_CAST "http://www.w3.org/2002/06/xmldsig-filter2", BAD_CAST "dsig-xpath");
		xmlSetProp(pN31, (const xmlChar*)"Filter", (const xmlChar*)"subtract");

		// c14N
		xmlNodePtr pN4 = xmlNewChild(pN2, NULL, (const xmlChar*)"ds:Transform", NULL);
		xmlSetProp(pN4, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/TR/2001/REC-xml-c14n-20010315");

		/*<ds:Transform Algorithm = "http://www.w3.org/2002/06/xmldsig-filter2">
			<dsig - xpath:XPath xmlns : dsig - xpath = "http://www.w3.org/2002/06/xmldsig-filter2" Filter = "subtract"> / descendant::ds:Signature< / dsig - xpath : XPath>
		</ ds : Transform>
		<ds : Transform Algorithm = "http://www.w3.org/TR/2001/REC-xml-c14n-20010315" / >*/

		//xmlSetProp(pN3, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#enveloped-signature");

		//xmlNodeAddContent(pN2, nl);
		//xmlNodeAddContent(pN1, nl);
	}

	// documents digest
	xmlNodePtr pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"ds:DigestMethod", NULL);

//	if(m_bXAdES)
//	{
		xmlSetProp(pN2, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2001/04/xmlenc#sha256");
//	}
//	else
//	{
//		xmlSetProp(pN2, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#sha1");
//	}

	//xmlNodeAddContent(pN1, nl);

	// digest value
	// hash del documento
	string strDocHashB64;
	string strCanonicalDoc;

	CanonicalizeAndHashBase64(pDocument, strDocHashB64, strCanonicalDoc);

	pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"ds:DigestValue", (const xmlChar*)strDocHashB64.c_str());
    //xmlNodeAddContent(pN1, nl);
	//xmlNodeAddContent(pSignedInfo, nl);

	if(!strQualifyingPropertiesB64Hash.empty())
	{
		// XAdES xadesSignedProperties
		//Type="http://www.w3.org/2000/09/xmldsig#SignatureProperties
		// <Reference>
		pN1 = xmlNewChild(pSignedInfo, NULL, (const xmlChar*)"ds:Reference", NULL);	
		xmlSetProp(pN1, (const xmlChar*)"URI", (const xmlChar*)"#xadesSignedProperties");
		xmlSetProp(pN1, (const xmlChar*)"Type", (const xmlChar*)"http://uri.etsi.org/01903#SignedProperties");

		// Transformation
		xmlNodePtr pN02 = xmlNewChild(pN1, NULL, (const xmlChar*)"ds:Transforms", NULL);
		// c14N
		xmlNodePtr pN04 = xmlNewChild(pN02, NULL, (const xmlChar*)"ds:Transform", NULL);
		xmlSetProp(pN04, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/TR/2001/REC-xml-c14n-20010315");

		// documents digest
		pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"ds:DigestMethod", NULL);
		/*
		if(m_bXAdES)
		{*/
			xmlSetProp(pN2, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2001/04/xmlenc#sha256");
/*		}
		else
		{
			xmlSetProp(pN2, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#sha1");
		}
*/
		//xmlSetProp(pN2, (const xmlChar*)"Algorithm", (const xmlChar*)DIGEST_METHOD_SHA1);

		//xmlNodeAddContent(pN1, nl);

		pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"ds:DigestValue", (const xmlChar*)strQualifyingPropertiesB64Hash.c_str());
		//xmlNodeAddContent(pN1, nl);
		//xmlNodeAddContent(pSignedInfo, nl);
	}

	//xmlNodeAddContent(pSignatureNode, nl);

	return doc;

	// altri eventuali reference
	/*
	for(i = 0; i < m_refecenceVect.size(); i++) 
	{
		char buf1[100];

		char szID[100];

		// generate reference ID
		time_t t = time(NULL);
		sprintf(szID, "%d", t);

		// documents digest
		xmlNodePtr pN1 = xmlNewChild(pnSigInfo, NULL, (const xmlChar*)"Reference", nl);
		
		Reference* pRef = m_refecenceVect[i];

		if(pRef->nType == REFTYPE_CONTENT)
		{
			//snprintf(buf1, sizeof(buf1), "#%s", szID);
			xmlSetProp(pN1, (const xmlChar*)"URI", (const xmlChar*)"#");

		"http://www.w3.org/2000/09/xmldsig#Object"

		xmlNodePtr pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"DigestMethod", nl);
		xmlSetProp(pN2, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#sha256");
		xmlNodeAddContent(pN1, nl);

		// in ver 1.0 we use Transforms
		if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) 
		{
			pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"Transforms", NULL);
			pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"Transform", NULL);
			xmlSetProp(pN3, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#enveloped-signature");
			xmlNodeAddContent(pN1, nl);
		}

		l1 = sizeof(buf1);
		encode(pSigInfo->pDocs[i]->szDigest, pSigInfo->pDocs[i]->nDigestLen, (byte*)buf1, &l1);
		pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"DigestValue", (const xmlChar*)buf1);
		xmlNodeAddContent(pN1, nl);
		xmlNodeAddContent(pnSigInfo, nl);
		/*
		// in ver 1.1 we don't use mime digest Reference blocks
		if(!strcmp(pSigDoc->szFormatVer, SK_XML_1_VER)) {
		  // document mime type digest
		  pN1 = xmlNewChild(pnSigInfo, NULL, (const xmlChar*)"Reference", nl);
		  if(!strcmp(pSigDoc->szFormatVer, "1.0"))
		snprintf(buf1, sizeof(buf1), "#%s-MIME", pSigInfo->pDocs[i]->szDocId);
		  else // current version is 1.1
		snprintf(buf1, sizeof(buf1), "#%s@MimeType", pSigInfo->pDocs[i]->szDocId);
		  xmlSetProp(pN1, (const xmlChar*)"URI", (const xmlChar*)buf1);
		  pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"DigestMethod", nl);
		  xmlSetProp(pN2, (const xmlChar*)"Algorithm", (const xmlChar*)DIGEST_METHOD_SHA1);
		  xmlNodeAddContent(pN1, nl);
		  pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"Transforms", NULL);
		  pN3 = xmlNewChild(pN2, NULL, (const xmlChar*)"Transform", NULL);
		  xmlSetProp(pN3, (const xmlChar*)"Algorithm", (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#enveloped-signature");
		  xmlNodeAddContent(pN1, nl);
		  l1 = sizeof(buf1);
		  encode(pSigInfo->pDocs[i]->szMimeDigest, 
			 pSigInfo->pDocs[i]->nMimeDigestLen, (byte*)buf1, &l1);
		  pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"DigestValue", (const xmlChar*)buf1);
		  xmlNodeAddContent(pN1, nl);
		}
		*/
	  
	//}
}

xmlDocPtr CXAdESGenerator::CreateQualifyingProperties(xmlDocPtr pDocument, CCertificate* pCertificate)
{
	// xml signed doc
	static xmlChar nl[] = "\n";

	// XML doc
	xmlDocPtr doc = xmlNewDoc((const xmlChar*)"1.0");
	doc->children = xmlNewDocNode(doc, NULL, BAD_CAST "xades:QualifyingProperties", NULL);
  
	// QualifyingProperties
	xmlNodePtr pQualifyingProperties = doc->children;// xmlNewChild(doc->children, NULL, BAD_CAST "xades:QualifyingProperties", nl);
	xmlNewProp(pQualifyingProperties, BAD_CAST "Target", BAD_CAST m_szID);
	//xmlNsPtr ns = xmlNewNs(pQualifyingProperties, BAD_CAST NAMESPACE_XADES_132, NULL);

	// <SignedProperties>
	xmlNodePtr pSignedProperties = xmlNewChild(pQualifyingProperties, NULL, (const xmlChar*)"xades:SignedProperties", NULL);
	xmlNewProp(pSignedProperties, BAD_CAST "Id", BAD_CAST "xadesSignedProperties");
	
	// <SignedSignatureProperties>
	xmlNodePtr pSignedSignatureProperties = xmlNewChild(pSignedProperties, NULL, (const xmlChar*)"xades:SignedSignatureProperties", NULL);

	// <SigningTime>
	/* Get UNIX-style time and display as number and string. */
	time_t ltime;
    time( &ltime );
	tm* pCurTime = gmtime(&ltime);// localtime(&ltime);
	
	char szTime[100];

	strftime (szTime,100,"%Y-%m-%dT%H:%M:%SZ",pCurTime);

	xmlNodePtr pSigningTime = xmlNewChild(pSignedSignatureProperties, NULL, (const xmlChar*)"xades:SigningTime", BAD_CAST szTime);
	//xmlNodeAddContent(pSignedSignatureProperties, nl);

	// SigningCertificate
	xmlNodePtr pSigningCertificate = xmlNewChild(pSignedSignatureProperties, NULL, (const xmlChar*)"xades:SigningCertificate", NULL);

	// Cert
	xmlNodePtr pCert = xmlNewChild(pSigningCertificate, NULL, (const xmlChar*)"xades:Cert", NULL);

	// CertDigest
	xmlNodePtr pCertDigest = xmlNewChild(pCert, NULL, (const xmlChar*)"xades:CertDigest", NULL);

	// DigestMethod
	xmlNodePtr pDigestMethod = xmlNewChild(pCertDigest, NULL, (const xmlChar*)"ds:DigestMethod", NULL);//(const xmlChar*)"http://www.w3.org/2000/09/xmldsig#sha1");
	//ns = xmlNewNs(pDigestMethod, BAD_CAST NAMESPACE_XML_DSIG, NULL);
	
	xmlNewProp(pDigestMethod, BAD_CAST "Algorithm", BAD_CAST DIGEST_METHOD_SHA256);
	//xmlNodeAddContent(pCertDigest, nl);

	// DigestValue
	// extract the cert value
	BYTE cv[50];

	UUCByteArray certval;
	pCertificate->toByteArray(certval);
	
	// calcola l'hash SHA1
	sha2(certval.getContent(), certval.getLength(), cv, 0);

	/*
	SHA1Context sha;
	SHA1Reset(&sha);
	SHA1Input(&sha, certval.getContent(), certval.getLength());
	SHA1Result(&sha);

	char szAux[100];
	sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0], sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3], sha.Message_Digest[4]);
	*/

	UUCByteArray hashaux(cv, 32);
	
	// base 64
	const string in((char*)hashaux.getContent(), hashaux.getLength());
	string out;
	Base64::Encode(in, &out);

	string strHashB64 = out.c_str();// base64_encode((char*)hashaux.getContent(), hashaux.getLength());

	// DigestValue
	xmlNodePtr pDigestValue = xmlNewChild(pCertDigest, NULL, (const xmlChar*)"ds:DigestValue", (const xmlChar*)strHashB64.c_str());
	//ns = xmlNewNs(pDigestValue, BAD_CAST NAMESPACE_XML_DSIG, NULL);
	//xmlNodeAddContent(pCertDigest, nl);

	//xmlNodeAddContent(pCert, nl);

	// IssuerSerial
	xmlNodePtr pIssuerSerial = xmlNewChild(pCert, NULL, (const xmlChar*)"xades:IssuerSerial", NULL);

	// X509IssuerName
	UUCByteArray strIssuerName;
	pCertificate->getIssuer().getNameAsString(strIssuerName);
	xmlNodePtr pX509IssuerName = xmlNewChild(pIssuerSerial, NULL, (const xmlChar*)"ds:X509IssuerName", (const xmlChar*)strIssuerName.getContent());
	//ns = xmlNewNs(pX509IssuerName, BAD_CAST NAMESPACE_XML_DSIG, NULL);
	//xmlNodeAddContent(pIssuerSerial, nl);

	// X509SerialNumber
	CASN1Integer serialNumber(pCertificate->getSerialNumber());
	UUCByteArray* pSerialNumber = (UUCByteArray*)serialNumber.getValue();
	const BYTE* content = pSerialNumber->getContent();
	
	BigInteger sernum = dataToBigInteger<const BYTE>(pSerialNumber->getContent(), pSerialNumber->getLength(), BigInteger::positive);


	string strSerNum = bigIntegerToString(sernum);

	//int len = pSerialNumber->getLength();


	/*long double dSerVal = content[0];

	for (int i = 1; i < len; i++)
	{
		dSerVal += content[i] * pow(256, i);
	}*/

	//long double dSerVal = content[len - 1];

	//for (int i = len - 2; i >= 0; i--)
	//{
	//	dSerVal += content[i] * pow(256, len - i);
	//}

	/*char szSerNum[200];
	sprintf(szSerNum, "%.0lf", dSerVal);
*/
	xmlNodePtr pX509SerialNumber = xmlNewChild(pIssuerSerial, NULL, (const xmlChar*)"ds:X509SerialNumber", (const xmlChar*)strSerNum.c_str());
	//xmlNodePtr pX509SerialNumber = xmlNewChild(pIssuerSerial, NULL, (const xmlChar*)"X509SerialNumber", (const xmlChar*)szSerNum);
	//ns = xmlNewNs(pX509SerialNumber, BAD_CAST NAMESPACE_XML_DSIG, NULL);
	//xmlNodeAddContent(pIssuerSerial, nl);

	//xmlNodeAddContent(pCert, nl);

	//xmlNodeAddContent(pSigningCertificate, nl);

	//xmlNodeAddContent(pSignedSignatureProperties, nl);
	
	//xmlNodeAddContent(pSignedProperties, nl);

	//xmlNodeAddContent(pQualifyingProperties, nl);
	
	// UnsignedProperties
	xmlNodePtr pUnsignedProperties = xmlNewChild(pQualifyingProperties, NULL, (const xmlChar*)"xades:UnsignedProperties", NULL);

	// UnsignedSignatureProperties
	xmlNodePtr pUnsignedSignatureProperties = xmlNewChild(pUnsignedProperties, NULL, (const xmlChar*)"xades:UnsignedSignatureProperties", NULL);

	//xmlNodeAddContent(pUnsignedProperties, nl);   

	//xmlNodeAddContent(pQualifyingProperties, nl);

	return doc;
}

/* N[0] - contains least significant bits, N[3] - most significant */
char* Bin128ToDec(const uint32 N[4])
{
	// log10(x) = log2(x) / log2(10) ~= log2(x) / 3.322
	static char s[128 / 3 + 1 + 1];
	uint32 n[4];
	char* p = s;
	int i;

	memset(s, '0', sizeof(s) - 1);
	s[sizeof(s) - 1] = '\0';

	memcpy(n, N, sizeof(n));

	for (i = 0; i < 128; i++)
	{
		int j, carry;

		carry = (n[3] >= 0x80000000);
		// Shift n[] left, doubling it
		n[3] = ((n[3] << 1) & 0xFFFFFFFF) + (n[2] >= 0x80000000);
		n[2] = ((n[2] << 1) & 0xFFFFFFFF) + (n[1] >= 0x80000000);
		n[1] = ((n[1] << 1) & 0xFFFFFFFF) + (n[0] >= 0x80000000);
		n[0] = ((n[0] << 1) & 0xFFFFFFFF);

		// Add s[] to itself in decimal, doubling it
		for (j = sizeof(s) - 2; j >= 0; j--)
		{
			s[j] += s[j] - '0' + carry;

			carry = (s[j] > '9');

			if (carry)
			{
				s[j] -= 10;
			}
		}
	}

	while ((p[0] == '0') && (p < &s[sizeof(s) - 2]))
	{
		p++;
	}

	return p;
}

//int main(void)
//{
//	static const uint32 testData[][4] =
//	{
//		{ 0, 0, 0, 0 },
//		{ 1048576, 0, 0, 0 },
//		{ 0xFFFFFFFF, 0, 0, 0 },
//		{ 0, 1, 0, 0 },
//		{ 0x12345678, 0x90abcdef, 0xfedcba90, 0x8765421 }
//	};
//	printf("%s\n", Bin128ToDec(testData[0]));
//	printf("%s\n", Bin128ToDec(testData[1]));
//	printf("%s\n", Bin128ToDec(testData[2]));
//	printf("%s\n", Bin128ToDec(testData[3]));
//	printf("%s\n", Bin128ToDec(testData[4]));
//	return 0;
//}

void printBigInt(const BYTE* buffer, int buflen, string& sDecimalValue) 
{
	for (int i = 0; i < buflen; i++)
	{
		unsigned x = buffer[i];
		char buf[(sizeof(x) * CHAR_BIT) / 3 + 2]; // slightly oversize buffer
		char *result = buf + sizeof(buf) - 1; // index of next output digit

		// add digits to result, starting at 
		//   the end (least significant digit)

		*result = '\0'; // terminating null
		do {
			*--result = '0' + (x % 10);  // remainder gives the next digit
			x /= 10;
		} while (x); // keep going until x reaches zero

		sDecimalValue.append(result);
	}
}
