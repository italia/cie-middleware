#ifndef _CIE_ATR_H_
#define _CIE_ATR_H_

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

using namespace std;

enum CIE_Type {
	CIE_Unknown,
	CIE_Gemalto,
	CIE_STM,
	CIE_STM2,
	CIE_STM3,
	CIE_NXP,
	CIE_ACTALIS,
	CIE_BIT4ID
};

typedef struct _cie_atr {
	CIE_Type cie_type;
	string type;
	vector<uint8_t> atr;
}cie_atr;


string get_manufacturer(vector<uint8_t> atr);
CIE_Type get_type(vector<uint8_t> atr);

#endif // _CIE_ATR_H_

