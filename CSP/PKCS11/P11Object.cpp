#include "..\StdAfx.h"
#include "p11object.h"
#include "cardtemplate.h"

static char *szCompiledFile=__FILE__;

namespace p11 {

DWORD CP11Object::dwP11ObjectCnt=0;

CP11Object::CP11Object(CK_OBJECT_CLASS objClass,void *TemplateData)
{
	ObjClass=objClass;
	pTemplateData=TemplateData;
	addAttribute(CKA_CLASS,ByteArray((BYTE*)&ObjClass,sizeof(CK_OBJECT_CLASS)));
}

RESULT CP11Object::addAttribute(CK_ATTRIBUTE_TYPE type,ByteArray &data)
{
	init_func
	attributes[type].alloc_copy(data);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CP11Object::addAttribute(CK_ATTRIBUTE_TYPE type,BYTE *pData,DWORD dwLen)
{
	init_func
	attributes[type].alloc_copy(pData,dwLen);
	_return(OK)
	exit_func
	_return(FAIL)
}

RESULT CP11Object::getAttribute(CK_ATTRIBUTE_TYPE type,ByteArray *&pValue)
{
	init_func
	AttributeMap::const_iterator pPair;
	pPair=attributes.find(type);
	if (pPair==attributes.end()) {
		pValue=NULL;
		_return(OK)
	}
	pValue=(ByteArray *)&pPair->second;
	_return(OK)
	exit_func
	_return(FAIL)
}

CK_RV CP11Object::GetAttributeValue(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
{
	init_func
	CK_RV response=CKR_OK;	
	for (unsigned int i=0;i<ulCount;i++) {
		ByteArray *attr=NULL;
		RESULT ris;

		CK_ULONG ulValLen=pTemplate[i].ulValueLen;
		pTemplate[i].ulValueLen=-1;

		if (ris=getAttribute(pTemplate[i].type,attr)) {
			if (ris==ERR_ATTRIBUTE_IS_SENSITIVE) {
				_return(response = CKR_ATTRIBUTE_TYPE_INVALID);
			}
		}
		if (attr) {
			if (pTemplate[i].pValue==NULL)
				pTemplate[i].ulValueLen=attr->size();
			else {
				if (attr->size()>ulValLen)
					_return(CKR_BUFFER_TOO_SMALL)
					memcpy_s(pTemplate[i].pValue, attr->size(), attr->lock(), attr->size());
				pTemplate[i].ulValueLen=attr->size();
			}
		}
		else
			_return(CKR_ATTRIBUTE_TYPE_INVALID);
	}
	_return(response)
	exit_func
	_return(CKR_GENERAL_ERROR)
}

RESULT CP11Object::GetObjectSize(CK_ULONG_PTR pulSize)
{
	init_func
	// devo almeno leggerlo dalla carta per sapere che dimensioni ha
	if (!bReadValue) {
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateReadObjectAttributes(pSlot->pTemplateData,this),
			ERR_CANT_READ_FROM_CARD)
	}
	DWORD ret=pSlot->pTemplate->FunctionList.templateGetObjectSize(pSlot->pTemplateData,this,pulSize);
	_return(ret);
	exit_func
	_return(FAIL)
}

RESULT CP11Object::SetAttributes(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount)
{
	init_func
	for (DWORD i=0;i<ulCount;i++) {
		addAttribute(pTemplate[i].type,ByteArray((BYTE*)pTemplate[i].pValue,pTemplate[i].ulValueLen));
	}
	_return(OK)
	exit_func
	_return(FAIL)
}
CP11Certificate::CP11Certificate(void *TemplateData) : CP11Object(CKO_CERTIFICATE,TemplateData)
{
	bReadValue=false;
}

RESULT CP11Certificate::SetAttributes(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount) 
{
	init_func
	RESULT ris=CP11Object::SetAttributes(pTemplate,ulCount);
	if (!ris)
		bReadValue=true;
	_return(ris)
	exit_func
	_return(FAIL)
}

RESULT CP11Certificate::getAttribute(CK_ATTRIBUTE_TYPE type,ByteArray *&pValue) {
	init_func

	AttributeMap::iterator it=attributes.find(type);
	if (it==attributes.end() && !bReadValue) {
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateReadObjectAttributes(pSlot->pTemplateData,this),
			ERR_CANT_READ_FROM_CARD)
	}

	DWORD ret=CP11Object::getAttribute(type,pValue);
	_return(ret)
	exit_func
	_return(FAIL)
}

CP11Data::CP11Data(void *TemplateData) : CP11Object(CKO_DATA,TemplateData)
{
	bReadValue=false;
}

RESULT CP11Data::getAttribute(CK_ATTRIBUTE_TYPE type,ByteArray *&pValue) {
	init_func
	AttributeMap::iterator it=attributes.find(type);
	if (it==attributes.end() && !bReadValue) {
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateReadObjectAttributes(pSlot->pTemplateData,this),
			ERR_CANT_READ_FROM_CARD)
	}

	DWORD ret=CP11Object::getAttribute(type,pValue);
	_return(ret)
	exit_func
	_return(FAIL)
}

RESULT CP11Data::SetAttributes(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount) 
{
	init_func
	RESULT ris=CP11Object::SetAttributes(pTemplate,ulCount);
	if (!ris)
		bReadValue=true;
	_return(ris)
	exit_func
	_return(FAIL)
}

RESULT CP11Object::IsPrivate(bool &bPrivate)
{
	init_func	
	bPrivate=false;
	ByteArray *baVal=NULL;
	P11ER_CALL(getAttribute(CKA_PRIVATE,baVal),
		ERR_GET_ATTRIBUTE)

	if (baVal==NULL)
		_return(OK)
	else {
		if (ByteArrayToVar(*baVal,CK_BBOOL)==FALSE)
			_return(OK)
		else
			bPrivate=true;
	}
	_return(OK)
	exit_func
	_return(FAIL)
}

CP11PrivateKey::CP11PrivateKey(void *TemplateData) : CP11Object(CKO_PRIVATE_KEY,TemplateData)
{
	bReadValue=false;
}

RESULT CP11PrivateKey::getAttribute(CK_ATTRIBUTE_TYPE type,ByteArray *&pValue) {
	init_func

	if (type==CKR_ATTRIBUTE_SENSITIVE || 
		type==CKA_PRIME_1 ||
		type==CKA_PRIME_2 ||
		type==CKA_EXPONENT_1 ||
		type==CKA_EXPONENT_2 ||
		type==CKA_COEFFICIENT ||
		type==CKA_PRIME_1)
		_return(ERR_ATTRIBUTE_IS_SENSITIVE)

	AttributeMap::iterator it=attributes.find(type);
	if (it==attributes.end() && !bReadValue) {
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateReadObjectAttributes(pSlot->pTemplateData,this),
			ERR_CANT_READ_FROM_CARD)
	}

	DWORD ret=CP11Object::getAttribute(type,pValue);
	_return(ret)
	exit_func
	_return(FAIL)
}

CP11PublicKey::CP11PublicKey(void *TemplateData) : CP11Object(CKO_PUBLIC_KEY,TemplateData)
{
	bReadValue=false;
}

RESULT CP11PublicKey::getAttribute(CK_ATTRIBUTE_TYPE type,ByteArray *&pValue) {
	init_func

	AttributeMap::iterator it=attributes.find(type);
	if (it==attributes.end() && !bReadValue) {
		P11ER_CALL(pSlot->pTemplate->FunctionList.templateReadObjectAttributes(pSlot->pTemplateData,this),
			ERR_CANT_READ_FROM_CARD)
	}

	DWORD ret=CP11Object::getAttribute(type,pValue);
	_return(ret)
	exit_func
	_return(FAIL)
}

}