#pragma once
#include <vector>
#include <memory>

DWORD GetASN1DataLenght(ByteArray &data);

class CASNTag ;
typedef std::vector<std::unique_ptr<CASNTag>> CASNTagArray;
class CASNTag {
public:
	CASNTag(void);
	std::vector<BYTE> tag;
	ByteDynArray content;
	CASNTagArray tags;
	bool isSequence();
	RESULT Encode(ByteArray &data, DWORD &len);
	DWORD EncodeLen();
	DWORD ContentLen();
	DWORD Reparse();
	DWORD tagInt();

	CASNTag &Child(std::size_t num, BYTE tag);
	void Verify(ByteArray &content);
	CASNTag &CheckTag(BYTE tag);

	int startPos, endPos;
private:
	bool forcedSequence;
};

class CASNParser
{
public:
	CASNParser(void);
	RESULT Encode(ByteArray &data, CASNTagArray &tags);
	RESULT Encode(ByteDynArray &data);
	RESULT Parse(ByteArray &data);
	RESULT Parse(ByteArray &data, CASNTagArray &tags, int startseq);
	CASNTagArray tags;

	DWORD CalcLen();
};
