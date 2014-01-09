/*
	NmeaParser.c

	General purpose tools for parsing NMEA streams.

	Data Flow:

	- bytes received through calls to receiveNmeaByte()
	- bytes buffered until end of sentence is detected
	- 
*/

//===================================================
// INCLUDES
//===================================================

#include "NmeaParser.h"
#include "PvtManager.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//===================================================
// DECLARATIONS
//===================================================

#define BYTE_BUFFER_SIZE 100


#define SENTENCE_VALIDITY_MIN_LENGTH 10
#define SENTENCE_ID_BUFFER_SIZE 12
#define SENTENCE_VALIDITY_MAX_LENGTH 80

typedef struct {
	char byteBuffer[BYTE_BUFFER_SIZE];
	int byteBufferPointer;
} nmea_parser_local_t;


nmea_parser_local_t pLocal;


void processSentence(char *sentence, int length);
bool ensureSentenceValidity(char *sentence, int length);
void extractSentenceId(char *sentenceIdBuffer, char *sentence);
//struct timeval convertTimeOfDayAndDateToEpochTime(char *hhmmss, char *date);
//int get_weekday(char * str);
//struct tm makeTmStruct(char *hhmmss, char *datestr);
struct timeval makeTimevalStruct(char *hhmmss, char *datestr);

// Sentence types
void processRMC(char *sentence);



//===================================================
// FUNCTION DEFINITIONS
//===================================================

void nmeaParserInit()
{
	pLocal.byteBufferPointer = 0;
}

void receiveNmeaByte(char byte)
{

	if (pLocal.byteBufferPointer < BYTE_BUFFER_SIZE)
	{
		pLocal.byteBuffer[pLocal.byteBufferPointer++] = byte;
	}
    
    if (byte == 0xA)  // trigger sentence processing at line feed
    {
        pLocal.byteBuffer[pLocal.byteBufferPointer] = 0;
        processSentence(pLocal.byteBuffer,pLocal.byteBufferPointer);
        
        pLocal.byteBufferPointer = 0;
    }
}

void processSentence(char *sentence, int length)
{
    
    char sentenceId[SENTENCE_ID_BUFFER_SIZE];
    bool sentenceValid;
    
    sentenceValid = ensureSentenceValidity(sentence, length);
    
    
    if (sentenceValid)
    {
        
        extractSentenceId(sentenceId,sentence);
        
        
        if (strstr(sentenceId, "RMC") != NULL)
        {
            //printf("%s",sentence);
            processRMC(sentence);
            //printf("\n");
        }
        else if (strstr(sentenceId, "GGA") != NULL)
        {
            //printf("%s",sentence);
        }
        else /* default: */
        {
        }
        
    }
    else
    {
        //printf("Invalid sentence detected.\n");
        //printf("%s",sentence);
    }
    
    
    
}



bool ensureSentenceValidity(char *sentence, int length)
{
    if (length < SENTENCE_VALIDITY_MIN_LENGTH)
    {
        return false;
    }
    
    if (sentence[0] != '$')
    {
        return false;
    }
    
    return true;
}




void extractSentenceId(char *sentenceIdBuffer, char *sentence)
{
    int bufferPointer = 0;
    
    while (sentence[bufferPointer] != ',' && bufferPointer < SENTENCE_VALIDITY_MAX_LENGTH && sentence[bufferPointer] != 0)
    {
        sentenceIdBuffer[bufferPointer] = sentence[bufferPointer];
        bufferPointer++;
    }
}



/*
 $GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
    RMC,hhmmss.ss,A,llll.ll,a,yyyyyy.yy,a,x.x,x.x,xxxx,x.x,a*hh<CR><LF>
 
 Where:
 RMC          Recommended Minimum sentence C
 123519       Fix taken at 12:35:19 UTC
 A            Status A=active or V=Void.
 4807.038,N   Latitude 48 deg 07.038' N
 01131.000,E  Longitude 11 deg 31.000' E
 022.4        Speed over the ground in knots
 084.4        Track angle in degrees True
 230394       Date - 23rd of March 1994
 003.1,W      Magnetic Variation
 *6A          The checksum data, always begins with *
*/
void processRMC(char *sentence)
{
    //printf("RMC: %s",sentence);
    
    char hhmmss[15];
    char rmcDate[7];
    
    bool dateFound = false;
    bool sentenceValid = true;
    
    gnss_location_t location;
    
    int tokenIndex = 0;
    
     char processBuffer[BYTE_BUFFER_SIZE];
     strcpy(processBuffer, sentence);
     char *token = strtok(processBuffer, ",");
     while (token && sentenceValid)
     {
         //printf("token: %s\n", token);
         token = strtok(NULL, ",");
         
         tokenIndex++;
         
         switch (tokenIndex) {
             case 1:
                 // TODO: handle token too long
                 strcpy(hhmmss, token);
                 break;
                 
             case 2:
                 // Detect sentence marked as invalid fix
                 if (strcmp(token, "V") == 0)
                 {
                     sentenceValid = false;
                 }
                 break;
                 
             case 3:
                 location.latitude = atof(token) / 100;
                 break;
                
             case 4:
                 // Detect negative latitude - "South"
                 if (strcmp(token, "S") == 0)
                 {
                     location.latitude = -location.latitude;
                 }
                 break;
                 
             case 5:
                 location.longitude = atof(token) / 100;
                 break;
                 
             case 6:
                 // Detect negative longitude - "West"
                 if (strcmp(token, "W") == 0)
                 {
                     location.longitude = -location.longitude;
                 }
                 break;
                 
            /*
             case 8:
                 speed = atof(token);
                 break;
             */
                 
             case 9:
                 // TODO: handle token too long
                 strcpy(rmcDate, token);
                 dateFound = true;
                 break;
                 
             default:
                 break;
         }
     }
    
    if (dateFound && sentenceValid)
    {
        //location.timetag = convertTimeOfDayAndDateToEpochTime(hhmmss, rmcDate);
        location.timetag = makeTimevalStruct(hhmmss, rmcDate);
        
        receivedGnssLocation(location);
    }
    
    
    
}



struct timeval makeTimevalStruct(char *hhmmss, char *datestr)
{
    struct tm tm;
    time_t t;
    struct timeval timeStruct;
    
    char buf[11];
    buf[0] = datestr[0];
    buf[1] = datestr[1];
    buf[2] = '-';
    buf[3] = datestr[2];
    buf[4] = datestr[3];
    buf[5] = '-';
    
    if (datestr[4] == '9')
    {
        buf[6] = '1';
        buf[7] = '9';
    }
    else
    {
        buf[6] = '2';
        buf[7] = '0';
    }
    buf[8] = datestr[4];
    buf[9] = datestr[5];
    buf[10] = 0;
    
    
    char hourString[2];
    char minString[2];
    char secString[2];
     
    char fracSecString[6];
    
    
    int hour;
    int min;
    int sec;
    int fracSec;
    int fracSecIndex;
    int fracSecCopyIndex;
    
    if (strlen(hhmmss) > 5)
    {
        
        hourString[0] = hhmmss[0];
        hourString[1] = hhmmss[1];
        minString[0] = hhmmss[2];
        minString[1] = hhmmss[3];
        secString[0] = hhmmss[4];
        secString[1] = hhmmss[5];
        
        fracSecCopyIndex = 7;
    }
    else
    {
        
        hourString[0] = hhmmss[0];
        minString[0] = hhmmss[1];
        minString[1] = hhmmss[2];
        secString[0] = hhmmss[3];
        secString[1] = hhmmss[4];
        
        fracSecCopyIndex = 6;
    }
    
    
    hour = atoi(hourString);
    min = atoi(minString);
    sec = atoi(secString);
    
    
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;
    
    
    
    if (strptime(buf, "%d-%m-%Y", &tm) != NULL)
    {
        t = mktime(&tm);
    }
    
    
    
    
    fracSecIndex = 0;
    while (hhmmss[fracSecCopyIndex] != 0)
    {
        fracSecString[fracSecIndex] = hhmmss[fracSecCopyIndex];
        fracSecCopyIndex++;
        fracSecIndex++;
    }
    
    fracSec = atof(fracSecString);
    
    // Convert to microseconds
    // TODO: Handle fracSec size > 6
    fracSec = fracSec * pow(10,6-fracSecIndex);
    
    
    timeStruct.tv_sec = t;
    timeStruct.tv_usec = fracSec;
    
    return timeStruct;
    
}

