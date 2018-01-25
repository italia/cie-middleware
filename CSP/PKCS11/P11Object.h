#pragma once

#include "session.h"
#include <map>

#pragma pack(1)
#include "pkcs11.h"
#pragma pack()

namespace p11 {

	typedef std::map <CK_ATTRIBUTE_TYPE, ByteDynArray> AttributeMap;

	class CSession;

	class CP11Object
	{
	public:
		bool bReadValue;
		static size_t P11ObjectCnt;

		CSlot *pSlot;
		void *pTemplateData; //dati specifici per il template della carta

		CP11Object(CK_OBJECT_CLASS objClass, void *TemplateData);
		CK_OBJECT_CLASS ObjClass;
		AttributeMap attributes;
		void addAttribute(CK_ATTRIBUTE_TYPE type, ByteArray &data);

		/// nullptr come valore di ritorno sognifica che l'attibuto non fa parte della mappa di attributi dell'oggetto
		virtual ByteArray* getAttribute(CK_ATTRIBUTE_TYPE type); 

		virtual void GetAttributeValue(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
		virtual void SetAttributes(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
		virtual CK_ULONG GetObjectSize();

		bool IsPrivate();
	};

	class CP11Certificate : public CP11Object
	{
	public:
		CP11Certificate(void *TemplateData);
		virtual ByteArray* getAttribute(CK_ATTRIBUTE_TYPE type);
		virtual void SetAttributes(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
	};

	class CP11Data : public CP11Object
	{
	public:
		CP11Data(void *TemplateData);
		virtual ByteArray* getAttribute(CK_ATTRIBUTE_TYPE type);
		virtual void SetAttributes(CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount);
	};

	class CP11PublicKey : public CP11Object
	{
	public:
		CP11PublicKey(void *TemplateData);
		virtual ByteArray* getAttribute(CK_ATTRIBUTE_TYPE type);
	};

	class CP11PrivateKey : public CP11Object
	{
	public:
		CP11PrivateKey(void *TemplateData);
		virtual ByteArray* getAttribute(CK_ATTRIBUTE_TYPE type);
	};

}