#pragma once
#include "../PCSC/token.h"
#if defined(WIN32) || defined(_WIN32)
#include "../CSP/Cardmod.h"
#endif

#include "../CSP/ATR.h"
#include <map>
#include <string>
#include <vector>

#define DirCIE				"CIE"

#define EfDH				"EF.DH"
#define EfSerial			"EF.Serial"
#define EfIdServizi			"EF.IdServizi"
#define EfCertCIE			"EF.CertCIE"
#define EfSOD				"EF.SOD"
#define EfIntAuth			"EF.IntAuth"
#define EfIntAuthServizi	"EF.IntAuthServizi"

#define FULL_PIN 0x80000000

#if defined(WIN32) || defined(_WIN32)
extern bool switchDesktop;
extern BOOL CheckOneInstance(char *nome);
#endif
extern ByteArray baExtAuth_PrivExp;


enum CIE_DF {
	DF_Root,
	DF_IAS,
	DF_CIE
};

enum CIE_RequestedSM {
	CIE_SM,
	CIE_NoSM,
	CIE_AnySM
};



class IAS
{
	CIE_Type type = CIE_Type::CIE_Unknown;
	ByteDynArray dh_g,dh_p,dh_q;
	ByteDynArray sessENC, sessMAC, sessSSC;
	ByteDynArray dh_pubKey, dh_ICCpubKey;
	ByteDynArray CA_module, CA_pubexp, CA_privexp, CA_CHR, CA_CHA, CA_CAR, CA_AID;
	ByteDynArray IAS_AID;
	ByteDynArray CIE_AID;
	ByteDynArray ATR;
	ByteDynArray Certificate;
	ByteDynArray CardEncKey, CardEncIv;
	StatusWord SendAPDU(const ByteArray &head, const ByteArray &data, ByteDynArray &resp, uint8_t *le = NULL);
	StatusWord SendAPDU_SM(const ByteArray &head, const ByteArray &data, ByteDynArray &resp, uint8_t *le = NULL);
	StatusWord getResp(ByteDynArray &Cardresp, StatusWord sw, ByteDynArray &resp);
	StatusWord getResp_SM(const ByteArray &Cardresp, StatusWord sw, ByteDynArray &resp);

	ByteDynArray SM(const ByteArray &keyEnc, const ByteArray &keySig, const ByteArray &apdu, ByteArray &seq);
	StatusWord respSM(const ByteArray &keyEnc, const ByteArray &keySig, const ByteArray &apdu, ByteArray &seq, ByteDynArray &elabResp);

	void readfile_SM(uint16_t id, ByteDynArray &content);
	void readfile(uint16_t id, ByteDynArray &content);

	void increment(const ByteArray &seq);
	void ReadCIEType();

	bool IsImpersonationRunning();
public:
	CToken token;
	
	IAS(CToken::TokenTransmitCallback transmit,ByteArray ATR);
	~IAS();

	void SetCardContext(void *);
	void SelectAID_IAS(bool SM = false);
	void SelectAID_CIE(bool SM = false);

	ByteDynArray PAN;
	ByteDynArray DappModule;
	ByteDynArray DappPubKey;

	void ReadPAN();
	void ReadSOD(ByteDynArray &data);

	void ReadDH(ByteDynArray &data);
	void ReadCertCIE(ByteDynArray &data);
	void ReadDappPubKey(ByteDynArray &data);
	void ReadServiziPubKey(ByteDynArray &data);
	void ReadSerialeCIE(ByteDynArray &data);
	void ReadIdServizi(ByteDynArray &data);

	void InitEncKey();
	void InitDHParam();
	void InitExtAuthKeyParam();
	void DHKeyExchange();
	void DAPP();
	StatusWord VerifyPIN(const ByteArray &PIN);
	StatusWord VerifyPUK(const ByteArray &PUK);
	StatusWord UnblockPIN();
	StatusWord ChangePIN(const ByteArray &oldPIN, const ByteArray &newPIN);
	StatusWord ChangePIN(const ByteArray &newPIN);
	void Sign(const ByteArray &data, ByteDynArray &signedData);
	void Deauthenticate();
	void GetCertificate(ByteDynArray &certificate, bool askEnable = true);
	void GetFirstPIN(ByteDynArray &PIN);
	void SetCache(const char *PAN, const ByteArray &certificate, const ByteArray &FirstPIN);
	bool IsEnrolled();
	bool Unenroll();
	static bool IsEnrolled(const char *szPAN);
	static bool Unenroll(const char *szPAN);
	void IconaSbloccoPIN();
	void IconaCertificatoScaduto(const char *seriale);

	uint8_t GetSODDigestAlg(const ByteArray &SOD);
	void VerificaSOD(const ByteArray &SOD, std::map<uint8_t, ByteDynArray> &hashSet);
	void VerificaSODPSS(const ByteArray &SOD, std::map<uint8_t, ByteDynArray> &hashSet);

	void(*Callback)(int progress, char *desc,void *data);
	void* CallbackData;

	// usato da CardUnblockPin per comunicare i tentativi di verifica del PUK rimasti
	int attemptsRemaining;

	bool ActiveSM;
	CIE_DF ActiveDF;

	
};

