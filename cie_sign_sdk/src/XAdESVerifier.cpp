
#include "XAdESVerifier.h"
#include "CertStore.h"


#include <string.h>
#define MAX_FILENAME 250

xmlXPathObjectPtr getRequestedNode(xmlChar* path, xmlXPathContextPtr xpathCtx);
xmlNodeSetPtr getRequestedNodes(xmlChar* path, xmlXPathContextPtr xpathCtx);

bool CXAdESVerifier::m_bLibXmlInitialized = false;

CXAdESVerifier::CXAdESVerifier(void)
: m_pXAdESDoc(NULL)
{
	if(!m_bLibXmlInitialized)
	{
		m_bLibXmlInitialized = true;
		/* Init libxml */     
		xmlInitParser();
		LIBXML_TEST_VERSION
	}
}

CXAdESVerifier::~CXAdESVerifier(void)
{
	if(m_pXAdESDoc)
	{
		if(m_pXAdESDoc->ppSignatures)
		{
			int count = m_pXAdESDoc->nSignatures;
			for(int i = 0; i < count; i++)
			{
				SAFEDELETE(m_pXAdESDoc->ppSignatures[i]->pX509Cert);
				SAFEDELETE(m_pXAdESDoc->ppSignatures[i]);					
			}

			SAFEDELETE(*(m_pXAdESDoc->ppSignatures));
			SAFEDELETE(m_pXAdESDoc->ppSignatures);
		}

		SAFEDELETE(m_pXAdESDoc);
	}
}

CCertificate* CXAdESVerifier::GetCertificate(int index)
{
	SignatureInfo* pSignatureInfo = m_pXAdESDoc->ppSignatures[index];
	return pSignatureInfo->pX509Cert;
}

CASN1ObjectIdentifier CXAdESVerifier::GetDigestAlgorithm(int index)
{
	SignatureInfo* pSignatureInfo = m_pXAdESDoc->ppSignatures[index];
	
	switch (pSignatureInfo->nDigestAlgo)
	{
	case DISIGON_ALGO_SHA256:
		return CASN1ObjectIdentifier(szSHA256OID);

	case DISIGON_ALGO_SHA512:
		return CASN1ObjectIdentifier(szSHA512OID);

	default:
	//case DISIGON_ALGO_SHA1:
		return CASN1ObjectIdentifier(szSHA1OID);
	}
}

int CXAdESVerifier::verifySignature(int index, const char* szDateTime, REVOCATION_INFO* pRevocationInfo)
{
	if(!m_pXAdESDoc)
		return -1;
	
	int bitmask = 0;
	
	SignatureInfo* pSignatureInfo = m_pXAdESDoc->ppSignatures[index];

	// verifica il certificato
	if(pSignatureInfo->pX509Cert->isValid(szDateTime))
	{
		bitmask |= VERIFIED_CERT_VALIDITY;
	}
	
	if(pSignatureInfo->pX509Cert->isQualified())
	{
		bitmask |= VERIFIED_CERT_QUALIFIED;
	}
	
	if(pSignatureInfo->pX509Cert->isSHA256())
	{
		bitmask |= VERIFIED_CERT_SHA256;
	}
	
	if(pSignatureInfo->nDigestAlgo == DISIGON_ALGO_SHA256)
	{
		bitmask |= VERIFIED_SHA256;
	}

	if(pRevocationInfo)
	{
		pRevocationInfo->nRevocationStatus = REVOCATION_STATUS_UNKNOWN;
		// verify revocation status only if the certificate is valid
		if(bitmask & VERIFIED_CERT_VALIDITY)
		{
			int verifyStatus = pSignatureInfo->pX509Cert->verifyStatus(szDateTime, pRevocationInfo);
			
			switch (verifyStatus) 
			{
				case REVOCATION_STATUS_GOOD:
					bitmask |= VERIFIED_CERT_GOOD;
					bitmask |= VERIFIED_CRL_LOADED;
					break;
					
				case REVOCATION_STATUS_REVOKED:
					bitmask |= VERIFIED_CRL_LOADED;
					bitmask |= VERIFIED_CERT_REVOKED;
					break;
					
				case REVOCATION_STATUS_SUSPENDED:
					bitmask |= VERIFIED_CERT_SUSPENDED;
					bitmask |= VERIFIED_CRL_LOADED;
					break;
					
				case REVOCATION_STATUS_UNKNOWN:
					break;
			}
		}
	}
	
	// verifica la cert chain
//	UUCByteArray issuer;
//	pSignatureInfo->pX509Cert->getIssuer().getNameAsString(issuer);//getField(OID_COMMON_NAME);
	
    CCertificate* pCert = pSignatureInfo->pX509Cert;
	CCertificate* pCACert = CCertStore::GetCertificate(*pSignatureInfo->pX509Cert);
    while(pCACert && pCert->verifySignature(*pCACert))
    {
        bitmask |= VERIFIED_CACERT_FOUND;

        //NSLog(@"issuer: %s, SN: %s", issuer.c_str(), serialNumber.toHexString());
        
        if(pCACert->isValid(szDateTime))
        {
            bitmask |= VERIFIED_CACERT_VALIDITY;
            if(pRevocationInfo)
            {
                int verifyStatus = pCACert->verifyStatus(szDateTime, NULL);
        
                switch (verifyStatus)
                {
                    case REVOCATION_STATUS_GOOD:
                        bitmask |= VERIFIED_CACERT_GOOD;
                        bitmask |= VERIFIED_CACRL_LOADED;
                        break;
                        
                    case REVOCATION_STATUS_REVOKED:
                        bitmask |= VERIFIED_CACRL_LOADED;
                        bitmask |= VERIFIED_CACERT_REVOKED;
                        break;
                        
                    case REVOCATION_STATUS_SUSPENDED:
                        bitmask |= VERIFIED_CACERT_SUSPENDED;
                        bitmask |= VERIFIED_CACRL_LOADED;
                        break;
                        
                    case REVOCATION_STATUS_UNKNOWN:
                        break;
                }
            }
        }
        
        pCert = pCACert;
        pCACert = CCertStore::GetCertificate(*pCACert);
    }
    
    if(!pCACert)
    {
        bitmask |= VERIFIED_CERT_CHAIN;
    }
    else
    {
        //NSLog(@"CA Cert not valid");
    }
	
	// Verifica Qualified
	if(pSignatureInfo->bCAdES)
	{
		// TODO:
		bitmask |= VERIFIED_SIGNED_ATTRIBUTE_MD;
		bitmask |= VERIFIED_SIGNED_ATTRIBUTE_CT;
		bitmask |= VERIFIED_SIGNED_ATTRIBUTE_SC;
	}

	// verifica la firma
	// TODO:
	bitmask |= VERIFIED_SIGNATURE;

	
	return bitmask;
}

long CXAdESVerifier::Load(char *szFilename)
{
	m_pXAdESDoc = parseXAdESFile(szFilename);
	if(!m_pXAdESDoc)
		return DISIGON_ERROR_INVALID_FILE;
	else
		return m_pXAdESDoc->nSignatures;
}

XAdESDoc* CXAdESVerifier::parseXAdESFile(char *szFilename) 
{
    xmlDocPtr doc;
    XAdESDoc* pXAdESDoc;
    xmlNodePtr curNode;


#ifdef LIBXML_SAX1_ENABLED
    /*
     * build an XML tree from a the file;
     */
    doc = xmlParseFile(szFilename);//, NULL, 0);
    if (doc == NULL) 
		return(NULL);
#else
    /*
     * the library has been compiled without some of the old interfaces
     */
    return(NULL);
#endif /* LIBXML_SAX1_ENABLED */

    /*
     * Check the document is of the right kind
     */
    
    curNode = xmlDocGetRootElement(doc);
    if (curNode == NULL) 
	{
        //fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return(NULL);
    }

	// get signature node by XPath
	xmlXPathContextPtr xpathCtx; 
	xmlXPathObjectPtr xpathObj; 

	// Create xpath evaluation context 
	xpathCtx = xmlXPathNewContext(doc);
	if(xpathCtx == NULL) 
	{
		//fprintf(stderr,"Error: unable to create new XPath context\n");
		xmlFreeDoc(doc); 
		return NULL;
	}
    
	int r = xmlXPathRegisterNs(xpathCtx,  BAD_CAST "ds", BAD_CAST "http://www.w3.org/2000/09/xmldsig#");

	r = xmlXPathRegisterNs(xpathCtx, BAD_CAST  "xs", BAD_CAST  "http://uri.etsi.org/01903/v1.3.2#");

	r = xmlXPathRegisterNs(xpathCtx, BAD_CAST  "xades", BAD_CAST  "http://uri.etsi.org/01903/v1.3.2#");

	xmlChar* path = BAD_CAST "//ds:Signature";
	// Evaluate xpath expression
	xpathObj = xmlXPathEvalExpression(path, xpathCtx);
	if(xpathObj == NULL) 
	{
		//fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
		xmlXPathFreeContext(xpathCtx); 
		xmlFreeDoc(doc); 
		return NULL;
	}

	if(xpathObj->nodesetval->nodeNr > 0)
	{
		// gets the first node
		pXAdESDoc = new XAdESDoc;

		curNode = xpathObj->nodesetval->nodeTab[0];		
		parseSignatureNode(xpathCtx, xpathObj->nodesetval, pXAdESDoc);
	}

	// Cleanup of XPath data
	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeContext(xpathCtx); 

	xmlFreeDoc(doc);
	return pXAdESDoc;
}

void CXAdESVerifier::parseSignatureNode(xmlXPathContextPtr xpathCtx, xmlNodeSetPtr signatureNodes, XAdESDoc* pXAdESDoc)
{
	pXAdESDoc->nSignatures = signatureNodes->nodeNr;
	pXAdESDoc->ppSignatures = new SignatureInfo*;
	*pXAdESDoc->ppSignatures = new SignatureInfo[pXAdESDoc->nSignatures];
	
	xmlChar* path;	
	xmlXPathObjectPtr xpathObj; 
	xmlNodePtr canonicalizationMethodNode;
	xmlNodePtr signatureMethodNode;
	xmlNodeSetPtr referenceNodeSet;
	xmlNodePtr curNode;
	xmlNodePtr signatureValueNode;
	xmlNodeSetPtr certificateNodeSet;
	xmlNodePtr qualifyingPropertiesValueNode;
	
	char szPath[BUFFERSIZE];

	for (int i = 0; i < signatureNodes->nodeNr; i++)
	{
		pXAdESDoc->ppSignatures[i] = new SignatureInfo;

		curNode = signatureNodes->nodeTab[i];
	
		xmlChar* id = xmlGetProp(curNode, BAD_CAST "Id");

		// Canonicalization method
		sprintf(szPath, "//ds:Signature[@Id='%s']/ds:SignedInfo/ds:CanonicalizationMethod", id);
		path = BAD_CAST szPath;

		// Evaluate xpath expression
		xpathObj = xmlXPathEvalExpression(path, xpathCtx);
		if(xpathObj) 
		{
			if(xpathObj->nodesetval->nodeNr > 0)
			{
				canonicalizationMethodNode = xpathObj->nodesetval->nodeTab[0];
			}

			xmlXPathFreeObject(xpathObj);
		}

		// SignatureMethod
		sprintf(szPath, "//ds:Signature[@Id='%s']/ds:SignedInfo/ds:SignatureMethod", id);
		path = BAD_CAST szPath;

		// Evaluate xpath expression
		xpathObj = xmlXPathEvalExpression(path, xpathCtx);
		if(xpathObj) 
		{
			if(xpathObj->nodesetval->nodeNr > 0)
			{
				signatureMethodNode = xpathObj->nodesetval->nodeTab[0];
				char* szContent = (char*)signatureMethodNode->properties[0].children[0].content;// xmlNodeContent(signatureMethodNode);

				if(strstr(szContent, "sha256"))
				{
					(*pXAdESDoc->ppSignatures[i]).nDigestAlgo = DISIGON_ALGO_SHA256;
				}
				else if(strstr(szContent, "sha512"))
				{
					(*pXAdESDoc->ppSignatures[i]).nDigestAlgo = DISIGON_ALGO_SHA512;
				}  
				else if(strstr(szContent, "sha1"))
				{
					(*pXAdESDoc->ppSignatures[i]).nDigestAlgo = DISIGON_ALGO_SHA1;
				}

			}

			xmlXPathFreeObject(xpathObj);
		}

		// References
		sprintf(szPath, "//ds:Signature[@Id='%s']/ds:SignedInfo/ds:Reference", id);
		path = BAD_CAST szPath;

		// Evaluate xpath expression
		xpathObj = xmlXPathEvalExpression(path, xpathCtx);
		if(xpathObj) 
		{
			if(xpathObj->nodesetval->nodeNr > 0)
			{
				referenceNodeSet = xpathObj->nodesetval;
			}

			xmlXPathFreeObject(xpathObj);
		}

		// SignatureValue
		sprintf(szPath, "//ds:Signature[@Id='%s']/ds:SignatureValue", id);
		path = BAD_CAST szPath;

		// Evaluate xpath expression
		xpathObj = xmlXPathEvalExpression(path, xpathCtx);
		if(xpathObj) 
		{
			if(xpathObj->nodesetval->nodeNr > 0)
			{
				signatureValueNode = xpathObj->nodesetval->nodeTab[0];
				
				char* szContent = (char*)xmlNodeGetContent(signatureValueNode);
				(*pXAdESDoc->ppSignatures[i]).sigValue.append((BYTE*)szContent, strlen(szContent));
			}

			xmlXPathFreeObject(xpathObj);
		}

		// X509Certificates
		sprintf(szPath, "//ds:Signature[@Id='%s']/ds:KeyInfo/ds:X509Data/ds:X509Certificate", id);
		path = BAD_CAST szPath;

		// Evaluate xpath expression
		xpathObj = xmlXPathEvalExpression(path, xpathCtx);
		if(xpathObj) 
		{
			if(xpathObj->nodesetval->nodeNr > 0)
			{
				certificateNodeSet = xpathObj->nodesetval;
				char* szContent = (char*)xmlNodeGetContent(certificateNodeSet->nodeTab[0]);
				UUCByteArray content((BYTE*)szContent, strlen(szContent));
				(*pXAdESDoc->ppSignatures[i]).pX509Cert = CCertificate::createCertificate(content);
			}

			xmlXPathFreeObject(xpathObj);
		}

		// QualifyingProperties
		sprintf(szPath, "//ds:Signature[@Id='%s']/ds:Object/xades:QualifyingProperties", id);
		path = BAD_CAST szPath;

		// Evaluate xpath expression
		xpathObj = xmlXPathEvalExpression(path, xpathCtx);
		if(xpathObj) 
		{
			(*pXAdESDoc->ppSignatures[i]).bCAdES = false;
			if(xpathObj->nodesetval->nodeNr > 0)
			{
				qualifyingPropertiesValueNode = xpathObj->nodesetval->nodeTab[0];
				(*pXAdESDoc->ppSignatures[i]).bCAdES = true;
			}

			xmlXPathFreeObject(xpathObj);
		}
	}
}
/*
xmlXPathObjectPtr getRequestedNode(xmlChar* path, xmlXPathContextPtr xpathCtx)
{
	// Evaluate xpath expression
	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(path, xpathCtx);
	if(xpathObj) 
	{
		if(xpathObj->nodesetval->nodeNr > 0)
		{
			return xpathObj->nodesetval->nodeTab[0];
		}
	}

	return NULL;
}

xmlNodeSetPtr getRequestedNodes(xmlChar* path, xmlXPathContextPtr xpathCtx)
{
	// Evaluate xpath expression
	xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression(path, xpathCtx);
	if(xpathObj) 
	{
		return xpathObj->nodesetval;
	}

	return NULL;
}
*/
