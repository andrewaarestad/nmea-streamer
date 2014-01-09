//
//  AppDelegate.m
//  NmeaTester
//
//  Created by Andrew on 12/28/13.
//  Copyright (c) 2013 AA Engineering LLC. All rights reserved.
//

#import "AppDelegate.h"

#import "NmeaTester.h"

@interface AppDelegate()

- (IBAction)processFileButtonPressed:(id)sender;
- (IBAction)stopButtonPressed:(id)sender;

@property (weak) IBOutlet NSTextField *processStatusLabel;
@property (unsafe_unretained) IBOutlet NSTextView *consoleTextField;
@property (weak) IBOutlet NSTextField *customRateTextField;
@property (weak) IBOutlet NSMatrix *rateSelectionRadioButton;
@property (weak) IBOutlet NSButton *serverCommunicationCheckbox;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

- (IBAction)processFileButtonPressed:(id)sender
{    
    
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory2 = [paths objectAtIndex:0];
    NSURL *myUrl = [NSURL fileURLWithPath:documentsDirectory2];
    [openDlg setDirectoryURL:myUrl];
    
    // Enable the selection of files in the dialog.
    [openDlg setCanChooseFiles:YES];
    
    // Enable the selection of directories in the dialog.
    [openDlg setCanChooseDirectories:YES];
    
    [openDlg setAllowsMultipleSelection:NO];
    
    
    
    // Display the dialog.  If the OK button was pressed,
    // process the files.
    if ( [openDlg runModal] == NSOKButton )
    {
        // Get an array containing the full filenames of all
        // files and directories selected.
        NSURL* fileURL = [openDlg URL];
        
        
        [_processStatusLabel setStringValue:@"Working..."];
        
        [self console:@"Processing file..."];
        
        NSInteger playbackRate = 0;
        
        if ([_rateSelectionRadioButton selectedRow] == 1)
        {
            playbackRate = [_customRateTextField.stringValue integerValue];
        }
        
        
        [NmeaTester processFile:fileURL rate:playbackRate];
        
        
    }
}

- (IBAction)stopButtonPressed:(id)sender {
    [NmeaTester stopPlayback];
    [_processStatusLabel setStringValue:@"Idle"];
    [self console:@"Stopped playback."];
}


-(void)console:(NSString*)text
{
    [_consoleTextField setString:[_consoleTextField.string stringByAppendingFormat:@"%@\n",text]];
    
    [_consoleTextField scrollPageDown:nil];
    
    
}

-(void)playbackEnded
{
    [_processStatusLabel setStringValue:@"Idle"];
}
@end
