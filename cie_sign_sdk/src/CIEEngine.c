//
//  CIEEngine.c
//  CIESDK
//
//  Created by ugo chirico on 26.02.2020.
//

#include <stdio.h>
#include <string.h>
#include "CIEEngine.h"
#include "CIEEngineHelper.h"
#include <openssl/engine.h>
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/ecdsa.h>
#include <openssl/ecdh.h>

unsigned char* cie_certificate;
unsigned long cie_certlen;
unsigned char* cie_pin;
unsigned long cie_pinlen;
unsigned short cie_error;

X509* cie_x509_certificate = NULL;

#define CIE_CMD_OK 0x9000

static const char *engine_cie_id = "cie";
static const char *engine_cie_name = "cie engine";

static int (*orig_pkey_rsa_sign_init) (EVP_PKEY_CTX *ctx);
static int (*orig_pkey_rsa_sign) (EVP_PKEY_CTX *ctx,
    unsigned char *sig, size_t *siglen,
    const unsigned char *tbs, size_t tbslen);
static int (*orig_pkey_rsa_decrypt_init) (EVP_PKEY_CTX *ctx);
static int (*orig_pkey_rsa_decrypt) (EVP_PKEY_CTX *ctx,
    unsigned char *out, size_t *outlen,
    const unsigned char *in, size_t inlen);

static sign sign_cb_impl;

void dump(const char *text,
          FILE *stream, unsigned char *ptr, size_t size,
          char nohex);

#define CMD_PIN        (ENGINE_CMD_BASE+1)
#define CMD_LOAD_CERT_CTRL    (ENGINE_CMD_BASE+2)


static const ENGINE_CMD_DEFN cie_engine_cmd_defns[] = {
    {CMD_LOAD_CERT_CTRL,
        "LOAD_CERT_CTRL",
        "Get the certificate from CIE",
        ENGINE_CMD_FLAG_INTERNAL},
    {0, NULL, NULL, 0}
};


// Make Digest Info
    int makeDigestInfo(int algid, unsigned char* pbtDigest, size_t btDigestLen, unsigned char* pbtDigestInfo, size_t* pbtDigestInfoLen)
{
    size_t requestedLen;
    switch(algid)
    {
    
    case NID_sha1:
        //printf("** HASH = SHA1 **");
        
        requestedLen = 2 + 2 + 9 + 2 + btDigestLen;
        if(*pbtDigestInfoLen < requestedLen)// protezione memoria e memcpy in basso
                        return 0;
            
        pbtDigestInfo[0] = 0x30;
        pbtDigestInfo[1] = 2 + 9 + 2 + btDigestLen;
        pbtDigestInfo[2] = 0x30;
        pbtDigestInfo[3] = 0x09;
            pbtDigestInfo[4] = 0x06;
            pbtDigestInfo[5] = 0x05;
                pbtDigestInfo[6]  = 0x2b;
                pbtDigestInfo[7]  = 0x0e;
                pbtDigestInfo[8]  = 0x03;
                pbtDigestInfo[9]  = 0x02;
                pbtDigestInfo[10] = 0x1a;// SHA1 02 1a
            pbtDigestInfo[11] = 0x05;
            pbtDigestInfo[12] = 0x00;
        pbtDigestInfo[13] = 0x04;
        pbtDigestInfo[14] = btDigestLen;
        // Ricopia l'hash
        memcpy(pbtDigestInfo + 15, pbtDigest, btDigestLen);
        *pbtDigestInfoLen = 2 + 2 + 9 + 2 + btDigestLen;
        break;

    case NID_sha256:
           //printf("** HASH = SHA256 **");
            
            requestedLen = 2 + 2 + 9 + 6 + btDigestLen;
            if(*pbtDigestInfoLen < requestedLen)// protezione memoria e memcpy in basso
             return 0;

           pbtDigestInfo[0] = 0x30;
           pbtDigestInfo[1] = 2 + 9 + 6 + btDigestLen;
           pbtDigestInfo[2] = 0x30;
           pbtDigestInfo[3] = 0x0D;
               pbtDigestInfo[4] = 0x06;
               pbtDigestInfo[5] = 0x09;  // SHA512 60 86 48 01 65 03 04 02 03
                   pbtDigestInfo[6]  = 0x60;
                   pbtDigestInfo[7]  = 0x86;
                   pbtDigestInfo[8]  = 0x48;
                   pbtDigestInfo[9]  = 0x01;
                   pbtDigestInfo[10] = 0x65;
                   pbtDigestInfo[11] = 0x03;
                   pbtDigestInfo[12] = 0x04;
                   pbtDigestInfo[13] = 0x02;
                   pbtDigestInfo[14] = 0x01;
               pbtDigestInfo[15] = 0x05;
               pbtDigestInfo[16] = 0x00;
           pbtDigestInfo[17] = 0x04;
           pbtDigestInfo[18] = btDigestLen;
           // Ricopia l'hash
           memcpy(pbtDigestInfo + 19, pbtDigest, btDigestLen);
           *pbtDigestInfoLen = 2 + 2 + 9 + 6 + btDigestLen;
           break;
            
    case NID_sha384:
        //printf("** HASH = SHA384 **");
            
        requestedLen = 2 + 2 + 9 + 6 + btDigestLen;
        if(*pbtDigestInfoLen < requestedLen)// protezione memoria e memcpy in basso
             return 0;

        pbtDigestInfo[0] = 0x30;
        pbtDigestInfo[1] = 2 + 9 + 6 + btDigestLen;
        pbtDigestInfo[2] = 0x30;
        pbtDigestInfo[3] = 0x0D;
            pbtDigestInfo[4] = 0x06;
            pbtDigestInfo[5] = 0x09;  // SHA512 60 86 48 01 65 03 04 02 03
                pbtDigestInfo[6]  = 0x60;
                pbtDigestInfo[7]  = 0x86;
                pbtDigestInfo[8]  = 0x48;
                pbtDigestInfo[9]  = 0x01;
                pbtDigestInfo[10] = 0x65;
                pbtDigestInfo[11] = 0x03;
                pbtDigestInfo[12] = 0x04;
                pbtDigestInfo[13] = 0x02;
                pbtDigestInfo[14] = 0x02;
            pbtDigestInfo[15] = 0x05;
            pbtDigestInfo[16] = 0x00;
        pbtDigestInfo[17] = 0x04;
        pbtDigestInfo[18] = btDigestLen;
        // Ricopia l'hash
        memcpy(pbtDigestInfo + 19, pbtDigest, btDigestLen);
        *pbtDigestInfoLen = 2 + 2 + 9 + 6 + btDigestLen;
        break;
            
    case NID_sha512:
        //printf("** HASH = SHA512 **");
        
        requestedLen = 2 + 2 + 9 + 6 + btDigestLen;
        if(*pbtDigestInfoLen < requestedLen)// protezione memoria e memcpy in basso
                 return 0;
            
        pbtDigestInfo[0] = 0x30;
        pbtDigestInfo[1] = 2 + 9 + 6 + btDigestLen;
        pbtDigestInfo[2] = 0x30;
        pbtDigestInfo[3] = 0x0D;
            pbtDigestInfo[4] = 0x06;
            pbtDigestInfo[5] = 0x09;  // SHA512 60 86 48 01 65 03 04 02 03
                pbtDigestInfo[6]  = 0x60;
                pbtDigestInfo[7]  = 0x86;
                pbtDigestInfo[8]  = 0x48;
                pbtDigestInfo[9]  = 0x01;
                pbtDigestInfo[10] = 0x65;
                pbtDigestInfo[11] = 0x03;
                pbtDigestInfo[12] = 0x04;
                pbtDigestInfo[13] = 0x02;
                pbtDigestInfo[14] = 0x03;
            pbtDigestInfo[15] = 0x05;
            pbtDigestInfo[16] = 0x00;
        pbtDigestInfo[17] = 0x04;
        pbtDigestInfo[18] = btDigestLen;
        // Ricopia l'hash
        memcpy(pbtDigestInfo + 19, pbtDigest, btDigestLen);
        *pbtDigestInfoLen = 2 + 2 + 9 + 6 + btDigestLen;
        break;
            
//    case CALG_SSL3_SHAMD5:
//        //printf("** HASH = SSL3_SHAMD5 **");
//        memcpy(pbtDigestInfo, pbtDigest, btDigestLen);
//        *pbtDigestInfoLen = btDigestLen;
//        break;
    }
    return 1;
}


int cie_rsa_priv_enc(int flen, const unsigned char *from,
                      unsigned char *to, RSA *rsa, int padding)
{
    printf("call cie_rsa_priv_enc\n");
    return 0;
}

static int cie_rsa_free(RSA *rsa)
{
    printf("call cie_rsa_priv_free\n");
    return 0;
}

static int cie_pkey_rsa_decrypt(EVP_PKEY_CTX *evp_pkey_ctx,
        unsigned char *out, size_t *outlen,
        const unsigned char *in, size_t inlen)
{
    printf("call cie_pkey_rsa_decrypt\n");
    return 0;
}

static int cie_pkey_rsa_sign(EVP_PKEY_CTX *evp_pkey_ctx,
        unsigned char *sig, size_t *siglen,
        const unsigned char *tbs, size_t tbslen)
{
    printf("call cie_pkey_rsa_sign\n");
    
    EVP_PKEY *pkey;
    RSA *rsa;
    const EVP_MD *sig_md;

    printf("%s:%d cie_pkey_rsa_sign() "
        "sig=%p *siglen=%lu tbs=%p tbslen=%lu\n",
        __FILE__, __LINE__, sig, *siglen, tbs, tbslen);
    
    pkey = EVP_PKEY_CTX_get0_pkey(evp_pkey_ctx);
    if (!pkey)
        return 0;
    
    rsa = EVP_PKEY_get0_RSA(pkey);
    if (!rsa)
        return 0;

    if (!evp_pkey_ctx)
        return 0;

    if (EVP_PKEY_CTX_get_signature_md(evp_pkey_ctx, &sig_md) <= 0)
        return 0;

    if (tbslen != (size_t)EVP_MD_size(sig_md))
        return 0;

    int md_type = EVP_MD_type(sig_md);
    printf("hashtype: %x", md_type);
    
    int padding;
    EVP_PKEY_CTX_get_rsa_padding(evp_pkey_ctx, &padding);
    printf("requested padding: %x", padding);
    
    switch (padding) {
        case RSA_PKCS1_PSS_PADDING:
            printf("%s:%d padding=RSA_PKCS1_PSS_PADDING\n",
                __FILE__, __LINE__);
            printf("Unsopperted padding. CIE supports PKCS1_PADDING only\n");
            return 0;
            break;
        case RSA_PKCS1_PADDING:
            printf("%s:%d padding=RSA_PKCS1_PADDING\n",
                __FILE__, __LINE__);
            break;
        default:
            printf("%s:%d unsupported padding: %d\n",
                    __FILE__, __LINE__, padding);
            printf("Unsopperted padding. CIE supports PKCS1_PADDING only\n");
            return 0;
            break;
    } /* end switch(padding) */

    int ret = 0;
    
    // DigestInfo
    unsigned char digestinfo[256];
    size_t digestinfolen = 256;
    
    makeDigestInfo(md_type, tbs, tbslen, digestinfo, &digestinfolen);
        
    ret = sign_cb_impl(digestinfo, digestinfolen, sig, siglen) & 0x0000FFFF;

    
    if(ret != CIE_CMD_OK)
    {
        cie_error = ret;
        printf("sign Failed: %x", ret);
        return ret;
    }
    
    printf("signature OK\n");
    
    cie_error = 0;
    
    X509_free(cie_x509_certificate);
    cie_x509_certificate = NULL;
        
    return 1;

}

/*
 * Overload the default OpenSSL methods for ECDSA
 * If OpenSSL supports ECDSA_METHOD_new we will use it.
 * First introduced in 1.0.2, changed in 1.1-pre
 */

/* New way to allocate an ECDSA_METOD object */
/* OpenSSL 1.1 has single method  EC_KEY_METHOD for ECDSA and ECDH */


#if OPENSSL_VERSION_NUMBER >= 0x10100004L && !defined(LIBRESSL_VERSION_NUMBER)
typedef int (*compute_key_fn)(unsigned char **, size_t *,
    const EC_POINT *, const EC_KEY *);
#else
typedef int (*compute_key_fn)(void *, size_t,
    const EC_POINT *, const EC_KEY *,
    void *(*)(const void *, size_t, void *, size_t *));
#endif

#if OPENSSL_VERSION_NUMBER >= 0x10100000L && !defined(LIBRESSL_VERSION_NUMBER)

compute_key_fn fnossl_ecdh_compute_key;

int cie_ecdh_compute_key(unsigned char ** a, size_t * b, const EC_POINT *c, const EC_KEY *d)
{
    printf("call cie_compute_ec_key\n");
    int ret = fnossl_ecdh_compute_key(a,b,c,d);
    printf("returns: %d\n", ret);
    return ret;
}

EC_KEY_METHOD *cie_get_ec_key_method(void)
{
    printf("call cie_get_ec_key_method\n");
    
    static EC_KEY_METHOD *ops = NULL;
    int (*orig_sign)(int, const unsigned char *, int, unsigned char *,
        unsigned int *, const BIGNUM *, const BIGNUM *, EC_KEY *) = NULL;

    compute_key_fn ossl_ecdh_compute_key;
    
    //alloc_ec_ex_index();
    if (!ops) {
        ops = EC_KEY_METHOD_new((EC_KEY_METHOD *)EC_KEY_OpenSSL());
        EC_KEY_METHOD_get_sign(ops, &orig_sign, NULL, NULL);
        EC_KEY_METHOD_set_sign(ops, orig_sign, NULL, &orig_sign);
        EC_KEY_METHOD_get_compute_key(ops, &ossl_ecdh_compute_key);
        EC_KEY_METHOD_set_compute_key(ops, cie_ecdh_compute_key);
        
        fnossl_ecdh_compute_key = ossl_ecdh_compute_key;
    }
    return ops;
}
		
/* define old way to keep old engines working without ECDSA */
void *cie_get_ecdsa_method(void)
{
    printf("call cie_get_ecdsa_method\n");
    return NULL;
}

void *cie_get_ecdh_method(void)
{
    printf("call cie_get_ecdh_method\n");
    return NULL;
}

#else /* OPENSSL_VERSION_NUMBER */


/* define new way to keep new engines from crashing with older libp11 */
void *cie_get_ec_key_method(void)
{
    return NULL;
}

ECDSA_METHOD *cie_get_ecdsa_method(void)
{
    static ECDSA_METHOD *ops = NULL;

    if (!ops) {
        alloc_ec_ex_index();
        ops = ECDSA_METHOD_new((ECDSA_METHOD *)ECDSA_OpenSSL());
        ECDSA_METHOD_set_sign(ops, pkcs11_ecdsa_sign_sig);
    }
    return ops;
}

ECDH_METHOD *cie_get_ecdh_method(void)
{
    static ECDH_METHOD *ops = NULL;

    if (!ops) {
        alloc_ec_ex_index();
        ops = ECDH_METHOD_new((ECDH_METHOD *)ECDH_OpenSSL());
        ECDH_METHOD_get_compute_key(ops, &ossl_ecdh_compute_key);
        ECDH_METHOD_set_compute_key(ops, pkcs11_ec_ckey);
    }
    return ops;
}

#endif /* OPENSSL_VERSION_NUMBER */

static int cie_engine_ctrl(ENGINE *engine, int cmd, long i, void *p, void (*f) (void))
{
    printf("call cie_engine_ctrl\n");
    
    (void)i; /* We don't currently take integer parameters */
    (void)f; /* We don't currently take callback parameters */
    /*int initialised = ((pkcs11_dso == NULL) ? 0 : 1); */
    switch (cmd)
    {
        case CMD_LOAD_CERT_CTRL:
        {
            printf("CMD_LOAD_CERT_CTRL\n");
            struct {
                const char *s_slot_cert_id;
                X509 *cert;
            } *parms = p;
            
            if(!cie_x509_certificate)
            {
                cie_x509_certificate = d2i_X509(NULL, &cie_certificate, cie_certlen);
            }
            
            parms->cert = X509_dup(cie_x509_certificate);
            parms->s_slot_cert_id = "cie";
            
            return 1;
        }
        case ENGINE_CTRL_SET_USER_INTERFACE:
            printf("ENGINE_CTRL_SET_USER_INTERFACE\n");
            return 0;
        case ENGINE_CTRL_SET_CALLBACK_DATA:
            printf("ENGINE_CTRL_SET_CALLBACK_DATA\n");
            return 0;
        default:
            return 0;
        break;
    }
}

static EVP_PKEY *cie_load_privkey(ENGINE *eng, const char *key_id,
                                     UI_METHOD *ui_method,
                                     void *callback_data)
{
    printf("call cie_load_privkey\n");
    
    EVP_PKEY *pubkey = NULL;

    X509* cert = X509_dup(cie_x509_certificate);
    
    if ((pubkey = X509_get_pubkey(cert)) == NULL)
    {
        printf("get_pubKey failed\n");
       return 0;
    }
        
    X509_free(cert);
    
    return pubkey;
}

static EVP_PKEY *cie_load_pubkey(ENGINE *engine, const char *s_key_id,
        UI_METHOD *ui_method, void *callback_data)
{
    
    printf("call cie_load_pubkey\n");
    
    return 1;
}


static EVP_PKEY_METHOD *cie_pkey_method_rsa()
{
    printf("call cie_pkey_method_rsa\n");
    
    EVP_PKEY_METHOD *orig_meth, *new_meth;

    orig_meth = (EVP_PKEY_METHOD *)EVP_PKEY_meth_find(EVP_PKEY_RSA);
    EVP_PKEY_meth_get_sign(orig_meth,
        &orig_pkey_rsa_sign_init, &orig_pkey_rsa_sign);
    EVP_PKEY_meth_get_decrypt(orig_meth,
        &orig_pkey_rsa_decrypt_init,
        &orig_pkey_rsa_decrypt);
    
    new_meth = EVP_PKEY_meth_new(EVP_PKEY_RSA,
        EVP_PKEY_FLAG_AUTOARGLEN);

    EVP_PKEY_meth_copy(new_meth, orig_meth);

    EVP_PKEY_meth_set_sign(new_meth,
        orig_pkey_rsa_sign_init, cie_pkey_rsa_sign);
    EVP_PKEY_meth_set_decrypt(new_meth,
        orig_pkey_rsa_decrypt_init, cie_pkey_rsa_decrypt);

    
    return new_meth;
}

int cie_pkey_meths(ENGINE *e, EVP_PKEY_METHOD **pmeth,
        const int **nids, int nid)
{
    printf("call cie_pkey_meths\n");
    
    static int pkey_nids[] = {
        EVP_PKEY_RSA,
        0
    };
    EVP_PKEY_METHOD *pkey_method_rsa = NULL;
    
    (void)e; /* squash the unused parameter warning */
    /* all PKCS#11 engines currently share the same pkey_meths */

    if (!pmeth) { /* get the list of supported nids */
        *nids = pkey_nids;
        return sizeof(pkey_nids) / sizeof(int) - 1;
    }

    /* get the EVP_PKEY_METHOD */
    switch (nid) {
    case EVP_PKEY_RSA:
        pkey_method_rsa = cie_pkey_method_rsa();
        if (!pkey_method_rsa)
            return 0;
        *pmeth = pkey_method_rsa;
        return 1; /* success */
    }
    *pmeth = NULL;
    return 0;
}

/*
 * Overload the default OpenSSL methods for RSA
 */
RSA_METHOD *cie_get_rsa_method(void)
{
    printf("call cie_pkey_meths\n");
    
    static RSA_METHOD *ops = NULL;

    if (ops == NULL) {
        //alloc_rsa_ex_index();
        ops = RSA_meth_dup(RSA_get_default_method());
        if (ops == NULL)
            return NULL;
        RSA_meth_set1_name(ops, "CIE RSA method");
        RSA_meth_set_flags(ops, 0);
        RSA_meth_set_finish(ops, cie_rsa_free);
    }
    return ops;
}

int cie_init(ENGINE *e) {
    printf("CIE Engine Initializatzion!\n");
    return 1;
}

int cie_finish(ENGINE *e) {
    printf("CIE Engine finalization!\n");
    return 1;
}

int cie_destroy(ENGINE *e) {
    printf("CIE Engine destroy!\n");
    return 1;
}

int bind_helper(ENGINE * e, const char *id)
{
    if (!ENGINE_set_id(e, engine_cie_id) ||
        !ENGINE_set_name(e, engine_cie_name) ||
        !ENGINE_set_init_function(e,cie_init) ||
        !ENGINE_set_RSA(e, cie_get_rsa_method()) ||
        !ENGINE_set_destroy_function(e, cie_destroy) ||
        !ENGINE_set_finish_function(e, cie_finish) ||
        !ENGINE_set_pkey_meths(e, cie_pkey_meths) ||
        !ENGINE_set_cmd_defns(e, cie_engine_cmd_defns) ||
        !ENGINE_set_ctrl_function(e, cie_engine_ctrl) ||
        !ENGINE_set_load_privkey_function(e, cie_load_privkey) ||
        !ENGINE_set_load_pubkey_function(e, cie_load_pubkey) //||
        //!ENGINE_set_EC(e, cie_get_ec_key_method())
        )
            
    {
        return 0;
    }
    
    return 1;
}



IMPLEMENT_DYNAMIC_BIND_FN(bind_helper);
IMPLEMENT_DYNAMIC_CHECK_FN();


static ENGINE *engine_cie(void)
{
    ENGINE *e = ENGINE_new();
    if (e == NULL)
        return NULL;
    if (!bind_helper(e, engine_cie_id)) {
        ENGINE_free(e);
        return NULL;
    }
    return e;
}

void engine_load_cie(sign sign_cb)
{

    sign_cb_impl = sign_cb;
    ENGINE *toadd = engine_cie();
    if (!toadd)
        return;
    ENGINE_add(toadd);
    ENGINE_free(toadd);
    ERR_clear_error();
}
