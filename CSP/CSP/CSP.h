#pragma once

#define init_CSP_func \
	CFuncCallInfo info(__FUNCTION__, Log); \
	try {

#define exit_CSP_func } \
	catch (CSP_error &CSPErr) { \
		OutputDebugString("EXCLOG->"); \
		OutputDebugString(CSPErr.what()); \
		OutputDebugString("<-EXCLOG");\
		return CSPErr.getCSPErrorCode(); \
	} \
	catch (std::exception &err) { \
		OutputDebugString("EXCLOG->"); \
		OutputDebugString(err.what()); \
		OutputDebugString("<-EXCLOG");\
		return E_UNEXPECTED; \
	} \
catch (...) { return E_UNEXPECTED; }

class CSP_error : public logged_error {
	DWORD CSPErrorCode;
public:
	CSP_error(DWORD CSPErrorCode, const char *message) : CSPErrorCode(CSPErrorCode), logged_error(message) {}
	CSP_error(DWORD CSPErrorCode) : CSP_error(CSPErrorCode, stdPrintf("%s:%08x", "Errore CSP", CSPErrorCode).c_str()) { }
	DWORD getCSPErrorCode() { return CSPErrorCode; }
};
