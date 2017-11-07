#pragma once
#include <vector>

bool CacheExists(const char *PAN);
void CacheGetCertificate(const char *PAN, std::vector<BYTE>&certificate);
void CacheGetPIN(const char *PAN, std::vector<BYTE>&PIN);
void CacheSetData(const char *PAN, BYTE *certificate, int certificateSize, BYTE *FirstPIN, int FirstPINSize);
