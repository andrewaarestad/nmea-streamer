NMEA Streaming Test Harness

This software allows you to test NMEA stream consumers.  It takes an NMEA file and streams the bytes of each fix report to your software.

Simply add code to -(void) streamByte:(char)byte in NmeaTester.m to process the bytes with your software.

