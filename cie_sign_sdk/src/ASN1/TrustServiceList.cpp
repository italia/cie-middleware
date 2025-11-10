#include "TrustServiceList.h"
#include "Base64.h"
#include "UUCLogger.h"
#include <curl/curl.h>
#include <algorithm>
#include <cctype>
#include <ctime> 

#ifdef _WIN32
#include <shlobj.h> 
#include <windows.h> 
#endif

USE_LOG;

const char* TSL_IT_URL = "https://eidas.agid.gov.it/TL/TSL-IT.xml";

// Singleton
CTrustServiceList* CTrustServiceList::s_pInstance = NULL;

extern long HTTPRequest(UUCByteArray& data, const char* szUrl, const char* szContentType, UUCByteArray& response);

CTrustServiceList::CTrustServiceList()
{
}

CTrustServiceList::~CTrustServiceList()
{
    for(auto provider : m_providers)
        delete provider;
}

bool CTrustServiceList::Initialize()
{
    if(s_pInstance)
        return true;
    
    LOG_DBG((0, "CTrustServiceList::Initialize", "Initializing TSL"));
    
    s_pInstance = new CTrustServiceList();
    
    char szCacheFile[MAX_PATH];
    
#ifdef _WIN32
    char szAppData[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szAppData)))
    {
        char szCIEPKIDir[MAX_PATH];
        sprintf_s(szCIEPKIDir, MAX_PATH, "%s\\CIEPKI", szAppData);
        
        DWORD dwAttrib = GetFileAttributesA(szCIEPKIDir);
        if(dwAttrib == INVALID_FILE_ATTRIBUTES)
        {
            if(!CreateDirectoryA(szCIEPKIDir, NULL))
            {
                LOG_ERR((0, "CTrustServiceList::Initialize", "Failed to create CIEPKI directory"));
            }
        }
        
        sprintf_s(szCacheFile, MAX_PATH, "%s\\tsl_cache.dat", szCIEPKIDir);
        LOG_DBG((0, "CTrustServiceList::Initialize", "Cache file path: %s", szCacheFile));
    }
    else
    {
        LOG_ERR((0, "CTrustServiceList::Initialize", "Failed to get PROGRAMDATA path"));
        strcpy_s(szCacheFile, MAX_PATH, "tsl_cache.dat");
    }
#else
    strcpy(szCacheFile, "/tmp/tsl_cache.dat");
#endif
    
    if(s_pInstance->loadFromCache(szCacheFile))
    {
        if(!s_pInstance->isExpired())
        {
            LOG_DBG((0, "CTrustServiceList::Initialize", "Loaded valid TSL from cache"));
            return true;
        }
        else
        {
            LOG_DBG((0, "CTrustServiceList::Initialize", "Cache expired, downloading new TSL"));
            
            for(auto provider : s_pInstance->m_providers)
                delete provider;
            s_pInstance->m_providers.clear();
        }
    }
    
    if(!s_pInstance->downloadAndParse(TSL_IT_URL))
    {
        LOG_ERR((0, "CTrustServiceList::Initialize", "Failed to download TSL"));
        delete s_pInstance;
        s_pInstance = NULL;
        return false;
    }
    
    if(!s_pInstance->saveToCache(szCacheFile))
    {
        LOG_WAR((0, "CTrustServiceList::Initialize", "Failed to save TSL cache (non-blocking)"));
    }
    
    return true;
}

void CTrustServiceList::Cleanup()
{
    if(s_pInstance)
    {
        delete s_pInstance;
        s_pInstance = NULL;
    }
}

bool CTrustServiceList::downloadAndParse(const char* szTSLUrl)
{
    LOG_DBG((0, "CTrustServiceList::downloadAndParse", "Downloading TSL from: %s", szTSLUrl));
    
    m_tslURL = szTSLUrl;
    
    UUCByteArray request;
    UUCByteArray response;
    
    long nRet = HTTPRequest(request, szTSLUrl, NULL, response);
    if(nRet != 0)
    {
        LOG_ERR((0, "CTrustServiceList::downloadAndParse", "Failed to download TSL. Error: %d", nRet));
        return false;
    }
    
    if(response.getLength() == 0)
    {
        LOG_ERR((0, "CTrustServiceList::downloadAndParse", "Empty TSL response"));
        return false;
    }
    
    response.append((BYTE)'\0');
    string xmlContent((char*)response.getContent());
    
    LOG_DBG((0, "CTrustServiceList::downloadAndParse", "Downloaded %d bytes", response.getLength()));
    
    return parseXML(xmlContent);
}

string CTrustServiceList::extractTagContent(const string& xml, const string& tagName, size_t startPos)
{
    string openTag = "<" + tagName;
    string closeTag = "</" + tagName + ">";
    
    size_t start = xml.find(openTag, startPos);
    if(start == string::npos)
        return "";
    
    start = xml.find(">", start);
    if(start == string::npos)
        return "";
    start++; // Skip '>'
    
    size_t end = xml.find(closeTag, start);
    if(end == string::npos)
        return "";
    
    return xml.substr(start, end - start);
}

vector<string> CTrustServiceList::extractAllTagContents(const string& xml, const string& tagName)
{
    vector<string> results;
    string openTag = "<" + tagName;
    string closeTag = "</" + tagName + ">";
    
    size_t pos = 0;
    while(true)
    {
        size_t start = xml.find(openTag, pos);
        if(start == string::npos)
            break;
        
        start = xml.find(">", start);
        if(start == string::npos)
            break;
        start++;
        
        size_t end = xml.find(closeTag, start);
        if(end == string::npos)
            break;
        
        results.push_back(xml.substr(start, end - start));
        pos = end + closeTag.length();
    }
    
    return results;
}

bool CTrustServiceList::parseXML(const string& xmlContent)
{
    LOG_DBG((0, "CTrustServiceList::parseXML", "Parsing TSL XML"));
    
    try
    {
        string schemeInfo = extractTagContent(xmlContent, "SchemeInformation");
        if(!schemeInfo.empty())
        {
            string nextUpdate = extractTagContent(schemeInfo, "NextUpdate");
            if(!nextUpdate.empty())
            {
                m_nextUpdate = extractTagContent(nextUpdate, "dateTime");
                LOG_DBG((0, "CTrustServiceList::parseXML", "NextUpdate: %s", m_nextUpdate.c_str()));
            }
        }
        
        string tspList = extractTagContent(xmlContent, "TrustServiceProviderList");
        if(tspList.empty())
        {
            LOG_ERR((0, "CTrustServiceList::parseXML", "TrustServiceProviderList not found"));
            return false;
        }
        
        vector<string> tspElements = extractAllTagContents(tspList, "TrustServiceProvider");
        
        LOG_DBG((0, "CTrustServiceList::parseXML", "Found %d Trust Service Providers", tspElements.size()));
        
        for(const auto& tspXml : tspElements)
        {
            TrustServiceProvider* pProvider = new TrustServiceProvider();
            
            string tspInfo = extractTagContent(tspXml, "TSPInformation");
            if(!tspInfo.empty())
            {
                string tspName = extractTagContent(tspInfo, "TSPName");
                if(!tspName.empty())
                {
                    pProvider->providerName = extractTagContent(tspName, "Name");
                    LOG_DBG((0, "CTrustServiceList::parseXML", "Provider: %s", pProvider->providerName.c_str()));
                }
            }
            
            string tspServices = extractTagContent(tspXml, "TSPServices");
            if(!tspServices.empty())
            {
                vector<string> serviceElements = extractAllTagContents(tspServices, "TSPService");
                
                for(const auto& serviceXml : serviceElements)
                {
                    TrustServiceInfo* pService = new TrustServiceInfo();
                    
                    string serviceInfo = extractTagContent(serviceXml, "ServiceInformation");
                    if(!serviceInfo.empty())
                    {
                        string serviceName = extractTagContent(serviceInfo, "ServiceName");
                        if(!serviceName.empty())
                        {
                            pService->serviceName = extractTagContent(serviceName, "Name");
                        }
                        
                        string serviceStatus = extractTagContent(serviceInfo, "ServiceStatus");
                        serviceStatus.erase(remove_if(serviceStatus.begin(), serviceStatus.end(), ::isspace), serviceStatus.end());

                        string statusLower = serviceStatus;
                        transform(statusLower.begin(), statusLower.end(), statusLower.begin(), ::tolower);

                        //LOG_DBG((0, "CTrustServiceList::parseXML", "  Service status: '%s'", serviceStatus.c_str()));

                        if(statusLower.find("granted") != string::npos || 
                           statusLower.find("recognisedatnationallevel") != string::npos)
                            pService->status = TSL_STATUS_GRANTED;
                        else if(statusLower.find("withdrawn") != string::npos)
                            pService->status = TSL_STATUS_WITHDRAWN;
                        else if(statusLower.find("suspended") != string::npos)
                            pService->status = TSL_STATUS_SUSPENDED;
                        else if(statusLower.find("revoked") != string::npos)
                            pService->status = TSL_STATUS_REVOKED;
                        else
                        {
                            LOG_WAR((0, "CTrustServiceList::parseXML", "  Unknown service status: '%s'", serviceStatus.c_str()));
                            pService->status = TSL_STATUS_UNKNOWN;
                        }
                        
                        string serviceDigitalId = extractTagContent(serviceInfo, "ServiceDigitalIdentity");
                        if(!serviceDigitalId.empty())
                        {
                            vector<string> x509Certs = extractAllTagContents(serviceDigitalId, "X509Certificate");
                            
                            for(const auto& certBase64 : x509Certs)
                            {
                                string cleanBase64 = certBase64;
                                cleanBase64.erase(remove_if(cleanBase64.begin(), cleanBase64.end(), ::isspace), cleanBase64.end());
                                
                                if(!cleanBase64.empty())
                                {
                                    CCertificate* pCert = extractCertificateFromBase64(cleanBase64);
                                    if(pCert)
                                    {
                                        pService->certificates.push_back(pCert);
                                        //LOG_DBG((0, "CTrustServiceList::parseXML", "  Loaded certificate for: %s", pService->serviceName.c_str()));
                                    }
                                }
                            }
                        }
                    }
                    
                    if(!pService->certificates.empty())
                        pProvider->services.push_back(pService);
                    else
                        delete pService;
                }
            }
            
            if(!pProvider->services.empty())
                m_providers.push_back(pProvider);
            else
                delete pProvider;
        }
        
        LOG_DBG((0, "CTrustServiceList::parseXML", "Successfully loaded %d providers", m_providers.size()));
        return true;
    }
    catch(...)
    {
        LOG_ERR((0, "CTrustServiceList::parseXML", "Exception during parsing"));
        return false;
    }
}

CCertificate* CTrustServiceList::extractCertificateFromBase64(const string& base64Cert)
{
    try
    {
        size_t decLen = base64_decoded_size(base64Cert.length());
        char* szDecoded = base64_decode(base64Cert.c_str());
        
        if(!szDecoded)
        {
            LOG_ERR((0, "CTrustServiceList::extractCertificateFromBase64", "Base64 decode failed"));
            return NULL;
        }
        
        UUCBufferedReader reader((BYTE*)szDecoded, decLen);
        CCertificate* pCert = new CCertificate(reader);
        
        free(szDecoded);
        
        return pCert;
    }
    catch(...)
    {
        LOG_ERR((0, "CTrustServiceList::extractCertificateFromBase64", "Exception parsing certificate"));
        return NULL;
    }
}

bool CTrustServiceList::isTrustedCA(CCertificate& cert, TSL_SERVICE_STATUS* pStatus)
{
    LOG_DBG((0, "CTrustServiceList::isTrustedCA", "Checking certificate against TSL"));
    
    CName issuer = cert.getIssuer();
    
    UUCByteArray issuerStr;
    issuer.getNameAsString(issuerStr);
    issuerStr.append((BYTE)'\0');
    LOG_DBG((0, "CTrustServiceList::isTrustedCA", "Certificate Issuer: %s", (char*)issuerStr.getContent()));
    
    LOG_DBG((0, "CTrustServiceList::isTrustedCA", "Total providers in TSL: %d", m_providers.size()));
    
    int providerIdx = 0;
    for(auto provider : m_providers)
    {
        LOG_DBG((0, "CTrustServiceList::isTrustedCA", "Provider %d: %s (services: %d)", 
                providerIdx++, provider->providerName.c_str(), provider->services.size()));
        
        int serviceIdx = 0;
        for(auto service : provider->services)
        {
            LOG_DBG((0, "CTrustServiceList::isTrustedCA", "  Service %d: %s (status: %d, certs: %d)", 
                    serviceIdx++, service->serviceName.c_str(), service->status, service->certificates.size()));
            
            if(service->status != TSL_STATUS_GRANTED)
            {
                LOG_DBG((0, "CTrustServiceList::isTrustedCA", "    Skipping service (not granted)"));
                continue;
            }
            
            int certIdx = 0;
            for(auto caCert : service->certificates)
            {
                CName caSubject = caCert->getSubject();
                
                UUCByteArray caSubjectStr;
                caSubject.getNameAsString(caSubjectStr);
                caSubjectStr.append((BYTE)'\0');

                //LOG_DBG((0, "CTrustServiceList::isTrustedCA", "    CA Cert issuer: %s caSubject: %s", (char*)issuerStr.getContent(), (char*)caSubjectStr.getContent()));

                if(issuer == caSubject)
                {
                    LOG_DBG((0, "CTrustServiceList::isTrustedCA", "Certificate from trusted CA: %s", service->serviceName.c_str()));
                    
                    if(cert.verifySignature(*caCert))
                    {
                        LOG_DBG((0, "CTrustServiceList::isTrustedCA", 
                                "Certificate from trusted CA (signature verified): %s", 
                                service->serviceName.c_str()));
                        
                        if(pStatus)
                            *pStatus = service->status;
                        
                        return true;
                    }
                    else
                    {
                        LOG_ERR((0, "CTrustServiceList::isTrustedCA", 
                                "Certificate issuer matches but signature verification FAILED!"));
                    }
                }
            }
        }
    }
    
    LOG_ERR((0, "CTrustServiceList::isTrustedCA", "Certificate NOT from trusted CA - no match found"));
    return false;
}

bool CTrustServiceList::saveToCache(const char* szCacheFile)
{
    LOG_DBG((0, "CTrustServiceList::saveToCache", "Saving TSL cache to: %s", szCacheFile));
    
    try
    {
        FILE* f = nullptr;
        
#ifdef _WIN32
        errno_t err = fopen_s(&f, szCacheFile, "wb");
        if(err != 0 || !f)
#else
        f = fopen(szCacheFile, "wb");
        if(!f)
#endif
        {
            LOG_ERR((0, "CTrustServiceList::saveToCache", "Failed to open cache file for writing: %s", szCacheFile));
            return false;
        }
        
        const char* CACHE_MAGIC = "TSL_CACHE_V1";
        fwrite(CACHE_MAGIC, 1, strlen(CACHE_MAGIC), f);
        
        size_t nextUpdateLen = m_nextUpdate.length();
        fwrite(&nextUpdateLen, sizeof(size_t), 1, f);
        if(nextUpdateLen > 0)
            fwrite(m_nextUpdate.c_str(), 1, nextUpdateLen, f);
        
        size_t urlLen = m_tslURL.length();
        fwrite(&urlLen, sizeof(size_t), 1, f);
        if(urlLen > 0)
            fwrite(m_tslURL.c_str(), 1, urlLen, f);
        
        size_t providerCount = m_providers.size();
        fwrite(&providerCount, sizeof(size_t), 1, f);
        
        LOG_DBG((0, "CTrustServiceList::saveToCache", "Saving %d providers", providerCount));
        
        for(auto provider : m_providers)
        {
            size_t nameLen = provider->providerName.length();
            fwrite(&nameLen, sizeof(size_t), 1, f);
            if(nameLen > 0)
                fwrite(provider->providerName.c_str(), 1, nameLen, f);
            
            size_t serviceCount = provider->services.size();
            fwrite(&serviceCount, sizeof(size_t), 1, f);
            
            for(auto service : provider->services)
            {
                size_t svcNameLen = service->serviceName.length();
                fwrite(&svcNameLen, sizeof(size_t), 1, f);
                if(svcNameLen > 0)
                    fwrite(service->serviceName.c_str(), 1, svcNameLen, f);
                
                int status = (int)service->status;
                fwrite(&status, sizeof(int), 1, f);
                
                size_t certCount = service->certificates.size();
                fwrite(&certCount, sizeof(size_t), 1, f);
                
                for(auto cert : service->certificates)
                {
                    UUCByteArray certData;
                    cert->toByteArray(certData);
                    
                    size_t certLen = certData.getLength();
                    fwrite(&certLen, sizeof(size_t), 1, f);
                    fwrite(certData.getContent(), 1, certLen, f);
                }
            }
        }
        
        fclose(f);
        
        LOG_DBG((0, "CTrustServiceList::saveToCache", "TSL cache saved successfully"));
        return true;
    }
    catch(...)
    {
        LOG_ERR((0, "CTrustServiceList::saveToCache", "Exception saving cache"));
        return false;
    }
}

bool CTrustServiceList::loadFromCache(const char* szCacheFile)
{
    LOG_DBG((0, "CTrustServiceList::loadFromCache", "Loading TSL cache from: %s", szCacheFile));
    
    try
    {
        FILE* f = nullptr;
        
#ifdef _WIN32
        errno_t err = fopen_s(&f, szCacheFile, "rb");
        if(err != 0 || !f)
#else
        f = fopen(szCacheFile, "rb");
        if(!f)
#endif
        {
            LOG_DBG((0, "CTrustServiceList::loadFromCache", "Cache file not found: %s", szCacheFile));
            return false;
        }
        
        const char* CACHE_MAGIC = "TSL_CACHE_V1";
        char magicBuf[20] = {0};
        size_t magicLen = strlen(CACHE_MAGIC);
        fread(magicBuf, 1, magicLen, f);
        
        if(strncmp(magicBuf, CACHE_MAGIC, magicLen) != 0)
        {
            LOG_ERR((0, "CTrustServiceList::loadFromCache", "Invalid cache file format"));
            fclose(f);
            return false;
        }
        
        size_t nextUpdateLen = 0;
        fread(&nextUpdateLen, sizeof(size_t), 1, f);
        if(nextUpdateLen > 0 && nextUpdateLen < 1024)
        {
            char* buf = new char[nextUpdateLen + 1];
            fread(buf, 1, nextUpdateLen, f);
            buf[nextUpdateLen] = '\0';
            m_nextUpdate = buf;
            delete[] buf;
        }
        
        size_t urlLen = 0;
        fread(&urlLen, sizeof(size_t), 1, f);
        if(urlLen > 0 && urlLen < 1024)
        {
            char* buf = new char[urlLen + 1];
            fread(buf, 1, urlLen, f);
            buf[urlLen] = '\0';
            m_tslURL = buf;
            delete[] buf;
        }
        
        size_t providerCount = 0;
        fread(&providerCount, sizeof(size_t), 1, f);
        
        LOG_DBG((0, "CTrustServiceList::loadFromCache", "Loading %d providers", providerCount));
        
        if(providerCount > 1000)
        {
            LOG_ERR((0, "CTrustServiceList::loadFromCache", "Invalid provider count: %d", providerCount));
            fclose(f);
            return false;
        }
        
        for(size_t p = 0; p < providerCount; p++)
        {
            TrustServiceProvider* provider = new TrustServiceProvider();
            
            size_t nameLen = 0;
            fread(&nameLen, sizeof(size_t), 1, f);
            if(nameLen > 0 && nameLen < 1024)
            {
                char* buf = new char[nameLen + 1];
                fread(buf, 1, nameLen, f);
                buf[nameLen] = '\0';
                provider->providerName = buf;
                delete[] buf;
            }
            
            size_t serviceCount = 0;
            fread(&serviceCount, sizeof(size_t), 1, f);
            
            if(serviceCount > 1000)
            {
                delete provider;
                fclose(f);
                return false;
            }
            
            for(size_t s = 0; s < serviceCount; s++)
            {
                TrustServiceInfo* service = new TrustServiceInfo();
                
                size_t svcNameLen = 0;
                fread(&svcNameLen, sizeof(size_t), 1, f);
                if(svcNameLen > 0 && svcNameLen < 1024)
                {
                    char* buf = new char[svcNameLen + 1];
                    fread(buf, 1, svcNameLen, f);
                    buf[svcNameLen] = '\0';
                    service->serviceName = buf;
                    delete[] buf;
                }
                
                int status = 0;
                fread(&status, sizeof(int), 1, f);
                service->status = (TSL_SERVICE_STATUS)status;
                
                size_t certCount = 0;
                fread(&certCount, sizeof(size_t), 1, f);
                
                if(certCount > 100) 
                {
                    delete service;
                    delete provider;
                    fclose(f);
                    return false;
                }
                
                for(size_t c = 0; c < certCount; c++)
                {
                    size_t certLen = 0;
                    fread(&certLen, sizeof(size_t), 1, f);
                    
                    if(certLen > 0 && certLen < 10240) // Max 10KB per cert
                    {
                        BYTE* certBuf = new BYTE[certLen];
                        fread(certBuf, 1, certLen, f);
                        
                        try
                        {
                            UUCBufferedReader reader(certBuf, certLen);
                            CCertificate* cert = new CCertificate(reader);
                            service->certificates.push_back(cert);
                        }
                        catch(...)
                        {
                            LOG_ERR((0, "CTrustServiceList::loadFromCache", "Failed to parse cached certificate"));
                        }
                        
                        delete[] certBuf;
                    }
                }
                
                provider->services.push_back(service);
            }
            
            m_providers.push_back(provider);
        }
        
        fclose(f);
        
        LOG_DBG((0, "CTrustServiceList::loadFromCache", "TSL cache loaded successfully. Providers: %d", m_providers.size()));
        return true;
    }
    catch(...)
    {
        LOG_ERR((0, "CTrustServiceList::loadFromCache", "Exception loading cache"));
        return false;
    }
}

bool CTrustServiceList::isExpired()
{
    // If NextUpdate is not set, consider it expired
    if(m_nextUpdate.empty())
    {
        LOG_DBG((0, "CTrustServiceList::isExpired", "NextUpdate not set, cache expired"));
        return true;
    }
    
    try
    {
        // NextUpdate format: "YYYY-MM-DDTHH:MM:SS" (ISO 8601)
        // Example: "2025-01-21T12:00:00Z"
        
        // Parse NextUpdate
        int year, month, day, hour, min, sec;
        if(sscanf(m_nextUpdate.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d", 
                  &year, &month, &day, &hour, &min, &sec) != 6)
        {
            LOG_ERR((0, "CTrustServiceList::isExpired", "Failed to parse NextUpdate: %s", m_nextUpdate.c_str()));
            return true;
        }
        
        struct tm nextUpdateTm = {0};
        nextUpdateTm.tm_year = year - 1900;
        nextUpdateTm.tm_mon = month - 1;
        nextUpdateTm.tm_mday = day;
        nextUpdateTm.tm_hour = hour;
        nextUpdateTm.tm_min = min;
        nextUpdateTm.tm_sec = sec;
        nextUpdateTm.tm_isdst = -1;
        
        time_t nextUpdateTime = mktime(&nextUpdateTm);
        time_t now = time(NULL);
        
        if(now >= nextUpdateTime)
        {
            LOG_DBG((0, "CTrustServiceList::isExpired", "TSL cache expired. NextUpdate: %s", m_nextUpdate.c_str()));
            return true;
        }
        
        LOG_DBG((0, "CTrustServiceList::isExpired", "TSL cache valid until: %s", m_nextUpdate.c_str()));
        return false;
    }
    catch(...)
    {
        LOG_ERR((0, "CTrustServiceList::isExpired", "Exception checking expiration"));
        return true;
    }
}

bool compareNames(CName& name1, CName& name2)
{
    UUCByteArray str1, str2;
    name1.getNameAsString(str1);
    name2.getNameAsString(str2);
    
    str1.append((BYTE)'\0');
    str2.append((BYTE)'\0');
    
    return strcmp((char*)str1.getContent(), (char*)str2.getContent()) == 0;
}