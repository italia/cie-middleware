#pragma once

template <class T,typename P1=int,typename P2=int,typename P3=int,typename P4=int> class ArrayAllocator {
	T* pObject;
public:
	ArrayAllocator(DWORD size) {
		pObject=new T[size];
	}
	ArrayAllocator(DWORD size,P1 param1) {
		pObject=new T(param1)[size];
	}

	ArrayAllocator(DWORD size,P1 param1,P2 param2) {
		pObject=new T(param1,param2)[size];
	}

	ArrayAllocator(DWORD size,P1 param1,P2 param2,P3 param3) {
		pObject=new T(param1,param2,param3)[size];
	}

	ArrayAllocator(DWORD size,P1 param1,P2 param2,P3 param3,P4 param4) {
		pObject=new T(param1,param2,param3,param4)[size];
	}

	inline operator T* () {
		_return(pObject)
	}

	virtual ~ArrayAllocator() {
		if (pObject)
			delete(pObject);
	}
	
	inline T* detach() {
		T* pObj=pObject;
		pObject=NULL;
		_return(pObj)
	}
	
	inline T* operator -> () {
		_return(pObject)
	}
};

template <class T,typename P1=int,typename P2=int,typename P3=int,typename P4=int> class Allocator {
	T *pTempVal;
	T **ppObject;
public:
	Allocator(T *&pObj) : ppObject(&pObj) {
		pTempVal=NULL;
	}

	Allocator() : ppObject(&pTempVal) {
		//pTempVal=new T;
		pTempVal=new T;
	}

	Allocator(P1 param1) : ppObject(&pTempVal) {
		pTempVal=new T(param1);
	}

	Allocator(P1 param1,P2 param2) : ppObject(&pTempVal) {
		pTempVal=new T(param1,param2);
	}

	Allocator(P1 param1,P2 param2,P3 param3) : ppObject(&pTempVal) {
		pTempVal=new T(param1,param2,param3);
	}

	Allocator(P1 param1,P2 param2,P3 param3,P4 param4) : ppObject(&pTempVal) {
		pTempVal=new T(param1,param2,param3,param4);
	}

	inline operator T* () {
		return *ppObject;
	}

	virtual ~Allocator() {
		if (*ppObject)
			delete(*ppObject);
		*ppObject=NULL;
	}
	
	inline T* detach() {
		T* pObj=*ppObject;
		pTempVal=NULL;
		ppObject=&pTempVal;
		return(pObj);
	}
	
	inline T* operator -> () {
		return(*ppObject);
	}
};
