#include "FirmaConCIE.h"
#include "cryptoki.h"
#include "IAS.h"
#include "PCSC.h"
#include "CIESign.h"
#include <functional>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <exception>

#define CARD_PAN_MISMATCH (CK_RV)(0x000000F1)

#ifndef SCARD_ATTR_ATR_STRING
#define SCARD_ATTR_ATR_STRING 0x00090303
#endif

#ifndef CK_ENTRY
#define CK_ENTRY __attribute__((visibility("default")))
#endif

static int TokenTransmitCallback(safeConnection *data, uint8_t *apdu, DWORD apduSize, uint8_t *resp, DWORD *respSize)
{
    if (apduSize == 2) {
        WORD code = *(WORD*)apdu;
        if (code == 0xfffd) {
            *respSize = sizeof(data->hCard) + 2;
            memcpy(resp, &data->hCard, sizeof(data->hCard));
            resp[sizeof(data->hCard)] = 0;
            resp[sizeof(data->hCard) + 1] = 0;
            return SCARD_S_SUCCESS;
        }
        if (code == 0xfffe) {
            DWORD protocol = 0;
            auto sw = SCardReconnect(data->hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_UNPOWER_CARD, &protocol);
            if (sw == SCARD_S_SUCCESS)
                SCardBeginTransaction(data->hCard);
            return 0x9000;
        } else if (code == 0xffff) {
            DWORD protocol = 0;
            auto sw = SCardReconnect(data->hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx, SCARD_RESET_CARD, &protocol);
            if (sw == SCARD_S_SUCCESS)
                SCardBeginTransaction(data->hCard);
            return 0x9000;
        }
    }
    return SCardTransmit(data->hCard, SCARD_PCI_T1, apdu, apduSize, NULL, resp, respSize);
}

static CK_RV withCIECard(PROGRESS_CALLBACK progressCallBack,
                         const std::function<CK_RV(IAS&, safeConnection&, ByteDynArray& idServizi)>& body)
{
    char* readers = nullptr;
    BYTE* atrBuf = nullptr;
    try {
        SCARDCONTEXT hSC;
        long nRet = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &hSC);
        if (nRet != SCARD_S_SUCCESS)
            return CKR_DEVICE_ERROR;

        DWORD len = 0;
        nRet = SCardListReaders(hSC, nullptr, nullptr, &len);
        if (nRet != SCARD_S_SUCCESS || len <= 1)
            return CKR_TOKEN_NOT_PRESENT;

        readers = (char*)malloc(len);
        if (!readers)
            return CKR_GENERAL_ERROR;

        if (SCardListReaders(hSC, nullptr, readers, &len) != SCARD_S_SUCCESS) {
            free(readers);
            return CKR_TOKEN_NOT_PRESENT;
        }

        bool foundCIE = false;
        CK_RV result = CKR_TOKEN_NOT_RECOGNIZED;

        for (char *curreader = readers; curreader[0] != 0; curreader += strnlen(curreader, len) + 1) {
            safeConnection conn(hSC, curreader, SCARD_SHARE_SHARED);
            if (!conn.hCard)
                continue;

            DWORD atrLen = 0;
            if (SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, nullptr, &atrLen) != SCARD_S_SUCCESS || atrLen == 0)
                continue;

            atrBuf = (BYTE*)malloc(atrLen);
            if (!atrBuf)
                continue;

            if (SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, atrBuf, &atrLen) != SCARD_S_SUCCESS) {
                free(atrBuf);
                atrBuf = nullptr;
                continue;
            }

            if (progressCallBack)
                progressCallBack(20, "Connessione alla CIE");

            ByteArray atrBa(atrBuf, atrLen);
            IAS ias((CToken::TokenTransmitCallback)TokenTransmitCallback, atrBa);
            ias.SetCardContext(&conn);

            ias.token.Reset();
            ias.SelectAID_IAS();
            ias.ReadPAN();

            ByteDynArray IntAuth;
            ias.SelectAID_CIE();
            ias.ReadDappPubKey(IntAuth);
            ias.SelectAID_CIE();
            ias.InitEncKey();

            ByteDynArray IdServizi;
            ias.ReadIdServizi(IdServizi);
            foundCIE = true;

            result = body(ias, conn, IdServizi);

            free(atrBuf);
            atrBuf = nullptr;
            free(readers);
            return result;
        }

        free(readers);
        return foundCIE ? result : CKR_TOKEN_NOT_RECOGNIZED;
    } catch (std::exception &ex) {
        printf("firmaConCIE exception: %s\n", ex.what());
        if (atrBuf) free(atrBuf);
        if (readers) free(readers);
        return CKR_GENERAL_ERROR;
    }
}

extern "C" CK_RV CK_ENTRY leggiPANCIE(char* outPan, unsigned int outLen)
{
    if (!outPan || outLen < 13)
        return CKR_GENERAL_ERROR;

    return withCIECard(nullptr, [&](IAS&, safeConnection&, ByteDynArray& idServizi) -> CK_RV {
        size_t n = idServizi.size();
        if (n + 1 > outLen)
            return CKR_GENERAL_ERROR;
        memcpy(outPan, idServizi.data(), n);
        outPan[n] = 0;
        return CKR_OK;
    });
}

extern "C" CK_RV CK_ENTRY firmaConCIE(const char* inFilePath, const char* type, const char* pin, const char* pan,
                                      int page, float x, float y, float w, float h,
                                      const char* imagePathFile, const char* outFilePath,
                                      PROGRESS_CALLBACK progressCallBack,
                                      SIGN_COMPLETED_CALLBACK completedCallBack)
{
    printf("****** Starting firmaConCIE ******\n");

    if (!inFilePath || !type || !pin || !outFilePath || !progressCallBack || !completedCallBack)
        return CKR_GENERAL_ERROR;

    return withCIECard(progressCallBack, [&](IAS& ias, safeConnection&, ByteDynArray& IdServizi) -> CK_RV {
        if (pan && pan[0] != 0) {
            ByteArray baPan((uint8_t*)pan, strlen(pan));
            if (IdServizi.size() != baPan.size() ||
                memcmp(baPan.data(), IdServizi.data(), IdServizi.size()) != 0) {
                return CARD_PAN_MISMATCH;
            }
        }

        ByteDynArray FullPIN;
        ByteArray LastPIN((uint8_t*)pin, strlen(pin));
        ias.GetFirstPIN(FullPIN);
        FullPIN.append(LastPIN);
        ias.token.Reset();

        progressCallBack(40, "Firma in corso");

        char fullPinCStr[9];
        if (FullPIN.size() < 8)
            return CKR_PIN_INVALID;
        memcpy(fullPinCStr, FullPIN.data(), 8);
        fullPinCStr[8] = 0;

        CIESign cieSign(&ias);
        uint16_t ret = cieSign.sign(inFilePath, type, fullPinCStr, page, x, y, w, h, imagePathFile, outFilePath);

        if ((ret & 0x63C0) == 0x63C0)
            return CKR_PIN_INCORRECT;
        if (ret == 0x6983)
            return CKR_PIN_LOCKED;
        if (ret != 0)
            return CKR_GENERAL_ERROR;

        progressCallBack(100, "Firma completata");
        completedCallBack(ret);
        return CKR_OK;
    });
}
