#pragma once

bool CacheExists(char *PAN);
void CacheGetCertificate(char *PAN, BYTE *&certificate, int &certificateSize);
void CacheGetPIN(char *PAN, BYTE *&PIN, int &PINSize);
void CacheSetData(char *PAN, BYTE *certificate, int certificateSize, BYTE *FirstPIN, int FirstPINSize);
void CacheFree(BYTE *data);
