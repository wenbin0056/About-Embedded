//
//  thttpd.h
//  HttpServer
//
//  Created by Space Zhong on 2018/9/12.
//  Copyright © 2018年 Space Zhong. All rights reserved.
//

#ifndef thttpd_h
#define thttpd_h

#include "timers.h"

typedef struct THTTPDAttr
{
    int argc;
    char ** argv;
}thttpAttr;

extern void* thttpd_start_main(void* pvParam);

void occasional( ClientData client_data, struct timeval* nowP );

//extern int thttpd_start_main( int argc, char** argv);

#endif /* thttpd_h */
