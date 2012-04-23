//
//  ssocket.c
//
//  Created by Jerome Poichet on 3/16/10.
//  Copyright 2010 Jerome Poichet. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ssocket.h"

#define BUFFER_SIZE 1024

int ssocket_connect(ssocket *sock);


// I/O
int ssocket_write(ssocket *sock, const char *buffer);
int ssocket_read(ssocket *sock, char **buffer, int len);

ssocket *ssocket_create(const char *hostname, unsigned short port, unsigned short ssl)
{
    ssocket *sock = NULL;
    sock = (ssocket *)malloc(sizeof(ssocket));
    memset(sock, 0, sizeof(ssocket));

    sock->hostname = strdup(hostname);
    sock->port = port;
    sock->doSSL = ssl;

    sock->verbose = 0;

    sock->connect = ssocket_connect;
    sock->write = ssocket_write;
    sock->read = ssocket_read;
    
    return sock;
}

void ssocket_destroy(ssocket **sock)
{
    if ((*sock)->doSSL) {
        SSL_shutdown((*sock)->ssl);
        SSL_free((*sock)->ssl);
        SSL_CTX_free((*sock)->ctx);
    }
    close((*sock)->sd);
    free((*sock)->hostname);
    free(*sock);
}

int ssocket_connect(ssocket *sock)
{
    struct hostent *hp;
	struct sockaddr_in pin;

    if ((hp = gethostbyname(sock->hostname)) == 0) {
        fprintf(stderr, "Failed to retrieve hostname\n");
        return -1;
    }

	memset(&pin, 0, sizeof(pin));
	pin.sin_family = AF_INET;
	pin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
	pin.sin_port = htons(sock->port);
    
    if ((sock->sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Failed to create socket\n");
        return -1;
    }

    if (connect(sock->sd, (struct sockaddr *)&pin, sizeof(pin)) == -1) {
        fprintf(stderr, "Failed to connect\n");
        return -1;
    }

    sock->sdf = fdopen(sock->sd, "r");

    if (sock->verbose) fprintf(stderr, "Connected to %s:%d\n", sock->hostname, sock->port);

    // TODO if SSL, do SSL negotiation here
    if (sock->doSSL) {
        if (sock->verbose) fprintf(stderr, "SSL Enabled\n");
        SSL_library_init();
        SSL_load_error_strings();
        sock->ctx = SSL_CTX_new(SSLv23_client_method());

        sock->ssl = SSL_new(sock->ctx);
        if (sock->ssl == NULL) {
            long err = ERR_get_error();
            fprintf(stderr, "Error preparing SSL context: %s\n", ERR_error_string(err, NULL));
        }
                    
        SSL_set_fd(sock->ssl, sock->sd);

        if (SSL_connect(sock->ssl) <= 0) {
            long err = ERR_get_error();
            fprintf(stderr, "SSL Connect error: %s\n", ERR_error_string(err, NULL));
            return -1;
        }
        
        if (sock->verbose) fprintf(stderr, "SSL Connected\n");
                    
    }

    return 0;
}

int ssocket_write(ssocket *sock, const char *buffer)
{
    int len = 0;
    
    if (!sock) return -1;
    if (!sock->sd) return -1;
    
    if (sock->verbose) fprintf(stderr, "-> %s", buffer);

    if (sock->doSSL) {
        if ((len = SSL_write(sock->ssl, buffer, strlen(buffer))) <= 0) {
            fprintf(stderr, "Failed to send message\n");
            return -1;
        }
    } else {
        if ((len = send(sock->sd, buffer, strlen(buffer), 0)) == -1) {
            fprintf(stderr, "Failed to send message\n");
            return -1;
        }
    }
    
    return len;
}

int ssocket_read(ssocket *sock, char **buffer, int len)
{
    int cnt;
    
    if (!sock) return -1;
    if (!sock->sd) return -1;

    
    if (sock->doSSL) {
        if ((cnt = SSL_read(sock->ssl, *buffer, len)) <= 0) {
            fprintf(stderr, "Failed to read from socket (SSL)\n");
            return -1;
        } else {
            (*buffer)[cnt] = '\0';
            if (sock->verbose) fprintf(stderr, "<- (%ld) %s", strlen(*buffer), *buffer);
            return cnt;
        }
    } else {
        if ((cnt = recv(sock->sd, *buffer, len, 0)) <= 0) {
            fprintf(stderr, "Failed to read from socket\n");
            return -1;            
        }
        if (sock->verbose) fprintf(stderr, "<- (%ld) %s", strlen(*buffer), *buffer);
        return cnt;
    }
}

