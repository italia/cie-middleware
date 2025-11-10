#pragma once

#include "Certificate.h"
#include <string>
#include <vector>

using namespace std;

// Status dei Trust Service
enum TSL_SERVICE_STATUS
{
    TSL_STATUS_UNKNOWN = 0,
    TSL_STATUS_GRANTED = 1,
    TSL_STATUS_WITHDRAWN = 2,
    TSL_STATUS_SUSPENDED = 3,
    TSL_STATUS_REVOKED = 4
};

// Informazioni su un servizio Trust
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

// Trust Service Provider
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
    
    // Download e parsing della TSL
    bool downloadAndParse(const char* szTSLUrl);
    
    // Verifica se un certificato è emesso da una CA nella TSL
    bool isTrustedCA(CCertificate& cert, TSL_SERVICE_STATUS* pStatus = NULL);
    
    // Gestione cache
    bool saveToCache(const char* szCacheFile);
    bool loadFromCache(const char* szCacheFile);
    bool isExpired();
    
    // Inizializzazione globale
    static bool Initialize();
    static void Cleanup();
    static CTrustServiceList* GetInstance() { return s_pInstance; }
    
private:
    vector<TrustServiceProvider*> m_providers;
    string m_tslURL;
    string m_nextUpdate;
    
    // Parser XML leggero (senza librerie esterne)
    bool parseXML(const string& xmlContent);
    
    // Utility per parsing XML minimale
    string extractTagContent(const string& xml, const string& tagName, size_t startPos = 0);
    vector<string> extractAllTagContents(const string& xml, const string& tagName);
    string extractAttribute(const string& xmlTag, const string& attrName);
    
    // Estrazione certificati
    CCertificate* extractCertificateFromBase64(const string& base64Cert);
    
    // Singleton
    static CTrustServiceList* s_pInstance;
};