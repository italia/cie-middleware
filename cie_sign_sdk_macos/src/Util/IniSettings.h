#pragma once
#include <vector>
#include <string>
#include "Array.h"
#include "definitions.h"

class IniSettings;
extern std::vector<IniSettings*> _iniSettings;

class IniSettings {
public:
	int typeId;
	std::string section;
	std::string name;
	std::string description;

	IniSettings(int typeId, const char* section, const char* name, const char *description);
	int GetTypeId();
	virtual ~IniSettings();
};

class IniSettingsInt : public IniSettings {
public:
	int defaultVal;
	IniSettingsInt(const char* section,const char* name,int defaultValue,const char *description);
	~IniSettingsInt();
	int GetValue(const char *fileName);
};

class IniSettingsString : public IniSettings {
public:
	std::string defaultVal;
	IniSettingsString(const char* section,const char* name,const char* defaultValue,const char *description);
	~IniSettingsString();
	void GetValue(const char *fileName, std::string &value);
};

class IniSettingsBool : public IniSettings {
public:
	bool defaultVal;
	IniSettingsBool(const char* section,const char* name,bool defaultValue,const char *description);
	~IniSettingsBool();
	bool GetValue(const char *fileName);
};

class IniSettingsByteArray : public IniSettings {
public:
	ByteDynArray defaultVal;
	IniSettingsByteArray(const char* section,const char* name,ByteArray defaultValue,const char *description);
	~IniSettingsByteArray();
	void GetValue(const char *fileName, ByteDynArray &value);
};

class IniSettingsB64 : public IniSettings {
public:
	ByteDynArray defaultVal;
	IniSettingsB64(const char* section,const char* name,ByteArray defaultValue,const char *description);
	IniSettingsB64(const char* section, const char* name, const char *defaultValueB64, const char *description);
	~IniSettingsB64();
	void GetValue(const char *fileName, ByteDynArray &value);
};

extern "C" {
	int	GetNumIniSettings();
	int GetIniSettings(int i,void* data);
}
