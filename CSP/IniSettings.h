#pragma once
#include <vector>

class IniSettings;
extern std::vector<IniSettings*> _iniSettings;

class IniSettings {
public:
	int typeId;
	String section;
	String name;
	String description;

	IniSettings(int typeId,char* section,char* name,char *description);
	int GetTypeId();
	virtual ~IniSettings();
};

class IniSettingsInt : public IniSettings {
public:
	int defaultVal;
	IniSettingsInt(char* section,char* name,int defaultValue,char *description);
	~IniSettingsInt();
	int GetValue(char *fileName);
};

class IniSettingsString : public IniSettings {
public:
	String defaultVal;
	IniSettingsString(char* section,char* name,char* defaultValue,char *description);
	~IniSettingsString();
	void GetValue(char *fileName,String &value);
};

class IniSettingsBool : public IniSettings {
public:
	bool defaultVal;
	IniSettingsBool(char* section,char* name,bool defaultValue,char *description);
	~IniSettingsBool();
	bool GetValue(char *fileName);
};

class IniSettingsByteArray : public IniSettings {
public:
	ByteDynArray defaultVal;
	IniSettingsByteArray(char* section,char* name,ByteArray defaultValue,char *description);
	~IniSettingsByteArray();
	void GetValue(char *fileName,ByteDynArray &value);
};

class IniSettingsB64 : public IniSettings {
public:
	ByteDynArray defaultVal;
	IniSettingsB64(char* section,char* name,ByteArray defaultValue,char *description);
	IniSettingsB64(char* section,char* name,char *defaultValueB64,char *description);
	~IniSettingsB64();
	void GetValue(char *fileName,ByteDynArray &value);
};

extern "C" {
	int	GetNumIniSettings();
	int GetIniSettings(int i,void* data);
}
