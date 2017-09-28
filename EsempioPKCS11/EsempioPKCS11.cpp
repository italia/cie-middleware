// EsempioPKCS11.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string>

#pragma pack(1)
#include "pkcs11.h"
#pragma pack()


// la specifica PKCS#11 di riferimento può essere consultata all'indirizzo https://www.cryptsoft.com/pkcs11doc/STANDARD/pkcs-11v2-11r1.pdf


void _tmain(int argc, _TCHAR* argv[])
{
	HMODULE mod = LoadLibrary("CIEPKI.dll");
	CK_C_GetFunctionList GetFunctionList = (CK_C_GetFunctionList)GetProcAddress(mod, "C_GetFunctionList");
	CK_FUNCTION_LIST_PTR FunctionList;
	GetFunctionList(&FunctionList);
	FunctionList->C_Initialize(NULL);

	CK_SLOT_ID Slots[20];
	CK_SLOT_ID Slot;
	CK_ULONG SlotsNum = 20;
	FunctionList->C_GetSlotList(TRUE, Slots, &SlotsNum);

	if (SlotsNum == 0) {
		std::cout << "Appoggiare la CIE sul lettore\n";
		FunctionList->C_WaitForSlotEvent(0, &Slot, NULL);
	}
	else {
		Slot = Slots[0];
	}
	CK_SESSION_HANDLE Session;
	CK_RV result = FunctionList->C_OpenSession(Slot, CKF_SERIAL_SESSION, NULL, NULL, &Session);
	if (result != CKR_OK) {
		std::cout << "Errore nella lettura della CIE\n";
		return;
	}

	CK_TOKEN_INFO Token;
	FunctionList->C_GetTokenInfo(Slot, &Token);
	std::cout << "ID_Servizi: " << std::string((char*)Token.serialNumber, sizeof(Token.serialNumber)) << "\n";

	CK_ATTRIBUTE CertAttribute;
	CK_OBJECT_CLASS CertClass = CKO_CERTIFICATE;
	CertAttribute.type = CKA_CLASS;
	CertAttribute.pValue = &CertClass;
	CertAttribute.ulValueLen = sizeof(CertClass);
	FunctionList->C_FindObjectsInit(Session, &CertAttribute, 1);
	CK_OBJECT_HANDLE Object;
	CK_ULONG NumObjects=0;
	FunctionList->C_FindObjects(Session, &Object, 1, &NumObjects);
	FunctionList->C_FindObjectsFinal(Session);

	CK_ATTRIBUTE CertValue;
	CertValue.type = CKA_VALUE;
	CertValue.pValue = NULL;
	CertValue.ulValueLen = 0;

	FunctionList->C_GetAttributeValue(Session, Object, &CertValue, 1);
	CertValue.pValue = new BYTE[CertValue.ulValueLen];
	FunctionList->C_GetAttributeValue(Session, Object, &CertValue, 1);

	FunctionList->C_CloseSession(Session);
	FunctionList->C_Finalize(NULL);

	PCCERT_CONTEXT cer = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, (BYTE*)CertValue.pValue, CertValue.ulValueLen);

	int NameSize = CertNameToStr(X509_ASN_ENCODING, &cer->pCertInfo->Subject, CERT_X500_NAME_STR, NULL, 0);
	char *Name = new char[NameSize];
	CertNameToStr(X509_ASN_ENCODING, &cer->pCertInfo->Subject, CERT_X500_NAME_STR, Name, NameSize);
	std::cout << "Titolare :" << Name;

	delete CertValue.pValue;
	delete Name;
	return;
}

