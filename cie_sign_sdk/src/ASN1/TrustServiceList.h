#pragma once

#include "Certificate.h"
#include <string>
#include <vector>

using namespace std;

enum TSL_SERVICE_STATUS
{
    TSL_STATUS_UNKNOWN = 0,
    TSL_STATUS_GRANTED = 1,
    TSL_STATUS_WITHDRAWN = 2,
    TSL_STATUS_SUSPENDED = 3,
    TSL_STATUS_REVOKED = 4
};

struct TrustServiceInfo
{
    string serviceName;
    TSL_SERVICE_STATUS status;
    vector<CCertificate*> certificates;
    
    TrustServiceInfo() : status(TSL_STATUS_UNKNOWN) {}
    ~TrustServiceInfo()
    {
        for(auto cert : certificates)
            delete cert;
    }
};

struct TrustServiceProvider
{
    string providerName;
    vector<TrustServiceInfo*> services;
    
    ~TrustServiceProvider()
    {
        for(auto service : services)
            delete service;
    }
};

class CTrustServiceList
{
public:
    CTrustServiceList();
    virtual ~CTrustServiceList();
    
    bool downloadAndParse(const char* szTSLUrl);
    bool isTrustedCA(CCertificate& cert, TSL_SERVICE_STATUS* pStatus = NULL);
    
    bool saveToCache(const char* szCacheFile);
    bool loadFromCache(const char* szCacheFile);
    bool isExpired();
    
    static bool Initialize();
    static void Cleanup();
    static CTrustServiceList* GetInstance() { return s_pInstance; }
    
private:
    vector<TrustServiceProvider*> m_providers;
    string m_tslURL;
    string m_nextUpdate;
    
    bool parseXML(const string& xmlContent);
    
    string extractTagContent(const string& xml, const string& tagName, size_t startPos = 0);
    vector<string> extractAllTagContents(const string& xml, const string& tagName);
    string extractAttribute(const string& xmlTag, const string& attrName);
    
    CCertificate* extractCertificateFromBase64(const string& base64Cert);
    
    static CTrustServiceList* s_pInstance;
};