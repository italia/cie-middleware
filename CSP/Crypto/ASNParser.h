#pragma once
#include <vector>

DWORD GetASN1DataLenght(ByteArray &data);

class CASNTag ;
typedef std::vector<CASNTag*> CASNTagArray;
class CASNTag {
public:
	CASNTag(void);
	~CASNTag(void);
	std::vector<BYTE> tag;
	ByteDynArray content;
	CASNTagArray tags;
	bool isSequence();
	RESULT Encode(ByteArray &data, DWORD &len);
	DWORD EncodeLen();
	DWORD ContentLen();
	DWORD Reparse();
	DWORD tagInt();

	CASNTag &Child(int num, BYTE tag);
	void Verify(ByteArray &content);
	CASNTag &CheckTag(BYTE tag);

	int startPos, endPos;
private:
	void Free();
	bool forcedSequence;
};

class CASNParser
{
public:
	CASNParser(void);
	~CASNParser(void);
	RESULT Encode(ByteArray &data, CASNTagArray &tags);
	RESULT Encode(ByteDynArray &data);
	RESULT Parse(ByteArray &data);
	RESULT Parse(ByteArray &data, CASNTagArray &tags, int startseq);
	void Free();
	CASNTagArray tags;

	DWORD CalcLen();
};
