//
//  PvtManager.h
//  NmeaTester
//
//  Created by Andrew on 12/29/13.
//  Copyright (c) 2013 AA Engineering LLC. All rights reserved.
//

#ifndef NmeaTester_PvtManager_h
#define NmeaTester_PvtManager_h

//#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
//#include <time.h>
//#include <math.h>



typedef struct {
    double latitude;
    double longitude;
    double altitude;
    struct timeval timetag;
} gnss_location_t;


void receivedGnssLocation(gnss_location_t location);

#endif
