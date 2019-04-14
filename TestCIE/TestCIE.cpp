// TestCIE.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//
/*
 *  Copyright (c) 2000-2018 by Ugo Chirico - http://www.ugochirico.com
 *  All Rights Reserved
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <Windows.h>
#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <fstream>
#include <string>

 // directive for PKCS#11
#include "../PKCS11/cryptoki.h"

#include <memory.h>
#include <time.h>
#include <map>

#include "UUCByteArray.h"

typedef CK_RV(*C_GETFUNCTIONLIST)(CK_FUNCTION_LIST_PTR_PTR ppFunctionList);
CK_FUNCTION_LIST_PTR g_pFuncList;
int g_nLogLevel = 5;

std::map<CK_MECHANISM_TYPE, std::string> mechanismMap;

void error(CK_RV rv)
{
	printf("  -------------------\n");
	printf("  <e> Errore n. 0x%X\n", rv);
	printf("  -------------------\n");

	//std::cout << "  <e> Errore n. " << rv << std::endl;
}

void init()
{
	// Inizializza
	if (g_nLogLevel > 1)
		std::cout << "  -> Inizializza la libreria\n    - C_Initialize" << std::endl;

	CK_C_INITIALIZE_ARGS* pInitArgs = NULL_PTR;
	CK_RV rv = g_pFuncList->C_Initialize(pInitArgs);
	if (rv != CKR_OK)
	{
		error(rv);
		return;
	}

	if (g_nLogLevel > 1)
		std::cout << "  -- Inizializzazione completata " << std::endl;
}

void close()
{
	if (g_nLogLevel > 1)
		std::cout << "  -> Chiude la sessione con la libreria\n    - C_Finalize" << std::endl;

	CK_RV rv = g_pFuncList->C_Finalize(NULL_PTR);
	if (rv != CKR_OK)
	{
		error(rv);
		return;
	}
}

bool getSlotInfo(CK_SLOT_ID slotid)
{
	CK_SLOT_INFO slotInfo;
	CK_RV rv = g_pFuncList->C_GetSlotInfo(slotid, &slotInfo);
	if (rv != CKR_OK)
	{
		error(rv);
		return false;
	}

	if (g_nLogLevel > 2)
	{
		std::cout << "    - " << slotInfo.slotDescription << std::endl;
		std::cout << "    - " << slotInfo.manufacturerID << std::endl;
		std::cout << "    - " << slotInfo.flags << std::endl;
		if (slotInfo.flags & CKF_TOKEN_PRESENT)
			std::cout << "    - Carta inserita" << std::endl;
		else
			std::cout << "    - Carta non inserita" << std::endl;
	}

	return true;
}

CK_SLOT_ID_PTR getSlotList(bool bPresent, CK_ULONG* pulCount)
{
	// carica gli slot disponibili

	// legge la lista delle funzioni
	if (g_nLogLevel > 1)
		std::cout << "  -> Chiede la lista degli slot disponibili\n    - C_GetSlotList\n    - C_GetSlotInfo" << std::endl;

	CK_SLOT_ID_PTR pSlotList;

	// riceve la lista degli slot disponibili
	CK_RV rv = g_pFuncList->C_GetSlotList(bPresent, NULL_PTR, pulCount);
	if (rv != CKR_OK)
	{
		error(rv);
		return NULL_PTR;
	}

	if (*pulCount > 0)
	{
		if (g_nLogLevel > 2)
			std::cout << "  -> Slot disponibili: " << *pulCount << std::endl;

		pSlotList = (CK_SLOT_ID_PTR)malloc(*pulCount * sizeof(CK_SLOT_ID));
		rv = g_pFuncList->C_GetSlotList(bPresent, pSlotList, pulCount);
		if (rv != CKR_OK)
		{
			error(rv);
			free(pSlotList);
			return NULL_PTR;
		}

		for (unsigned int i = 0; i < *pulCount; i++)
		{
			getSlotInfo(pSlotList[i]);
		}

		if (g_nLogLevel > 1)
			std::cout << "  -- Richiesta completata " << std::endl;

		return pSlotList;
	}
	else
	{
		std::cout << "  -> Nessuno Slot disponibile " << std::endl;
		return NULL_PTR;
	}
}

void getTokenInfo(CK_SLOT_ID slotid)
{
	// Legge le info sul token inserito

	if (g_nLogLevel > 1)
		std::cout << "  -> Chiede le info sul token inserito\n    - C_GetTokenInfo" << std::endl;

	CK_TOKEN_INFO tkInfo;

	CK_RV rv = g_pFuncList->C_GetTokenInfo(slotid, &tkInfo);
	if (rv != CKR_OK)
	{
		error(rv);
		return;
	}

	if (g_nLogLevel > 3)
	{
		std::cout << "  -> Token Info:" << std::endl;
		std::cout << "    - Label: " << tkInfo.label << std::endl;
		std::cout << "    - Model: " << tkInfo.model << std::endl;
		std::cout << "    - S/N: " << tkInfo.serialNumber << std::endl;
	}

	if (g_nLogLevel > 1)
		std::cout << "  -- Richiesta completata " << std::endl;
}

void mechanismList(CK_SLOT_ID slotid)
{
	if (g_nLogLevel > 1)
	{
		std::cout << "  -> Legge i maccanismi disponibili nello slot " << slotid << " - C_GetMechanismList" << std::endl;
	}

	CK_MECHANISM_TYPE_PTR pMechanismType = NULL;
	CK_ULONG count = 0;
	CK_RV rv = g_pFuncList->C_GetMechanismList(slotid, pMechanismType, &count);
	if (rv != CKR_OK)
	{
		error(rv);
		return;
	}

	pMechanismType = new CK_MECHANISM_TYPE[count];

	rv = g_pFuncList->C_GetMechanismList(slotid, pMechanismType, &count);
	if (rv != CKR_OK)
	{
		error(rv);
		return;
	}

	for (CK_ULONG i = 0; i < count; i++)
	{		
		std::cout << "  -- " << mechanismMap[pMechanismType[i]] << ": " << pMechanismType[i] << std::endl;
	}
}

CK_SESSION_HANDLE openSession(CK_SLOT_ID slotid)
{
	if (g_nLogLevel > 1)
	{
		std::cout << "  -> Apre una sessione con lo slot " << slotid << " - C_OpenSession" << std::endl;
	}

	CK_SESSION_HANDLE hSession;
	CK_RV rv = g_pFuncList->C_OpenSession(slotid, CKF_RW_SESSION | CKF_SERIAL_SESSION, NULL, NULL, &hSession);
	if (rv != CKR_OK)
	{
		error(rv);
		return NULL_PTR;
	}

	if (g_nLogLevel > 1)
		std::cout << "  -- Sessione aperta: " << hSession << std::endl;

	return hSession;
}

bool login(CK_SESSION_HANDLE hSession)
{
	if (g_nLogLevel > 1)
		std::cout << "  -> Login allo slot\n    - C_Login" << std::endl;


	bool pinIsGood = false;
	std::string sPIN;
	while (!pinIsGood)
	{
		std::cout << "   - Inserire la seconda parte del PIN ";
		std::cin >> sPIN;
		//std::getline(std::cin, sPIN);
		size_t len = sPIN.size();
		if (len != 4)
		{
			std::cout << "   Attenzione: Il pin deve essere composto da 4 numeri" << std::endl;;
		}
		else
		{
			const char* szPIN = sPIN.c_str();

			size_t i = 0;
			while (i < len && (szPIN[i] >= '0' && szPIN[i] <= '9'))
				i++;

			if (i == len)
				pinIsGood = true;
			else
				std::cout << "   Attenzione: Il pin deve essere composto da 4 numeri" << std::endl;;
		}
	}

	CK_RV rv = g_pFuncList->C_Login(hSession, CKU_USER, (CK_CHAR_PTR)sPIN.c_str(), sPIN.size());
	if (rv != CKR_OK)
	{
		error(rv);
		return false;
	}

	if (g_nLogLevel > 1)
		std::cout << "  -- Login Effettuato " << std::endl;
	return true;
}

bool logout(CK_SESSION_HANDLE hSession)
{
	if (g_nLogLevel > 1)
		std::cout << "  -> Logout allo slot\n    - C_Logout" << std::endl;

	CK_RV rv = g_pFuncList->C_Logout(hSession);
	if (rv != CKR_OK)
	{
		error(rv);
		return false;
	}

	if (g_nLogLevel > 1)
		std::cout << "  -- Logout Effettuato" << std::endl;
	return true;
}

bool findObject(CK_SESSION_HANDLE hSession, CK_ATTRIBUTE_PTR pAttributes, CK_ULONG ulCount, CK_OBJECT_HANDLE_PTR pObjects, CK_ULONG_PTR pulObjCount)
{
	if (g_nLogLevel > 1)
		std::cout << "  -> Ricerca di oggetti \n    - C_FindObjectsInit\n    - C_FindObjects\n    - C_FindObjectsFinal" << std::endl;

	CK_RV rv;

	rv = g_pFuncList->C_FindObjectsInit(hSession, pAttributes, ulCount);
	if (rv != CKR_OK)
	{
		std::cout << "  ->     - C_FindObjectsInit fails" << std::endl;
		error(rv);
		return false;
	}

	if (g_nLogLevel > 2)
		std::cout << "      - C_FindObjectsInit OK" << std::endl;

	rv = g_pFuncList->C_FindObjects(hSession, pObjects, *pulObjCount, pulObjCount);
	if (rv != CKR_OK)
	{
		std::cout << "      - C_FindObjects fails found" << *pulObjCount << std::endl;
		error(rv);
		g_pFuncList->C_FindObjectsFinal(hSession);
		return false;
	}

	if (g_nLogLevel > 2)
		std::cout << "      - C_FindObjects OK. Objects found: " << *pulObjCount << std::endl;

	rv = g_pFuncList->C_FindObjectsFinal(hSession);
	if (rv != CKR_OK)
	{
		std::cout << "      - C_FindObjectsFinal fails" << std::endl;
		error(rv);
		g_pFuncList->C_FindObjectsFinal(hSession);
		return false;
	}

	if (g_nLogLevel > 1)
		std::cout << "      - C_FindObjectsFinal OK" << std::endl;

	return true;

}

void closeSession(CK_SESSION_HANDLE hSession)
{
	if (g_nLogLevel > 1)
		std::cout << "  -> Chiude una sessione con lo slot\n    - C_CloseSession" << std::endl;

	CK_RV rv = g_pFuncList->C_CloseSession(hSession);
	if (rv != CKR_OK)
	{
		error(rv);
		return;
	}

	if (g_nLogLevel > 1)
		std::cout << "  -- Sessione chiusa: " << hSession << std::endl;
}

void showAttributes(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject)
{
	if (g_nLogLevel > 3)
		std::cout << "  -> Legge gli attributi di un oggetto \n    - C_GetAttributeValue" << std::endl;

	CK_BBOOL        bPrivate = 0;
	CK_BBOOL        bToken = 0;

	char* btLabel = NULL;
	char* btID = NULL;

	CK_ATTRIBUTE    attr[] = {
		{CKA_PRIVATE, &bPrivate, sizeof(bPrivate)},
		{CKA_TOKEN, &bToken, sizeof(bToken)},
		{CKA_LABEL, btLabel, 0},
		{CKA_ID, btID, 0}
	};

	CK_RV rv = g_pFuncList->C_GetAttributeValue(hSession, hObject, attr, 4);
	if (rv != CKR_OK)
	{
		error(rv);
	}


	attr[2].pValue = malloc(attr[2].ulValueLen + 2);
	attr[3].pValue = malloc(attr[3].ulValueLen + 2);

	rv = g_pFuncList->C_GetAttributeValue(hSession, hObject, attr, 4);
	if (rv != CKR_OK)
	{
		free(attr[2].pValue);
		free(attr[3].pValue);
		error(rv);
	}

	btLabel = (char*)attr[2].pValue;
	btID = (char*)attr[3].pValue;

	btLabel[attr[2].ulValueLen] = 0;
	btID[attr[3].ulValueLen] = 0;

	if (g_nLogLevel > 3)
	{
		std::cout << "      - Label: " << btLabel << std::endl;
		std::cout << "      - Private: " << (bPrivate ? "true" : "false") << std::endl;
		std::cout << "      - Token: " << (bToken ? "true" : "false") << std::endl;
		std::cout << "      - ID: " << btID << std::endl;
	}

	free(attr[2].pValue);
	free(attr[3].pValue);
}

void showCertAttributes(CK_SESSION_HANDLE hSession, CK_OBJECT_HANDLE hObject)
{
	if (g_nLogLevel > 3)
		std::cout << "  -> Legge gli attributi di un oggetto \n    - C_GetAttributeValue" << std::endl;

	CK_BBOOL        bPrivate = 0;
	CK_BBOOL        bToken = 0;

	CK_ATTRIBUTE    attr[] = {
		{CKA_PRIVATE, &bPrivate, sizeof(bPrivate)},
		{CKA_TOKEN, &bToken, sizeof(bToken)},
		{CKA_LABEL, NULL, 256},
		{CKA_ISSUER, NULL, 256},
		{CKA_SERIAL_NUMBER, NULL, 256},
		{CKA_ID, NULL, 256},
		{CKA_SUBJECT, NULL, 256},
		{CKA_VALUE, NULL, 0},
		//{CKA_VALUE, szValue, 256}
	};

	CK_RV rv = g_pFuncList->C_GetAttributeValue(hSession, hObject, attr, 8);
	if (rv != CKR_OK)
	{
		error(rv);
	}

	for (int i = 0; i < 8; i++)
	{
		attr[i].pValue = malloc(attr[i].ulValueLen + 1);
	}

	rv = g_pFuncList->C_GetAttributeValue(hSession, hObject, attr, 8);
	if (rv != CKR_OK)
	{
		error(rv);
	}

	for (int i = 0; i < 8; i++)
	{
		((char*)attr[i].pValue)[attr[i].ulValueLen] = 0;
	}

	if (g_nLogLevel > 3)
	{
		std::cout << "      - Label: " << (char*)attr[2].pValue << std::endl;
		std::cout << "      - Issuer: " << UUCByteArray((BYTE*)attr[3].pValue, attr[3].ulValueLen).toHexString() << std::endl;
		std::cout << "      - Subject: " << UUCByteArray((BYTE*)attr[6].pValue, attr[6].ulValueLen).toHexString() << std::endl;
		std::cout << "      - Value: " << UUCByteArray((BYTE*)attr[7].pValue, attr[7].ulValueLen).toHexString() << std::endl;
		std::cout << "      - Serial: " << (char*)attr[4].pValue << std::endl;
		std::cout << "      - ID: " << (char*)attr[5].pValue << std::endl;
	}

	for (int i = 0; i < 8; i++)
	{
		free(attr[i].pValue);
	}
}


bool signVerify(CK_SESSION_HANDLE hSession, CK_MECHANISM_TYPE mechanism)
{
	if (g_nLogLevel > 1)
		std::cout << "  -> Firma e verifica con algo " << mechanismMap[mechanism] << "\n   - C_Sign\n    - C_Verify" << std::endl;

	UUCByteArray dataValHashed;

	CK_OBJECT_HANDLE hObjectPriKey;
	CK_OBJECT_HANDLE hObjectPubKey;
	CK_ULONG ulCount = 1;

	CK_OBJECT_CLASS ckClassPri = CKO_PRIVATE_KEY;
	CK_OBJECT_CLASS ckClassPub = CKO_PUBLIC_KEY;

	CK_ATTRIBUTE template_cko_keyPri[] = {
		{CKA_CLASS, &ckClassPri, sizeof(ckClassPri)},
	};

	if (!findObject(hSession, template_cko_keyPri, 1, &hObjectPriKey, &ulCount))
	{
		std::cout << "  -> Operazione fallita" << std::endl;
		return false;
	}

	if (ulCount < 1)
	{
		std::cout << "  -> Oggetto chiave privata non trovato" << std::endl;
		return false;
	}


	showAttributes(hSession, hObjectPriKey);

	CK_ATTRIBUTE template_cko_keyPub[] = {
		{CKA_CLASS, &ckClassPub, sizeof(ckClassPub)},
	};


	if (!findObject(hSession, template_cko_keyPub, 1, &hObjectPubKey, &ulCount))
	{
		std::cout << "  -> Operazione fallita" << std::endl;
		return false;
	}

	if (ulCount < 1)
	{
		std::cout << "  -> Oggetto chiave publica non trovato" << std::endl;
		return false;
	}

	showAttributes(hSession, hObjectPubKey);

	CK_MECHANISM pMechanism[] = { mechanism, NULL_PTR, 0 };
	BYTE* pOutput;
	CK_ULONG outputLen = 256;

	//    char* szHex =  "0959208F14CC999E619536D85FEC0E7488FA2E99624228629A79512FE61274D4333824294279941F57DF36BCCC89DDB459D6607E43209874F2B631BD541ECA4A3FDFA8893E6ED0E0D4508AE335E7C85D77EEDFCB548472E172A661CAE514FFF6359407EF0FD595BED3D0BED606CA18032C1286606BE57A09CED2DD41955BB8176243BD0363D055B42C3017E726E26F4A680AF3FCE685C0AFF73ABF4F78DF55E4FD304DC2F8C81F4D918A23A5DE79E2BF7637DFC064C92ED7EE50FA2F168773C3D541CEB7439327C8B7176FBA3CEE8ADDC125A1518A6BBD4E5BB2D13EB9FF884875175774D60EB34F4EE08E81B8C65DB0CBBB251619B54BCFDA8C243DE0D037BC";
	//
	//    UUCByteArray dataVal(szHex);

	const char* szToSign = "some text to sign";
	UUCByteArray dataVal((BYTE*)szToSign, strlen(szToSign));

	if (g_nLogLevel > 2)
		std::cout << "  -> Appone la Firma digitale : " << std::endl;

	CK_RV rv = g_pFuncList->C_SignInit(hSession, pMechanism, hObjectPriKey);
	if (rv != CKR_OK)
	{
		error(rv);
		return false;
	}

	rv = g_pFuncList->C_Sign(hSession, (BYTE*)dataVal.getContent(), dataVal.getLength(), NULL, &outputLen);
	if (rv != CKR_OK)
	{
		error(rv);
		return false;
	}

	pOutput = (BYTE*)malloc(outputLen);

	rv = g_pFuncList->C_Sign(hSession, (BYTE*)dataVal.getContent(), dataVal.getLength(), pOutput, &outputLen);
	if (rv != CKR_OK)
	{
		delete pOutput;
		error(rv);
		return false;
	}

	UUCByteArray output(pOutput, outputLen);

	if (g_nLogLevel > 2)
		std::cout << "  -- Firma digitale apposta: " << std::endl << "     " << output.toHexString() << std::endl;

	std::cout << "  -> Verifica la Firma digitale : " << std::endl;

	rv = g_pFuncList->C_VerifyInit(hSession, pMechanism, hObjectPubKey);
	if (rv != CKR_OK)
	{
		delete pOutput;
		error(rv);
		return false;
	}

	rv = g_pFuncList->C_Verify(hSession, (BYTE*)dataVal.getContent(), dataVal.getLength(), pOutput, outputLen);
	if (rv != CKR_OK)
	{
		delete pOutput;
		error(rv);
		return false;
	}

	if (g_nLogLevel > 1)
		std::cout << "  -- Verifica completata: " << std::endl << "     " << output.toHexString() << std::endl;

	delete pOutput;

	return true;
}

bool digest(CK_SESSION_HANDLE hSession, CK_MECHANISM_TYPE mechanism)
{
	if (g_nLogLevel > 1)
		std::cout << "  -> Hash con algo " << mechanismMap[mechanism] << std::endl;

	UUCByteArray dataValHashed;

	CK_ULONG ulCount = 1;

	CK_MECHANISM pMechanism[] = { mechanism, NULL_PTR, 0 };
	BYTE* pOutput;
	CK_ULONG outputLen = 256;

	//    char* szHex =  "0959208F14CC999E619536D85FEC0E7488FA2E99624228629A79512FE61274D4333824294279941F57DF36BCCC89DDB459D6607E43209874F2B631BD541ECA4A3FDFA8893E6ED0E0D4508AE335E7C85D77EEDFCB548472E172A661CAE514FFF6359407EF0FD595BED3D0BED606CA18032C1286606BE57A09CED2DD41955BB8176243BD0363D055B42C3017E726E26F4A680AF3FCE685C0AFF73ABF4F78DF55E4FD304DC2F8C81F4D918A23A5DE79E2BF7637DFC064C92ED7EE50FA2F168773C3D541CEB7439327C8B7176FBA3CEE8ADDC125A1518A6BBD4E5BB2D13EB9FF884875175774D60EB34F4EE08E81B8C65DB0CBBB251619B54BCFDA8C243DE0D037BC";
	//
	//    UUCByteArray dataVal(szHex);

	const char* szToHash = "some text to hash";
	UUCByteArray dataVal((BYTE*)szToHash, strlen(szToHash));

	if (g_nLogLevel > 2)
		std::cout << "  -> Appone la Firma digitale : " << std::endl;

	CK_RV rv = g_pFuncList->C_DigestInit(hSession, pMechanism);
	if (rv != CKR_OK)
	{
		error(rv);
		return false;
	}

	rv = g_pFuncList->C_Digest(hSession, (BYTE*)dataVal.getContent(), dataVal.getLength(), NULL, &outputLen);
	if (rv != CKR_OK)
	{
		error(rv);
		return false;
	}

	pOutput = (BYTE*)malloc(outputLen);

	rv = g_pFuncList->C_Digest(hSession, (BYTE*)dataVal.getContent(), dataVal.getLength(), pOutput, &outputLen);
	if (rv != CKR_OK)
	{
		delete pOutput;
		error(rv);
		return false;
	}

	UUCByteArray output(pOutput, outputLen);

	if (g_nLogLevel > 2)
		std::cout << "  -- Hash calcolato: " << std::endl << "     " << output.toHexString() << std::endl;

	delete pOutput;

	return true;
}

bool encryptDecrypt(CK_SESSION_HANDLE hSession)
{
	if (g_nLogLevel > 1)
		std::cout << "  -> Cifra e decifra con chiave importata\n    - C_Encrypt\n    - C_Decrypt" << std::endl;

	UUCByteArray dataValHashed;
	UUCByteArray dataVal;

	CK_MECHANISM pMechanism[] = { CKM_RSA_PKCS, NULL_PTR, 0 };
	BYTE pOutput[128];
	CK_ULONG outputLen = 128;

	CK_OBJECT_HANDLE hObjectPriKey;
	CK_OBJECT_HANDLE hObjectPubKey;
	CK_ULONG ulCount = 1;

	CK_OBJECT_CLASS ckClassPri = CKO_PRIVATE_KEY;
	CK_OBJECT_CLASS ckClassPub = CKO_PUBLIC_KEY;

	CK_ATTRIBUTE template_cko_keyPri[] = {
		{CKA_CLASS, &ckClassPri, sizeof(ckClassPri)},
	};

	if (!findObject(hSession, template_cko_keyPri, 1, &hObjectPriKey, &ulCount))
	{
		std::cout << "  -> Operazione fallita" << std::endl;
		return false;
	}

	if (ulCount < 1)
	{
		std::cout << "  -> Oggetto chiave privata non trovato" << std::endl;
		return false;
	}


	showAttributes(hSession, hObjectPriKey);

	CK_ATTRIBUTE template_cko_keyPub[] = {
		{CKA_CLASS, &ckClassPub, sizeof(ckClassPub)},
	};


	if (!findObject(hSession, template_cko_keyPub, 1, &hObjectPubKey, &ulCount))
	{
		std::cout << "  -> Operazione fallita" << std::endl;
		return false;
	}

	if (ulCount < 1)
	{
		std::cout << "  -> Oggetto chiave publica non trovato" << std::endl;
		return false;
	}

	showAttributes(hSession, hObjectPubKey);

	if (g_nLogLevel > 2)
		std::cout << "  -> Cifra un testo: " << std::endl;

	const char* szToEncrypt = "some text to encrypt";
	dataVal.append((BYTE*)szToEncrypt, (int)strlen(szToEncrypt));

	CK_RV rv = g_pFuncList->C_EncryptInit(hSession, pMechanism, hObjectPubKey);
	if (rv != CKR_OK)
	{
		error(rv);
		return false;
	}

	rv = g_pFuncList->C_Encrypt(hSession, (BYTE*)dataVal.getContent(), dataVal.getLength(), pOutput, &outputLen);
	if (rv != CKR_OK)
	{
		error(rv);
		return false;
	}

	UUCByteArray output(pOutput, outputLen);

	if (g_nLogLevel > 2)
		std::cout << "  -- Cifratura completata: " << std::endl << "     " << output.toHexString() << std::endl;

	if (g_nLogLevel > 2)
		std::cout << "  -> Decifra: " << std::endl;

	rv = g_pFuncList->C_DecryptInit(hSession, pMechanism, hObjectPriKey);
	if (rv != CKR_OK)
	{
		error(rv);
		return false;
	}

	BYTE pData[128];
	CK_ULONG ulDataLen = 128;
	rv = g_pFuncList->C_Decrypt(hSession, pOutput, outputLen, pData, &ulDataLen);
	if (rv != CKR_OK)
	{
		error(rv);
		return false;
	}

	UUCByteArray data(pData, ulDataLen);

	if (g_nLogLevel > 1)
		std::cout << "  -- Decifratura completata: " << std::endl << "     " << dataVal.toHexString() << " == " << data.toHexString() << std::endl;

	return true;
}

int main(int argc, char* argv[])
{
	std::cout << "----------------------------------------------" << std::endl;
	std::cout << "- Benvenuti nella Console di test del PKCS#11" << std::endl;
	std::cout << "- Copyright (c) 2006-2019 by Ugo Chirico\n- http://www.ugochirico.com\n- All right reserved" << std::endl;
	std::cout << "----------------------------------------------" << std::endl;

	g_nLogLevel = 5;

	mechanismMap[CKM_RSA_PKCS] = "CKM_RSA_PKCS";
	mechanismMap[CKM_SHA1_RSA_PKCS] = "CKM_SHA1_RSA_PKCS";
	mechanismMap[CKM_SHA256_RSA_PKCS] = "CKM_SHA256_RSA_PKCS";
	mechanismMap[CKM_MD5_RSA_PKCS] = "CKM_MD5_RSA_PKCS";	
	mechanismMap[CKM_MD5] = "CKM_MD5";
	mechanismMap[CKM_SHA_1] = "CKM_SHA1";
	mechanismMap[CKM_SHA256] = "CKM_SHA256";

	
#ifdef WIN32
	LPCWSTR szCryptoki = L"ciepki.dll";
	std::cout << "Load Module " << szCryptoki << std::endl;
	HMODULE hModule = LoadLibrary(szCryptoki);
	if (!hModule)
	{
		std::cout << "  -> Modulo " << szCryptoki << " non trovato" << std::endl;
		exit(1);
	}
#else

	const char* szCryptoki = "libcie-pkcs11.dylib";
	std::cout << "Load Module " << szCryptoki << std::endl;

	void* hModule = dlopen(szCryptoki, RTLD_LOCAL | RTLD_LAZY);
	if (!hModule)
	{
		std::cout << "  -> Modulo " << szCryptoki << " non trovato" << std::endl;
		exit(1);
	}
#endif

#ifdef WIN32
	C_GETFUNCTIONLIST pfnGetFunctionList = (C_GETFUNCTIONLIST)GetProcAddress(hModule, "C_GetFunctionList");
#else
	C_GETFUNCTIONLIST pfnGetFunctionList = (C_GETFUNCTIONLIST)dlsym(hModule, "C_GetFunctionList");
#endif
	if (!pfnGetFunctionList)
	{
#ifdef WIN32
		FreeLibrary(hModule);
#else
		dlclose(hModule);
#endif
		std::cout << "  -> Funzione C_GetFunctionList non trovata" << std::endl;
		exit(1);
	}

	// legge la lista delle funzioni
	if (g_nLogLevel > 2)
		std::cout << "  -> Chiede la lista delle funzioni esportate\n    - C_GetFunctionList" << std::endl;

	CK_RV rv = pfnGetFunctionList(&g_pFuncList);
	if (rv != CKR_OK)
	{
#ifdef WIN32
		FreeLibrary(hModule);
#else
		dlclose(hModule);
#endif
		std::cout << "  -> Funzione C_GetFunctionList ritorna errore " << rv << std::endl;
		exit(1);
	}

	if (g_nLogLevel > 2)
		std::cout << "  -- Richiesta completata " << std::endl;

	std::string sCommandLine;
	char* szCmd;
	bool bEnd = false;

	while (!bEnd)
	{
		if (argc > 1 && strlen(argv[1]) <= 2)
		{
			szCmd = argv[1];
			bEnd = true;
		}
		else
		{
			std::cout << "\nTest numbers:" << std::endl;
			std::cout << "1 Init and Finalize" << std::endl;
			std::cout << "2 Read slot list" << std::endl;
			std::cout << "3 Read slot and inserted token with token info" << std::endl;
			std::cout << "4 Open Session" << std::endl;
			std::cout << "5 Find objects" << std::endl;
			std::cout << "6 WaitForSlotEvent" << std::endl;
			std::cout << "7 Read Mechanism List" << std::endl;
			std::cout << "8 Sign + Verify RSA PKCS" << std::endl;
			std::cout << "9 Sign + Verify SHA1 RSA PKCS" << std::endl;
			std::cout << "10 Sign + Verify SHA256 RSA PKCS" << std::endl;
			std::cout << "11 Sign + Verify MD5 RSA PKCS" << std::endl;
			std::cout << "12 Hash SHA1" << std::endl;
			std::cout << "13 Hash SHA256" << std::endl;
			std::cout << "14 Hash MD5" << std::endl;
			//            std::cout << "8 Encrypt + Decrypt" << std::endl;
			std::cout << "20 Exit" << std::endl;
			std::cout << "Insert the test number:" << std::endl;
			std::cin >> sCommandLine;

			if (sCommandLine.length() <= 2)
				szCmd = (char*)sCommandLine.c_str();
			else
				szCmd = (char*)"";
		}

		//        long starttime = GetTickCount();
		std::cout << "---------------------------------------------------------" << std::endl;

		if (strcmp(szCmd, "?") == 0)
		{
			std::cout << "Uso:" << std::endl;
			std::cout << "testsimp11 <testnumber> " << std::endl;
			std::cout << "dove <testnumber> indica il numero del test come specificato nel documento di test" << std::endl;
			bEnd = true;
		}
		else if (strcmp(szCmd, "1") == 0)
		{
			std::cout << "-> Test 01 inizializzazione e chiusura libreria" << std::endl;
			init();
			close();
			std::cout << "-> Test 01 concluso" << std::endl;
		}
		else if (strcmp(szCmd, "2") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 02 Lettura slot list" << std::endl;
			init();
			CK_SLOT_ID_PTR pSlotList = getSlotList(false, &ulCount);
			if (pSlotList != NULL_PTR)
			{
				free(pSlotList);
			}
			close();
			std::cout << "-> Test 02 concluso" << std::endl;
		}
		else if (strcmp(szCmd, "3") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 03 Lettura slot con token inserito e info token" << std::endl;
			init();
			CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
			if (pSlotList != NULL_PTR)
			{
				for (CK_ULONG i = 0; i < ulCount; i++)
				{
					getTokenInfo(pSlotList[i]);
				}

				free(pSlotList);
			}
			close();
			std::cout << "-> Test 03 concluso" << std::endl;

		}
		else if (strcmp(szCmd, "4") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 04 Apertura sessione" << std::endl;
			init();
			CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
			if (pSlotList != NULL_PTR)
			{
				CK_SESSION_HANDLE hSession = openSession(pSlotList[0]);
				free(pSlotList);
				closeSession(hSession);
			}

			close();
			std::cout << "-> Test 04 concluso" << std::endl;

		}
		else if (strcmp(szCmd, "5") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 05 Ricerca oggetti" << std::endl;
			init();
			CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
			if (pSlotList == NULL_PTR)
			{
				close();
				std::cout << "-> Test 05 non completato" << std::endl;
				continue;
			}

			CK_SESSION_HANDLE hSession = openSession(pSlotList[0]);
			if (hSession == NULL_PTR)
			{
				free(pSlotList);
				close();
				std::cout << "-> Test 05 non completato" << std::endl;
				continue;
			}

			if (!login(hSession))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test 05 non completato" << std::endl;
				continue;
			}

			CK_OBJECT_HANDLE phObject[200];
			CK_ULONG ulObjCount = 200;

			CK_OBJECT_CLASS ckClass = CKO_PRIVATE_KEY;

			CK_ATTRIBUTE template_ck[] = {
				{CKA_CLASS, &ckClass, sizeof(ckClass)} };

			std::cout << "  - Chiavi private " << std::endl;

			if (findObject(hSession, template_ck, 1, phObject, &ulObjCount))
			{
				for (CK_ULONG i = 0; i < ulObjCount; i++)
				{
					showAttributes(hSession, phObject[i]);
				}
			}

			ckClass = CKO_PUBLIC_KEY;
			ulObjCount = 200;

			std::cout << "  - Chiavi pubbliche " << std::endl;

			if (findObject(hSession, template_ck, 1, phObject, &ulObjCount))
			{
				for (CK_ULONG i = 0; i < ulObjCount; i++)
				{
					showAttributes(hSession, phObject[i]);
				}
			}

			ckClass = CKO_CERTIFICATE;
			ulObjCount = 200;

			std::cout << "  - Certificati " << std::endl;

			if (findObject(hSession, template_ck, 1, phObject, &ulObjCount))
			{
				for (CK_ULONG i = 0; i < ulObjCount; i++)
				{
					showCertAttributes(hSession, phObject[i]);
				}
			}

			ckClass = CKO_DATA;
			ulObjCount = 200;

			std::cout << "  - Data " << std::endl;

			if (findObject(hSession, template_ck, 1, phObject, &ulObjCount))
			{
				for (CK_ULONG i = 0; i < ulObjCount; i++)
				{
					showAttributes(hSession, phObject[i]);
				}
			}

			if (!logout(hSession))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test 05 non completato" << std::endl;
				continue;
			}

			free(pSlotList);
			closeSession(hSession);

			close();
			std::cout << "-> Test 05 concluso" << std::endl;
		}
		else if (strcmp(szCmd, "6") == 0)
		{
			std::cout << "-> Test 06 WaitForSlotEvent" << std::endl;
			init();
			CK_SLOT_ID pSlotList[1];
			std::cout << "-> Attende l'inserimento della carta" << std::endl;
			CK_RV rv = g_pFuncList->C_WaitForSlotEvent(0, pSlotList, 0);
			getSlotInfo(pSlotList[0]);
			std::cout << "-- Carta Inserita" << std::endl;

			std::cout << "-> Attende l'estrazione della carta" << std::endl;
			rv = g_pFuncList->C_WaitForSlotEvent(0, pSlotList, 0);
			getSlotInfo(pSlotList[0]);
			std::cout << "-- Carta Estratta" << std::endl;

			close();
			std::cout << "-> Test 06 concluso" << std::endl;
		}
		else if (strcmp(szCmd, "7") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 7 Read Mechanism List" << std::endl;
			init();

			CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
			if (pSlotList == NULL_PTR)
			{
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			mechanismList(pSlotList[0]);

			close();

			std::cout << "-> Test 07 concluso" << std::endl;
		}
		else if (strcmp(szCmd, "8") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 8 Sign + Verify By RSA_PKCS" << std::endl;
			init();

			CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
			if (pSlotList == NULL_PTR)
			{
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			CK_SESSION_HANDLE hSession = openSession(pSlotList[0]);
			if (hSession == NULL_PTR)
			{
				free(pSlotList);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!login(hSession))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!signVerify(hSession, CKM_RSA_PKCS))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!logout(hSession))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			free(pSlotList);
			closeSession(hSession);

			close();
			std::cout << "-> Test concluso" << std::endl;
		}
		else if (strcmp(szCmd, "9") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 9 Sign + Verify By RSA_SHA1_PKCS" << std::endl;
			init();

			CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
			if (pSlotList == NULL_PTR)
			{
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			CK_SESSION_HANDLE hSession = openSession(pSlotList[0]);
			if (hSession == NULL_PTR)
			{
				free(pSlotList);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!login(hSession))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!signVerify(hSession, CKM_SHA1_RSA_PKCS))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!logout(hSession))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			free(pSlotList);
			closeSession(hSession);

			close();
			std::cout << "-> Test concluso" << std::endl;
		}
		else if (strcmp(szCmd, "10") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 10 Sign + Verify By RSA_SHA256_PKCS" << std::endl;
			init();

			CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
			if (pSlotList == NULL_PTR)
			{
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			CK_SESSION_HANDLE hSession = openSession(pSlotList[0]);
			if (hSession == NULL_PTR)
			{
				free(pSlotList);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!login(hSession))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!signVerify(hSession, CKM_SHA256_RSA_PKCS))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!logout(hSession))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			free(pSlotList);
			closeSession(hSession);

			close();
			std::cout << "-> Test concluso" << std::endl;
		}
		else if (strcmp(szCmd, "11") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 11 Sign + Verify By RSA_MD5_PKCS" << std::endl;
			init();

			CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
			if (pSlotList == NULL_PTR)
			{
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			CK_SESSION_HANDLE hSession = openSession(pSlotList[0]);
			if (hSession == NULL_PTR)
			{
				free(pSlotList);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!login(hSession))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!signVerify(hSession, CKM_MD5_RSA_PKCS))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!logout(hSession))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			free(pSlotList);
			closeSession(hSession);

			close();
			std::cout << "-> Test concluso" << std::endl;
		}
		else if (strcmp(szCmd, "12") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 12 Hash SHA1" << std::endl;
			init();

			CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
			if (pSlotList == NULL_PTR)
			{
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			CK_SESSION_HANDLE hSession = openSession(pSlotList[0]);
			if (hSession == NULL_PTR)
			{
				free(pSlotList);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!digest(hSession, CKM_SHA_1))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			free(pSlotList);
			closeSession(hSession);

			close();
			std::cout << "-> Test concluso" << std::endl;
		}
		else if (strcmp(szCmd, "13") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 13 Hash SHA256" << std::endl;
			init();

			CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
			if (pSlotList == NULL_PTR)
			{
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			CK_SESSION_HANDLE hSession = openSession(pSlotList[0]);
			if (hSession == NULL_PTR)
			{
				free(pSlotList);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!digest(hSession, CKM_SHA256))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			free(pSlotList);
			closeSession(hSession);

			close();
			std::cout << "-> Test concluso" << std::endl;
		}
		else if (strcmp(szCmd, "14") == 0)
		{
			CK_ULONG ulCount = 0;
			std::cout << "-> Test 14 Hash SHA256" << std::endl;
			init();

			CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
			if (pSlotList == NULL_PTR)
			{
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			CK_SESSION_HANDLE hSession = openSession(pSlotList[0]);
			if (hSession == NULL_PTR)
			{
				free(pSlotList);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			if (!digest(hSession, CKM_MD5))
			{
				free(pSlotList);
				closeSession(hSession);
				close();
				std::cout << "-> Test non completato" << std::endl;
				continue;
			}

			free(pSlotList);
			closeSession(hSession);

			close();
			std::cout << "-> Test concluso" << std::endl;
		}

		//        else if(strcmp(szCmd, "8") == 0)
		//        {
		//            CK_ULONG ulCount = 0;
		//            std::cout << "-> Test 8 Encrypt + Decrypt By CIE Key Pair" << std::endl;
		//            init();
		//
		//            CK_SLOT_ID_PTR pSlotList = getSlotList(true, &ulCount);
		//            if(pSlotList == NULL_PTR)
		//            {
		//                close();
		//                std::cout << "-> Test non completato" << std::endl;
		//                continue;
		//            }
		//
		//            CK_SESSION_HANDLE hSession = openSession(pSlotList[0]);
		//            if(hSession == NULL_PTR)
		//            {
		//                free(pSlotList);
		//                close();
		//                std::cout << "-> Test non completato" << std::endl;
		//                continue;
		//            }
		//
		//            if(!login(hSession))
		//            {
		//                free(pSlotList);
		//                closeSession(hSession);
		//                close();
		//                std::cout << "-> Test non completato" << std::endl;
		//                continue;
		//            }
		//
		//            if(!encryptDecrypt(hSession))
		//            {
		//                free(pSlotList);
		//                closeSession(hSession);
		//                close();
		//                std::cout << "-> Test non completato" << std::endl;
		//                continue;
		//            }
		//
		//            if(!logout(hSession))
		//            {
		//                free(pSlotList);
		//                closeSession(hSession);
		//                close();
		//                std::cout << "-> Test non completato" << std::endl;
		//                continue;
		//            }
		//
		//            free(pSlotList);
		//            closeSession(hSession);
		//
		//            close();
		//            std::cout << "-> Test concluso" << std::endl;
		//        }
		else if (strcmp(szCmd, "20") == 0)
		{
			bEnd = true;
		}

		//        long endtime = GetTickCount();

		std::cout << "*************************" << std::endl;
		//        std::cout << "-> Time (in ms) " << endtime - starttime << std::endl;
		std::cout << "*************************" << std::endl;
		std::cout << "---------------------------------------------------------" << std::endl;
	}

#ifdef WIN32
	FreeLibrary(hModule);
#else
	dlclose(hModule);
#endif

	return 0;
}
