
#include "util.h"
//#include <conio.h>
#include <vector>
//#include <dbghelp.h>
#include <iomanip>
#include <sstream>

extern CLog Log;

DWORD ERR_ATTRIBUTE_IS_SENSITIVE = 0x40000008;
DWORD ERR_OBJECT_HASNT_ATTRIBUTE = 0x40000009;

BYTE hex2byte(char h) {
	if (h>='0' && h<='9') return(h-'0');
	if (h>='A' && h<='F') return(h+10-'A');
	if (h>='a' && h<='f') return(h+10-'a');
	return(0);
}

bool ishexdigit(char c) {
	if (c>='0' && c<='9') return true;
	if (c>='a' && c<='f') return true;
	if (c>='A' && c<='F') return true;
	return false;
}

unsigned long countHexData(const std::string &data)
{
	unsigned long cnt = 0;
	unsigned long slen= data.size();
	for (unsigned long i=0;i<slen;i++) {
		if (isspace(data[i]) || data[i]==',') continue;
		if (!isxdigit(data[i])) {
			throw  logged_error("Carattere non valido");
		}

		if ((i<slen-3) && data[i]=='0' && data[i+3]=='h')
			continue;

		if ((i<slen-2) && data[i]=='0' && data[i+1]=='x') {
			i+=1;
			continue;
		}
		int v=hex2byte(data[i]);
		i++;
		if (i<slen) {
			if (isxdigit(data[i])) {
				v<<=4;
				v|=hex2byte(data[i]);
			}
			else if (!isspace(data[i]))
				throw  logged_error("richiesto spazio");
		}
		cnt++;

		if (i<(slen-1) && data[i+1]=='h')
			i++;
	}
	return cnt;
}

unsigned long setHexData(const std::string &data, uint8_t *buf)
{
	unsigned long cnt = 0;
	unsigned long slen=data.size();
	for (unsigned long i=0;i<slen;i++) {
		if (isspace(data[i]) || data[i]==',') continue;
		if (!isxdigit(data[i])) {
			throw  logged_error("Carattere non valido");
		}

		if ((i<slen-3) && data[i]=='0' && data[i+3]=='h')
			continue;

		if ((i<slen-2) && data[i]=='0' && data[i+1]=='x') {
			i+=1;
			continue;
		}
		int v=hex2byte(data[i]);
		i++;
		if (i<slen) {
			if (isxdigit(data[i])) {
				v<<=4;
				v|=hex2byte(data[i]);
			}
			else if (!isspace(data[i]))
				throw  logged_error("richiesto spazio");
		}
		buf[0]=v;
		buf++;
		cnt++;

		if (i<(slen-1) && data[i+1]=='h')
			i++;
	}
	return cnt;
}

void readHexData(const std::string &data,ByteDynArray &ba)
{
	std::vector<uint8_t> dt;


	unsigned long slen=data.size();
	for (unsigned long i=0;i<slen;i++) {
		if (isspace(data[i]) || data[i]==',') continue;
		if (!isxdigit(data[i])) {
			throw  logged_error("Carattere non valido");
		}

		if ((i<slen-3) && data[i]=='0' && data[i+3]=='h')
			continue;

		if ((i<slen-2) && data[i]=='0' && data[i+1]=='x') {
			i+=1;
			continue;
		}
		int v=hex2byte(data[i]);
		i++;
		if (i<slen) {
			if (isxdigit(data[i])) {
				v<<=4;
				v|=hex2byte(data[i]);
			}
			else if (!isspace(data[i]))
				throw  logged_error("richiesto spazio");
		}
		dt.push_back(v);

		if (i<(slen-1) && data[i+1]=='h')
			i++;
	}

	if (dt.size()>0)
		ba = ByteArray(&dt[0], dt.size());
	else 
		ba.clear();
}

std::string HexByte(uint8_t data, bool uppercase) {
	std::stringstream dmp;
	dmp << std::hex << std::setfill('0');
	if (uppercase)
		dmp << std::uppercase;
	dmp << std::setw(2) << static_cast<unsigned>(data);
	return dmp.str();
}

std::string dumpHexData(ByteArray data, std::string& dump)
{
	dumpHexData(data,dump,true,true);
	return dump;
}

std::string dumpHexData(ByteArray data, std::string& dump, bool withSpace, bool uppercase)
{
	std::stringstream dmp;
	dmp << std::hex << std::setfill('0');
	if (uppercase)
		dmp << std::uppercase;
	for (unsigned long i = 0; i<data.size(); i++) {
		dmp << std::setw(2) << static_cast<unsigned>(data[i]);
		if (withSpace)
			dmp << " ";
	}
	dump.assign(dmp.str());
	return dump;
}

std::string dumpHexData(ByteArray data)
{
    std::string str;
    return dumpHexData(data, str, true, true);
}

void Debug(ByteArray ba) {
	std::string out;
	dumpHexData(ba,out);
	OutputDebugString(out.c_str());
	OutputDebugString("\n");
}

std::string dumpHexDataLowerCase(ByteArray data, std::string& dump)
{
	return dumpHexData(data, dump, false, false);
}

void PutPaddingBT0(ByteArray& ba, long dwLen)
{
	init_func

	if (dwLen>ba.size())
		throw logged_error("Lunghezza del padding errata");

	ba.left(ba.size()-dwLen).fill(0);
	exit_func
}


void PutPaddingBT1(ByteArray &ba, unsigned long dwLen)
{
	init_func
	if (dwLen>ba.size()-3)
		throw logged_error("Lunghezza del padding errata");

	ba[0]=0;
	ba[1]=1;
	ba.mid(2,ba.size()-dwLen-3).fill(0xff);
	ba[ba.size()-dwLen-1]=0;
	exit_func
}

void PutPaddingBT2(ByteArray &ba, unsigned long dwLen)
{
	init_func
	if (dwLen>ba.size()-3)
		throw logged_error("Lunghezza del padding errata");

	ba[0]=0;
	ba[1]=2;
	ba.mid(2, ba.size() - dwLen - 3).random();
	ba[ba.size() - dwLen - 1] = 0;
	exit_func
}

unsigned long RemoveSha1(ByteArray &paddedData) {
	static uint8_t SHA1Algo[] = { 0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14 };
	if (paddedData.left(sizeof(SHA1Algo)) == VarToByteArray(SHA1Algo))
		return sizeof(SHA1Algo);
	throw logged_error("OID Algoritmo SHA1 non presente");
}

unsigned long RemoveSha256(ByteArray &paddedData) {
	static uint8_t SHA256Algo[] = { 0x30, 0x31, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 };
	if (paddedData.left(sizeof(SHA256Algo)) == VarToByteArray(SHA256Algo))
		return sizeof(SHA256Algo);
	throw logged_error("OID Algoritmo SHA256 non presente");
}

unsigned long RemovePaddingBT1(ByteArray &paddedData)
{
	init_func
	if (paddedData[0]!=0)
		throw logged_error("Errore nel padding");
	if (paddedData[1]!=1)
		throw logged_error("Errore nel padding");
	for (unsigned long i = 2; i<paddedData.size(); i++) {
		if (paddedData[i]==0) {
			return i+1;
		}
		if (paddedData[i]!=0xff)
			throw logged_error("Errore nel padding");
	}
	throw logged_error("Errore nel padding");
	exit_func
}

unsigned long RemovePaddingBT2(ByteArray &paddedData)
{
	init_func
	if (paddedData[0]!=0)
		throw logged_error("Errore nel padding");
	if (paddedData[1]!=2)
		throw logged_error("Errore nel padding");
	for (unsigned long i = 2; i<paddedData.size(); i++)
		if (paddedData[i]==0) {
			return i+1;
		}
	throw logged_error("Errore nel padding");
	exit_func
}

unsigned long RemoveISOPad(ByteArray &paddedData)
{
	init_func
		for (unsigned long i = paddedData.size() - 1; i >= 0; i--) {
		if (paddedData[i]!=0) {
			if (paddedData[i]!=0x80) {
				throw logged_error("Errore nel padding");
			}
			else {
				return i;
			}
		}
	}
	throw logged_error("Errore nel padding");
	exit_func
}

unsigned long ANSIPadLen(unsigned long Len)
{
	if ((Len & 0x7) == 0)
		return(Len);
	else
		return(Len - (Len & 0x7) + 0x08);
}

void ANSIPad(ByteArray &Data, unsigned long DataLen)
{
	init_func
	Data.mid(DataLen).fill(0);
	exit_func

}

unsigned long ISOPadLen16(unsigned long Len)
{
	if ((Len & 0x10f) == 0)
		return(Len + 16);
	else
		return(Len - (Len & 0x0f) + 0x10);
}

unsigned long ISOPadLen(unsigned long Len)
{
	if ((Len & 0x7) == 0)
		return(Len+8);
	else
		return(Len - (Len & 0x7) + 0x08);
}

void ISOPad(const ByteArray& Data, unsigned long DataLen)
{
	init_func
//    printf("\nisopad 0: %s\n", dumpHexData(Data).c_str());
    
	Data.mid(DataLen).fill(0);
	Data[DataLen]=0x80;
    
//    printf("isopad 1: %s\n", dumpHexData(Data).c_str());
    
	exit_func
}

ByteDynArray ISOPad16(const ByteArray &data) {
	init_func
	ByteDynArray resp(ISOPadLen16(data.size()));
	resp.copy(data);
	ISOPad(resp, data.size());
    return resp;
	exit_func
}

ByteDynArray ISOPad(const ByteArray& data) {
	init_func
	ByteDynArray resp(ISOPadLen(data.size()));
	resp.copy(data);
	ISOPad(resp,data.size());
    
    //printf("resp: %s\n", dumpHexData(resp).c_str()); // DEBUG

    return resp;
	exit_func
}

long ByteArrayToInt(ByteArray &ba)
{
	init_func
	long val=0;
	for (unsigned long i = 0; i<ba.size(); i++) {
		val<<=8;
		val|=ba[i];
	}
	return val;
	exit_func
}


//std::string WinErr(HRESULT err) {
//    char szWinErrBuffer[300];
//    FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szWinErrBuffer, 300, NULL);
//    return std::string(szWinErrBuffer);
//}

char *  CardErr(DWORD dwSW) {
	char *msg;

	switch (dwSW) {
		case ERR_CARD_FILE_DEACTIVATED		: msg="File is deactivated"; break;
		case ERR_CARD_FILE_TERMINATED		: msg="File is terminated"; break;
		case ERR_CARD_AUTENTICATION_FAILED	: msg="Authentication failed"; break;
		case ERR_CARD_EEPROM_ERROR			: msg="EEPROM error; command aborted"; break;
		case ERR_CARD_LC_INVALID			: msg="LC invalid"; break;
		case ERR_CARD_LOGICAL_CHANNEL		: msg="Logical channel not supported"; break;
		case ERR_CARD_SM_MODE				: msg="SM mode not supported"; break;
		case ERR_CARD_CHAINING				: msg="Chaining error"; break;
		case ERR_CARD_FILE_STRUCTURE		: msg="Command cannot be used for file structure"; break;
		case ERR_CARD_ACCESS_RIGHT			: msg="Required access right not granted"; break;
		case ERR_CARD_BSOBJ_BLOCKED			: msg="BS object blocked"; break;
		case ERR_CARD_BSOBJ_FORMAT			: msg="BS object has invalid format"; break;
		case ERR_CARD_NO_RANDOM				: msg="Conditions of use not satisfied; no random number available"; break;
		case ERR_CARD_NO_CURRENT_EF			: msg="No current EF selected"; break;
		case ERR_CARD_NO_SM_KEY				: msg="Key object for SM not found"; break;
		case ERR_CARD_SMKEY_FORMAT			: msg="Key object used for SM has invalid format"; break;
		case ERR_CARD_INVALID_DATA_FIELD	: msg="Invalid parameters in data field"; break;
		case ERR_CARD_FUNCTION_NOT_SUPPORTED: msg="Function / mode not supported"; break;
		case ERR_CARD_FILE_NOT_FOUND		: msg="File not found"; break;
		case ERR_CARD_RECORD_NOT_FOUND		: msg="Record / object not found"; break;
		case ERR_CARD_NOT_ENOUGH_MEMORY		: msg="Not enough memory in file / in file system"; break;
		case ERR_CARD_TLV_LENGTH			: msg="LC does not fit the TLV structure of the data field"; break;
		case ERR_CARD_P1_P2_INVALID			: msg="P1/P2 invalid"; break;
		case ERR_CARD_LC_P1_P2_INVALID		: msg="LC does not fit P1/P2"; break;
		case ERR_CARD_OBJECT_NOT_FOUND		: msg="Object not found (GET DATA)"; break;
		case ERR_CARD_EF_ALREADY_EXISTS		: msg="File already exists"; break;
		case ERR_CARD_DF_ALREADY_EXISTS		: msg="DF name already exists"; break;
		case ERR_CARD_LE_TOO_SMALL			: msg="LE does not fit the data to be sent"; break;
		case ERR_CARD_INS_INVALID			: msg="INS invalid"; break;
		case ERR_CARD_CLA_INVALID			: msg="CLA invalid (Hi nibble)"; break;
		case ERR_CARD_TECHNICAL_ERROR		: msg="Technical error"; break;
		case ERR_CARD_LCYCLE_DEATH			: msg="Card life cycle was set to death"; break;
		case ERR_CARD_FAIL_CORRUPT			: msg="Code file corrupted and terminated"; break;
		case ERR_CARD_CHECKSUM_ERROR		: msg="File is invalid because of checksum error"; break;
		case ERR_CARD_NOT_ENOUGH_XXRAM		: msg="Not enough memory available in XRAM"; break;
		case ERR_CARD_TRANSACTION_ERROR		: msg="Transaction error"; break;
		case ERR_CARD_GENERAL_PROTECTION_FAULT	: msg="General protection fault"; break;
		case ERR_CARD_INTERNAL_FAULT		: msg="Internal failure of PK-API (wrong CCMS format?)"; break;
		case ERR_CARD_KEY_NOT_FOUND			: msg="Key object not found"; break;
		case ERR_CARD_HARDWARE_ATTACK		: msg="Internal hardware attack detected, change to life cycle death"; break;
		case ERR_CARD_TRANS_BUFFER_TOO_SMALL: msg="Transaction buffer too small"; break;
		case ERR_CARD_INTERNAL_ERROR		: msg="Internal assertion"; break;
		case CARD_OK						: msg="OK"; break;
		case CARD_OK_SECOND_TRIAL			: msg="OK, EEPROM written in second trial"; break;
		case ERR_CARD_OVERFLOW_UNDERFLOW	: msg="Overflow through INCREASE / Underflow through DECREASE"; break;
		default: msg="Unknown status code"; break;
	}

	return(msg);
}


//char *SystemErr(DWORD dwExcept){
//    switch (dwExcept){
//        case EXCEPTION_ACCESS_VIOLATION: return "Access Violation"; break;
//         case EXCEPTION_DATATYPE_MISALIGNMENT: return "Datatype Misalignment"; break;
//         case EXCEPTION_BREAKPOINT: return "Breakpoint"; break;
//         case EXCEPTION_SINGLE_STEP: return "Single Step"; break;
//         case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "Array Bounds Exceeded"; break;
//         case EXCEPTION_FLT_DENORMAL_OPERAND: return "Flt Denormal Operand"; break;
//         case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "Flt Divide By ZERO"; break;
//         case EXCEPTION_FLT_INEXACT_RESULT: return "Flt Inexact Result"; break;
//         case EXCEPTION_FLT_INVALID_OPERATION: return "Flt Invalid Operation"; break;
//         case EXCEPTION_FLT_OVERFLOW: return "Flt Overflow"; break;
//         case EXCEPTION_FLT_STACK_CHECK: return "Flt Stack Check"; break;
//         case EXCEPTION_FLT_UNDERFLOW: return "Flt Underflow"; break;
//         case EXCEPTION_INT_DIVIDE_BY_ZERO: return "Int Divide By Zero"; break;
//         case EXCEPTION_INT_OVERFLOW: return "Int Overflow"; break;
//         case EXCEPTION_PRIV_INSTRUCTION: return "Priv Instruction"; break;
//         case EXCEPTION_IN_PAGE_ERROR: return "In Page Error"; break;
//         case EXCEPTION_ILLEGAL_INSTRUCTION: return "Illegal Instruction"; break;
//         case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Noncontinuable Exception"; break;
//        case EXCEPTION_STACK_OVERFLOW: return "Stack Overflow"; break;
//         case EXCEPTION_INVALID_DISPOSITION: return "Invalid Disposition"; break;
//         case EXCEPTION_GUARD_PAGE: return "Guard Page"; break;
//         case EXCEPTION_INVALID_HANDLE: return "Invalid Handle"; break;
//        default: return ""; break;
//    }
//}

ByteDynArray ASN1Tag(DWORD tag,ByteArray& content) {
	ByteDynArray result = content.getASN1Tag(tag);
    return result;
}

unsigned long ASN1TLength(unsigned int tag) {
	int tLen=0;
	while (tag!=0) {
		tLen++;
		tag>>=8;
	}
	return tLen;
}
void putASN1Tag(unsigned int tag, ByteArray &data) {
	int tPos=0;
	while (tag!=0) {
		uint8_t b = tag >> 24;
		if (b!=0) {
			data[tPos]=b;
			tPos++;
		}
		tag<<=8;
	}
}

unsigned long ASN1LLength(unsigned long len) {
	if (len<0x80)
		return 1;
	else {
		// vediamo quanti byte mi servono...
		if (len<=0xff)
			return 2;
		else if (len<=0xffff)
			return 3;
		else if (len<=0xffffff)
			return 4;
		else if (len<=0xffffffff)
			return 5;
	}
	throw logged_error("Lunghezza ASN1 non valida");
}

void putASN1Length(unsigned long len, ByteArray &data) {
	if (len < 0x80) {
		data[0] = (uint8_t)len;
	}
	else {
		if (len <= 0xff) {
			data[0] = (0x81);
			data[1] = (uint8_t)(len);
		}
		else if (len <= 0xffff) {
			data[0] = (0x82);
			data[1] = (uint8_t)(len >> 8);
			data[2] = (len & 0xff);
		}
		else if (len <= 0xffffff) {
			data[0] = (0x83);
			data[1] = (uint8_t)(len >> 16);
			data[2] = ((len >> 8) & 0xff);
			data[3] = (len & 0xff);
		}
		else if (len <= 0xffffffff) {
			data[0] = (0x84);
			data[1] = (uint8_t)(len >> 24);
			data[2] = ((len >> 16) & 0xff);
			data[3] = ((len >> 8) & 0xff);
			data[4] = (len & 0xff);
		}
	}
}

std::string stdPrintf(const char *format, ...) {
	va_list args;
	va_start(args, format);
    
    auto size = std::snprintf(nullptr, 0, format, args);
    std::string result(size + 1, '\0');
    std::sprintf(&result[0], format, args);
    	
	va_end(args);
	return result;
}

SYSTEMTIME convertStringToSystemTime(const char *dateTimeString)
{
    SYSTEMTIME systime;
    
    memset(&systime,0,sizeof(systime));
    // Date string should be "yyyyMMddhhmm"
    sscanf(dateTimeString, "%04i%02i%02iT%02i%02i%02iZ",
             &systime.wYear,
             &systime.wMonth,
             &systime.wDay,
             &systime.wHour,
             &systime.wMinute,
             &systime.wSecond
           );
    return systime;
}
