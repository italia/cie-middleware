#include "stdafx.h"
#include "ASNParser.h"

#define BitValue(a,b) ((a>>b) & 1)

size_t GetASN1DataLenght(ByteArray &data) {
	size_t  l = 1;
	uint8_t *cur = data.data();

	size_t len = 0;
	uint8_t curv = cur[0];

	if ((curv & 0x1f) == 0x1f)
	{
		while (true)
		{
			l++;
			cur++;
			if (l >= data.size())
				throw logged_error("lunghezza eccessiva nell'ASN1");
			curv = cur[0];
			if ((curv & 0x80) != 0x80)
				break;
		}
	}

	size_t llen = 0;
	if (cur[1] == 0x80) {
		llen = 1;
		len = data.size() - l - 2;
	}
	else if (BitValue(cur[1], 7) == 1) {
		llen = (cur[1] & 0x7f);
		for (size_t k = 0; k < llen; k++) {
			len <<= 8;
			len |= cur[k + 2];
		}
		llen++;
	}
	else {
		llen = 1;
		len = cur[1];
	}
	return l + llen + len;
}
bool CASNTag::isSequence()
{
	return forcedSequence || ((tag.size()>=1) && (tag[0] & 0x20) == 0x20);
}


size_t CASNTag::ContentLen() {
	if (!isSequence())
		return content.size();
	else {
		size_t len = 0;
		for (CASNTagArray::iterator i = tags.begin(); i != tags.end(); i++)
			len += (*i)->EncodeLen();
		return len;
	}
}

size_t CASNTag::tagInt() {
	size_t intVal = 0;
	for (std::size_t i = 0; i < tag.size(); i++) {
		intVal = (intVal << 8) | tag[i];
	}
	return intVal;
}

void CASNTag::Reparse() {
	CASNParser parser;
	//se è una bit string salto il numero di bit non usati
	//attenzione in encode! non memorizzo il numero di bit non usati, quindi non posso
	//ricostruirl'array originale! quindi lancio un'eccezione
	if (tag.size()==1 && tag[0]==3)
		parser.Parse(content.mid(1));
	else
		parser.Parse(content);
	if (parser.tags.size() > 0) {
		forcedSequence = true;
		for (auto t=parser.tags.begin(); t!=parser.tags.end(); t++)
			tags.emplace_back(std::move(*t));
		parser.tags.clear();
		content.clear();
	}
}

size_t CASNTag::EncodeLen() {
	size_t  tlen = tag.size();
	size_t clen = ContentLen();
	size_t  llen = ASN1LLength(clen);
	return tlen + llen + clen;
}

void CASNTag::Encode(ByteArray &data, size_t &len) {
	int tlen = (int)tag.size();
	if (tlen==1 && tag[0]==3 && forcedSequence)
		throw logged_error("Bit string reparsed non gestite in encode!");
	data.copy(ByteArray(&tag[0], tlen));
	size_t  clen = ContentLen();
	size_t  llen = ASN1LLength(clen);
	putASN1Length(clen, data.mid(tlen));

	if (!isSequence()) {
		data.mid(tlen + llen).copy(content);
		len = tlen + llen + clen;
	}
	else {
		size_t  ptrPos = tlen+llen;
		for (CASNTagArray::iterator i = tags.begin(); i != tags.end(); i++) {
			size_t taglen;
			(*i)->Encode(data.mid(ptrPos), taglen);
			ptrPos += taglen;
		}
		len = ptrPos;
	}
}

CASNTag &CASNTag::Child(std::size_t num, uint8_t tag) {
	if (num >= tags.size())
		throw logged_error("Errore nella verifica della struttura ASN1");
	if (tags[num]->tag.size() == 1 && tags[num]->tag[0]==tag)
		return *tags[num];
	else
		throw logged_error("Errore nella verifica del tag ASN1");
}
CASNTag &CASNTag::CheckTag(uint8_t checkTag) {
	if (tag.size() != 1 || tag[0] != checkTag)
		throw logged_error("Errore nella verifica del tag ASN1");
	return *this;
}
void CASNTag::Verify(ByteArray &checkContent) {
	if (content!=checkContent)
		throw logged_error("Errore nella verifica del tag ASN1");
}


CASNTag::CASNTag(void)
{
	forcedSequence=false;
}

CASNParser::CASNParser(void)
{
}

size_t CASNParser::CalcLen() {
	size_t len=0;
	for (CASNTagArray::iterator i = tags.begin(); i != tags.end(); i++)
		len += (*i)->EncodeLen();
	return len;
}

void CASNParser::Encode(ByteArray &data, CASNTagArray &tags) {
	size_t ptrPos = 0;
	for (CASNTagArray::iterator i = tags.begin(); i != tags.end(); i++) {
		size_t  len;
		(*i)->Encode(data.mid(ptrPos), len);
		ptrPos += len;
	}
}

void CASNParser::Encode(ByteDynArray &data) {
	size_t  reqSize = CalcLen();
	data.resize(reqSize);
	Encode(data, tags);
}

void CASNParser::Parse(ByteArray &data) {
	init_func
	tags.clear();
	Parse(data,tags,0);
}

void CASNParser::Parse(ByteArray &data, CASNTagArray &tags, size_t  startseq)
{
	init_func
	size_t l=0;
	uint8_t *cur = data.data();
	while (l < data.size()) {
		size_t len = 0;

		std::vector<uint8_t> tagv;
		uint8_t curv = cur[0];
		tagv.push_back(curv);

		if ((curv & 0x1f) == 0x1f)
		{
			while (true)
			{
				l++;
				cur++;
				if (l >= data.size())
					throw logged_error("lunghezza eccessiva nell'ASN1");
				curv = cur[0];
				tagv.push_back(curv);
				if ((curv & 0x80) != 0x80)
					// è l'ultimo byte del tag
					break;
			}
		}

		int llen = 0;
		if (cur[1] == 0x80) {
			llen = 1;
			len = data.size() - l - 2;
		}
		else if (BitValue(cur[1], 7) == 1) {
			llen = (cur[1] & 0x7f);
			for (int k = 0; k < llen; k++) {
				len <<= 8;
				len |= cur[k + 2];
			}
			llen++;
		}
		else {
			llen = 1;
			len = cur[1];
		}
		if (tagv.size() > 0 && tagv[0] == 0 && len == 0) {
			return;
		}
		if (l + (len + llen + 1) > data.size())
			throw logged_error("lunghezza eccessiva nell'ASN1");

		auto tag = std::unique_ptr<CASNTag>(new CASNTag());
		tag->startPos = startseq + l;
		tag->tag = tagv;
		if (tag->isSequence()) {
			Parse(ByteArray(&cur[llen + 1], len), tag->tags, startseq + l + llen + 1);
		}
		else {
			// è un valore singolo
			tag->content = ByteArray(&cur[llen + 1], len);
		}
		l += len + llen + 1;
		cur += len + llen + 1;
		tag->endPos = tag->startPos + len + llen + 1;
		tags.emplace_back(std::move(tag));
	}
}

