//
//  SecureSocket.h
//
//  Created by Jerome Poichet on 3/16/10.
//  Copyright 2010 Jerome Poichet. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "ssocket.h"

@interface SecureSocket : NSObject {
    ssocket *sock;
    
    NSMutableString *buffer;
    char *scrap;
}

- (BOOL) connectToHost: (NSString *) hostname onPort: (int) port withSSL: (BOOL) doSSL;

- (NSString *) readLine;
- (void) write: (NSString *) message;


@end
