//
//  CIEEngineHelper.h
//  CIESDK
//
//  Created by ugo chirico on 26.02.2020.
//

#ifndef CIEEngineHelper_h
#define CIEEngineHelper_h

#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>

void alloc_rsa_ex_index(void);
void free_rsa_ex_index(void);
RSA_METHOD *RSA_meth_dup(const RSA_METHOD *meth);
int RSA_meth_set1_name(RSA_METHOD *meth, const char *name);
int RSA_meth_set_flags(RSA_METHOD *meth, int flags);
int RSA_meth_set_priv_enc(RSA_METHOD *meth,
        int (*priv_enc) (int flen, const unsigned char *from,
                         unsigned char *to, RSA *rsa, int padding));
int RSA_meth_set_priv_dec(RSA_METHOD *meth,
        int (*priv_dec) (int flen, const unsigned char *from,
                         unsigned char *to, RSA *rsa, int padding));

int RSA_meth_set_finish(RSA_METHOD *meth, int (*finish)(RSA *rsa));

#endif /* CIEEngineHelper_h */
