#pragma once

#define OK 0
#define FAIL ((DWORD)0xFFFFFFFF)
typedef unsigned long RESULT;

#define ERR_CARD_FILE_DEACTIVATED		0x6283
#define ERR_CARD_FILE_TERMINATED		0x6285
#define ERR_CARD_AUTENTICATION_FAILED	0x6300
#define ERR_CARD_EEPROM_ERROR			0x6581
#define ERR_CARD_LC_INVALID				0x6700
#define ERR_CARD_LOGICAL_CHANNEL		0x6881
#define ERR_CARD_SM_MODE				0x6882
#define ERR_CARD_CHAINING				0x6884
#define ERR_CARD_FILE_STRUCTURE			0x6981
#define ERR_CARD_ACCESS_RIGHT			0x6982
#define ERR_CARD_BSOBJ_BLOCKED			0x6983
#define ERR_CARD_BSOBJ_FORMAT			0x6984
#define ERR_CARD_NO_RANDOM				0x6985
#define ERR_CARD_NO_CURRENT_EF			0x6986
#define ERR_CARD_NO_SM_KEY				0x6987
#define ERR_CARD_SMKEY_FORMAT			0x6988
#define ERR_CARD_INVALID_DATA_FIELD		0x6A80
#define ERR_CARD_FUNCTION_NOT_SUPPORTED	0x6A81
#define ERR_CARD_FILE_NOT_FOUND			0x6A82
#define ERR_CARD_RECORD_NOT_FOUND		0x6A83
#define ERR_CARD_NOT_ENOUGH_MEMORY		0x6A84
#define ERR_CARD_TLV_LENGTH				0x6A85
#define ERR_CARD_P1_P2_INVALID			0x6A86
#define ERR_CARD_LC_P1_P2_INVALID		0x6A87
#define ERR_CARD_OBJECT_NOT_FOUND		0x6A88
#define ERR_CARD_EF_ALREADY_EXISTS		0x6A89
#define ERR_CARD_DF_ALREADY_EXISTS		0x6A8A
#define ERR_CARD_LE_TOO_SMALL			0x6C00
#define ERR_CARD_INS_INVALID			0x6D00
#define ERR_CARD_CLA_INVALID			0x6E00
#define ERR_CARD_TECHNICAL_ERROR		0x6F00
#define ERR_CARD_LCYCLE_DEATH			0x6F01
#define ERR_CARD_FAIL_CORRUPT			0x6F02
#define ERR_CARD_CHECKSUM_ERROR			0x6F81
#define ERR_CARD_NOT_ENOUGH_XXRAM		0x6F82
#define ERR_CARD_TRANSACTION_ERROR		0x6F83
#define ERR_CARD_GENERAL_PROTECTION_FAULT	0x6F84
#define ERR_CARD_INTERNAL_FAULT			0x6F85
#define ERR_CARD_KEY_NOT_FOUND			0x6F86
#define ERR_CARD_HARDWARE_ATTACK		0x6F87
#define ERR_CARD_TRANS_BUFFER_TOO_SMALL	0x6F88
#define ERR_CARD_INTERNAL_ERROR			0x6FFF
#define CARD_OK							0x9000
#define CARD_OK_SECOND_TRIAL			0x9001
#define ERR_CARD_OVERFLOW_UNDERFLOW		0x9850


#define init_func_internal \
	DWORD _call_ris=0; \
	int _this_func_checkpoint=__LINE__; \
	char *_this_func_name=__FUNCTION__; \
	try {
#define exit_func_internal \
	} \
	catch(const char *ex) { \
		throw std::runtime_error(stdPrintf("Eccezione in %s:%s",_this_func_name,ex)); \
	} \
	catch(...) { \
		throw std::runtime_error(stdPrintf("Errore in %s",_this_func_name)); \
	}




//da spostare in define_err
/*

#define init_func_catch \
	DWORD _call_ris=0; \
	int _this_func_checkpoint=__LINE__; \
	char *_this_func_name=__FUNCTION__; \
	CFuncCallInfo info(_this_func_name,Log); \
	try {

#define init_main_func_catch \
	DWORD _call_ris=0; \
	int _this_func_checkpoint=__LINE__; \
	char *_this_func_name=__FUNCTION__; \
	CFuncCallInfo::startCall(); \
	CFuncCallInfo info(_this_func_name,Log); \
	try {

#define exit_func_catch \
	} \
	catch(CBaseException &ex) { \
		throw CStringException(ex,"Eccezione in %s",_this_func_name); \
	} \
	catch(...) { \
		throw CStringException("Errore in %s",_this_func_name); \
	}

#define exit_main_func_catch \
	} \
	catch(CBaseException &ex) { \
		String dump; \
		ex.DumpTree(dump); \
		Log.write((char*)dump.lock()); \
	} \
	catch(...) { \
		Log.write("Errore in %s",_this_func_name); \
	}

#define exit_main_func exit_main_func_catch
#define exit_func exit_func_catch
#define init_main_func init_main_func_catch
#define init_func init_func_catch

#define WIN_R_CALL(call) \
if ((_call_ris=(call))!=S_OK) { \
	throw CWinException(_call_ris); \
}

#define ER_CALL(call,err) \
if ((_call_ris=(call))==FAIL) { \
	throw CStringException(err); \
}

#define TRY_CALL(call,err) \
try { if ((_call_ris=(call))==FAIL) { \
	throw CStringException(err); } \
} catch (CBaseException &ex) { throw CStringException(ex,err); }

#define CARD_R_CALL(call) \
if ((_call_ris=(call))!=CARD_OK) { \
	throw CSCardException(_call_ris); \
}

#define ER_ASSERT(a,b) \
	if (!(a)) throw CStringException(b);

#define _return(a) {info.logRet(a,__LINE__);return a;}
#define _returnVoid {info.logRet(__LINE__);return;}
*/