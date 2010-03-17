//
//  ssocket.h
//
//  Created by Jerome Poichet on 3/16/10.
//  Copyright 2010 Jerome Poichet. All rights reserved.
//

#ifndef __SSOCKET_H__
#define __SSOCKET_H__

#include <openssl/ssl.h>
#include <openssl/err.h>

typedef struct ssocket_t ssocket;

struct ssocket_t {
    char *hostname;
    unsigned short port;
    unsigned short doSSL;

    int sd;
    FILE *sdf;
    
    SSL_CTX *ctx;
    SSL *ssl;

    int verbose;

    int (*connect)(ssocket *sock);    
    int (*write)(ssocket *sock, const char *buffer);
    int (*read)(ssocket *sock, char **buffer, int len);
};

ssocket *ssocket_create(const char *hostname, unsigned short port, unsigned short ssl);
void ssocket_destroy(ssocket **client);

#endif
