//
//  CIEEngine.h
//  CIESDK
//
//  Created by ugo chirico on 26.02.2020.
//

#ifndef CIEEngine_h
#define CIEEngine_h

#include <stdio.h>
#include <openssl/crypto.h>
#include <openssl/objects.h>
#include <openssl/engine.h>

typedef short (*sign)(unsigned char* tosign, size_t len, unsigned char* signature, size_t* psiglen);


void engine_load_cie(sign sign_cb);


#endif /* CIEEngine_h */
