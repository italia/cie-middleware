#include "..\stdafx.h"
#include "ASNParser.h"

#define BitValue(a,b) ((a>>b) & 1)

DWORD GetASN1DataLenght(ByteArray &data) {
	DWORD l = 1;
	BYTE *cur = data.lock();

	int len = 0;
	BYTE curv = cur[0];

	if ((curv & 0x1f) == 0x1f)
	{
		while (true)
		{
			l++;
			cur++;
			if (l >= data.size())
				throw CStringException("lunghezza eccessiva nell'ASN1");
			curv = cur[0];
			if ((curv & 0x80) != 0x80)
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
	return l + llen + len;
}
bool CASNTag::isSequence()
{
	return forcedSequence || ((tag.size()>=1) && (tag[0] & 0x20) == 0x20);
}

void CASNTag::Free() {
	for(std::size_t i=0;i<tags.size();i++) {
		delete tags[i];
	}
	tags.clear();
}

DWORD CASNTag::ContentLen() {
	if (!isSequence())
		return content.size();
	else {
		DWORD len = 0;
		for (CASNTagArray::iterator i = tags.begin(); i != tags.end(); i++)
			len += (*i)->EncodeLen();
		return len;
	}
}

DWORD CASNTag::tagInt() {
	DWORD intVal = 0;
	for (std::size_t i = 0; i < tag.size(); i++) {
		intVal = (intVal << 8) | tag[i];
	}
	return intVal;
}

DWORD CASNTag::Reparse() {
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
		for (std::size_t i = 0; i < parser.tags.size(); i++)
			tags.push_back(parser.tags[i]);
		parser.tags.clear();
		content.clear();
	}
	return OK;
}

DWORD CASNTag::EncodeLen() {
	int tlen = (int)tag.size();
	DWORD clen = ContentLen();
	int llen = ASN1LLength(clen);
	return tlen + llen + clen;
}

RESULT CASNTag::Encode(ByteArray &data,DWORD &len) {
	int tlen = (int)tag.size();
	if (tlen==1 && tag[0]==3 && forcedSequence)
		throw CStringException("Bit string reparsed non gestite in encode!");
	data.copy(&tag[0], tlen);
	DWORD clen = ContentLen();
	int llen = ASN1LLength(clen);
	putASN1Length(clen, data.mid(tlen));

	if (!isSequence()) {
		data.mid(tlen + llen).copy(content);
		len = tlen + llen + clen;
	}
	else {
		DWORD ptrPos = tlen+llen;
		for (CASNTagArray::iterator i = tags.begin(); i != tags.end(); i++) {
			DWORD taglen;
			(*i)->Encode(data.mid(ptrPos), taglen);
			ptrPos += taglen;
		}
		len = ptrPos;
	}
	return OK;
}

CASNTag &CASNTag::Child(std::size_t num, BYTE tag) {
	if (num >= tags.size())
		throw CStringException("Errore nella verifica della struttura ASN1");
	if (tags[num]->tag.size() == 1 && tags[num]->tag[0]==tag)
		return *tags[num];
	else
		throw CStringException("Errore nella verifica del tag ASN1");
}
CASNTag &CASNTag::CheckTag(BYTE checkTag) {
	if (tag.size() != 1 || tag[0] != checkTag)
		throw CStringException("Errore nella verifica del tag ASN1");
	return *this;
}
void CASNTag::Verify(ByteArray &checkContent) {
	if (content!=checkContent)
		throw CStringException("Errore nella verifica del tag ASN1");
}


CASNTag::CASNTag(void)
{
	forcedSequence=false;
}

CASNTag::~CASNTag(void)
{
	Free();
}

CASNParser::CASNParser(void)
{
}

CASNParser::~CASNParser(void)
{
	Free();
}

void CASNParser::Free() {
	for(std::size_t i=0;i<tags.size();i++) {
		delete tags[i];
	}
	tags.clear();
}


DWORD CASNParser::CalcLen() {
	DWORD len=0;
	for (CASNTagArray::iterator i = tags.begin(); i != tags.end(); i++)
		len += (*i)->EncodeLen();
	return len;
}

RESULT CASNParser::Encode(ByteArray &data, CASNTagArray &tags) {
	int ptrPos = 0;
	for (CASNTagArray::iterator i = tags.begin(); i != tags.end(); i++) {
		DWORD len;
		(*i)->Encode(data.mid(ptrPos), len);
		ptrPos += len;
	}
	return OK;
}

RESULT CASNParser::Encode(ByteDynArray &data) {
	int reqSize = CalcLen();
	data.resize(reqSize);
	Encode(data, tags);
	return OK;
}

RESULT CASNParser::Parse(ByteArray &data) {
	init_func
	Free();
	RESULT res=Parse(data,tags,0);
	_return(res)
	exit_func
	_return(FAIL)
}

RESULT CASNParser::Parse(ByteArray &data, CASNTagArray &tags, int startseq)
{
	init_func
	DWORD l=0;
	BYTE *cur=data.pbtData;
	while (l<data.size()) {
		int len=0;

		std::vector<BYTE> tagv;
		BYTE curv = cur[0];
		tagv.push_back(curv);

        if ((curv & 0x1f) == 0x1f)
        {
            while (true)
            {
				l++;
				cur++;
                if (l>=data.size())
					throw CStringException("lunghezza eccessiva nell'ASN1");
                curv = cur[0];
				tagv.push_back(curv);
                if ((curv & 0x80) != 0x80)
                    // è l'ultimo byte del tag
                    break;
            }
        }

		int llen=0;
		if (cur[1]==0x80) {
			llen=1;
			len=data.size()-l-2;
		}
		else if (BitValue(cur[1],7)==1) {
			llen=(cur[1] & 0x7f);
			for (int k=0;k<llen;k++) {
				len<<=8;
				len|=cur[k+2];
			}
			llen++;
		}
		else {
			llen=1;
			len=cur[1];
		}
		if (tagv.size()>0 && tagv[0]==0 && len==0) {
			_return(OK)
		}
		if (l+(len+llen+1)>data.size()) 
			throw CStringException("lunghezza eccessiva nell'ASN1");

		CASNTag *tag=new CASNTag();
		tag->startPos = startseq + l;
		tag->tag=tagv;
		if (tag->isSequence())  {
			ER_CALL(Parse(ByteArray(&cur[llen + 1], len), tag->tags, startseq + l + llen + 1), "Errore nel parse del sub-tag");
		}
		else {
			// è un valore singolo
			tag->content.alloc_copy(&cur[llen+1],len);
		}
		tags.push_back(tag);
		l+=len+llen+1;
		cur+=len+llen+1;
		tag->endPos = tag->startPos + len + llen + 1;
	}
	_return(OK)
	exit_func
	_return(FAIL)
}

