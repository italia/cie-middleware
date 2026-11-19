#include "../stdafx.h"
#include "IniSettings.h"
#include "../crypto/base64.h"
#include <sstream>
#include <fstream>

std::vector<IniSettings*> _iniSettings;

#if defined(WIN32) || defined(_WIN32)
void GetIniString(const char *fileName, const char* section, const char* name, std::string &buf) {
	buf.resize(100);
	while (true) {
		DWORD size=GetPrivateProfileStringA(section,name,"",&buf[0],(DWORD)buf.size(),fileName);
		if (size<(buf.size()-2)) {
			buf.resize(size,true);
			return;
		}
		buf.resize(buf.size()*2);
	}
}
#else
static std::string trim(const std::string &s) {
	size_t start = s.find_first_not_of(" \t\r\n");
	if (start == std::string::npos) return "";
	size_t end = s.find_last_not_of(" \t\r\n");
	return s.substr(start, end - start + 1);
}

void GetIniString(const char *fileName, const char* section, const char* name, std::string &buf) {
	buf = "";
	std::ifstream f(fileName);
	if (!f) return;
	std::string targetSection = std::string("[") + section + "]";
	bool inSection = false;
	std::string line;
	while (std::getline(f, line)) {
		line = trim(line);
		if (line.empty() || line[0] == ';' || line[0] == '#') continue;
		if (line[0] == '[') {
			inSection = (line == targetSection);
			continue;
		}
		if (!inSection) continue;
		auto eq = line.find('=');
		if (eq == std::string::npos) continue;
		std::string key = trim(line.substr(0, eq));
		if (key == name) {
			buf = trim(line.substr(eq + 1));
			return;
		}
	}
}

static int GetPrivateProfileInt(const char *section, const char *name, int def, const char *fileName) {
	std::string val;
	GetIniString(fileName, section, name, val);
	if (val.empty()) return def;
	try { return std::stoi(val); } catch (...) { return def; }
}
#endif

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
	return GetPrivateProfileInt(section.c_str(),name.c_str(),defaultVal,fileName);
}

IniSettingsInt::~IniSettingsInt() {}

IniSettingsString::IniSettingsString(const char* section, const char* name, const char* defaultValue, const char *description) : IniSettings(1, section, name, description) {
	this->defaultVal=defaultValue;
}

void IniSettingsString::GetValue(const char* fileName, std::string &value) {
	GetIniString(fileName,section.c_str(),name.c_str(),value);
	if (value.size()==1)
		value = defaultVal;
}

IniSettingsString::~IniSettingsString() {}

IniSettingsBool::IniSettingsBool(const char* section, const char* name, bool defaultValue, const char *description) : IniSettings(2, section, name, description) {
	this->defaultVal=defaultValue;
}
bool IniSettingsBool::GetValue(const char* fileName) {
	int val = GetPrivateProfileInt(section.c_str(), name.c_str(), -100, fileName);
	if (val==-100)
		return defaultVal;
	return val!=0;
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
			memcpy_s(data,res.size(),res.c_str(),res.size());
		return (int)res.size();
	}
}