#include "TrustServiceList.h"
#include "Base64.h"
#include "UUCLogger.h"
#include <curl/curl.h>
#include <algorithm>
#include <cctype>
#include <ctime> 

#ifdef _WIN32
#include <shlobj.h> 
#include <windows.h> 
#endif

USE_LOG;

const char* TSL_IT_URL = "https://eidas.agid.gov.it/TL/TSL-IT.xml";

static const char CIE_CA_BUNDLE_PEM[] = R"(-----BEGIN CERTIFICATE-----
MIIG4DCCBMigAwIBAgIIRmop7bWMajUwDQYJKoZIhvcNAQELBQAwgasxQjBABgNV
BAMMOU5hdGlvbmFsIHJvb3QgQ0EgZm9yIHRoZSBJdGFsaWFuIEVsZWN0cm9uaWMg
SWRlbnRpdHkgQ2FyZDE3MDUGA1UECwwuRGlyZXouIENlbnRyLiBwZXIgaSBTZXJ2
aXppIERlbW9ncmFmaWNpIC0gQ05TRDEfMB0GA1UECgwWTWluaXN0ZXJvIGRlbGwn
SW50ZXJubzELMAkGA1UEBhMCSVQwHhcNMTYwNjA2MTQzNzI0WhcNMzYwNjA2MTQz
NzI0WjCBqzFCMEAGA1UEAww5TmF0aW9uYWwgcm9vdCBDQSBmb3IgdGhlIEl0YWxp
YW4gRWxlY3Ryb25pYyBJZGVudGl0eSBDYXJkMTcwNQYDVQQLDC5EaXJlei4gQ2Vu
dHIuIHBlciBpIFNlcnZpemkgRGVtb2dyYWZpY2kgLSBDTlNEMR8wHQYDVQQKDBZN
aW5pc3Rlcm8gZGVsbCdJbnRlcm5vMQswCQYDVQQGEwJJVDCCAiIwDQYJKoZIhvcN
AQEBBQADggIPADCCAgoCggIBAMKQoMWIoz5V+cTvVu+rAU4GXsrhCHBGSiPluVwE
DwrLa4VmUYcBpKV8bJMmSelbjE8TGtCvIdb3HmrK72ts/9cKIJItgMLEH4DWIO72
F1EW4oYFZ7V4QalK3x5AIk6YCIw+PLYiv+i/C/7z+OwMeOyaPDayPK7bwwBloJHS
/IwHy1eQLqZ58zyCNyjBAmtWQgqP6rLTFG44hRk6jCsHSBuMSe36qs21ZaA3qrui
tRVL2BMIe1+VnudwI+XWhrTaoa5UuZJygE52w7xbGmpv+Q6TapjVPc7s/l7aLBAv
lo/KW3zDWZ8Joj0qftAFWCRI+Qbuw8GR6lvkp7vm85ymUd9nS/Izo0iEsDyppED/
CuG0+n+y7CfGlZ6PqKM0WOeG856oH/BbuMrhPZ68w2S/0tgLFWcOLwFkBJubVTFW
5sWrYmgpxtdjj+8fpFmxstPXOA3DOt6NTEUOeT2xFYn323FpsQm+LV6tTzUcl/T2
ud9dw0ede5ucxI6caLR3MvaChqErUyZF0F09x3sKkedP8tHNutsvEjtgnkhTlypG
Fh+clAPFPJlwTFkp9VB2ULa7giOq8+iEqj7LApB1tp/vgR1qAxTNhWTJpegdrfjF
vLO+ApY0a6NNxKjXe19pPx32YWuGcGOp8At1KRTvBrW1qWlrUn6j99SDcTHjEwmV
54nHAgMBAAGjggEEMIIBADAdBgNVHQ4EFgQUR8qRudhBHPfA4cUWUD454S+L8jUw
DwYDVR0TAQH/BAUwAwEB/zAfBgNVHSMEGDAWgBRHypG52EEc98DhxRZQPjnhL4vy
NTBeBgNVHSAEVzBVMFMGBCtMLwEwSzBJBggrBgEFBQcCARY9aHR0cDovL3d3dy5j
YXJ0YWlkZW50aXRhLmludGVybm8uZ292Lml0L3BvbGljeS9yb290Y2FfY3BzLnBk
ZjA9BgNVHR8ENjA0MDKgMKAuhixodHRwOi8vbGRhcC5jaWUuaW50ZXJuby5nb3Yu
aXQvY2llcm9vdGNhLmNybDAOBgNVHQ8BAf8EBAMCAYYwDQYJKoZIhvcNAQELBQAD
ggIBAHzFm/7B4+D1bJWUM38mL2UD6SNr11RVm9ohHPx586rb9tDiea7wgTWBBLyZ
WpdbDWQfSU+13kSsMUvZH60Fg/A95WKrIGo29fjFdHkJrrJm0Tc9CcMLLULWyQBy
oJAv/01NYykGzdgyAJbdkvWTihhlZBpFZ2GMQT4g099Xd6iWXiEaKcDDMh9o8PU0
fZy0L274HE6ql5P5xdiqfv3dyiEM+eZ5OEMKOkkJPtlXX/H20g0AkUZpFlfqC6mh
uEad28YH3OlTJyl7sEYNPqjEp9UvcdD9XpAVr/GxGHf96H4Ozqu9ZQFCG6E5aMMY
XOxhXHNnRpnwkao1edaomxJTDtcDSdFE7Bp0fKRUpSVUpLbYcAda0/kgl7s/bZQl
xZKNhSqOZuQA91LKYB3CwXEHblJP9j45Em86J0rwfpB28Cb9ePNCkHaoSkJ25PtK
zNgKtikkGZCMmYpdiyQELuoBZHtxJ+Jo3tmc6Wy9WF9FzPYyKrC1d+NJdF8477zC
pXk8l5z5zDoFt5H30Xdz9x+toehpemDABBvbNd+jSfwpay6O7qr8cnr7O+9uBrzK
Yv5yjgxBQ7KVF367aEXYdplSzJzlj+dl/sqHzyeTcw7CaMptYy5XW9JbmkvaxWX2
vK8/rpnLv9Y9hX1KmmFMAK3PXgTw8Bym3+nM8Qc76Sx/LOHL
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIG7jCCBNagAwIBAgIIRs2LVrWtjRswDQYJKoZIhvcNAQELBQAwgasxQjBABgNV
BAMMOU5hdGlvbmFsIHJvb3QgQ0EgZm9yIHRoZSBJdGFsaWFuIEVsZWN0cm9uaWMg
SWRlbnRpdHkgQ2FyZDE3MDUGA1UECwwuRGlyZXouIENlbnRyLiBwZXIgaSBTZXJ2
aXppIERlbW9ncmFmaWNpIC0gQ05TRDEfMB0GA1UECgwWTWluaXN0ZXJvIGRlbGwn
SW50ZXJubzELMAkGA1UEBhMCSVQwHhcNMTYwNjA2MTYxNjQxWhcNMzEwNjAzMTYx
NjQxWjCBsjFJMEcGA1UEAwxASXNzdWluZyBzdWIgQ0EgZm9yIHRoZSBJdGFsaWFu
IEVsZWN0cm9uaWMgSWRlbnRpdHkgQ2FyZCAtIFNVQkNBMTE3MDUGA1UECwwuRGly
ZXouIENlbnRyLiBwZXIgaSBTZXJ2aXppIERlbW9ncmFmaWNpIC0gQ05TRDEfMB0G
A1UECgwWTWluaXN0ZXJvIGRlbGwnSW50ZXJubzELMAkGA1UEBhMCSVQwggIiMA0G
CSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQC3Sk0E2bwMzrp2oRZ52be8Z37f8osu
LwKHt4PTysD+lHyGi3/P37omXUzZewZ/SuSkDY0LzgloMjqiZdfVvobQ/rN+ICwg
cfB6Kf2fFPqN1mPTeJLI537kemkN/qwHIZO+HOlmklY+nJ/HrlVQJ6dxrfd8hWLp
XR4L2yTtqhB2bMrZGN+EtsiML8DetRM/i4/AV5UYTOrWEbexnKgwwATu9TgDSk+f
lqwB6R24islNbfDlnmm+XXaaba3Jflajk5vAt8cYyVkYx6QCwVDbifHWEG52z7GW
XD/TkDIV2FPm6qTCQdBPfEbmXfTpIcu8sN+gIQMlb/fbcsqi9liw26pxO4RlvO7K
74GbcTgUY4q7iG+SzNkXXLXRDlLsjloA+y3j/TPM0RwBSsnYghDrcxyq8KXNuKk6
JXtv5N68Qttr+SVYhE3EvMUeUJqgp1s1siDKswcuIasl07HlcgYZVC6joEg9xlEX
IcCMFibyNIjrm+oybA1srZ9lqf+14Z9ERGacJJTGy3Mkl7UxWNBeyK3DT15j8pZY
wVIvJBfNX8x+r59huVQCOqxwvCe2AHIAqPkuLSjqVFFek8q6l2az3mcY2mgE8Avu
Du6rilJEq2ZU3sGBM3ep7yuKZErwlU0sXlqxMvoDe44/rU8Puan+4/OI9+VDaHi4
P6RxTTcU4DkQfwIDAQABo4IBCzCCAQcwHQYDVR0OBBYEFOJDidPiBHX9JjL5SkxB
3NmJPuTBMBIGA1UdEwEB/wQIMAYBAf8CAQAwHwYDVR0jBBgwFoAUR8qRudhBHPfA
4cUWUD454S+L8jUwYgYDVR0gBFswWTBXBgQrTC8CME8wTQYIKwYBBQUHAgEWQWh0
dHA6Ly93d3cuY2FydGFpZGVudGl0YS5pbnRlcm5vLmdvdi5pdC9wb2xpY3kvc3Vi
Y2FfYXV0aF9jcHMucGRmMD0GA1UdHwQ2MDQwMqAwoC6GLGh0dHA6Ly9sZGFwLmNp
ZS5pbnRlcm5vLmdvdi5pdC9jaWVzdWJjYTEuY3JsMA4GA1UdDwEB/wQEAwIBhjAN
BgkqhkiG9w0BAQsFAAOCAgEAD+TJp+WNJQFfyCzlmPKNXsUTHiARHgLp5u08Rm5j
DO1ffSp3YAWQ0u4BIy5HzCRxnQReJcDe3sR+dxnAh+6DW69pu5zxJkBNhgw+J7mG
izyQ7moHxMZeYGMgKSGzHOQp84OA7vb2km4J8U90iJIEbfWo1o0qrFoFlyW/Xsts
wevyy7/G6uGccomwuJS/oklw1Jx8jIxAD+4KPfhg7MEO1Ae8CJ2i4bQdK+oRpCMH
Mxog23C9Ry3AqNRyQDn4HQ7gk2MgxlOjxfdrYE6u1YtV2mliBoOVmI6U10BzsqK5
mKbj+mNVG0wTTNlIbeqqQsnGqA2D2XA/l0p8gGG0dnjK+Y1bishGX9nI6c868r24
WYu1Xjw2XgjFeJzyL78iOVDxyewRjpgZhEuIo/DPyW/SkMH4JkvABa/CmNIe3Eua
E1/SkXMePWG73ZOmh4cytQ8yyMNviUqU274EQMhtWx3XFvfDB0eu+E9u3S5dqVaa
nArroE60WFp/Bl2dNDHJgis3CewlZLbCRxc4rH0g8Ipfqe0Wte9uvhLZhColvbZ/
SEXCp2yUzFp3UYmsmRxyVlcFGwDF4/6ZXXhsrWTH/0jV8Or9zV598Tv7Lo5gz7IA
+GI2BpZd96tz2+ItNxLdRl0ZVavpJYeL/6dQ/OyHUnGJrSJUq/3BKLtOgMTmEOpX
RUE=
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIG8DCCBNigAwIBAgIIbidwj1LWzEAwDQYJKoZIhvcNAQELBQAwgasxQjBABgNV
BAMMOU5hdGlvbmFsIHJvb3QgQ0EgZm9yIHRoZSBJdGFsaWFuIEVsZWN0cm9uaWMg
SWRlbnRpdHkgQ2FyZDE3MDUGA1UECwwuRGlyZXouIENlbnRyLiBwZXIgaSBTZXJ2
aXppIERlbW9ncmFmaWNpIC0gQ05TRDEfMB0GA1UECgwWTWluaXN0ZXJvIGRlbGwn
SW50ZXJubzELMAkGA1UEBhMCSVQwHhcNMjAwNTI1MDYxNDMyWhcNMzUwNTIyMDYx
NDMyWjCBtDFLMEkGA1UEAwxCSXNzdWluZyBzdWIgQ0EgZm9yIHRoZSBJdGFsaWFu
IEVsZWN0cm9uaWMgSWRlbnRpdHkgQ2FyZCAtIFNVQkNBMDAyMTcwNQYDVQQLDC5E
aXJlei4gQ2VudHIuIHBlciBpIFNlcnZpemkgRGVtb2dyYWZpY2kgLSBDTlNEMR8w
HQYDVQQKDBZNaW5pc3Rlcm8gZGVsbCdJbnRlcm5vMQswCQYDVQQGEwJJVDCCAiIw
DQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAJl0P3lxsxXbmvW/i6b4HOrzEzSw
mkmjhWcCgm8zykHw5Zm9yA8ex3fhsPTQaw/to1T2183kapcg/lnXnR2Rx5v3VvvR
JODs6kMzWIQ/elJjguitlsPrjmjgsIXYBI+Re8ITHP0J783jHhWM7kbhJ8XAjk39
mhX/NFx6mLgOBqdep/QAkldV8s2EApodclvdB42QfrkPd3yWZmo7jP5Jp5QJ5Lms
ThWXCmXKCPcAiXY1Z/NFWK32DmjrabBRJm2YyvOsXQVjiyjYjowwVIG0UeF0ZGBC
F0Wd7jjIghyivTi/hMWXOy++XsfmgCS5GD6ylnsSP+vJXmOFS6aINkTpUlfYHw35
VRU9RFIt9qunKaLJA+BgvHZxGLcfhe6mveLalunCH7KLip0oVDO5qw2/ct5kX/Pl
cDlGI97eWWI2EsHmaHeHns++n1RuRRpmHlF5cb92dF3ciFKgmkVKK3xHlG+j0++A
usJMKOSJZl2n8vL2YzeCWWcbidZeze/q5Mq0H+snRTKq9dnjfwln+81Fvh9GjtWv
iDgJ/Trd6CRrMEUhbCi3drpVvYTT2na9HVikLD44CurZgcOYCaPlf47hjKQwNT0d
BtvZdF6ThwJz9HFOTMHUTiaXD4vEC6kLRnAA1nL7ticLN7MfRECMK+/8c8hgHUe5
RB1DXiYfTus2WOGzAgMBAAGjggELMIIBBzAdBgNVHQ4EFgQU+S7pCGSforgrqDKV
QXPpfZ0MFRgwEgYDVR0TAQH/BAgwBgEB/wIBADAfBgNVHSMEGDAWgBRHypG52EEc
98DhxRZQPjnhL4vyNTBiBgNVHSAEWzBZMFcGBCtMLwIwTzBNBggrBgEFBQcCARZB
aHR0cDovL3d3dy5jYXJ0YWlkZW50aXRhLmludGVybm8uZ292Lml0L3BvbGljeS9z
dWJjYV9hdXRoX2Nwcy5wZGYwPQYDVR0fBDYwNDAyoDCgLoYsaHR0cDovL2xkYXAu
Y2llLmludGVybm8uZ292Lml0L2NpZXJvb3RjYS5jcmwwDgYDVR0PAQH/BAQDAgGG
MA0GCSqGSIb3DQEBCwUAA4ICAQBVmLny00jZq9Zop2SfljcgUtZ2WAZVUcI1tsXb
NRKsarhgqg1G3HKv9jufqxYoL+gfAOa5jRoPmqvWGuSBzM9fSMu8bURDdSljoTos
4OR+8hgEYz4NoDTHqyXxXgwGVcGRSz8jppFUQOFur9yScx+mGY1XPGiO3FBDwROu
2jhIT3eFBbTq0369lP2GI+qAgIkeoggjwR1u1WNasoCDeLViDOrWvSOB3/FPQIhG
VWhsJnIGq8zJQGfuo/P8ae33CleB69aZIsR3TuPQ17tWg5zG6Zbq7f/9p02pTm6E
rj0TLtPJqMoPLloW/LmO5EfB9BcTYT187vogjZ3M/ulrSM/Zm4bb3zHgPBNRY0Nl
27Mw7E9swaCXeIJySLGv/aS4o4cNqkFDJH5btrDrShQBGD9ANi77LlSlEQ4FmzY1
X4UZk5enWQ1A/uOylLrtfG3puXcekdcxTneCdkbAr+yo83Pzba/ZX7kec/tmnoL8
QfPFAbrQx2wnxK9AmxQmvxwAm9GmkRIe8TFgAWODIPiA/rRMVI2TvI37xGWWFZNO
xkgPY5uSajZuOArTrjDUGHqalKQSksylIZ65i4sUXsHGNyH0OFt7ZYAfC0YIl+f6
2yF2MwtqVv7bZB/GF7vb1JZGj9hqTUGVRjz0MFruMBsMhhAUUykCTegIq5C/MFcC
aJQEPQ==
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIG8DCCBNigAwIBAgIIJJZNjbfc1rcwDQYJKoZIhvcNAQELBQAwgasxQjBABgNV
BAMMOU5hdGlvbmFsIHJvb3QgQ0EgZm9yIHRoZSBJdGFsaWFuIEVsZWN0cm9uaWMg
SWRlbnRpdHkgQ2FyZDE3MDUGA1UECwwuRGlyZXouIENlbnRyLiBwZXIgaSBTZXJ2
aXppIERlbW9ncmFmaWNpIC0gQ05TRDEfMB0GA1UECgwWTWluaXN0ZXJvIGRlbGwn
SW50ZXJubzELMAkGA1UEBhMCSVQwHhcNMjQwNTE2MTQwMzM3WhcNMzkwNTEzMTQw
MzM3WjCBtDFLMEkGA1UEAwxCSXNzdWluZyBzdWIgQ0EgZm9yIHRoZSBJdGFsaWFu
IEVsZWN0cm9uaWMgSWRlbnRpdHkgQ2FyZCAtIFNVQkNBMDAzMTcwNQYDVQQLDC5E
aXJlei4gQ2VudHIuIHBlciBpIFNlcnZpemkgRGVtb2dyYWZpY2kgLSBDTlNEMR8w
HQYDVQQKDBZNaW5pc3Rlcm8gZGVsbCdJbnRlcm5vMQswCQYDVQQGEwJJVDCCAiIw
DQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAM+DgWkGOnWDMPtesNeqhW62p2ch
jnx181XY1iJ88rYVQj9ptLGFibURGbAFaSJ1uCniSxrMPByliDkadHkxMjTr+9te
Zu3kPnOoV2pgR78Tn8RLG8BnruX4riuaP1vyKKVFEoO7Td7a075v6i0ipYyZtZ0t
wrtpAD2ZQbrTdUCy3HuWdn0CD7q3YDGskIYfIbFI1R3zgpxM4wSbCO5bqSxiyk2i
MDbm/cQ/b0eX8shXdNhNPrN/imR4w1rZwzSp0JNH1Madf3D9YdAUDUTCQG405lVL
4ha0pUlG+g0Ig0BHDLxBAO6iZ7CZY+YcgV945FqosiBrH1Y8PFmN1VBlFKn351hZ
OAfgnLj1Y6gGgLz2+WUNWZpGDfdPhODWhmffqzeTGihuOgIzfQG3wL1uQ6aeTqQk
cDECs/Tll6lQW6WVqUdFGw8h4hHKHdAizTDi+/vYlGYUceuNCYM+woE+Omvwo8+1
BcqFpVO0YAxlmzVl/HN8kRvoaiwIy6XH5GON0I8aPSFsCqXl0tou8llpdueI6R33
COn6r7O3VjQNXJEpUesHpi9vTuIPaP9W4OCRO9wjrIot6QtmQKTpoN0tpdDsMLdQ
5//zdRG8D4qz3dH0kFEqr5li1rXOUnK79SAygYkdLuD7PRFUyUkVuAbMrn/snLAG
LaUpx0FtcpzQHQojAgMBAAGjggELMIIBBzASBgNVHRMBAf8ECDAGAQH/AgEAMB8G
A1UdIwQYMBaAFEdvJRrD1fP+XjSTEp/YgRXcUgxPMGIGA1UdIARbMFkwVwYEK0wv
AjBPME0GCCsGAQUFBwIBFkFodHRwOi8vd3d3LmNhcnRhaWRlbnRpdGEuaW50ZXJu
by5nb3YuaXQvcG9saWN5L3N1YmNhX2F1dGhfY3BzLnBkZjA9BgNVHR8ENjA0MDKg
MKAuhixodHRwOi8vbGRhcC5jaWUuaW50ZXJuby5nb3YuaXQvY2llcm9vdGNhLmNy
bDAdBgNVHQ4EFgQU+LbUqPVMY/CaGMSqUumS65tDx+EwDgYDVR0PAQH/BAQDAgGG
MA0GCSqGSIb3DQEBCwUAA4ICAQBsWaBdJkl1YU217X4HChaW3PYV0ocii+MFkTwx
DmAKqGXTEWbPZSz00NyHDORMKY/cx0s3eNZ14caed/KBbdgkEfl3Wymq9mWSWnjI
bB39d4L1XalZWOquA6iQMrM3+9Aa/cOwj63rxYeDHjDRwW5OETSk47WTdJoMU3c2
1kbXUSlIX3VgW+8pX5KDEMwQobuIyS2TUpc10DV2MbDDZFeaUePVXD+xuBkCJ4LR
6yXsWdad8NPRX0SK/dtOB0DSdPM7HpNMeurNF6KdxHK3w5rbt0n8n129NmPT4lnx
Aw1U0wZULU29gu9WhadlwbSLxvXSfsh4dLLwyLL4PwjBgIYYGkLt9n/JQD5XF2BM
94LOpdv5VBa6nhaCralW9npc1djK02CgINROFR+rwFhaWPptYZmnMVX0Pw9Qe8X+
Nrml576sJydJCh+Obc/auwbpjW9oTibvdqTeYAk7DGyitOSI3GZSQEccfaWxxcL2
t6hYmMXswMLrG8THL++8D7fBo01kV5V2sdhJ2NdFeKvylGfQBU6kmAGtEkq5EElz
3UhfArjj7d31FBPM6TKEmmMi5vJJ1IqyjxIHfHtRnRFjQadPxjf2Zdo3nsgCvL/5
HjcTFIj2d6ZJIrUMBLX/q/aQRsaEZciEWUDZV9LCfnmEmgPd3YEr5Lqqxt+9Ej7R
t+VL1w==
-----END CERTIFICATE-----
)";

CTrustServiceList* CTrustServiceList::s_pInstance = NULL;

extern long HTTPRequest(UUCByteArray& data, const char* szUrl, const char* szContentType, UUCByteArray& response);

CTrustServiceList::CTrustServiceList()
{
}

CTrustServiceList::~CTrustServiceList()
{
    for(auto provider : m_providers)
        delete provider;
}

bool CTrustServiceList::Initialize()
{
    if(s_pInstance)
        return true;
    
    LOG_DBG((0, "CTrustServiceList::Initialize", "Initializing TSL"));
    
    s_pInstance = new CTrustServiceList();
    
    char szCacheFile[MAX_PATH];
    
#ifdef _WIN32
    char szAppData[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szAppData)))
    {
        char szCIEPKIDir[MAX_PATH];
        sprintf_s(szCIEPKIDir, MAX_PATH, "%s\\CIEPKI", szAppData);
        
        DWORD dwAttrib = GetFileAttributesA(szCIEPKIDir);
        if(dwAttrib == INVALID_FILE_ATTRIBUTES)
        {
            if(!CreateDirectoryA(szCIEPKIDir, NULL))
            {
                LOG_ERR((0, "CTrustServiceList::Initialize", "Failed to create CIEPKI directory"));
            }
        }
        
        sprintf_s(szCacheFile, MAX_PATH, "%s\\tsl_cache.dat", szCIEPKIDir);
        LOG_DBG((0, "CTrustServiceList::Initialize", "Cache file path: %s", szCacheFile));
    }
    else
    {
        LOG_ERR((0, "CTrustServiceList::Initialize", "Failed to get PROGRAMDATA path"));
        strcpy_s(szCacheFile, MAX_PATH, "tsl_cache.dat");
    }
#else
    strcpy(szCacheFile, "/tmp/tsl_cache.dat");
#endif
    
    if(s_pInstance->loadFromCache(szCacheFile))
    {
        if(!s_pInstance->isExpired())
        {
            LOG_DBG((0, "CTrustServiceList::Initialize", "Loaded valid TSL from cache"));
            return true;
        }
        else
        {
            LOG_DBG((0, "CTrustServiceList::Initialize", "Cache expired, downloading new TSL"));
            
            for(auto provider : s_pInstance->m_providers)
                delete provider;
            s_pInstance->m_providers.clear();
        }
    }
    
    if(!s_pInstance->downloadAndParse(TSL_IT_URL))
    {
        LOG_ERR((0, "CTrustServiceList::Initialize", "Failed to download TSL"));
        delete s_pInstance;
        s_pInstance = NULL;
        return false;
    }
    
    if(!s_pInstance->saveToCache(szCacheFile))
    {
        LOG_WAR((0, "CTrustServiceList::Initialize", "Failed to save TSL cache (non-blocking)"));
    }
    
    return true;
}

void CTrustServiceList::Cleanup()
{
    if(s_pInstance)
    {
        delete s_pInstance;
        s_pInstance = NULL;
    }
}

bool CTrustServiceList::downloadAndParse(const char* szTSLUrl)
{
    LOG_DBG((0, "CTrustServiceList::downloadAndParse", "Downloading TSL from: %s", szTSLUrl));
    
    m_tslURL = szTSLUrl;
    
    UUCByteArray request;
    UUCByteArray response;
    
    long nRet = HTTPRequest(request, szTSLUrl, NULL, response);
    if(nRet != 0)
    {
        LOG_ERR((0, "CTrustServiceList::downloadAndParse", "Failed to download TSL. Error: %d", nRet));
        return false;
    }
    
    if(response.getLength() == 0)
    {
        LOG_ERR((0, "CTrustServiceList::downloadAndParse", "Empty TSL response"));
        return false;
    }
    
    response.append((BYTE)'\0');
    string xmlContent((char*)response.getContent());
    
    LOG_DBG((0, "CTrustServiceList::downloadAndParse", "Downloaded %d bytes", response.getLength()));
    
    return parseXML(xmlContent);
}

string CTrustServiceList::extractTagContent(const string& xml, const string& tagName, size_t startPos)
{
    string openTag = "<" + tagName;
    string closeTag = "</" + tagName + ">";
    
    size_t start = xml.find(openTag, startPos);
    if(start == string::npos)
        return "";
    
    start = xml.find(">", start);
    if(start == string::npos)
        return "";
    start++; // Skip '>'
    
    size_t end = xml.find(closeTag, start);
    if(end == string::npos)
        return "";
    
    return xml.substr(start, end - start);
}

vector<string> CTrustServiceList::extractAllTagContents(const string& xml, const string& tagName)
{
    vector<string> results;
    string openTag = "<" + tagName;
    string closeTag = "</" + tagName + ">";
    
    size_t pos = 0;
    while(true)
    {
        size_t start = xml.find(openTag, pos);
        if(start == string::npos)
            break;
        
        start = xml.find(">", start);
        if(start == string::npos)
            break;
        start++;
        
        size_t end = xml.find(closeTag, start);
        if(end == string::npos)
            break;
        
        results.push_back(xml.substr(start, end - start));
        pos = end + closeTag.length();
    }
    
    return results;
}

bool CTrustServiceList::parseXML(const string& xmlContent)
{
    LOG_DBG((0, "CTrustServiceList::parseXML", "Parsing TSL XML"));
    
    try
    {
        string schemeInfo = extractTagContent(xmlContent, "SchemeInformation");
        if(!schemeInfo.empty())
        {
            string nextUpdate = extractTagContent(schemeInfo, "NextUpdate");
            if(!nextUpdate.empty())
            {
                m_nextUpdate = extractTagContent(nextUpdate, "dateTime");
                LOG_DBG((0, "CTrustServiceList::parseXML", "NextUpdate: %s", m_nextUpdate.c_str()));
            }
        }
        
        string tspList = extractTagContent(xmlContent, "TrustServiceProviderList");
        if(tspList.empty())
        {
            LOG_ERR((0, "CTrustServiceList::parseXML", "TrustServiceProviderList not found"));
            return false;
        }
        
        vector<string> tspElements = extractAllTagContents(tspList, "TrustServiceProvider");
        
        LOG_DBG((0, "CTrustServiceList::parseXML", "Found %d Trust Service Providers", tspElements.size()));
        
        for(const auto& tspXml : tspElements)
        {
            TrustServiceProvider* pProvider = new TrustServiceProvider();
            
            string tspInfo = extractTagContent(tspXml, "TSPInformation");
            if(!tspInfo.empty())
            {
                string tspName = extractTagContent(tspInfo, "TSPName");
                if(!tspName.empty())
                {
                    pProvider->providerName = extractTagContent(tspName, "Name");
                    LOG_DBG((0, "CTrustServiceList::parseXML", "Provider: %s", pProvider->providerName.c_str()));
                }
            }
            
            string tspServices = extractTagContent(tspXml, "TSPServices");
            if(!tspServices.empty())
            {
                vector<string> serviceElements = extractAllTagContents(tspServices, "TSPService");
                
                for(const auto& serviceXml : serviceElements)
                {
                    TrustServiceInfo* pService = new TrustServiceInfo();
                    
                    string serviceInfo = extractTagContent(serviceXml, "ServiceInformation");
                    if(!serviceInfo.empty())
                    {
                        string serviceName = extractTagContent(serviceInfo, "ServiceName");
                        if(!serviceName.empty())
                        {
                            pService->serviceName = extractTagContent(serviceName, "Name");
                        }
                        
                        string serviceStatus = extractTagContent(serviceInfo, "ServiceStatus");
                        serviceStatus.erase(remove_if(serviceStatus.begin(), serviceStatus.end(), ::isspace), serviceStatus.end());

                        string statusLower = serviceStatus;
                        transform(statusLower.begin(), statusLower.end(), statusLower.begin(), ::tolower);

                        //LOG_DBG((0, "CTrustServiceList::parseXML", "  Service status: '%s'", serviceStatus.c_str()));

                        if(statusLower.find("granted") != string::npos || 
                           statusLower.find("recognisedatnationallevel") != string::npos)
                            pService->status = TSL_STATUS_GRANTED;
                        else if(statusLower.find("withdrawn") != string::npos)
                            pService->status = TSL_STATUS_WITHDRAWN;
                        else if(statusLower.find("suspended") != string::npos)
                            pService->status = TSL_STATUS_SUSPENDED;
                        else if(statusLower.find("revoked") != string::npos)
                            pService->status = TSL_STATUS_REVOKED;
                        else
                        {
                            LOG_WAR((0, "CTrustServiceList::parseXML", "  Unknown service status: '%s'", serviceStatus.c_str()));
                            pService->status = TSL_STATUS_UNKNOWN;
                        }
                        
                        string serviceDigitalId = extractTagContent(serviceInfo, "ServiceDigitalIdentity");
                        if(!serviceDigitalId.empty())
                        {
                            vector<string> x509Certs = extractAllTagContents(serviceDigitalId, "X509Certificate");
                            
                            for(const auto& certBase64 : x509Certs)
                            {
                                string cleanBase64 = certBase64;
                                cleanBase64.erase(remove_if(cleanBase64.begin(), cleanBase64.end(), ::isspace), cleanBase64.end());
                                
                                if(!cleanBase64.empty())
                                {
                                    CCertificate* pCert = extractCertificateFromBase64(cleanBase64);
                                    if(pCert)
                                    {
                                        pService->certificates.push_back(pCert);
                                        //LOG_DBG((0, "CTrustServiceList::parseXML", "  Loaded certificate for: %s", pService->serviceName.c_str()));
                                    }
                                }
                            }
                        }
                    }
                    
                    if(!pService->certificates.empty())
                        pProvider->services.push_back(pService);
                    else
                        delete pService;
                }
            }
            
            if(!pProvider->services.empty())
                m_providers.push_back(pProvider);
            else
                delete pProvider;
        }
        
        LOG_DBG((0, "CTrustServiceList::parseXML", "Successfully loaded %d providers", m_providers.size()));
        return true;
    }
    catch(...)
    {
        LOG_ERR((0, "CTrustServiceList::parseXML", "Exception during parsing"));
        return false;
    }
}

CCertificate* CTrustServiceList::extractCertificateFromBase64(const string& base64Cert)
{
    try
    {
        size_t decLen = base64_decoded_size(base64Cert.length());
        char* szDecoded = base64_decode(base64Cert.c_str());
        
        if(!szDecoded)
        {
            LOG_ERR((0, "CTrustServiceList::extractCertificateFromBase64", "Base64 decode failed"));
            return NULL;
        }
        
        UUCBufferedReader reader((BYTE*)szDecoded, decLen);
        CCertificate* pCert = new CCertificate(reader);
        
        free(szDecoded);
        
        return pCert;
    }
    catch(...)
    {
        LOG_ERR((0, "CTrustServiceList::extractCertificateFromBase64", "Exception parsing certificate"));
        return NULL;
    }
}

bool CTrustServiceList::isTrustedCA(CCertificate& cert, TSL_SERVICE_STATUS* pStatus)
{
    LOG_DBG((0, "CTrustServiceList::isTrustedCA", "Checking certificate against TSL"));
    
    CName issuer = cert.getIssuer();
    
    UUCByteArray issuerStr;
    issuer.getNameAsString(issuerStr);
    issuerStr.append((BYTE)'\0');
    LOG_DBG((0, "CTrustServiceList::isTrustedCA", "Certificate Issuer: %s", (char*)issuerStr.getContent()));
    
    LOG_DBG((0, "CTrustServiceList::isTrustedCA", "Total providers in TSL: %d", m_providers.size()));
    
    int providerIdx = 0;
    for(auto provider : m_providers)
    {
        LOG_DBG((0, "CTrustServiceList::isTrustedCA", "Provider %d: %s (services: %d)", 
                providerIdx++, provider->providerName.c_str(), provider->services.size()));
        
        int serviceIdx = 0;
        for(auto service : provider->services)
        {
            LOG_DBG((0, "CTrustServiceList::isTrustedCA", "  Service %d: %s (status: %d, certs: %d)", 
                    serviceIdx++, service->serviceName.c_str(), service->status, service->certificates.size()));
            
            if(service->status != TSL_STATUS_GRANTED)
            {
                LOG_DBG((0, "CTrustServiceList::isTrustedCA", "    Skipping service (not granted)"));
                continue;
            }
            
            int certIdx = 0;
            for(auto caCert : service->certificates)
            {
                CName caSubject = caCert->getSubject();
                
                UUCByteArray caSubjectStr;
                caSubject.getNameAsString(caSubjectStr);
                caSubjectStr.append((BYTE)'\0');

                //LOG_DBG((0, "CTrustServiceList::isTrustedCA", "    CA Cert issuer: %s caSubject: %s", (char*)issuerStr.getContent(), (char*)caSubjectStr.getContent()));

                if(issuer == caSubject)
                {
                    LOG_DBG((0, "CTrustServiceList::isTrustedCA", "Certificate from trusted CA: %s", service->serviceName.c_str()));
                    
                    if(cert.verifySignature(*caCert))
                    {
                        LOG_DBG((0, "CTrustServiceList::isTrustedCA", 
                                "Certificate from trusted CA (signature verified): %s", 
                                service->serviceName.c_str()));
                        
                        if(pStatus)
                            *pStatus = service->status;
                        
                        return true;
                    }
                    else
                    {
                        LOG_ERR((0, "CTrustServiceList::isTrustedCA", 
                                "Certificate issuer matches but signature verification FAILED!"));
                    }
                }
            }
        }
    }
    
    LOG_ERR((0, "CTrustServiceList::isTrustedCA", "Certificate NOT from trusted CA - no match found"));
    return false;
}

bool CTrustServiceList::saveToCache(const char* szCacheFile)
{
    LOG_DBG((0, "CTrustServiceList::saveToCache", "Saving TSL cache to: %s", szCacheFile));
    
    try
    {
        FILE* f = nullptr;
        
#ifdef _WIN32
        errno_t err = fopen_s(&f, szCacheFile, "wb");
        if(err != 0 || !f)
#else
        f = fopen(szCacheFile, "wb");
        if(!f)
#endif
        {
            LOG_ERR((0, "CTrustServiceList::saveToCache", "Failed to open cache file for writing: %s", szCacheFile));
            return false;
        }
        
        const char* CACHE_MAGIC = "TSL_CACHE_V1";
        fwrite(CACHE_MAGIC, 1, strlen(CACHE_MAGIC), f);
        
        size_t nextUpdateLen = m_nextUpdate.length();
        fwrite(&nextUpdateLen, sizeof(size_t), 1, f);
        if(nextUpdateLen > 0)
            fwrite(m_nextUpdate.c_str(), 1, nextUpdateLen, f);
        
        size_t urlLen = m_tslURL.length();
        fwrite(&urlLen, sizeof(size_t), 1, f);
        if(urlLen > 0)
            fwrite(m_tslURL.c_str(), 1, urlLen, f);
        
        size_t providerCount = m_providers.size();
        fwrite(&providerCount, sizeof(size_t), 1, f);
        
        LOG_DBG((0, "CTrustServiceList::saveToCache", "Saving %d providers", providerCount));
        
        for(auto provider : m_providers)
        {
            size_t nameLen = provider->providerName.length();
            fwrite(&nameLen, sizeof(size_t), 1, f);
            if(nameLen > 0)
                fwrite(provider->providerName.c_str(), 1, nameLen, f);
            
            size_t serviceCount = provider->services.size();
            fwrite(&serviceCount, sizeof(size_t), 1, f);
            
            for(auto service : provider->services)
            {
                size_t svcNameLen = service->serviceName.length();
                fwrite(&svcNameLen, sizeof(size_t), 1, f);
                if(svcNameLen > 0)
                    fwrite(service->serviceName.c_str(), 1, svcNameLen, f);
                
                int status = (int)service->status;
                fwrite(&status, sizeof(int), 1, f);
                
                size_t certCount = service->certificates.size();
                fwrite(&certCount, sizeof(size_t), 1, f);
                
                for(auto cert : service->certificates)
                {
                    UUCByteArray certData;
                    cert->toByteArray(certData);
                    
                    size_t certLen = certData.getLength();
                    fwrite(&certLen, sizeof(size_t), 1, f);
                    fwrite(certData.getContent(), 1, certLen, f);
                }
            }
        }
        
        fclose(f);
        
        LOG_DBG((0, "CTrustServiceList::saveToCache", "TSL cache saved successfully"));
        return true;
    }
    catch(...)
    {
        LOG_ERR((0, "CTrustServiceList::saveToCache", "Exception saving cache"));
        return false;
    }
}

bool CTrustServiceList::loadFromCache(const char* szCacheFile)
{
    LOG_DBG((0, "CTrustServiceList::loadFromCache", "Loading TSL cache from: %s", szCacheFile));
    
    try
    {
        FILE* f = nullptr;
        
#ifdef _WIN32
        errno_t err = fopen_s(&f, szCacheFile, "rb");
        if(err != 0 || !f)
#else
        f = fopen(szCacheFile, "rb");
        if(!f)
#endif
        {
            LOG_DBG((0, "CTrustServiceList::loadFromCache", "Cache file not found: %s", szCacheFile));
            return false;
        }
        
        const char* CACHE_MAGIC = "TSL_CACHE_V1";
        char magicBuf[20] = {0};
        size_t magicLen = strlen(CACHE_MAGIC);
        fread(magicBuf, 1, magicLen, f);
        
        if(strncmp(magicBuf, CACHE_MAGIC, magicLen) != 0)
        {
            LOG_ERR((0, "CTrustServiceList::loadFromCache", "Invalid cache file format"));
            fclose(f);
            return false;
        }
        
        size_t nextUpdateLen = 0;
        fread(&nextUpdateLen, sizeof(size_t), 1, f);
        if(nextUpdateLen > 0 && nextUpdateLen < 1024)
        {
            char* buf = new char[nextUpdateLen + 1];
            fread(buf, 1, nextUpdateLen, f);
            buf[nextUpdateLen] = '\0';
            m_nextUpdate = buf;
            delete[] buf;
        }
        
        size_t urlLen = 0;
        fread(&urlLen, sizeof(size_t), 1, f);
        if(urlLen > 0 && urlLen < 1024)
        {
            char* buf = new char[urlLen + 1];
            fread(buf, 1, urlLen, f);
            buf[urlLen] = '\0';
            m_tslURL = buf;
            delete[] buf;
        }
        
        size_t providerCount = 0;
        fread(&providerCount, sizeof(size_t), 1, f);
        
        LOG_DBG((0, "CTrustServiceList::loadFromCache", "Loading %d providers", providerCount));
        
        if(providerCount > 1000)
        {
            LOG_ERR((0, "CTrustServiceList::loadFromCache", "Invalid provider count: %d", providerCount));
            fclose(f);
            return false;
        }
        
        for(size_t p = 0; p < providerCount; p++)
        {
            TrustServiceProvider* provider = new TrustServiceProvider();
            
            size_t nameLen = 0;
            fread(&nameLen, sizeof(size_t), 1, f);
            if(nameLen > 0 && nameLen < 1024)
            {
                char* buf = new char[nameLen + 1];
                fread(buf, 1, nameLen, f);
                buf[nameLen] = '\0';
                provider->providerName = buf;
                delete[] buf;
            }
            
            size_t serviceCount = 0;
            fread(&serviceCount, sizeof(size_t), 1, f);
            
            if(serviceCount > 1000)
            {
                delete provider;
                fclose(f);
                return false;
            }
            
            for(size_t s = 0; s < serviceCount; s++)
            {
                TrustServiceInfo* service = new TrustServiceInfo();
                
                size_t svcNameLen = 0;
                fread(&svcNameLen, sizeof(size_t), 1, f);
                if(svcNameLen > 0 && svcNameLen < 1024)
                {
                    char* buf = new char[svcNameLen + 1];
                    fread(buf, 1, svcNameLen, f);
                    buf[svcNameLen] = '\0';
                    service->serviceName = buf;
                    delete[] buf;
                }
                
                int status = 0;
                fread(&status, sizeof(int), 1, f);
                service->status = (TSL_SERVICE_STATUS)status;
                
                size_t certCount = 0;
                fread(&certCount, sizeof(size_t), 1, f);
                
                if(certCount > 100) 
                {
                    delete service;
                    delete provider;
                    fclose(f);
                    return false;
                }
                
                for(size_t c = 0; c < certCount; c++)
                {
                    size_t certLen = 0;
                    fread(&certLen, sizeof(size_t), 1, f);
                    
                    if(certLen > 0 && certLen < 10240) // Max 10KB per cert
                    {
                        BYTE* certBuf = new BYTE[certLen];
                        fread(certBuf, 1, certLen, f);
                        
                        try
                        {
                            UUCBufferedReader reader(certBuf, certLen);
                            CCertificate* cert = new CCertificate(reader);
                            service->certificates.push_back(cert);
                        }
                        catch(...)
                        {
                            LOG_ERR((0, "CTrustServiceList::loadFromCache", "Failed to parse cached certificate"));
                        }
                        
                        delete[] certBuf;
                    }
                }
                
                provider->services.push_back(service);
            }
            
            m_providers.push_back(provider);
        }
        
        fclose(f);
        
        LOG_DBG((0, "CTrustServiceList::loadFromCache", "TSL cache loaded successfully. Providers: %d", m_providers.size()));
        return true;
    }
    catch(...)
    {
        LOG_ERR((0, "CTrustServiceList::loadFromCache", "Exception loading cache"));
        return false;
    }
}

bool CTrustServiceList::isExpired()
{
    // If NextUpdate is not set, consider it expired
    if(m_nextUpdate.empty())
    {
        LOG_DBG((0, "CTrustServiceList::isExpired", "NextUpdate not set, cache expired"));
        return true;
    }
    
    try
    {
        // NextUpdate format: "YYYY-MM-DDTHH:MM:SS" (ISO 8601)
        // Example: "2025-01-21T12:00:00Z"
        
        // Parse NextUpdate
        int year, month, day, hour, min, sec;
        if(sscanf(m_nextUpdate.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d", 
                  &year, &month, &day, &hour, &min, &sec) != 6)
        {
            LOG_ERR((0, "CTrustServiceList::isExpired", "Failed to parse NextUpdate: %s", m_nextUpdate.c_str()));
            return true;
        }
        
        struct tm nextUpdateTm = {0};
        nextUpdateTm.tm_year = year - 1900;
        nextUpdateTm.tm_mon = month - 1;
        nextUpdateTm.tm_mday = day;
        nextUpdateTm.tm_hour = hour;
        nextUpdateTm.tm_min = min;
        nextUpdateTm.tm_sec = sec;
        nextUpdateTm.tm_isdst = -1;
        
        time_t nextUpdateTime = mktime(&nextUpdateTm);
        time_t now = time(NULL);
        
        if(now >= nextUpdateTime)
        {
            LOG_DBG((0, "CTrustServiceList::isExpired", "TSL cache expired. NextUpdate: %s", m_nextUpdate.c_str()));
            return true;
        }
        
        LOG_DBG((0, "CTrustServiceList::isExpired", "TSL cache valid until: %s", m_nextUpdate.c_str()));
        return false;
    }
    catch(...)
    {
        LOG_ERR((0, "CTrustServiceList::isExpired", "Exception checking expiration"));
        return true;
    }
}

bool compareNames(CName& name1, CName& name2)
{
    UUCByteArray str1, str2;
    name1.getNameAsString(str1);
    name2.getNameAsString(str2);
    
    str1.append((BYTE)'\0');
    str2.append((BYTE)'\0');
    
    return strcmp((char*)str1.getContent(), (char*)str2.getContent()) == 0;
}