//
//  SecureSocket.m
//
//  Created by Jerome Poichet on 3/16/10.
//  Copyright 2010 Jerome Poichet. All rights reserved.
//

#import "SecureSocket.h"


@implementation SecureSocket

- (void) dealloc
{
    if (scrap) {
        free(scrap);
    }
    [buffer release];
    if (sock) {
        ssocket_destroy(&sock);
        sock = nil;
    }
    [super dealloc];
}

- (BOOL) connectToHost: (NSString *) hostname onPort: (int) port withSSL: (BOOL) doSSL
{
    const char *host = [hostname cStringUsingEncoding: NSASCIIStringEncoding];
    sock = ssocket_create(host, port, doSSL ? 1 : 0);
    if (!sock) {
        NSLog(@"Could not create socket");
        return NO;
    }
    
    sock->verbose = 0;
    if (sock->connect(sock) == -1) {
        ssocket_destroy(&sock);
        sock = nil;
        return NO;
    }
    
    return YES;
}

- (NSString *) readLine
{
    int cnt;
    NSRange range;

    if (!sock) return nil;

    if (!buffer) {
        buffer = [[[NSMutableString alloc] init] retain];
    }
    if (!scrap) {
        scrap = (char *)malloc(1024);
        memset(scrap, 0, 1024);
        
    }
    
    range = [buffer rangeOfString: @"\n"];
    while (range.location == NSNotFound) {
        if ((cnt = sock->read(sock, &scrap, 1024)) <= 0) {
            return nil;
        }
        
        [buffer appendString: [NSString stringWithCString: scrap encoding: NSASCIIStringEncoding]];        
        range = [buffer rangeOfString: @"\n"];
    }
    
    
    //NSLog(@"line range %d/%d", range.location, range.length);
    range.length = range.location + 1;
    range.location = 0;
    NSString *line = [buffer substringWithRange: range];
    range.length += 0;
    //NSLog(@"line (%d) '%@'", [line length], line);
    //NSLog(@"buffer %d", [buffer length]);
    //NSLog(@"buffer range %d/%d", range.location, range.length);
    [buffer deleteCharactersInRange: range];
    //NSLog(@"buffer %d", [buffer length]);
    return line;
}

- (void) write: (NSString *) message
{
    if (!sock) return;
    
    const char *msg = [message cStringUsingEncoding: NSASCIIStringEncoding];
    sock->write(sock, msg);
}


@end
