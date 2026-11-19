#pragma once
#include "defines.h"
#include "UtilException.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <type_traits>
#include <stdexcept>
#include <string>
#include <cstdint>

#ifndef min1
#define min1(a,b) ((a) < (b)) ? (a) : (b)
#endif

class ByteArray;
class ByteDynArray;

std::string stdPrintf(const char *format, ...); 
void putASN1Tag(unsigned int tag, ByteArray &data);
void putASN1Length(size_t len, ByteArray &data);
size_t ASN1TLength(unsigned int tag);
size_t ASN1LLength(size_t len);

class ByteArray {
protected:
	size_t _size;
	uint8_t *_data;
public:

	ByteArray();
	ByteArray(uint8_t* data, size_t size);
	ByteArray(const ByteArray& ba, size_t start,size_t size);
	ByteArray(const ByteArray& ba, size_t start);
	ByteArray(const ByteArray &src);
	bool operator==(const ByteArray &src) const;
	bool operator<(const ByteArray &src) const;
	bool operator>(const ByteArray &src) const;
	bool operator!=(const ByteArray &src) const;
	bool isEmpty() const {
		return(_size == 0);
	}

	inline bool isNull() const {
		return(_data == nullptr);
	}

	inline uint8_t *data() const  {
		return _data;
	}

	inline uint8_t & operator[] (size_t pos) const{
		if (pos >= _size) throw logged_error(stdPrintf("Accesso all'array alla posizione %i non consentito; dimensione massima %i", pos, _size));
		return _data[pos];
	}

	inline size_t size() const {
		return(_size);
	}

	void copy(const ByteArray &src, size_t start = 0);
	void rightcopy(const ByteArray &src, size_t end = 0);

	ByteArray &fill(const uint8_t value);
	ByteArray &random();
	ByteArray &reverse();

	ByteArray right(size_t size) const;
	ByteArray left(size_t size) const;
	ByteArray mid(size_t start) const;
	ByteArray mid(size_t start, size_t size) const;
	ByteArray revmid(size_t toend) const;
	ByteArray revmid(size_t toend, size_t size) const;
	bool indexOf(ByteArray &data, size_t &position) const;

	int atoi() const;
	ByteDynArray getASN1Tag(unsigned int tag) const;
	
	virtual ~ByteArray();
};

void readHexData(const std::string &data, ByteDynArray &ba);
size_t countHexData(const std::string &data);
size_t setHexData(const std::string &data, uint8_t *buf);

class ByteDynArray : public ByteArray
{
	void alloc_copy(const ByteArray &src);

public:
	ByteDynArray();
	ByteDynArray(const ByteArray &src);
	ByteDynArray(const ByteDynArray &src);
	ByteDynArray(size_t size);
	ByteDynArray(const std::string &hexdata);
	ByteDynArray(ByteDynArray &&src);

	~ByteDynArray();
	ByteDynArray &operator=(const ByteDynArray &src);
	ByteDynArray &operator=(ByteDynArray &&src);

	void resize(size_t size, bool bKeepData = false);
	void clear();
	ByteDynArray &append(const ByteArray &src);
	ByteDynArray &push(const uint8_t data);
	uint8_t* detach();

private:
	size_t internalSet(ByteArray* ba, uint8_t data) {
		if (ba != nullptr)
			(*ba)[0] = data;
		return 1;
	}

	size_t internalSet(ByteArray* ba, const ByteArray *data) {
		if (ba != nullptr)
			ba->copy(*data);
		return data->size();
	}

	size_t internalSet(ByteArray* ba, const std::string &data) {
		if (ba != nullptr)
			return setHexData(data, ba->data());
		return countHexData(data);
	}

	size_t internalSet(ByteArray* ba) {
		return 0;
	}

public:
	template<typename Arg0, typename ... Args>
	ByteDynArray& set(Arg0 &&arg0, Args &&... args) {
		size_t totSize = internalSet((ByteArray*)nullptr, std::forward<Arg0>(arg0));

		size_t totSize2 = 0;
		int dummy[] = { 0, ((void)(totSize2 += internalSet((ByteArray*)nullptr, std::forward<Args>(args))), 0) ... };

		resize(totSize + totSize2);

		ByteArray buffer(*this);
		buffer = buffer.mid(internalSet(&buffer, std::forward<Arg0>(arg0)));
		int dummy2[] = { 0, ((void)(buffer = buffer.mid(internalSet(&buffer, std::forward<Args>(args)))), 0) ... };

		return *this;
	}

	ByteDynArray &setASN1Tag(unsigned int tag, ByteArray& content);
	void load(const char *fname);
};

#define VarToByteArray(a) (ByteArray((uint8_t*)&(a),sizeof(a)))
#define VarToByteDynArray(a) (ByteDynArray(VarToByteArray(a)))
#define ByteArrayToVar(a,b) (*(b*)(a).data())

