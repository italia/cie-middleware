#include "TSAClient.h"


#include <stdio.h>
#include <string.h>
#include "ASN1/TimeStampResponse.h"
#include "ASN1/TimeStampRequest.h"
#include "ASN1/TimeStampToken.h"
#include "Base64.h"
#include "UUCLogger.h"

USE_LOG;

#ifdef __ANDROID__
long  GetTSAResponse(char* szTsaURL, char* szTsaUsername, char* szTsaPassword, UUCByteArray& request, UUCByteArray& response);
#else
#include "curl/curl.h"
#endif

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

CTSAClient::CTSAClient(void)
{
	m_szTSAUsername[0] = NULL;
}

CTSAClient::~CTSAClient(void)
{
}

void CTSAClient::SetTSAUrl(const char* szUrl)
{
	strcpy(m_szTSAUrl,szUrl);
}

void CTSAClient::SetCredential(const char* szUsername, const char* szPassword)
{
	strcpy(m_szTSAUsername, (char*)szUsername);
	strcpy(m_szTSAPassword,(char*)szPassword);
}

void CTSAClient::SetUsername(const char* szUsername)
{
	strcpy(m_szTSAUsername, (char*)szUsername);
}

void CTSAClient::SetPassword(const char* szPassword)
{
	strcpy(m_szTSAPassword , (char*)szPassword);
}

long CTSAClient::GetTimeStampToken(UUCByteArray& digest, const char* szPolicyID, CTimeStampToken** ppTimeStampToken)
{
	CASN1Integer nounce(1);
	CTimeStampRequest request(szSHA256OID, digest, szPolicyID, nounce);
	//CTimeStampRequest request(szSHA1OID, digest, szPolicyID, nounce);

	UUCByteArray tsaRequest;
	request.toByteArray(tsaRequest);

	UUCByteArray tsdata;
	
#ifdef __ANDROID__

	long nRet = GetTSAResponse(m_szTSAUrl, m_szTSAUsername, m_szTSAPassword, tsaRequest, tsdata);
	if(nRet != 0)
		return nRet;
#else

	//general initilization
	CURL* ctx = curl_easy_init();

	//set URL
	curl_easy_setopt(ctx, CURLOPT_URL, m_szTSAUrl);

	//set POST method
	curl_easy_setopt(ctx, CURLOPT_POST, 1);

	//give the data you want to post
	curl_easy_setopt(ctx, CURLOPT_POSTFIELDS, tsaRequest.getContent());

	//give the data lenght
	curl_easy_setopt(ctx, CURLOPT_POSTFIELDSIZE, tsaRequest.getLength());

	//show messages for debugging
	//curl_easy_setopt(ctx, CURLOPT_VERBOSE);

	//set the callback function that handle the data return from server
	//if you don't set this, the return data just show up on the screen
	//size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userp)
	curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, WriteCallback);

	// we pass our 'chunk' struct to the callback function
	curl_easy_setopt(ctx, CURLOPT_WRITEDATA, (void *)&tsdata);

	curl_easy_setopt(ctx, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(ctx, CURLOPT_SSL_VERIFYHOST, false);


	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/timestamp-query");

	//talor the header to application/binary
	if(m_szTSAUsername[0])
	{
		curl_easy_setopt(ctx, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);

		curl_easy_setopt(ctx, CURLOPT_USERNAME, m_szTSAUsername);

		curl_easy_setopt(ctx, CURLOPT_PASSWORD, m_szTSAPassword);
	}

	curl_easy_setopt(ctx, CURLOPT_HTTPHEADER, headers);

	//let's do it...
	CURLcode ret = curl_easy_perform(ctx);

	// Check for errors 
    if(ret != CURLE_OK)
	{
		LOG_MSG((0, "HTTPRequest", "error: %x", ret));

      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(ret));

	  return ret;
	}
	
	//clean up
	curl_slist_free_all(headers);
	curl_easy_cleanup(ctx);
	
#endif  // __ANDROID__

	try
	{
		CTimeStampResponse tsResponse((BYTE*)tsdata.getContent(), (int)tsdata.getLength());

		CPKIStatusInfo statusInfo(tsResponse.getPKIStatusInfo());

		if(statusInfo.getStatus().getIntValue() == 0)
		{
			*ppTimeStampToken = new CTimeStampToken(tsResponse.getTimeStampToken());
		}
		else
		{
			LOG_ERR((0, "TSACLient", "CPKIStatusInfo error: %x", statusInfo.getStatus().getIntValue()));

			*ppTimeStampToken = NULL;
		}
	}
	catch(...)
	{
		*ppTimeStampToken = NULL;
	}	

	if(*ppTimeStampToken == NULL)
		return -1;

	return 0;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	UUCByteArray* pData = (UUCByteArray*)userp;

	size_t realsize = size * nmemb;
	pData->append((BYTE*)contents, realsize);

	/*
	CTimeStampToken** ppTimeStampToken = (CTimeStampToken**)userp;

	size_t realsize = size * nmemb;

	CTimeStampResponse tsResponse((BYTE*)contents, (int)realsize);

	CPKIStatusInfo statusInfo(tsResponse.getPKIStatusInfo());

	if(statusInfo.getStatus().getIntValue() == 0)
	{
		*ppTimeStampToken = new CTimeStampToken(tsResponse.getTimeStampToken());
	}
	else
	{
		*ppTimeStampToken = NULL;
	}
*/
	return realsize;
}




