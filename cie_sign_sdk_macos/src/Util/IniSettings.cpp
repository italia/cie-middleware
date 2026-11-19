#include "IniSettings.h"
#include "../Crypto/Base64.h"
#include <sstream>
#include "cryptopp/misc.h"

std::vector<IniSettings*> _iniSettings;

void GetIniString(const char *fileName, const char* section, const char* name, std::string &buf) {
	buf.resize(100);
//    while (true) {
//        DWORD size=GetPrivateProfileStringA(section,name,"",&buf[0],(DWORD)buf.size(),fileName);
//        if (size<(buf.size()-2)) {
//            buf.resize(size+1,true);
//            return;
//        }
//        buf.resize(buf.size()*2);
//    }
}

IniSettings::IniSettings(int typeIdconst, const char* section, const char* name, const char *description) {
	_iniSettings.push_back(this);
	this->typeId = typeIdconst;
	this->section = section;
	this->name = name;
	this->description = description;
}

int IniSettings::GetTypeId() { return typeId; }

IniSettings::~IniSettings() {}

IniSettingsInt::IniSettingsInt(const char* section, const char* name, int defaultValue, const char *description) : IniSettings(0, section, name, description) {
	this->defaultVal=defaultValue;
}

int IniSettingsInt::GetValue(const char* fileName) {
    return this->defaultVal;
//    return 0;//GetPrivateProfileInt(section.c_str(),name.c_str(),defaultVal,fileName);
}

IniSettingsInt::~IniSettingsInt() {}

IniSettingsString::IniSettingsString(const char* section, const char* name, const char* defaultValue, const char *description) : IniSettings(1, section, name, description) {
	this->defaultVal=defaultValue;
}

void IniSettingsString::GetValue(const char* fileName, std::string &value) {
	GetIniString(fileName,section.c_str(),name.c_str(),value);
	if (value.size()==1 || value.c_str()[0] == 0)
		value = defaultVal;
}

IniSettingsString::~IniSettingsString() {}

IniSettingsBool::IniSettingsBool(const char* section, const char* name, bool defaultValue, const char *description) : IniSettings(2, section, name, description) {
	this->defaultVal=defaultValue;
}
bool IniSettingsBool::GetValue(const char* fileName) {
//    int val = GetPrivateProfileInt(section.c_str(), name.c_str(), -100, fileName);
//    if (val==-100)
        return defaultVal;
//    return val!=0;
    
//    return false;
}
IniSettingsBool::~IniSettingsBool() {}

IniSettingsByteArray::IniSettingsByteArray(const char* section, const char* name, ByteArray defaultValue, const char *description) : IniSettings(3, section, name, description) {
	this->defaultVal=defaultValue;
}

void IniSettingsByteArray::GetValue(const char* fileName, ByteDynArray &value) {
	std::string buf;
	GetIniString(fileName, section.c_str(), name.c_str(), buf);
	if (buf.size()==1)
		value = defaultVal;
	else
		value.set(buf.c_str());
}

IniSettingsByteArray::~IniSettingsByteArray() {}

IniSettingsB64::IniSettingsB64(const char* section, const char* name, ByteArray defaultValue, const char *description) : IniSettings(4, section, name, description) {
	this->defaultVal=defaultValue;
}

IniSettingsB64::IniSettingsB64(const char* section, const char* name, const char *defaultValueB64, const char *description) : IniSettings(4, section, name, description) {
	CBase64 b64;
	b64.Decode(defaultValueB64,defaultVal);
}

void IniSettingsB64::GetValue(const char* fileName, ByteDynArray &value) {
	CBase64 b64;
	std::string buf;
	GetIniString(fileName, section.c_str(), name.c_str(), buf);
	if (buf.size()==1)
		value = defaultVal;
	else
		b64.Decode(buf.c_str(),value);
}

IniSettingsB64::~IniSettingsB64() {}

#ifdef _WIN64
	#pragma comment(linker, "/export:GetNumIniSettings")
	#pragma comment(linker, "/export:GetIniSettings")
#else
	#pragma comment(linker, "/export:GetNumIniSettings=_GetNumIniSettings")
	#pragma comment(linker, "/export:GetIniSettings=_GetIniSettings")
#endif

extern "C" {
	int	GetNumIniSettings() {
		return (int)_iniSettings.size();
	}

	int GetIniSettings(int i, void* data) {
		CBase64 b64;
		IniSettings* is=_iniSettings[i];
		int id=is->GetTypeId();
		std::string out;
		{
			std::stringstream th;
			th << is->section << "|" << is->name << "|" << is->description << "|" << is->GetTypeId() << "|";
			out = th.str();
		}

		std::string out2;
		if (id==0) {
			out2 = ((IniSettingsInt*)is)->defaultVal;
		}
		else if (id==1) {
			out2 = ((IniSettingsString*)is)->defaultVal;
		}
		else if (id==2) {
			out2 = ((IniSettingsBool*)is)->defaultVal ? 1 : 0;
		}
		else if (id==3 || id==4) {
			b64.Encode(((IniSettingsByteArray*)is)->defaultVal,out2);
		}
		std::string res = out + out2;
		if (data!=NULL) 
            CryptoPP::memcpy_s(data,res.size(),res.c_str(),res.size());
		return (int)res.size();
	}
}
