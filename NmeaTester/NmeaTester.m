//
//  NmeaTester.m
//  NmeaTester
//
//  Created by Andrew on 12/28/13.
//  Copyright (c) 2013 AA Engineering LLC. All rights reserved.
//

#import "NmeaTester.h"


#import "AppDelegate.h"

#define MAX_STRINGS_PER_UPDATE 500

@interface NmeaTester()

@property (nonatomic) NSTimer *playbackTimer;
@property (nonatomic) NSArray *stringArray;
@property (nonatomic) NSInteger masterBufferIndex;
@property (nonatomic, weak) AppDelegate *appDelegate;

-(void) streamByte:(char)byte;

@end

@implementation NmeaTester

#pragma mark Public Class Methods

+(void) processFile:(NSURL*)fileURL rate:(NSInteger)rate
{
    NmeaTester *tt = [self sharedInstance];
    
    if (rate > 0)
    {
        tt.playbackTimer = [NSTimer timerWithTimeInterval:1.0f / rate target:tt selector:@selector(timerTrigger:) userInfo:Nil repeats:YES];
        
        NSString *fileContents = [NSString stringWithContentsOfURL:fileURL
                                                          encoding:NSUTF8StringEncoding
                                                             error:nil];
        
        tt.stringArray = [fileContents componentsSeparatedByString:@"\r\n"];

        tt.masterBufferIndex = 0;
        
        NSRunLoop *runLoop = [NSRunLoop currentRunLoop];
        [runLoop addTimer:tt.playbackTimer forMode:NSDefaultRunLoopMode];
    }
    else
    {
        // send entire file directly to test harness
        NSString *fileContent = [NSString stringWithContentsOfURL:fileURL encoding:NSUTF8StringEncoding error:nil];
        
        [tt sendStringsToTestHarness:[NSArray arrayWithObject:fileContent]];
    }
}

+(void) stopPlayback
{
    NmeaTester *tt = [self sharedInstance];
    [tt stopPlayback];
}

#pragma mark -
#pragma mark Private Class Methods

+ (id)sharedInstance
{
    static NmeaTester *sharedInstance = nil;
    static dispatch_once_t onceToken = 0;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[self alloc] init];
        sharedInstance.appDelegate = (AppDelegate *)[[NSApplication sharedApplication] delegate];
    });
    return sharedInstance;
}


#pragma mark -
#pragma mark Private Instance Methods

-(void)timerTrigger:(NSTimer *)timer
{
    // Get strings up to next RMC
    NSMutableArray *currentStrings = [self fillBufferToRMC];
    
    
    // Feed current buffer through test harness
    [self sendStringsToTestHarness:currentStrings];
    
    
    // Detect end of master buffer
    if (self.masterBufferIndex == [self.stringArray count])
    {
        [self stopPlayback];
        [self.appDelegate console:@"Playback complete."];
    }
    else
    {
        // Increment to next sentence for next block
        self.masterBufferIndex++;
    }
}

-(NSMutableArray *)fillBufferToRMC
{
    NSMutableArray *currentStrings = [NSMutableArray array];
    BOOL rmcFound = NO;
    
    // Fill buffer to next RMC
    while (!rmcFound &&
           [currentStrings count] < MAX_STRINGS_PER_UPDATE &&
           self.masterBufferIndex < [self.stringArray count])
    {
        if ([[self.stringArray objectAtIndex:self.masterBufferIndex] rangeOfString:@"RMC,"].location == NSNotFound)
        {
            [currentStrings addObject:[self.stringArray objectAtIndex:self.masterBufferIndex]];
            self.masterBufferIndex++;
        }
        else
        {
            rmcFound = YES;
        }
        
        //NSLog(@"string: %@",[self.stringArray objectAtIndex:self.currentString]);
        
    }
    return currentStrings;
}


-(void)sendStringsToTestHarness:(NSArray*)strings
{
    NSData *stringData;
    const char *bytes;
    NSUInteger currentBytes = 0;
    for (NSString *string in strings)
    {
        stringData = [string dataUsingEncoding:NSUTF8StringEncoding];
        
        bytes = [stringData bytes];
        for (int i = 0; i < [stringData length]; i++)
        {
            [self streamByte:bytes[i]];
            currentBytes++;
        }
    }
    
    [self.appDelegate console:[NSString stringWithFormat:@"Streamed %li bytes.",currentBytes]];
}


-(void) stopPlayback
{
    [self.playbackTimer invalidate];
    
    [(AppDelegate*)[[NSApplication sharedApplication] delegate] playbackEnded];
}


-(void) streamByte:(char)byte
{
    // Override point for stream processing
    // Add your code here
}

@end
