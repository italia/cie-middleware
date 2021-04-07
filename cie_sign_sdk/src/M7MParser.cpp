/*
 *  M7MParser.cpp
 *  iDigitalSApp
 *
 *  Created by svp on 05/09/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "M7MParser.h"
#include "Base64.h"

char* find(const char* szContent, int len, char* szSubstr);

M7MParser::M7MParser()
{
}

int M7MParser::Load(const char* m7m, int m7mlen)
{
	int len = m7mlen;

	const char* begin = strstr(m7m, "boundary=\"");
	if(begin == NULL)
		return -1;
	
	const char* end = strstr(begin + 10, "\"");//m7m.find("\"", begin+10);
	if(end == NULL)
		return -1;
	
	UUCByteArray boundary;
	boundary.append((BYTE*)"--", 2);
	boundary.append((BYTE*)begin + 10, (end - begin - 10));
	
	char* szBoundary = (char*)boundary.getContent();
	int boundadyLen = boundary.getLength();
	szBoundary[boundadyLen] = 0;

	begin = find(m7m, len, szBoundary);
	if(begin == NULL)
		return -1;
	
	begin += boundadyLen;
	
	len = m7mlen - (begin - m7m);

	end = find(begin, len, szBoundary);
	if(end == NULL)
		return -1;
	
	const char* firstPart = begin;
	int firstPartLen = end - begin;
	
	len = m7mlen - (end - m7m);

	begin = find(end, len, szBoundary);
	if(begin == NULL)
		return -1;
	
	begin += boundadyLen;
	
	len = m7mlen - (begin - m7m);

	end = find(begin + boundadyLen, len, szBoundary);
	if(end == NULL)
		return -1;

	char* secondPart = (char*)begin;
	int secondPartLen = end - begin;
    
	char* toFind = "\r\n\r\n";
	
	begin = strstr(firstPart, toFind);
	if(begin == NULL)
		return -1;
	
	begin += strlen(toFind);
	char* szContent = (char*)begin;
	int contentLen = firstPartLen - strlen(toFind);

	if(strstr(firstPart, "pkcs7-mime") != NULL)
		m_p7m.append((BYTE*)szContent, contentLen);
	else if(strstr(firstPart, "timestamp") != NULL)
		m_tsr.append((BYTE*)szContent, contentLen);

	end = 0;
	begin = strstr(secondPart, toFind);
	if(begin == NULL)
		return -1;
	
	end = secondPart + secondPartLen;
	begin += strlen(toFind);
	
	szContent = (char*)begin;
	contentLen = end - begin;
	
	if(strstr(secondPart, "pkcs7-mime") != NULL)
		m_p7m.append((BYTE*)szContent, contentLen);
	else if(strstr(secondPart, "timestamp") != NULL)
		m_tsr.append((BYTE*)szContent, contentLen);
	
	
	return 0;
	
}

int M7MParser::GetP7M(UUCByteArray& p7m)
{
	p7m.append(m_p7m);
	return 0;
}


int M7MParser::GetTSR(UUCByteArray& tsr)
{
	if(m_tsr.getContent()[0] != 0x30)
	{
		char* szB64 = (char*)m_tsr.getContent();
		szB64[m_tsr.getLength()] = 0;

		char* szEncoded = new char[m_tsr.getLength() + 1];
		strcpy(szEncoded, "");

		char* szPart = strtok(szB64, "\r\n");
		while(szPart)
		{
			strcat(szEncoded, szPart);
			szPart = strtok(NULL, "\r\n");
		}

		size_t decLen = base64_decoded_size(strlen(szEncoded));
		char* szDecoded = base64_decode(szEncoded);

		tsr.append((BYTE*)szDecoded, decLen);
	}
	else 
	{
		tsr.append(m_tsr);
	}

	return 0;
}

char* find(const  char* szContent, int len, char* szSubstr)
{
	int substrlen = strlen(szSubstr);

	for(int i = 0; i < len - substrlen; i++)
	{
		if(memcmp(szContent + i, szSubstr, substrlen) == 0)
			return (char*)(szContent + i);
	}

	return NULL;
}
