#pragma once
#include "../PCSC/token.h"
#include "../CSP/Cardmod.h"
#include <map>

#define DirCIE				"CIE"

#define EfDH				"EF.DH"
#define EfSerial			"EF.Serial"
#define EfIdServizi			"EF.IdServizi"
#define EfCertCIE			"EF.CertCIE"
#define EfSOD				"EF.SOD"
#define EfIntAuth			"EF.IntAuth"
#define EfIntAuthServizi	"EF.IntAuthServizi"

#define FULL_PIN 0x80000000

extern bool switchDesktop;
extern BOOL CheckOneInstance(char *nome);
extern ByteArray baExtAuth_PrivExp;

enum CIE_Type {
	CIE_Unknown,
	CIE_Gemalto,
	CIE_NXP,
	CIE_STM,
	CIE_STM2
};

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
	StatusWord SendAPDU(ByteArray &head, ByteArray &data, ByteDynArray &resp, uint8_t *le = NULL);
	StatusWord SendAPDU_SM(ByteArray &head, ByteArray &data, ByteDynArray &resp, uint8_t *le = NULL);
	StatusWord getResp(ByteDynArray &Cardresp, StatusWord sw, ByteDynArray &resp);
	StatusWord getResp_SM(ByteArray &Cardresp, StatusWord sw, ByteDynArray &resp);

	ByteDynArray SM(ByteArray &keyEnc, ByteArray &keySig, ByteArray &apdu, ByteArray &seq);
	StatusWord respSM(ByteArray &keyEnc, ByteArray &keySig, ByteArray &apdu, ByteArray &seq, ByteDynArray &elabResp);

	void readfile_SM(uint16_t id, ByteDynArray &content);
	void readfile(uint16_t id, ByteDynArray &content);

	void increment(ByteArray &seq);
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
	StatusWord VerifyPIN(ByteArray &PIN);
	StatusWord VerifyPUK(ByteArray &PUK);
	StatusWord UnblockPIN();
	StatusWord ChangePIN(ByteArray &oldPIN, ByteArray &newPIN);
	StatusWord ChangePIN(ByteArray &newPIN);
	void Sign(ByteArray &data, ByteDynArray &signedData);
	void Deauthenticate();
	void GetCertificate(ByteDynArray &certificate, bool askEnable = true);
	void GetFirstPIN(ByteDynArray &PIN);
	void SetCache(const char *PAN, ByteArray &certificate, ByteArray &FirstPIN);
	bool IsEnrolled();
	bool Unenroll();
	static bool IsEnrolled(const char *szPAN);
	static bool Unenroll(const char *szPAN);
	void IconaSbloccoPIN();
	void IconaCertificatoScaduto(const char *seriale);

	uint8_t GetSODDigestAlg(ByteArray &SOD);
	void VerificaSOD(ByteArray &SOD, std::map<uint8_t, ByteDynArray> &hashSet);
	void VerificaSODPSS(ByteArray &SOD, std::map<uint8_t, ByteDynArray> &hashSet);

	void(*Callback)(int progress, char *desc,void *data);
	void* CallbackData;

	// usato da CardUnblockPin per comunicare i tentativi di verifica del PUK rimasti
	int attemptsRemaining;

	bool ActiveSM;
	CIE_DF ActiveDF;

	
};

