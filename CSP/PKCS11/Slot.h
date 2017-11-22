#pragma once

#pragma pack(1)
#include "pkcs11.h"
#pragma pack()
#include "../util/syncromutex.h"
#include "../util/thread.h"
#pragma pack()
#include "cardcontext.h"
#include <map>
#include <vector>
#include <memory>

namespace p11 {

typedef std::map<CK_SLOT_ID,std::shared_ptr<class CSlot>> SlotMap;
typedef std::map<CK_OBJECT_HANDLE,std::shared_ptr<class CP11Object>> HandleObjMap;
typedef std::map<std::shared_ptr<class CP11Object>,CK_OBJECT_HANDLE> ObjHandleMap;

typedef std::vector<std::shared_ptr<class CP11Object>> P11ObjectVector;

// lo slot contiene la mappa degli oggetti della carta che ci
// sta dentro; quindi ogni sessione su quella carta condivide
// la mappa di oggetti dello slot. Quando sfilo la carta
// devo chiudere tutte le sessioni e cancellare la mappa di oggetti
// corrente

class CCardTemplate;

#define CKU_NOBODY	0xffffff

enum SlotEvent {
	SE_NoEvent,
	SE_Removed,
	SE_Inserted
};

class CSlot
{
private:
	static DWORD dwSlotCnt; //counter degli slot (ID P11)
	static char *mutexName(const char *szName);
	RESULT GetATR(ByteDynArray &ATR);

public:
	SCARDHANDLE hCard;
	RESULT Connect();
	DWORD dwSessionCount; // numero di session aperte su questo slot

	static SlotMap g_mSlots; //mappa globale degli slot
	static bool bMonitorUpdate; //mappa globale degli slot

	CK_SLOT_ID hSlot; // ID P11 dello slot

	std::string szName; // nome del lettore associato

	bool bUpdated;	// flag: la mappa degli oggetti è aggiornata alla carta
					// che attualmente è nel lettore?
	
	ByteDynArray baSerial;
	std::shared_ptr<CCardTemplate> pSerialTemplate;

	ByteDynArray baATR;
	RESULT GetATR(ByteArray &ATR);
	
	DWORD dwP11ObjCnt;			//counter degli oggetti (ID P11)
	HandleObjMap HandleP11Map;	// mi servono due mappe per gestire correttamente
	ObjHandleMap ObjP11Map;		// gli ID oggetti specifici per uno slot
								// una per tradurre gli ID passati dall'applicazione,
								// un'altra per sapere se un oggetto restituito
								// ha già un ID o meno

	RESULT GetNewObjectID(CK_OBJECT_HANDLE &hObject);
	RESULT GetIDFromObject(const std::shared_ptr<CP11Object>& pObject,CK_OBJECT_HANDLE &hObject);
								// restituisce l'handle di sessione dell'oggetto corrispondente
								// pObject, e lo crea se non esiste
	RESULT DelObjectHandle(const std::shared_ptr<CP11Object>& pObject);
								// cancella l'handle dell'oggetto pObject
	RESULT GetObjectFromID(CK_OBJECT_HANDLE hObjectHandle,std::shared_ptr<CP11Object>&pObject);


	CK_USER_TYPE User; 

	CSlot(const char *szName);
	~CSlot();
	static RESULT GetNewSlotID(CK_SLOT_ID *pSlotID);
	static RESULT InitSlotList();
	static RESULT DeleteSlotList();
	static RESULT GetSlotFromID(CK_SLOT_ID hSlotId,std::shared_ptr<CSlot>&ppSlot);
	static RESULT GetSlotFromReaderName(const char *name,std::shared_ptr<CSlot>&ppSlot);
	static RESULT AddSlot(std::shared_ptr<CSlot> pSlot,CK_SLOT_ID *pSlotID);
	static RESULT DeleteSlot(CK_SLOT_ID hSlotId);
	RESULT Init();
	void Final();

	RESULT AddP11Object(std::shared_ptr<CP11Object> object);
	RESULT FindP11Object(CK_OBJECT_CLASS objClass, CK_ATTRIBUTE_TYPE attr, CK_BYTE *val, int valLen, std::shared_ptr<CP11Object>&ppObject);
	RESULT DelP11Object(const std::shared_ptr<CP11Object>& pObject);
	RESULT ClearP11Objects();
	RESULT IsTokenPresent(bool *bPresent);

	P11ObjectVector P11Objects; // vettore degli oggetti

	std::shared_ptr<CCardTemplate> pTemplate;	// template della carta
												// (aggoirnato se bUpdated=true

	void *pTemplateData;
								// i dati specifici del template della carta,
								// gestiti dalla DLL manager

	static CThread Thread;		// thread monitor degli eventi
	static CCardContext *ThreadContext; // context del monitor degli eventi

	CSyncroMutex slotMutex;		// mutex per il lock alla carta
	SlotEvent lastEvent;

	CK_RV GetInfo(CK_SLOT_INFO_PTR pInfo);
	CK_RV GetTokenInfo(CK_TOKEN_INFO_PTR pInfo);
	CK_RV CloseAllSessions();

	RESULT SessionCount(DWORD &dwSessCount);
	RESULT RWSessionCount(DWORD &dwRWSessCount);

	CCardContext Context;
};

}