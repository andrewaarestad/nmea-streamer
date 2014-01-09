//
//  AppDelegate.h
//  NmeaTester
//
//  Created by Andrew on 12/28/13.
//  Copyright (c) 2013 AA Engineering LLC. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;

-(void)console:(NSString*)text;
-(void)playbackEnded;

@end
