#pragma once
#include <vector>
#include <stdint.h>

bool CacheExists(const char *PAN);
void CacheGetCertificate(const char *PAN, std::vector<uint8_t>&certificate);
void CacheGetPIN(const char *PAN, std::vector<uint8_t>&PIN);
void CacheSetData(const char *PAN, uint8_t *certificate, int certificateSize, uint8_t *FirstPIN, int FirstPINSize);
bool CacheRemove(const char *PAN);
