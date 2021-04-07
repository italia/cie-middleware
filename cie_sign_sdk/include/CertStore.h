#pragma once


#include "Certificate.h"
#include <map>

using namespace std;

class CCertStore
{
public:

    static void AddCertificate(CCertificate& caCertificate);

	static CCertificate* GetCertificate(CCertificate& certificate);

	static void CleanUp();

private:
	static map<unsigned long, CCertificate*> m_certMap;
};
