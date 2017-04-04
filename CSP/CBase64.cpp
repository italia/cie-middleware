
#include "stdafx.h"
#include "CBase64.h"
#include <string.h>

//#define USE_CRLF


CBase64Ex::CBase64Ex ()
	{
	}

unsigned int CBase64Ex::CalculateRecquiredEncodeOutputBufferSize (unsigned int p_InputByteCount)
	{
	div_t result = div (p_InputByteCount, 3);

	unsigned int RecquiredBytes = 0;
	if (result.rem == 0)
		{
		// Number of encoded characters
		RecquiredBytes = result.quot * 4;

		// CRLF -> "\r\n" each 76 characters
		result = div (RecquiredBytes, 76);

#ifdef USE_CRLF		
		RecquiredBytes += result.quot * 2;
#else
		RecquiredBytes += result.quot;
#endif
		// Terminating null for the Encoded String
		RecquiredBytes += 1;

		return RecquiredBytes;
		}
	else
		{
		// Number of encoded characters
		RecquiredBytes = result.quot * 4 + 4;

		// CRLF -> "\r\n" each 76 characters
		result = div (RecquiredBytes, 76);

#ifdef USE_CRLF		
		RecquiredBytes += result.quot * 2;
#else
		RecquiredBytes += result.quot;
#endif

		// Terminating null for the Encoded String
		RecquiredBytes += 1;

		return RecquiredBytes;
		}
	}

unsigned int CBase64Ex::CalculateRecquiredDecodeOutputBufferSize (const char* p_pInputBufferString)
	{
	unsigned int BufferLength = strlen (p_pInputBufferString);
	int execBufSize=0;
	int lastsig=0;
	for (int i=0;i<BufferLength;i++) {
		if ((p_pInputBufferString [i] >= 48 && p_pInputBufferString [i] <=  57) ||
			(p_pInputBufferString [i] >= 65 && p_pInputBufferString [i] <=  90) ||
			(p_pInputBufferString [i] >= 97 && p_pInputBufferString [i] <= 122) ||
			p_pInputBufferString [i] == '+' || p_pInputBufferString [i] == '/' || p_pInputBufferString [i] == '=') {
			 execBufSize++;
			 lastsig=i;
		}
	}

	lastsig++;
	div_t result = div (execBufSize, 4);

	if (p_pInputBufferString [lastsig - 1] != '=')
		{
		return result.quot * 3;
		}
	else
		{
		if (p_pInputBufferString [lastsig - 2] == '=')
			{
			return result.quot * 3 - 2;
			}
		else
			{
			return result.quot * 3 - 1;
			}
		}
	}

void CBase64Ex::EncodeByteTriple  (const char* p_pInputBuffer, unsigned int InputCharacters, char* p_pOutputBuffer)
	{
	unsigned int mask = 0xfc000000;
	unsigned int buffer = 0;


	char* temp = (char*) &buffer;
	temp [3] = p_pInputBuffer [0];
	if (InputCharacters > 1)
		temp [2] = p_pInputBuffer [1];
	if (InputCharacters > 2)
		temp [1] = p_pInputBuffer [2];

	switch (InputCharacters)
		{
		case 3:
			{
			p_pOutputBuffer [0] = BASE64_ALPHABET [(buffer & mask) >> 26];
			buffer = buffer << 6;
			p_pOutputBuffer [1] = BASE64_ALPHABET [(buffer & mask) >> 26];
			buffer = buffer << 6;
			p_pOutputBuffer [2] = BASE64_ALPHABET [(buffer & mask) >> 26];
			buffer = buffer << 6;
			p_pOutputBuffer [3] = BASE64_ALPHABET [(buffer & mask) >> 26];
			break;
			}
		case 2:
			{
			p_pOutputBuffer [0] = BASE64_ALPHABET [(buffer & mask) >> 26];
			buffer = buffer << 6;
			p_pOutputBuffer [1] = BASE64_ALPHABET [(buffer & mask) >> 26];
			buffer = buffer << 6;
			p_pOutputBuffer [2] = BASE64_ALPHABET [(buffer & mask) >> 26];
			p_pOutputBuffer [3] = '=';
			break;
			}
		case 1:
			{
			p_pOutputBuffer [0] = BASE64_ALPHABET [(buffer & mask) >> 26];
			buffer = buffer << 6;
			p_pOutputBuffer [1] = BASE64_ALPHABET [(buffer & mask) >> 26];
			p_pOutputBuffer [2] = '=';
			p_pOutputBuffer [3] = '=';
			break;
			}
		}
	}

unsigned int CBase64Ex::DecodeByteQuartet (const char* p_pInputBuffer, char* p_pOutputBuffer)
	{
	unsigned int buffer = 0;

	if (p_pInputBuffer[3] == '=')
		{
		if (p_pInputBuffer[2] == '=')
			{
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[0]]) << 6;
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[1]]) << 6;
			buffer = buffer << 14;

			char* temp = (char*) &buffer;
			p_pOutputBuffer [0] = temp [3];
			
			return 1;
			}
		else
			{
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[0]]) << 6;
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[1]]) << 6;
			buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[2]]) << 6;
			buffer = buffer << 8;

			char* temp = (char*) &buffer;
			p_pOutputBuffer [0] = temp [3];
			p_pOutputBuffer [1] = temp [2];
			
			return 2;
			}
		}
	else
		{
		buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[0]]) << 6;
		buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[1]]) << 6;
		buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[2]]) << 6;
		buffer = (buffer | BASE64_DEALPHABET [p_pInputBuffer[3]]) << 6; 
		buffer = buffer << 2;

		char* temp = (char*) &buffer;
		p_pOutputBuffer [0] = temp [3];
		p_pOutputBuffer [1] = temp [2];
		p_pOutputBuffer [2] = temp [1];

		return 3;
		}

	return -1;
	}

void CBase64Ex::EncodeBuffer (const char* p_pInputBuffer, unsigned int p_InputBufferLength, char* p_pOutputBufferString)
	{
	unsigned int FinishedByteQuartetsPerLine = 0;
	unsigned int InputBufferIndex  = 0;
	unsigned int OutputBufferIndex = 0;

	memset (p_pOutputBufferString, 0, CalculateRecquiredEncodeOutputBufferSize (p_InputBufferLength));

	while (InputBufferIndex < p_InputBufferLength)
		{
		if (p_InputBufferLength - InputBufferIndex <= 2)
			{
			FinishedByteQuartetsPerLine ++;
			EncodeByteTriple (p_pInputBuffer + InputBufferIndex, p_InputBufferLength - InputBufferIndex, p_pOutputBufferString + OutputBufferIndex);
			break;
			}
		else
			{
			FinishedByteQuartetsPerLine++;
			EncodeByteTriple (p_pInputBuffer + InputBufferIndex, 3, p_pOutputBufferString + OutputBufferIndex);
			InputBufferIndex  += 3;
			OutputBufferIndex += 4;
			}

		if (FinishedByteQuartetsPerLine == 19)
			{
#ifdef USE_CRLF
			p_pOutputBufferString [OutputBufferIndex  ] = '\r';
			p_pOutputBufferString [OutputBufferIndex+1] = '\n';
			p_pOutputBufferString += 2;
#else
			p_pOutputBufferString [OutputBufferIndex] = '\n';
			p_pOutputBufferString++;
#endif
			FinishedByteQuartetsPerLine = 0;
			}
		}
	}

unsigned int CBase64Ex::DecodeBuffer (const char* p_pInputBufferString, char* p_pOutputBuffer)
	{
	unsigned int InputBufferIndex  = 0;
	unsigned int OutputBufferIndex = 0;
	unsigned int InputBufferLength = strlen (p_pInputBufferString);

	char ByteQuartet [5];

	while (InputBufferIndex < InputBufferLength)
		{
		int i;
		for (i = 0; i < 5; i++)
			{
			ByteQuartet [i] = p_pInputBufferString [InputBufferIndex];

			// Ignore all characters except the ones in BASE64_ALPHABET
			if ((ByteQuartet [i] >= 48 && ByteQuartet [i] <=  57) ||
				(ByteQuartet [i] >= 65 && ByteQuartet [i] <=  90) ||
				(ByteQuartet [i] >= 97 && ByteQuartet [i] <= 122) ||
				 ByteQuartet [i] == '+' || ByteQuartet [i] == '/' || ByteQuartet [i] == '=')
				{
				}
			else
				{
				// Invalid character
				i--;
				}

			InputBufferIndex++;
			if (InputBufferIndex==InputBufferLength) break;
			}

			OutputBufferIndex += DecodeByteQuartet (ByteQuartet, p_pOutputBuffer + OutputBufferIndex);
			if (i==5) 
				InputBufferIndex--;
		}

	// OutputBufferIndex gives us the next position of the next decoded character
	// inside our output buffer and thus represents the number of decoded characters
	// in our buffer.
	return OutputBufferIndex;
	}

unsigned int CBase64Ex::CreateMatchingEncodingBuffer (unsigned int p_InputByteCount, char** p_ppEncodingBuffer)
	{
	unsigned int Size = CalculateRecquiredEncodeOutputBufferSize (p_InputByteCount);
	(*p_ppEncodingBuffer) = new char[Size];
	memset (*p_ppEncodingBuffer, 0, Size);
	return Size;
	}

unsigned int CBase64Ex::CreateMatchingDecodingBuffer (const char* p_pInputBufferString, char** p_ppDecodingBuffer)
	{
	unsigned int Size = CalculateRecquiredDecodeOutputBufferSize (p_pInputBufferString);
	(*p_ppDecodingBuffer) = new char[Size];
	memset (*p_ppDecodingBuffer, 0, Size);
	return Size;
	}

