//
//  NmeaTester.h
//  NmeaTester
//
//  Created by Andrew on 12/28/13.
//  Copyright (c) 2013 AA Engineering LLC. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NmeaTester : NSObject

+(void) processFile:(NSURL*)fileURL rate:(NSInteger)rate;
+(void) stopPlayback;

@end
