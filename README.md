# SecureSocket

SecureSocket is an Objective-C (Cocoa) API to UNIX TCP sockets, with the added possibility to enable SSL. 

## Installation

Add the 4 files to your project, and link your project to libcrypto.dylib and libssl.dylib.

## Utilisation

    #import "SecureSocket.h"

    - (void) someMethod
    {
        SecureSocket *socket = [[SecureSocket alloc] init];

        if ([socket connectToHost: "example.com" onPort: 465 withSSL: YES]) {
            NSString *banner = [socket readLine];
            [socket write: @"EHLO some.host.com\r\n"];
        }
    }

## License

You can use this code in commercial project, attribution is welcomed but not required. If you modify the code in a manner that could be useful to others then please do share, again you don't have too.

