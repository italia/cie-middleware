#pragma once

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
	String _errDump; \
	CFuncCallInfo::startCall(); \
	CFuncCallInfo info(_this_func_name,Log);  \
	try {

#define exit_func_catch \
	} \
	catch(CBaseException &ex) { \
		throw CStringException(ex,"Eccezione in %s",_this_func_name); \
	} \
	catch(char *ex) { \
		throw CStringException(ex,_this_func_name); \
	} \
	catch(...) { \
		throw CStringException("Errore in %s",_this_func_name); \
	}

#define exit_main_func_catch \
	} \
	catch(CBaseException &ex) { \
		 ex.DumpTree(_errDump); \
		 Log.write((char*)_errDump.lock()); \
	} \
	catch(char *ex) { \
		_errDump=ex; \
		 Log.write((char*)ex); \
	} \
	catch(...) { \
		 Log.write("Errore in %s",_this_func_name); \
	}

#define exit_main_func exit_main_func_catch
#define exit_func exit_func_catch
#define init_main_func init_main_func_catch
#define init_func init_func_catch


#define WIN_R_CALL(call,expected) \
if ((_call_ris=(call))!=(expected)) { \
	throw CWinException(__LINE__,__FILE__,_call_ris); \
}

#define ER_CALL(call,err) \
if ((_call_ris=(call))==FAIL) { \
	throw CStringException(__LINE__,__FILE__,err); \
}

#define TRY_CALL(call,err) \
try { if ((_call_ris=(call))==FAIL) { \
	throw CStringException(err); } \
} catch (CBaseException &ex) { throw CStringException(ex,__LINE__,__FILE__,err); }

#define CARD_R_CALL(call) \
if ((_call_ris=(call))!=CARD_OK) { \
	throw CSCardException(__LINE__,__FILE__,(WORD)_call_ris); \
}

#define SC_R_CALL(call) WIN_R_CALL(call,SCARD_S_SUCCESS)




#define ER_ASSERT(a,b) \
	if (!(a)) throw CStringException(__LINE__,__FILE__,b);
