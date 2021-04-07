
#include "definitions.h"

//static unsigned long g_nErr;
#include <string>

using namespace std;

int atox(const char* szVal)
{
	int nVal = 0;

	if(szVal[1] >='0' && szVal[1] <= '9')
	{
		nVal = szVal[1] - '0';
	}
	else if(szVal[1] >='a' && szVal[1] <= 'f')
	{
		nVal = szVal[1] - 'a' + 10;
	}
	else if(szVal[1] >='A' && szVal[1] <= 'F')
	{
		nVal = szVal[1] - 'A' + 10;
	}
	else
	{
		throw(-1);
	}


	if(szVal[0] >='0' && szVal[0] <= '9')
	{
		nVal += (szVal[0] - '0') * 16;
	}
	else if(szVal[0] >='a' && szVal[0] <= 'f')
	{
		nVal += (szVal[0] - 'a' + 10) * 16;
	}
	else if(szVal[0] >='A' && szVal[0] <= 'F')
	{
		nVal += (szVal[0] - 'A' + 10) * 16;
	}
	else
	{
		throw(-1);
	}

	return nVal;
}

