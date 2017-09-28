#include "../stdafx.h"
#include "IniSettings.h"
#include "../crypto/base64.h"

std::vector<IniSettings*> _iniSettings;

void GetIniString(char *fileName,char* section,char* name,String &buf) {
	buf.resize(100);
	while (true) {
		DWORD size=GetPrivateProfileStringA(section,name,"",buf.lock(),buf.size(),fileName);
		if (size<(buf.size()-2)) {
			buf.resize(size+1,true);
			return;
		}
		buf.resize(buf.size()*2);
	}	
}

IniSettings::IniSettings(int typeId,char* section,char* name,char *description) {
	_iniSettings.push_back(this);
	this->typeId = typeId;
	this->section = section;
	this->name = name;
	this->description = description;
}

int IniSettings::GetTypeId() { return typeId; }

IniSettings::~IniSettings() {}

IniSettingsInt::IniSettingsInt(char* section,char* name,int defaultValue,char *description) : IniSettings(0,section,name,description) {
	this->defaultVal=defaultValue;
}

int IniSettingsInt::GetValue(char* fileName) {
	return GetPrivateProfileInt(section.lock(),name.lock(),defaultVal,fileName);
}

IniSettingsInt::~IniSettingsInt() {}

IniSettingsString::IniSettingsString(char* section,char* name,char* defaultValue,char *description) : IniSettings(1,section,name,description) {
	this->defaultVal=defaultValue;
}

void IniSettingsString::GetValue(char* fileName,String &value) {
	GetIniString(fileName,section.lock(),name.lock(),value);
	if (value.size()==1)
		value = defaultVal;
}

IniSettingsString::~IniSettingsString() {}

IniSettingsBool::IniSettingsBool(char* section,char* name,bool defaultValue,char *description) : IniSettings(2,section,name,description) {
	this->defaultVal=defaultValue;
}
bool IniSettingsBool::GetValue(char* fileName) {
	int val=GetPrivateProfileInt(section.lock(),name.lock(),-100,fileName);
	if (val==-100)
		return defaultVal;
	return val!=0;
}
IniSettingsBool::~IniSettingsBool() {}

IniSettingsByteArray::IniSettingsByteArray(char* section,char* name,ByteArray defaultValue,char *description) : IniSettings(3,section,name,description) {
	this->defaultVal=defaultValue;
}

void IniSettingsByteArray::GetValue(char* fileName,ByteDynArray &value) {
	String buf;
	GetIniString(fileName,section.lock(),name.lock(),buf);
	if (buf.size()==1)
		value = defaultVal;
	else
		value.set(1,buf.lock());
}

IniSettingsByteArray::~IniSettingsByteArray() {}

IniSettingsB64::IniSettingsB64(char* section,char* name,ByteArray defaultValue,char *description) : IniSettings(4,section,name,description) {
	this->defaultVal=defaultValue;
}

IniSettingsB64::IniSettingsB64(char* section,char* name,char *defaultValueB64,char *description) : IniSettings(4,section,name,description) {
	CBase64 b64;
	b64.Decode(defaultValueB64,defaultVal);
}

void IniSettingsB64::GetValue(char* fileName,ByteDynArray &value) {
	CBase64 b64;
	String buf;
	GetIniString(fileName,section.lock(),name.lock(),buf);
	if (buf.size()==1)
		value = defaultVal;
	else
		b64.Decode(buf.lock(),value);
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
		String out;
		out.printf("%s|%s|%s|%i|",is->section.lock(),is->name.lock(),is->description.lock(),id);
		String out2;
		if (id==0) {
			out2.printf("%i",((IniSettingsInt*)is)->defaultVal);
		}
		else if (id==1) {
			out2.printf("%s",((IniSettingsString*)is)->defaultVal.lock());
		}
		else if (id==2) {
			out2.printf("%i",((IniSettingsBool*)is)->defaultVal ? 1 : 0);
		}
		else if (id==3 || id==4) {
			b64.Encode(((IniSettingsByteArray*)is)->defaultVal,out2);
		}
		String res;
		res.printf("%s%s",out.lock(),out2.lock());
		if (data!=NULL) 
			memcpy_s(data,res.size(),res.lock(),res.size());
		return res.size();
	}
}