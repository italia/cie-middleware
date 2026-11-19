#pragma once
#include <vector>
#include <memory>

size_t GetASN1DataLenght(const ByteArray &data);

class CASNTag ;
typedef std::vector<std::unique_ptr<CASNTag>> CASNTagArray;
class CASNTag {
public:
	CASNTag(void);
	std::vector<BYTE> tag;
	ByteDynArray content;
	CASNTagArray tags;
	bool isSequence();
	void Encode(ByteArray &data, size_t &len);
	size_t EncodeLen();
	size_t ContentLen();
	void Reparse();
	size_t tagInt();

	CASNTag &Child(std::size_t num, uint8_t tag);
	void Verify(const ByteArray &content);
	CASNTag &CheckTag(uint8_t tag);

	size_t startPos, endPos;
private:
	bool forcedSequence;
};

class CASNParser
{
public:
	CASNParser(void);
	void Encode(ByteArray &data, CASNTagArray &tags);
	void Encode(ByteDynArray &data);
	void Parse(const ByteArray &data);
	void Parse(const ByteArray &data, CASNTagArray &tags, size_t  startseq);
	CASNTagArray tags;

	size_t CalcLen();
};
