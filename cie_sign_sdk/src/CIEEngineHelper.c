//
//  CIEEngineHelper.c
//  CIESDK
//
//  Created by ugo chirico on 26.02.2020.
//

#include "CIEEngineHelper.h"
#include <string.h>

static int rsa_ex_index = 0;

void alloc_rsa_ex_index()
{
    if (rsa_ex_index == 0) {
        while (rsa_ex_index == 0) /* Workaround for OpenSSL RT3710 */
            rsa_ex_index = RSA_get_ex_new_index(0, "CIE rsa",
                NULL, NULL, NULL);
        if (rsa_ex_index < 0)
            rsa_ex_index = 0; /* Fallback to app_data */
    }
}

void free_rsa_ex_index()
{
    /* CRYPTO_free_ex_index requires OpenSSL version >= 1.1.0-pre1 */
#if OPENSSL_VERSION_NUMBER >= 0x10100001L && !defined(LIBRESSL_VERSION_NUMBER)
    if (rsa_ex_index > 0) {
        CRYPTO_free_ex_index(CRYPTO_EX_INDEX_RSA, rsa_ex_index);
        rsa_ex_index = 0;
    }
#endif
}

#if OPENSSL_VERSION_NUMBER < 0x10100005L || ( defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x2080000L )

RSA_METHOD *RSA_meth_dup(const RSA_METHOD *meth)
{
    RSA_METHOD *ret = OPENSSL_malloc(sizeof(RSA_METHOD));
    if (ret == NULL)
        return NULL;
    memcpy(ret, meth, sizeof(RSA_METHOD));
    ret->name = OPENSSL_strdup(meth->name);
    if (ret->name == NULL) {
        OPENSSL_free(ret);
        return NULL;
    }
    return ret;
}

int RSA_meth_set1_name(RSA_METHOD *meth, const char *name)
{
    char *tmp = OPENSSL_strdup(name);
    if (tmp == NULL)
        return 0;
    OPENSSL_free((char *)meth->name);
    meth->name = tmp;
    return 1;
}

#endif

#if OPENSSL_VERSION_NUMBER < 0x10100005L || ( defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x3000000L )

int RSA_meth_set_flags(RSA_METHOD *meth, int flags)
{
    meth->flags = flags;
    return 1;
}
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100005L || ( defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x2080000L )

int RSA_meth_set_priv_enc(RSA_METHOD *meth,
        int (*priv_enc) (int flen, const unsigned char *from,
        unsigned char *to, RSA *rsa, int padding))
{
    meth->rsa_priv_enc = priv_enc;
    return 1;
}

int RSA_meth_set_priv_dec(RSA_METHOD *meth,
        int (*priv_dec) (int flen, const unsigned char *from,
        unsigned char *to, RSA *rsa, int padding))
{
    meth->rsa_priv_dec = priv_dec;
    return 1;
}

int RSA_meth_set_finish(RSA_METHOD *meth, int (*finish)(RSA *rsa))
{
    meth->finish = finish;
    return 1;
}

#endif


