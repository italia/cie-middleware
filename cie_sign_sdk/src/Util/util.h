#pragma once

//#include "stdafx.h"
//#include <string>
//#include "Array.h"

#define ERR_BAD_POINTER "Puntatore non valido"
#define ERR_CANT_CREATE_MUTEX "Impossibile creare il mutex"
#define ERR_CANT_CREATE_EVENT "Impossibile creare l'evento"

// session.cpp
#define ERR_SESSION_NOT_OPENED "Sessione non aperta"
#define ERR_ADD_P11_OBJECT "Errore nella creazione oggetto P11"
#define ERR_GET_NEW_SESSION "Errore nel nuovo ID sessione"
#define ERR_CANT_GET_SESSION "Errore nella determinazione della sessione dall'ID"
#define ERR_SELECT_MF "Errore nella selezione del MF"
#define ERR_SELECT_GDO "Errore nella selezione del GDO"
#define ERR_PARSE_ASN "Errore nel parse ASN1"
#define ERR_PARSE_GDO "Errore nel parse del GDO"
#define ERR_CANT_GET_OBJECT "Errore nella determinazione dell'oggetto dall'ID"
#define ERR_CANT_READ_VALUE "Errore nella lettura del valore di un attributo"
#define ERR_CANT_WRITE_CERTIFICATE "Errore nella scrittura del certificato"
#define ERR_GET_ATTRIBUTE "Errore nell'estrazione di un attributo "
#define ERR_READ_GDO "Errore nella lettura del GDO"
#define ERR_CANT_ADD_SESSION "Errore nella scrittura della sessione nella mappa globale"
#define ERR_WRONG_OBJECT_TYPE "Tipo di oggetto non corretto"
#define ERR_UNCOMPRESS_DATA "Errore nella decompressione dei dati"
#define ERR_WRITE_GDO "Errore nella scrittura del GDO"
#define ERR_INIT_SESSION "Errore nell'inizializzazione della sessione"
#define ERR_INIT_TEMPLATE "Errore nell'inizializzazione del template"
#define ERR_CANT_LOGIN "Errore nel login"
#define ERR_RO_SESSION "Errore nella verifica delle sessioni RO"
#define ERR_SO_RW_SESSION "Errore nella verifica delle sessioni SO R/W"
#define ERR_GET_OBJECT_HANDLE "Errore nella determinazione dell'handle oggetto di sessione"
#define ERR_GET_NEW_OBJECT "Errore nel nuovo ID di oggetto"
#define ERR_GET_PRIVATE "Impossibile determinare se un oggetto è privato"
#define ERR_CHECK_MECHANISM_PARAM "Errore nella verifica dei parametri del mechanism"
#define ERR_CANT_GET_PUBKEY_EXPONENT "Impossibile leggere l'esponente della chiave pubblica"
#define ERR_CANT_GET_PUBKEY_LENGTH "Impossibile leggere la lunghezza chiave pubblica"
#define ERR_CANT_GET_PUBKEY_MODULUS "Impossibile leggere il modulo della chiave pubblica"
#define ERR_PADDING "Errore nell'applicazione del padding"
#define ERR_CRYPTO_ERROR "Errore in un operazione crittografica"
#define ERR_CANT_GET_PRIVKEY_LENGTH "Impossibile leggere la lunghezza chiave privata"
#define ERR_CANT_SIGN "Impossibile eseguire la firma"
#define ERR_CANT_INITIALIZE_MECHANISM "Impossibile inizializzare il mechanism"
#define ERR_CANT_UPDATE_MECHANISM "Impossibile aggiungere dati al mechanism"
#define ERR_CANT_FINAL_MECHANISM "Impossibile terminare il mechanism"
#define ERR_CANT_GET_MECHANISM_LENGTH "Impossibile trovare la lunghezza dei dati del mechanism"
#define ERR_CANT_DECRYPT_SIGNATURE "Impossibile decodificare la firma"
#define ERR_CANT_GET_DIGEST_INFO "Impossibile estrarre il digestInfo"
#define ERR_CANT_GET_KEY_LENGTH "Impossibile estrarre la lunghezza della chiave"
#define ERR_CANT_GET_DIGEST_LENGTH "Impossibile estrarre la lunghezza del digest"
#define ERR_SESSION_COUNT "Errore nel conteggio delle sessioni"
#define ERR_GENERATE_RANDOM "Errore nella generazione del random"
#define ERR_FINAL_SESSION "Errore nella chiusura della sessione"
#define ERR_CANT_LOGOUT "Errore nel logout"
#define ERR_INIT_PIN "Errore nell'inizializzazione del PIN"
#define ERR_SET_ATTRIBUTE "Errore nel settaggio di un attributo"
#define ERR_CREATE_OBJECT "Errore nella creazione di un oggetto"
#define ERR_GET_OPERATION_STATE "Errore su GetOperationState"
#define ERR_SET_OPERATION_STATE "Errore su SetOperationState"
#define ERR_FIND_OBJECT "Impossibile trovare un oggetto"
#define ERR_ENCODE_GDO "Errore nella codifica del GDO"
#define ERR_WRITE_DATA "Errore nella scrittura di un oggetto data"
#define ERR_CANT_DELETE_OBJECT "Errore nella distruzione di un oggetto"
#define ERR_DELETE_FILE "Errore nella cancellazione di un oggetto sulla carta"
#define ERR_MECHANISM_CACHE "Errore nella cache del machanism"

// token.cpp
#define ERR_CANT_CONNECT "Impossibile connettersi alla carta"
#define ERR_CANT_READ_ATR "Impossibile leggere l'ATR"
#define ERR_CARD_NOT_CONNECTED "Carta non connessa"
#define ERR_TRANSMISSION_ERROR "Errore nell'invio dell'APDU"
#define ERR_CANT_GET_CARD_ATTRIBUTE "Errore in SCardGetAttribute"
#define ERR_READ_RECORD "Errore nella lettura di un record"
#define ERR_CANT_DISCONNECT "Errore nella disconnessione dalla carta"
#define ERR_CANT_ESTABLISH_CONTEXT "Impossibile connettersi al servizio smart card"

// slot.cpp
#define ERR_GET_TEMPLATE "Errore nella determinazione del template carta"
#define ERR_CANT_ADD_SLOT "Errore nella scrittura dello slot nella mappa globale"
#define ERR_CANT_DELETE_SLOTLIST "Errore nella cancellazione della lista degli slot"
#define ERR_CANT_GET_SLOT "Errore nella determinazione dello slot dall'ID"
#define ERR_CANT_INIT_SLOTLIST "Errore nell'inizializzazione della lista degli slot"
#define ERR_GET_NEW_SLOT "Errore nel nuovo ID slot"
#define ERR_SLOT_NOT_PRESENT "Slot non presente"
#define ERR_TOKEN_PRESENT "Errore nella verifica di Token presente"
#define ERR_CANT_FIND_PATH "Impossibile trovare l'attributo Path"
#define ERR_CANT_WRITE_OBJECT "Impossibile scrivere l'oggetto"
#define ERR_COMPRESS_DATA "Errore nella compressione dei dati"
#define ERR_SELECT_PATH "Impossibile selezionare un DF"
#define ERR_READ_SERIAL "Impossibile leggere il seriale della carta"
#define ERR_SELECT_SERIAL "Impossibile selezionare il seriale della carta"
#define ERR_CANT_CREATE_THREAD "Impossibile creare un thread "
#define ERR_READ_MODEL "Impossibile leggere il modello della carta"
#define ERR_DELETE_SLOT "Errore nella cancellazione dello Slot"
#define ERR_GETSLOTBYNAME "Impossibile trovare lo slot dal nome del lettore"

//p11object.cpp
#define ERR_CANT_READ_FROM_CARD "Errore nella lettura di un oggetto dalla carta"
#define ERR_CANT_SET_VALUE "Errore nella scrittura dell'attributo Value"
#define ERR_CANT_FIND_ID "Impossibile trovare l'attributo ID"
#define ERR_SELECT_EF "Impossibile selezionare un EF"
#define ERR_READ_FILE "Impossibile leggere un EF"
#define ERR_CANT_READ_CERTIFICATE "Impossibile leggere il certificato"
#define ERR_ADD_ATTRIBUTE "Impossibile inserire un attributo P11"
extern DWORD ERR_ATTRIBUTE_IS_SENSITIVE;
extern DWORD ERR_OBJECT_HASNT_ATTRIBUTE;

//cardtemplate.cpp
#define ERR_CANT_INIT_TEMPLATES "Errore nell'inizializzazione della lista dei template"
#define ERR_CANT_ADD_TEMPLATE "Errore nella scrittura del template nell'elenco globale"
#define ERR_PARSE_XML_TEMPLATE "Errore nel parse XML del file dei template carta"
#define ERR_XML_TEMPLATE_STRUCT "Errore nella struttura XML del file dei  del template carta"
#define ERR_CANT_LOAD_TEMPLATE_LIBRARY "Errore nel caricamento della DLL del template"
#define ERR_CANT_LOAD_LIBRARY "Errore nel caricamento della DLL"
#define ERR_GET_LIBRARY_FUNCTION_LIST "Errore nel caricamento della TemplateGetFunctionList"
#define ERR_CALL_LIBRARY_FUNCTION_LIST "Errore nella chiamata a TemplateGetFunctionList"
#define ERR_INIT_LIBRARY "Errore nell'inizializzazione della libreria"
#define ERR_MATCH_CARD_TEMPLATE "Errore nel match della carta con il template"
#define ERR_WRONG_TEMPLATE_DATA "Errore nei parametri del template"
#define ERR_CANT_DELETE_TEMPLATES "Errore nella cancellazione della lista dei template"

//carddata.cpp
#define ERR_CANT_INITIALIZE_CARD_STRUCTURES "Errore nell'inizializzazione delle struttura dati della carta"
#define ERR_ADD_CNS_GDO "Errore nell'inizializzazione del GDO CNS"
#define ERR_CANT_FIND_PIN "Impossibile trovare l'oggetto PIN"
#define ERR_FIND_GDO_OBJECT "Errore nella ricerca dell'oggetto GDO"
#define ERR_VERIFY_PIN "Errore nella verifica del PIN"
#define ERR_CACHE_PIN "Errore nel cache del PIN"
#define ERR_FIND_GDO_ATTRIBUTE "Impossibile trovare il valore di un attributo di un oggetto GDO"
#define ERR_USE_PADDING "Impossibile determinare se usare il padding"
#define ERR_USE_SM "Impossibile determinare se usare il Secure Messaging"
#define ERR_GDO_TO_P11_OBJECT "Impossibile creare l'oggetto P11 dall'oggetto GDO"
#define ERR_KEY_UNSUITED "Chiave inadatta al mechanism utilizzato"
#define ERR_KEY_ALGO_UNKNOWN "Algoritmo della chiave sconosciuto"
#define ERR_CANT_ENCRYPT_DATA "Errore nella cifratura dei dati"
#define ERR_PARSE_CERTIFICATE "Errore nel parsing del certificato X509"
#define ERR_SM_KEYS_DERIVATION "Errore nella derivazione delle chiavi di SM"
#define ERR_MASTER_DECYPHER "Errore nella decifrazione delle chiavi master"
#define ERR_GET_PIN "Impossibile trovare il valore di un PIN in cache"
#define ERR_SET_PIN "Impossibile settare il valore di un PIN in cache"
#define ERR_USE_KEY "Impossibile usare una chiave"
#define ERR_UI_PIN "Errore nell'interfaccia utente per la richiesta del PIN"
#define ERR_UNBLOCK_PIN "Errore nello sblocco del PIN"
#define ERR_OBJECT_LABEL "Impossibile trovare la label di un oggetto"
#define ERR_CANT_GET_PIN "Impossibile trovare il valore del PIN"
#define ERR_PIN_MAXLEN "Impossibile trovare la lunghezza massima del PIN"
#define ERR_CHANGE_PIN "Errore nel cambio del PIN"
#define ERR_PIN_MINLEN "Impossibile trovare la lunghezza minimadel PIN"
#define ERR_OBJECT_SIZE "Impossibile trovare la dimensione di un oggetto"
#define ERR_KEY_NOT_SECAUTH "La chiave non è protetta con secondary authentication"
#define ERR_CHANGE_SECAUTH_PIN "Errore nel cambio del PIN di secondary authentication"
#define ERR_GET_SECAUTH_PIN "Impossibile restutuire il PIN di secondary authentication"
#define ERR_SET_SECAUTH_PIN "Impossibile impostare il PIN di secondary authentication"
#define ERR_UPDATE_FILE "Errore nell'aggiornamento di un EF"
#define ERR_WRITE_CERTIFICATE "Errore nella scrittura del certificato"
#define ERR_GET_TEMPLATE_ATTRIBUTE "Errore nella ricerca di u attributo in un template P11"
#define ERR_UNBLOCK_SECAUTH_PIN "Errore nello sblocco di un pin di secondary authentication"
#define ERR_UNBLOCK_CHANGE_PIN "Errore nel cambio e sblocco di un pin"
#define ERR_PIN_DECRYPT "Errore nella decifra di un PIN"
#define ERR_CHECK_CERT "Errore nella verifica di certificato vuoto"
#define ERR_CREATE_CERTIFICATE "Errore nella creazione del certificato vuoto"
#define ERR_NO_FREE_CERTIFICATE "Impossibile trovare un certificato disponibile nel GDO"
#define ERR_ADD_RECORD "Impossibile Aggiungere un record a un file"
#define ERR_CHANGE_KEY_DATA "Impossibile cambiare il valore di una chiave"
#define ERR_CREATE_KEY "Errore nella creazione della chiave"
#define ERR_NO_FREE_KEY "Impossibile trovare una chiave disponibile nel GDO"
#define ERR_WRITE_KEY "Errore nella scrittura della chiave sulla carta"
#define ERR_CHANGE_BSO_AC "Errore nel cambio della AC di una chiave"
#define ERR_CREATE_DATA "Errore nella creazione dell'oggetto data"
#define ERR_CHECK_KEY "Errore nella verifica di una chiave attiva"
#define ERR_ADD_BIO_GDO "Errore nell'inizializzazione del GDO Biometrico"

//ASN errors
#define ERR_ASN_TOINT "Impossibile convertire un valore ASN in intero"
#define ERR_ASNCOPY "Impossibile copiare un valore ASN"
#define ERR_ASN_CONTENTSIZE "Impossibile trovare la llunghezza del contenuto di un TAG ASN"
#define ERR_ASN_ENCODE "Impossibile codificare una struttura ASN"
#define ERR_ASN_ENCODEDSIZE "Impossibile trovare la llunghezza della codifica di una struttura ASN"
#define ERR_ASN_GETTAG "Impossibile creare un oggetto corrispondente a un tag ASN"
#define ERR_ASN_ISSEQUENCE "Impossibile determinare se un tag ASN e una sequence"
#define ERR_ASN_LENGTH "Lunghezza errata nella struttura ASN"

//cardinterface
#define ERR_WRONG_PADDING_LENGTH "Errore nella lunghezza del buffer per il padding"
#define ERR_CRYPTO_RSA "Errore in un'operazione RSA"
#define ERR_APDU_ENCODE "Errore nella codifica dell'APDU in SM"
#define ERR_CHALLENGE_LENGTH "Errore nella lunghezza del challenge"
#define ERR_CRYPTO_DES "Errore in un'operazione 3DES"
#define ERR_CYPERTEXT_BLOCK "Errore nella creazione del cyphertext block"
#define ERR_GENERATE_SW_RANDOM "Errore nella generazione del random"
#define ERR_GIVE_RANDOM "Errore passaggio del random alla carta"
#define ERR_HEADER_BLOCK "Errore nella creazione dell'header block"
#define ERR_MAC_BLOCK "Errore nella creazione del MAC block"
#define ERR_NETLE_BLOCK "Errore nella creazione del netLE block"
#define ERR_TLV "Errore nella struttura TLV"
#define ERR_TLV_VALUE_LENGTH "Errore nella lunghezza di un valore TLV"
#define ERR_DES_KEY_LENGTH "Errore nella lunghezza della chiave DES"
#define ERR_CRYPTO_MAC "Errore in un'operazione 3DES"
#define ERR_APDU_DECODE "Errore nella decodifica dell'APDU in SM"
#define ERR_GET_CHALLENGE "Errore in GetChallenge"
#define ERR_CANT_FIND_EF_SIZE "Impossibile trovare la dimensione di un EF"

#define ZeroMem(var) memset(&var,0,sizeof(var))

uint8_t hex2byte(char h);
void readHexData(const std::string &data,ByteDynArray &ba);
std::string HexByte(uint8_t data, bool uppercase = true);
std::string &dumpHexData(ByteArray &data, std::string &dump);
std::string &dumpHexData(ByteArray &data, std::string &dump, bool withSpace, bool uppercase = true);
std::string &dumpHexDataLowerCase(ByteArray &data, std::string &dump);

void PutPaddingBT0(ByteArray &ba, size_t dwLen);
void PutPaddingBT1(ByteArray &ba, size_t dwLen);
void PutPaddingBT2(ByteArray &ba, size_t dwLen);
size_t RemovePaddingBT1(ByteArray &paddedData);
size_t RemovePaddingBT2(ByteArray &paddedData);
size_t RemoveISOPad(ByteArray &paddedData);

size_t RemoveSha1(ByteArray &paddedData);
size_t RemoveSha256(ByteArray &paddedData);

size_t ANSIPadLen(size_t Len);
void ANSIPad(ByteArray &Data, size_t DataLen);
size_t ISOPadLen(size_t Len);
void ISOPad(const ByteArray &Data, size_t DataLen);
long ByteArrayToInt(ByteArray &ba);
const ByteDynArray ISOPad(const ByteArray &data);
const ByteDynArray ISOPad16(const ByteArray &data);

 std::string WinErr(HRESULT ris);
 char * CardErr(DWORD dwSW);
 char * SystemErr(DWORD dwExcept);
 
 void Debug(ByteArray ba);
 ByteDynArray ASN1Tag(DWORD tag,ByteArray &content);

 std::string stdPrintf(const char *format, ...);


 template< typename t >
 class scopeExitClass {
	 t o;
	 bool toDelete = true;
 public:
	 scopeExitClass(t in_o) : o(std::move(in_o)) {}

	 scopeExitClass(scopeExitClass &&se) : o(se.o) { se.toDelete = false; };
	 scopeExitClass(scopeExitClass const &) = delete;

	 ~scopeExitClass() noexcept {
		 static_assert(noexcept(o()), "lambda as noexcept.");
		 if (toDelete)
			 o();
	 }
 };

 template< typename t >
 scopeExitClass< t > scopeExit(t o) { return { std::move(o) }; }
