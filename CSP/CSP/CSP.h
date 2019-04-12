#pragma once

#define CIE_KEY_BITLEN 2048

#define CIE_CONTAINER_ID 0
#define CIE_CONTAINER_NAME "CIE"
#define CIE_PIN_ID ROLE_USER
#define CIE_PUK_ID ROLE_ADMIN
#define CIE_SUPPORTED_CYPHER_ALGORITHM L"\0"
#define CIE_SUPPORTED_ASYMMETRIC_ALGORITHM L"RSA\0"


#define init_CSP_func \
	CFuncCallInfo info(__FUNCTION__, Log); \
	try {

#define exit_CSP_func } \
	catch (CSP_error &CSPErr) { \
		Log.write("EXCLOG %s", CSPErr.what()); \
		OutputDebugString("EXCLOG->"); \
		OutputDebugString(CSPErr.what()); \
		OutputDebugString("<-EXCLOG");\
		return CSPErr.getCSPErrorCode(); \
	} \
	catch (std::exception &err) { \
			Log.write("EXCLOG %s", err.what()); \
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
