//
//  SPS_CMDConst.h
//  HttpServer
//
//  Created by Space Zhong on 2018/9/18.
//  Copyright © 2018年 Space Zhong. All rights reserved.
//

#ifndef SPS_CMDConst_h
#define SPS_CMDConst_h
#include "hi_type.h"

#define MAX_CGI_INTER 64
#define MAX_CMD_INTER 128
#define HISNET   "HISNET"
#define CGI_POSTFIX ".cgi"
#define CGI_METHOD_GET "get"
#define CGI_METHOD_SET "set"
#define MAX_CNT_CMD 4098 /* Command Line Maximum Length */
#define MAX_CNT_CGI 3072 /* Maximum Length for CGI */
#define MAX_CNT_CGI_OUT 2048 /* Maximum Output String Length */
#define MAX_NUM_CMD 128  /* Maximum Param Number */
#define MAX_CNT_EVERYCMD 1024 /* Maximum Length for Parameter */
#define MAX_CGI_INTER 64
#define MAX_CMD_INTER 128
#define MAX_LEN_URL 128
#define HI_NULL_PTR ((void*)0)
#define HISNET_MAX_ACT_LEN          10
#define MAX_LEN 1024
#define HISNET_LOG(pszModeName, u32Level, fmt, args ...)  ProductLog(u32Level, pszModeName, fmt, ##args)
#define ProductLog(level, module, fmt, args...)
#define MAX_ACCESSLOG_LEN (1024)
#define MAX_HTTP_STATUS_CODE_LEN (64)
#define MAX_HTTP_MIME_TYPE_LEN (64)
#define MAX_HTTP_METHOD_LEN (16)
#define DEBUG_CGI_PRINT(x, y, ...)
#define _U  0x01    /* upper */
#define _L  0x02    /* lower */
#define _D  0x04    /* digit */
#define _C  0x08    /* cntrl */
#define _P  0x10    /* punct */
#define _S  0x20    /* white space (space/lf/tab) */
#define _X  0x40    /* hex digit */
#define _SP 0x80    /* hard space (0x20) */
#define hi_ismask(x) (hi_ctype[(int)(unsigned char)(x)])
#define hi_isprint(c)  ((hi_ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define hi_isxdigit(c) ((hi_ismask(c)&(_D|_X)) != 0)

/************ ****** arguments parse ****** ****** ********/
#define HI_ERR_ARG_BASE                 0xA102C000
#define HI_ERR_ARG_WRONG_FMT            (HI_ERR_ARG_BASE + 0x01)
#define HI_ERR_ARG_UNKOWN_ARG           (HI_ERR_ARG_BASE + 0x02)
#define HI_ERR_ARG_NOT_DEFINE_NO_OPT    (HI_ERR_ARG_BASE + 0x03)
#define HI_ERR_ARG_NO_OPT               (HI_ERR_ARG_BASE + 0x05)
#define HI_ERR_ARG_NO_THIS_OPT          (HI_ERR_ARG_BASE + 0x06)
#define HI_ERR_ARG_OPT_NOTSET           (HI_ERR_ARG_BASE + 0x07)
#define HI_ERR_ARG_DEFINE               (HI_ERR_ARG_BASE + 0x08)
#define HI_ERR_ARG_MUSTOPT_NOTSET       (HI_ERR_ARG_BASE + 0x09)
#define HI_ERR_ARG_NOT_IN_REGION        (HI_ERR_ARG_BASE + 0x0A)
#define LOG_LEVEL_EMERG     PDT_LOG_ERROR      /* system is unusable */
#define LOG_LEVEL_ALERT      PDT_LOG_ERROR       /* action must be taken immediately */
#define LOG_LEVEL_FATAL      PDT_LOG_ERROR       /* critical conditions */
#define LOG_LEVEL_ERROR     PDT_LOG_ERROR       /* error conditions */
#define LOG_LEVEL_WARN      PDT_LOG_WARNING       /* warning conditions */
#define LOG_LEVEL_NOTICE    PDT_LOG_DEBUG       /* normal but significant condition */
#define LOG_LEVEL_INFO        PDT_LOG_DEBUG       /* informational */
#define LOG_LEVEL_DEBUG      PDT_LOG_DEBUG       /* debug-level messages */
#define LOG_LEVEL_TRACE      PDT_LOG_DEBUG       /*for trace only, if want to use this*/
#define ARG_S4 "    "     /*4 space*/
#define HISNET_SVR_RTN_ERROR                                    -2222
#define HISNET_SVR_REGFUNC_NULL                                 -3333
#define HISNET_SVR_CMD_INVALID                                  -4444
#define HISNET_SVR_ARGPARSER_ERROR                              -5555
#define HISNET_SVR_FeiYuCmdExecute_ERROR                        -6666





#define VALUE_REGION_REGION     1       /* Region */
#define VALUE_REGION_SINGLE     0xFE    /* Single Value */
#define VALUE_REGION_DWNLMT     2       /* Exist Minimum Limit */
#define VALUE_REGION_NO_DWNLMT  0xFD    /* No Minimum Limit */
#define VALUE_REGION_UPLMT      4       /* Exist Maximum Limit */
#define VALUE_REGION_NO_UPLMT   0xFB    /* No Maximum Limit */
#define ARG_SET_REGION(u8FRegion)       (u8FRegion |= VALUE_REGION_REGION)      /* Region Bit */
#define ARG_CLR_REGION(u8FRegion)       (u8FRegion &= VALUE_REGION_SINGLE)      /* Single Value */
#define ARG_SET_DWNLMT(u8FRegion)       (u8FRegion |= VALUE_REGION_DWNLMT)      /* Set Minimum Limit Bit */
#define ARG_CLR_DWNLMT(u8FRegion)       (u8FRegion &= VALUE_REGION_NO_DWNLMT)   /* Clear Minimum Limit */
#define ARG_SET_UPLMT(u8FRegion)        (u8FRegion |= VALUE_REGION_UPLMT)       /* Set Maximum Limit Bit */
#define ARG_CLR_UPLMT(u8FRegion)        (u8FRegion &= VALUE_REGION_NO_UPLMT)    /* Clear Maximum Limit */

#define ARG_IS_REGION(u8FRegion)        (u8FRegion & VALUE_REGION_REGION)           /* Region or not */
#define ARG_IS_DWNLMT(u8FRegion)        ((u8FRegion & VALUE_REGION_DWNLMT)  >> 1)   /* Exist Minimum Limit or not */
#define ARG_IS_UPLMT(u8FRegion)         ((u8FRegion & VALUE_REGION_UPLMT)   >> 2)   /* Exist Maximum Limit or not */

typedef int (*PfnCgiStringProc)(void *pData, char* pszfmt, ...);
typedef int (*CGI_CMD_CmdProc)(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData);

typedef struct cgi_cmd_trans
{
    char             cgi[MAX_CGI_INTER];
    char             cmd[MAX_CMD_INTER];
    char             method[8];             /*get|set|command*/
    CGI_CMD_CmdProc  cmdProc;               /*CallBack*/
}cgi_cmd_trans_s;
typedef struct hisnet_WEBSERVER_ATTR_S
{
    int argc;
    char **argv;
}HISNET_WEBSERVER_ATTR_S;

typedef struct hiARGOPT_STRUCT_S
{
    HI_CHAR*        pszOpt;         /** Option Name, eg. :"XXX" in "cmd -XXX"  */
    HI_U32          u32Type;        /** Use demo_arg_type, eg.:ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING */
    HI_CHAR*        pszValueRegion; /** Optional Value or Region, Application confirm
                                     1) Option Value : "1|2|3|all",  can be used to string,char,int
                                     2) Option Region : "0~10","1~","~12","a~z","a~";,  can be used to char,int,float
                                     2) Option Region :  "~-12|0~10|20","~a|c~e|x~y|X~Y";,  can be used to int,char,float */
    HI_CHAR         s8Isset;        /** 1: be set, 0 not set */
    HI_U8           au8resv[3];
    HI_CHAR*        pszHelpMsg;
    HI_VOID*        pValue;
    HI_U32          u32ValueBuffersize;/*pValue BufferSize*/
} ARG_OPT_S;

static unsigned char hi_ctype[] = {
    _C,_C,_C,_C,_C,_C,_C,_C,            /* 0-7 */
    _C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C,     /* 8-15 */
    _C,_C,_C,_C,_C,_C,_C,_C,            /* 16-23 */
    _C,_C,_C,_C,_C,_C,_C,_C,            /* 24-31 */
    _S|_SP,_P,_P,_P,_P,_P,_P,_P,            /* 32-39 */
    _P,_P,_P,_P,_P,_P,_P,_P,            /* 40-47 */
    _D,_D,_D,_D,_D,_D,_D,_D,            /* 48-55 */
    _D,_D,_P,_P,_P,_P,_P,_P,            /* 56-63 */
    _P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,  /* 64-71 */
    _U,_U,_U,_U,_U,_U,_U,_U,            /* 72-79 */
    _U,_U,_U,_U,_U,_U,_U,_U,            /* 80-87 */
    _U,_U,_U,_P,_P,_P,_P,_P,            /* 88-95 */
    _P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,  /* 96-103 */
    _L,_L,_L,_L,_L,_L,_L,_L,            /* 104-111 */
    _L,_L,_L,_L,_L,_L,_L,_L,            /* 112-119 */
    _L,_L,_L,_P,_P,_P,_P,_C,            /* 120-127 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* 128-143 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* 144-159 */
    _S|_SP,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,   /* 160-175 */
    _P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,_P,       /* 176-191 */
    _U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,       /* 192-207 */
    _U,_U,_U,_U,_U,_U,_U,_P,_U,_U,_U,_U,_U,_U,_U,_L,       /* 208-223 */
    _L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,       /* 224-239 */
    _L,_L,_L,_L,_L,_L,_L,_P,_L,_L,_L,_L,_L,_L,_L,_L};      /* 240-255 */

typedef enum hiARG_TYPE_E
{
    /** ARG_TYPE_NO_PARA / ARG_TYPE_NO_OPT only canbe select one of them */
    /** ARG_TYPE_STRING /ARG_TYPE_CHAR /ARG_TYPE_INT / ARG_TYPE_FLOAT only canbe select one of them */
    ARG_TYPE_MUST       = 1,   /* Required Arg */
    ARG_TYPE_NO_OPT     = 2,   /* No option is allowed(-XXX), only one parameter */
    ARG_TYPE_NO_PARA    = 4,   /* Single, no parameter */
    ARG_TYPE_STRING     = 8,   /* String */
    ARG_TYPE_CHAR       = 16,  /* Char */
    ARG_TYPE_INT        = 32,  /* Interger */
    ARG_TYPE_FLOAT      = 64,  /* Float */
    ARG_TYPE_HELP       = 128, /**/
    ARG_TYPE_SINGLE     = 256, /* Solo Option, or return parse error */
    ARG_TYPE_END        = 512,
    ARG_TYPE_BUTT       = 1024,
    ARG_TYPE_4BYTE      = 0xFFFFFFFF
} ARG_TYPE_E;



#endif /* SPS_CMDConst_h */
