#pragma once
#include "token.h"
#include "Cardmod.h"
#include <map>

#define DirCIE				"CIE"

#define EfDH				"EF.DH"
#define EfSerial			"EF.Serial"
#define EfIdServizi			"EF.IdServizi"
#define EfCertCIE			"EF.CertCIE"
#define EfSOD				"EF.SOD"
#define EfIntAuth			"EF.IntAuth"
#define EfIntAuthServizi	"EF.IntAuthServizi"

extern bool switchDesktop;
extern BOOL CheckOneInstance(char *nome);

class IAS
{
	ByteDynArray dh_g,dh_p,dh_q;
	ByteDynArray sessENC, sessMAC, sessSSC;
	ByteDynArray dh_pubKey, dh_ICCpubKey;
	ByteDynArray CA_module, CA_pubexp, CA_privexp, CA_CHR, CA_CHA, CA_CAR, CA_AID;
	ByteDynArray IAS_AID;
	ByteDynArray CIE_AID;
	DWORD SendAPDU(ByteArray &head, ByteArray &data, ByteDynArray &resp, BYTE *le = NULL);
	DWORD SendAPDU_SM(ByteArray &head, ByteArray &data, ByteDynArray &resp, BYTE *le = NULL);
	DWORD getResp(ByteDynArray &Cardresp, DWORD sw, ByteDynArray &resp);
	DWORD getResp_SM(ByteArray &Cardresp, DWORD sw, ByteDynArray &resp);

	DWORD SM(ByteArray &keyEnc, ByteArray &keySig, ByteArray &apdu, ByteArray &seq, ByteDynArray &elabResp);
	DWORD respSM(ByteArray &keyEnc, ByteArray &keySig, ByteArray &apdu, ByteArray &seq, ByteDynArray &elabResp);

	void readfile_SM(WORD id, ByteDynArray &content);
	void readfile(WORD id, ByteDynArray &content);

	void increment(ByteArray &seq);
public:
	CToken token;

	IAS(CToken::TokenTransmitCallback transmit);
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

	void InitDHParam();
	void InitExtAuthKeyParam();
	void DHKeyExchange();
	void DAPP();
	DWORD VerifyPIN(ByteArray &PIN);
	DWORD VerifyPUK(ByteArray &PUK);
	DWORD UnblockPIN();
	DWORD ChangePIN(ByteArray &oldPIN, ByteArray &newPIN);
	DWORD ChangePIN(ByteArray &newPIN);
	void Sign(ByteArray &data, ByteDynArray &signedData);
	void Deauthenticate();
	void GetCertificate(ByteDynArray &certificate, bool askEnable=true);
	void SetCertificate(char *PAN, ByteArray &certificate);
	void IconaSbloccoPIN();

	void VerificaSOD(ByteArray &SOD, std::map<BYTE, ByteDynArray> &hashSet);

	void(*Callback)(int progress, char *desc,void *data);
	void* CallbackData;

	// usato da CardUnblockPin per comunicare i tentativi di verifica del PUK rimasti
	int attemptsRemaining;
};

