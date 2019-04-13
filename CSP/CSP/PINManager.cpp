//
//  PINManager.cpp
//  cie-pkcs11
//
//  Created by ugo chirico on 06/10/18.
//  Copyright © 2018 IPZS. All rights reserved.
//

#include "PINManager.h"
#include "IAS.h"
#include "../PKCS11/PKCS11Functions.h"
#include "../PKCS11/Slot.h"
//#include "CSP.h"
#include "../Util/ModuleInfo.h"
#include "../Crypto/sha256.h"
#include <functional>
#include "../Crypto/ASNParser.h"
#include "../PCSC/PCSC.h"
#include <string>
#include <string>

extern "C" {
    CK_RV CK_ENTRY __stdcall ChangePIN(const char*  szCurrentPIN, const char*  szNuovoPIN, int* pAttempts, PROGRESS_CALLBACK progressCallBack);
    CK_RV CK_ENTRY __stdcall UnlockPIN(const char*  szPUK, const char*  szNuovoPIN, int* pAttempts, PROGRESS_CALLBACK progressCallBack);
}

int TokenTransmitCallback(safeConnection *data, uint8_t *apdu, DWORD apduSize, uint8_t *resp, DWORD *respSize);


CK_RV CK_ENTRY __stdcall ChangePIN(const char*  szCurrentPIN, const char*  szNewPIN, int* pAttempts, PROGRESS_CALLBACK progressCallBack)
{
    char* readers = NULL;
    char* ATR = NULL;
    
    try
    {
        DWORD len = 0;
        
        SCARDCONTEXT hSC;
        
        progressCallBack(10, "Connessione alla CIE");
        
        long nRet = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &hSC);
        if(nRet != SCARD_S_SUCCESS)
            return CKR_DEVICE_ERROR;
        
        if (SCardListReaders(hSC, nullptr, NULL, &len) != SCARD_S_SUCCESS) {
            return CKR_TOKEN_NOT_PRESENT;
        }
        
        if(len == 1)
            return CKR_TOKEN_NOT_PRESENT;
        
        readers = (char*)malloc(len);
        
        if (SCardListReaders(hSC, nullptr, (char*)readers, &len) != SCARD_S_SUCCESS) {
            free(readers);
            return CKR_TOKEN_NOT_PRESENT;
        }
        
        progressCallBack(10, "CIE Connessa");
        
        char *curreader = readers;
        bool foundCIE = false;
        
        for (; curreader[0] != 0; curreader += strnlen(curreader, len) + 1)
        {
            safeConnection conn(hSC, curreader, SCARD_SHARE_SHARED);
            if (!conn.hCard)
                continue;
            
            DWORD atrLen = 40;
            if(SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR, &atrLen) != SCARD_S_SUCCESS) {
                free(readers);
                return CKR_DEVICE_ERROR;
            }
            
            ATR = (char*)malloc(atrLen);
            
            if(SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR, &atrLen) != SCARD_S_SUCCESS) {
                free(readers);
                free(ATR);
                return CKR_DEVICE_ERROR;
            }
            
            ByteArray atrBa((BYTE*)ATR, atrLen);
            
            IAS ias((CToken::TokenTransmitCallback)TokenTransmitCallback, atrBa);
            ias.SetCardContext(&conn);
            ias.attemptsRemaining = -1;
            
            ias.token.Reset();
            ias.SelectAID_IAS();
            ias.ReadPAN();
            
            progressCallBack(20, "Lettura dati dalla CIE");
            
            ByteDynArray resp;
            ias.SelectAID_CIE();
            
            ias.InitEncKey();
            ias.ReadDappPubKey(resp);
            
            foundCIE = true;
            
            // leggo i parametri di dominio DH e della chiave di extauth
            ias.InitDHParam();
            
            ias.InitExtAuthKeyParam();
            
            progressCallBack(40, "Autenticazione...");
            
            ias.DHKeyExchange();
            
            // DAPP
            ias.DAPP();
            
            progressCallBack(80, "Cambio PIN...");
            
            ByteArray oldPINBa((BYTE*)szCurrentPIN, strlen(szCurrentPIN));
            
            StatusWord sw = ias.VerifyPIN(oldPINBa);
            
            if (sw == 0x6983) {
                free(readers);
                free(ATR);
                return CKR_PIN_LOCKED;
            }
            if (sw >= 0x63C0 && sw <= 0x63CF) {
                if (pAttempts!=nullptr)
                    *pAttempts = sw - 0x63C0;
                
                free(readers);
                free(ATR);
                return CKR_PIN_INCORRECT;
            }
            
            if (sw == 0x6700) {
                free(readers);
                free(ATR);
                return CKR_PIN_INCORRECT;
            }
            if (sw == 0x6300)
            {
                free(readers);
                free(ATR);
                return CKR_PIN_INCORRECT;
            }
            if (sw != 0x9000) {
                throw scard_error(sw);
            }
            
            ByteDynArray cert;
            bool isEnrolled = ias.IsEnrolled();
            
            if(isEnrolled)
                ias.GetCertificate(cert);
            
            
            ByteArray newPINBa((BYTE*)szNewPIN, strlen(szNewPIN));
            
            sw = ias.ChangePIN(oldPINBa, newPINBa);
            if (sw != 0x9000) {
                throw scard_error(sw);
            }
            
            if(isEnrolled)
            {
                std::string strPAN;
                dumpHexData(ias.PAN.mid(5,6), strPAN, false);
                ByteArray leftPINBa = newPINBa.left(4);
                ias.SetCache(strPAN.c_str(), cert,     leftPINBa);
            }
            
            progressCallBack(100, "Cambio PIN eseguito");
        }
        
        if (!foundCIE) {
            free(readers);
            free(ATR);
            return CKR_TOKEN_NOT_RECOGNIZED;
            
        }
    }
    catch(...)
    {
        if(readers)
            free(readers);
        if(ATR)
            free(ATR);
        return CKR_GENERAL_ERROR;
    }
    
    if(readers)
        free(readers);
    if(ATR)
        free(ATR);
    
    return CKR_OK;
}


CK_RV CK_ENTRY __stdcall UnlockPIN(const char*  szPUK, const char*  szNewPIN, int* pAttempts, PROGRESS_CALLBACK progressCallBack)
{
    char* readers = NULL;
    char* ATR = NULL;
    
    try
    {
        DWORD len = 0;
        
        SCARDCONTEXT hSC;
        
        progressCallBack(10, "Connessione alla CIE");
        
        long nRet = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &hSC);
        if(nRet != SCARD_S_SUCCESS)
            return CKR_DEVICE_ERROR;
        
        if (SCardListReaders(hSC, nullptr, NULL, &len) != SCARD_S_SUCCESS) {
            return CKR_TOKEN_NOT_PRESENT;
        }
        
        if(len == 1)
            return CKR_TOKEN_NOT_PRESENT;
        
        readers = (char*)malloc(len);
        
        if (SCardListReaders(hSC, nullptr, (char*)readers, &len) != SCARD_S_SUCCESS) {
            free(readers);
            return CKR_TOKEN_NOT_PRESENT;
        }
        
        progressCallBack(20, "CIE Connessa");
        
        char *curreader = readers;
        bool foundCIE = false;
       
        for (; curreader[0] != 0; curreader += strnlen(curreader, len) + 1)
        {
            safeConnection conn(hSC, curreader, SCARD_SHARE_SHARED);
            if (!conn.hCard)
                continue;
            
            DWORD atrLen = 40;
            if(SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR, &atrLen) != SCARD_S_SUCCESS) {
                free(readers);
                return CKR_DEVICE_ERROR;
            }
            
            ATR = (char*)malloc(atrLen);
            
            if(SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR, &atrLen) != SCARD_S_SUCCESS) {
                free(readers);
                free(ATR);
                return CKR_DEVICE_ERROR;
            }
            
            ByteArray atrBa((BYTE*)ATR, atrLen);
            
            IAS ias((CToken::TokenTransmitCallback)TokenTransmitCallback, atrBa);
            ias.SetCardContext(&conn);
            ias.attemptsRemaining = -1;
            
            ias.token.Reset();
            ias.SelectAID_IAS();
            ias.ReadPAN();
            
            progressCallBack(30, "Lettura dati dalla CIE");
            
            ByteDynArray resp;
            ias.SelectAID_CIE();
        
            ias.InitEncKey();
            ias.ReadDappPubKey(resp);
            
            foundCIE = true;
    
            // leggo i parametri di dominio DH e della chiave di extauth
            ias.InitDHParam();
            
            ias.InitExtAuthKeyParam();
            
            progressCallBack(50, "Autenticazione...");
            
            ias.DHKeyExchange();
            
            // DAPP
            ias.DAPP();
            
            progressCallBack(80, "Sblocco carta...");
            
            ByteArray pukBa((BYTE*)szPUK, strlen(szPUK));
            
            StatusWord sw = ias.VerifyPUK(pukBa);
            
            if (sw == 0x6983) {
                free(ATR);
                free(readers);
                return CKR_PIN_LOCKED;
            }
            if (sw >= 0x63C0 && sw <= 0x63CF) {
                free(ATR);
                free(readers);
                if (pAttempts!=nullptr)
                    *pAttempts = sw - 0x63C0;
                
                return CKR_PIN_INCORRECT;
            }
            
            if (sw == 0x6700) {
                free(ATR);
                free(readers);
                return CKR_PIN_INCORRECT;
            }
            if (sw == 0x6300) {
                free(ATR);
                free(readers);
                return CKR_PIN_INCORRECT;
            }
            if (sw != 0x9000) {
                
                throw scard_error(sw);
            }
            
            ByteDynArray cert;
            bool isEnrolled = ias.IsEnrolled();
            
            if(isEnrolled)
                ias.GetCertificate(cert);
            
            
            ByteArray newPINBa((BYTE*)szNewPIN, strlen(szNewPIN));
            
            sw = ias.ChangePIN(newPINBa);
            if (sw != 0x9000) {
                throw scard_error(sw);
            }
            
            if(isEnrolled)
            {
                std::string strPAN;
                dumpHexData(ias.PAN.mid(5,6), strPAN, false);
                ByteArray leftPINBa = newPINBa.left(4);
                ias.SetCache(strPAN.c_str(), cert,     leftPINBa);
            }
            
            progressCallBack(100, "Sblocco carta eseguito");
        }
        
        if (!foundCIE) {
            free(ATR);
            free(readers);
            return CKR_TOKEN_NOT_RECOGNIZED;            
        }
    }
    catch(...)
    {
        if(ATR)
            free(ATR);
        
        if(readers)
            free(readers);
        
        return CKR_GENERAL_ERROR;
    }
    
    if(ATR)
        free(ATR);
    
    if(readers)
        free(readers);
    
    return CKR_OK;
}
