#pragma once
#include <vector>

bool CacheExists(char *PAN);
void CacheGetCertificate(char *PAN, std::vector<BYTE>&certificate);
void CacheGetPIN(char *PAN, std::vector<BYTE>&PIN);
void CacheSetData(char *PAN, BYTE *certificate, int certificateSize, BYTE *FirstPIN, int FirstPINSize);
