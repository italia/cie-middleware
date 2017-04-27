#pragma once
#include "defines.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <openssl/rand.h>

#ifndef min
#define min(a,b) ((a)<(b)) ? (a) : (b)
#endif

char *err_string(const char *format,...);
template <class T> class Array;
void putASN1Tag(DWORD tag,Array<BYTE> &data);
void putASN1Length(LONG len,Array<BYTE> &data);
int ASN1TLength(DWORD tag);
int ASN1LLength(LONG len);

class ByteDynArray;
template <class T> class Array {
public:
	DWORD dwSize;
	T *pbtData;

	inline DWORD size() const {
		return(dwSize);
	}

	Array() {
		pbtData=NULL;
		dwSize=0;
	}

	Array(T* data,DWORD size) {
		pbtData=data;
		dwSize=size;
	}
	Array(const Array<T>& ba,DWORD size,DWORD start=0) {
		if (size==0xffffffff) size=ba.size()-start;
		if (start+size>ba.size()) 
			throw err_string("Array derivato troppo grande");
		dwSize=size;
		pbtData=ba.pbtData+start;
	}

	Array(const Array<T> &src) {
		init_func_internal
		dwSize=src.dwSize;
		pbtData=src.pbtData;
		exit_func_internal
  	};
  
  	inline bool operator==(const Array<T> &src) const {
  		init_func_internal
  		if (dwSize!=src.dwSize)
			return false;
		return (memcmp(pbtData,src.pbtData,dwSize)==0);
  		exit_func_internal
	};

	int atoi() {
  		init_func_internal
		int val=0;
		for (DWORD i=0;i<dwSize;i++) {
			if (val>(INT_MAX / 10))
				throw err_string("owerflow");
			val=(val*10)+pbtData[i]-'0';
		}
		return val;
  		exit_func_internal
	}
  	inline bool operator<(const Array<T> &src) const {
  		init_func_internal
  		if (dwSize<src.dwSize)
			return true;
		return (memcmp(pbtData,src.pbtData,dwSize)<0);
  		exit_func_internal
	};

  	inline bool operator>(const Array<T> &src) const {
  		init_func_internal
  		if (dwSize>src.dwSize)
			return true;
		return (memcmp(pbtData,src.pbtData,dwSize)>0);
  		exit_func_internal
	};

	inline bool operator!=(const Array<T> &src) const {
  		init_func_internal
  		if (dwSize!=src.dwSize)
  			return true;
		return (memcmp(pbtData,src.pbtData,dwSize)!=0);
  		return false;
  		exit_func_internal
	};

	inline bool isEmpty() const {
		return(dwSize==0);
	}

	inline bool isNull() const {
		return(pbtData==NULL);
	}

	inline T *lock(DWORD size=0xffffffff,DWORD start=0) const {
	#ifdef _BOUND_CHECK
		init_func_internal
		if (start<0) throw err_string("Lock dell'array al di fuori dei limiti; start %i",start);
		if (size==0xffffffff) {
			if (start==0) return pbtData;
			else return pbtData+start;
		}
		else
			if (size+start>dwSize) throw err_string("Lock dell'array al di fuori dei limiti; start %i, size %i, dimensione %i",start,size,dwSize);
		return pbtData+start;
		exit_func_internal
	#else
		return pbtData+start;
	#endif
	}

	inline T& operator[] (DWORD pos) const {
		init_func_internal
	#ifdef _BOUND_CHECK
		if (pos>=dwSize) throw err_string("Accesso all'array alla posizione %i non consentito; dimensione massima %i",pos,dwSize);
	#endif
		return pbtData[pos];
		exit_func_internal
	}
	inline T& at(DWORD pos) {
		return((*this)[pos]);
	}

	void *copy(const Array<T> &src,DWORD start=0) {
		init_func_internal
	#ifdef _BOUND_CHECK
		if (src.dwSize+start>dwSize)
			throw err_string("Dimensione array da copiare %i troppo grande; dimensione massima %i",src.dwSize+start,dwSize);
	#endif
		::memcpy_s(pbtData + start, dwSize - (start*sizeof(T)), src.pbtData, src.dwSize*sizeof(T));
		return pbtData + start;
		exit_func_internal
	}

	void *rightcopy(const Array<T> &src,DWORD end=0) {
		init_func_internal
	#ifdef _BOUND_CHECK
		if (src.dwSize+end>dwSize)
			throw err_string("Dimensione array da copiare %i troppo grande; dimensione massima %i",src.dwSize+end,dwSize);
	#endif
		::memcpy_s(pbtData + dwSize - end - src.dwSize, (end + src.dwSize)*sizeof(T), src.pbtData, src.dwSize*sizeof(T));
		return pbtData + dwSize - end - src.dwSize;
		exit_func_internal
	}

	void *copy(const T *src,DWORD size,DWORD start=0) {
		init_func_internal
	#ifdef _BOUND_CHECK
		if (size+start>dwSize) 
			throw err_string("Dimensione array da copiare %i troppo grande; dimensione massima %i",size+start,dwSize);
	#endif
		::memcpy_s(pbtData + start, (dwSize - start) *sizeof(T), src, size*sizeof(T));
		return pbtData + start;
		exit_func_internal
	}

	void *rightcopy(const T *src,DWORD size,DWORD end=0) {
		init_func_internal
	#ifdef _BOUND_CHECK
		if (size+end>dwSize) 
			throw err_string("Dimensione array da copiare %i troppo grande; dimensione massima %i",size+end,dwSize);
	#endif
		::memcpy_s(pbtData + dwSize - end - size, (end + size)*sizeof(T),src, size*sizeof(T));
		return pbtData + dwSize - end - size;
		exit_func_internal
	}

	void fill(const T &value) {
		init_func_internal
  		for (DWORD i=0;i<dwSize;i++)
			pbtData[i]=value;
		exit_func_internal
	}

	Array<T> &random() {
		RAND_bytes(pbtData, dwSize);
		//for (DWORD i = 0; i<dwSize; i++)
		//	pbtData[i] = rand() % 256;
		return *this;
	}

	Array<T> &reverse() {
  		DWORD dwHalfSize=dwSize>>1;
  		T temp;
  		for (DWORD i=0;i<dwHalfSize;i++) {
  			temp=pbtData[i];
  			pbtData[i]=pbtData[dwSize-i-1];
  			pbtData[dwSize-i-1]=temp;
  		}
		return *this;
  	}

	Array<T> right(DWORD size) const {
		init_func_internal
		if (size>dwSize) 
			throw err_string("Array derivato troppo grande");
		return(Array<T>(*this,size,dwSize-size));
		exit_func_internal
	}

	Array<T> left(DWORD size) const {
		init_func_internal
		if (size>dwSize) 
			throw err_string("Array derivato troppo grande");
		return(Array<T>(*this,size));
		exit_func_internal
	}

	Array<T> mid(DWORD start,DWORD size=0xffffffff) const {
		init_func_internal
		if (size==0xffffffff) size=dwSize-start;
		if (start+size>dwSize) 
			throw err_string("Array derivato troppo grande");
		return(Array<T>(*this,size,start));
		exit_func_internal
	}

	Array<T> revmid(DWORD toend,DWORD size=0xffffffff) const {
		init_func_internal
		if (size==0xffffffff) size=dwSize-toend;
		if (toend+size>dwSize) 
			throw err_string("Array derivato troppo grande");
		return(Array<T>(*this,size,dwSize-toend-size));
		exit_func_internal
	}

	virtual ~Array() {
		dwSize=0;
		pbtData=NULL;
	}

	ByteDynArray &setASN1Tag(int tag, ByteDynArray &result) {
		int tl=ASN1TLength(tag);
		int ll=ASN1LLength(size());
		result.resize(tl+ll+size());
		putASN1Tag(tag,result);
		putASN1Length(size(),result.mid(tl));
		result.mid(tl+ll).copy(*this);
		return result;
	}
};

typedef Array<BYTE> ByteArray;

template <class T> class DynArray : public Array<T>
{
public:
	DynArray()  { 
		dwSize=0;
		pbtData=NULL;
	};
	DynArray(const Array<T> &src) {
		init_func_internal
		dwSize=0;
		pbtData=NULL;
		alloc_copy(src);
		exit_func_internal
	};
	DynArray(const DynArray<T> &src) {
		init_func_internal
		dwSize=0;
		pbtData=NULL;
		alloc_copy(src);
		exit_func_internal
	};
	DynArray(const T *src,DWORD size) {
		init_func_internal
		dwSize=size;
		pbtData=(T*)malloc(sizeof(T)*size);
		copy(src,size);
		exit_func_internal
	}
	DynArray(DWORD size) {
		init_func_internal
		dwSize=size;
		pbtData=(T*)malloc(sizeof(T)*dwSize);
		exit_func_internal
	};
	~DynArray() {
		init_func_internal
		if (pbtData!=NULL)
			free(pbtData);
		exit_func_internal
	}
	DynArray<T> &operator=(const DynArray<T> src) {
		init_func_internal
		alloc_copy(src);
		return *this;
		exit_func_internal
	}

	void resize(DWORD size,bool bKeepData=false) {
		init_func_internal
		if (!bKeepData) {
			clear();
			dwSize=size;
			pbtData=(T*)malloc(sizeof(T)*dwSize);
		}
		else {
			T* pbtNewData=(T*)malloc(sizeof(T)*size);
			DWORD dwMinSize=min(size,dwSize);
			if (dwMinSize>0)
				memcpy_s(pbtNewData, sizeof(T)*size, pbtData, sizeof(T)*dwMinSize);
			clear();
			pbtData=pbtNewData;
			dwSize=size;
		}
		exit_func_internal
	}
	void clear() {
		init_func_internal
		if (pbtData!=NULL)
			free(pbtData);
		pbtData=NULL;
		dwSize=0;
		exit_func_internal
	}
	void alloc_copy(const Array<T> &src) {
		init_func_internal
		clear();
		dwSize=src.dwSize;
		pbtData=(T*)malloc(sizeof(T)*dwSize);
		copy(src);
		return;
		exit_func_internal
	}
	void alloc_copy(const T *src,DWORD size) {
		init_func_internal
		clear();
		dwSize=size;
		pbtData=(T*)malloc(sizeof(T)*size);
		copy(src,size);
		return;
		exit_func_internal
	}
	DynArray<T> & append(const Array<T> &src) {
		if (src.dwSize>0) {
			resize(dwSize+src.dwSize,true);
			rightcopy(src);
		}
		return *this;
	}
	DynArray<T> & random(int size) {
		resize(size,false);
		RAND_bytes(pbtData, size);
		//for (int i=0;i<size;i++)
		//	pbtData[i]=rand()%256;
		return *this;
	}

	DynArray<T> & push(const T data) {
		resize(dwSize+sizeof(T),true);
		pbtData[dwSize-1]=data;
		return *this;
	}

	T* detach() {
		return pbtData;
		pbtData = nullptr;
		dwSize = 0;
	}
};

extern DWORD term___set;
extern DWORD *endSet;

void readHexData(const char *data,ByteDynArray &ba);
DWORD countHexData(const char *data);
DWORD setHexData(const char *data,BYTE *buf);

// TODO: ByteDynArray migliorerebbe se usassi la
// Named Return Value Optimization piuttosto che ritornare 
// i risultati per riferimento
class ByteDynArray : public DynArray<BYTE> {
public:
	ByteDynArray(const char *data) {
		readHexData(data,*this);
	}
	ByteDynArray(BYTE* src,int len) : DynArray<BYTE>(src,len) {}
	ByteDynArray(int len) : DynArray<BYTE>(len) {}
	ByteDynArray() : DynArray<BYTE>() {}
	ByteDynArray(const ByteArray &ba) : DynArray<BYTE>(ba) {}
	ByteDynArray& init(const char *data) {
		readHexData(data,*this);
		return *this;
	}
	ByteDynArray& set(int num,...) {
		ByteDynArray s1;
		ByteArray s2;
		va_list params;
		void *bdaVf=*(void**)&s1;
		void *baVf=*(void**)&s2;
		if (num==0) {
			va_start (params, num);
			while(true) {
				int* cur=va_arg(params,int* );
				if ((DWORD)cur>256) {
					if (*cur==term___set)
						break;
				}
				num++;
				
			}
			va_end (params);
		}

		int totSize=0;
		va_start (params, num);
		for (int i=0;i<num;i++) {
			void* cur=va_arg(params,void*);
			if ((DWORD)cur<256)
				totSize++;
			else {
				void *derCur = nullptr;
				if (cur != nullptr)
					derCur = *(void**)cur;

				if (derCur == bdaVf){
					ByteDynArray *bda = (ByteDynArray *)cur;
					if (bda != nullptr)
						totSize += bda->size();
					else
						err_string("Parametro NULL");
				}
				else if (derCur == baVf){
					ByteArray *ba = (ByteArray *)cur;
					if (ba!=nullptr)
						totSize += ba->size();
					else
						err_string("Parametro NULL");
				}
				else if (cur == bdaVf){
					throw err_string("I dati di tipo ByteDynArray nel metodo set vanno passati per riferimento!");
				}
				else if (cur == baVf){
					throw err_string("I dati di tipo ByteArray nel metodo set vanno passati per riferimento!");
				}
				else {
					// suppongo sia una stringa
					totSize += countHexData((const char*)cur);
				}
			}
		}
		va_end (params);
		resize(totSize);

		int cntPos=0;
		va_start (params, num);
		for (int i=0;i<num;i++) {
			void* cur=va_arg(params,void*);
			if ((DWORD)cur<256) {
				pbtData[cntPos]=(BYTE)(DWORD)cur;
				cntPos++;
			}
			else if (*(void**)cur==bdaVf) {
				ByteDynArray *bda=(ByteDynArray *)cur;
				copy(*bda,cntPos);
				cntPos+=bda->size();
			}
			else if (*(void**)cur==baVf){
				ByteArray *ba=(ByteArray *)cur;
				copy(*ba,cntPos);
				cntPos+=ba->size();
			}
			else {
				// suppongo sia una stringa
				cntPos+=setHexData((const char*)cur,pbtData+cntPos);
			}
		}
		va_end (params);

		return *this;
	}
	ByteDynArray &setASN1Tag(int tag,ByteArray &content) {
		int tl=ASN1TLength(tag);
		int ll=ASN1LLength(content.size());
		resize(tl+ll+content.size());
		putASN1Tag(tag,*this);
		putASN1Length(content.size(),mid(tl));
		mid(tl+ll).copy(content);
		return *this;
	}
	void load(const char *fname) {
		FILE *fl = nullptr;
		fopen_s(&fl, fname, "rb");
		if (fl == nullptr)
			throw err_string("Il file non esiste");
		fseek(fl, 0, SEEK_END);
		resize(ftell(fl), false);
		fseek(fl, 0, SEEK_SET);
		fread_s(pbtData, dwSize, 1, dwSize, fl);
		fclose(fl);
	}
};

#define ASN1Tag(tag,content) ByteDynArray().setASN1Tag(tag,content)

typedef DynArray<char> CharDynArray;
class String : public CharDynArray
{
public:
	String() : CharDynArray() {};
	String(const char *src) {
		init_func_internal
			alloc_copy(src);
		exit_func_internal
	};

	String(const BYTE *src, DWORD size) {
		init_func_internal
			clear();
		dwSize = size + 1;
		pbtData = (char*)malloc(dwSize);
		fill(0);
		copy((char *)src, size);
		exit_func_internal
	};

	String(const CharDynArray &src) : CharDynArray(src) {};
	String(DWORD size) : CharDynArray(size) {};

	inline bool operator<(const String &op) const  {
		return(memcmp(pbtData, op.pbtData, min(dwSize, op.dwSize)) < 0);
	}

	inline bool operator>(const String &op) const  {
		return(memcmp(pbtData, op.pbtData, min(dwSize, op.dwSize)) > 0);
	}

	inline bool operator==(const char *op) const  {
		DWORD dwOpSize = (DWORD)::strnlen(op, dwSize + 1) + 1;
		if (dwSize != dwOpSize) return(false);
		return(memcmp(pbtData, op, dwSize) == 0);
	}

	inline bool operator!=(const char *op) const  {
		DWORD dwOpSize = (DWORD)::strnlen(op, dwSize + 1) + 1;
		if (dwSize != dwOpSize) return(false);
		return(memcmp(pbtData, op, dwSize) != 0);
	}

	inline bool operator==(const String &op) const  {
		if (dwSize != op.dwSize) return(false);
		return(memcmp(pbtData, op.pbtData, dwSize) == 0);
	}

	void alloc_copy(const char *str) {
		init_func_internal
			clear();
		if (str == 0)
			return;
		dwSize = (DWORD)::strlen(str) + 1;
		pbtData = (char*)malloc(dwSize);
		copy(str, dwSize);
		exit_func_internal
	}

	DWORD readFile(char * path) {
		FILE *f = nullptr;
		fopen_s(&f, path, "rb");
		if (f == nullptr)
			throw err_string("Errore in lettura file %s", path);
		fseek(f, 0, SEEK_END);
		int len = ftell(f);
		fseek(f, 0, SEEK_SET);
		resize(len + 1,false);
		pbtData[len] = 0;
		fread(pbtData, 1, len, f);
		fclose(f);
		return 0;
	}

	DWORD strlen() const {
		init_func_internal
		auto len = ::strnlen(pbtData, dwSize + 1);
		if (len == (dwSize + 1))
			throw err_string("Stringa non valida, fine stringa non trovato");
		return (DWORD)len;
		exit_func_internal
	}

	inline char *stringlock() {
		return (char*)lock();
		//return(lock((DWORD)strlen()+1));
	}

	String &printf(const char *format,...) {
		init_func_internal
		va_list params;
		va_start (params, format);
		resize(_vscprintf(format, params)+1);
		vsprintf_s(pbtData,dwSize,format, params);
		va_end(params);
		return *this;
		exit_func_internal
	}

	void printfList(const char *format,va_list params) {
		init_func_internal
		resize(_vscprintf(format, params)+1);
		vsprintf_s(pbtData,dwSize,format, params);
		exit_func_internal
	}

	ByteArray toByteArray() {
		init_func_internal
		return ByteArray((BYTE*)pbtData,dwSize-1);
		exit_func_internal
	}

};	

#define StringToByteArray(a) (ByteArray((BYTE*)(a),sizeof(a)-1))
#define S2B(a) (ByteArray((BYTE*)(a),sizeof(a)-1))
#define VarToByteArray(a) (ByteArray((BYTE*)&(a),sizeof(a)))
#define ByteArrayToVar(a,b) (*(b*)(a).lock(sizeof(b)))

