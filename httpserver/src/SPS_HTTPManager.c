//
//  SPS_CMDHandle.c
//  HttpServer
//
//  Created by Space Zhong on 2018/9/18.
//  Copyright © 2018年 Space Zhong. All rights reserved.
//

#include "SPS_HTTPManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>
#include "libhttpd.h"
#include "thttpd.h"
#include "timers.h"

#include "cmdproc.h"
#define DEBUG_LINE do{\
	printf("%s entry,%d\n",__func__,__LINE__);\
	}while(0);

pthread_t g_webserveru32Thread;

HI_S32 Hisnet_CMD_SYS_BuzzerAttr(int argc, const char* argv[], PfnCgiStringProc pfnCGIStringProc, void* pData);
HI_S32 Hisnet_Server_ARG_Parser(HI_S32 argc, const HI_CHAR ** argv, ARG_OPT_S * opts);
static ARG_OPT_S* Hisnet_ARGFindOpt_NoArg(ARG_OPT_S * opts);
HI_S32 Hisnet_ARG_SetValue(ARG_OPT_S * pOpt, const char * szArgv);
HI_S32 Hisnet_Server_ARG_OptIsSet(HI_CHAR * opt_name, ARG_OPT_S * opts);
static ARG_OPT_S* Hisnet_ARGFindOptStr(ARG_OPT_S* opts, const HI_CHAR* strOpt);
HI_S32 Hisnet_ARG_CheckRegion(ARG_OPT_S* pOpt, const char* pszArgv);
void textCGI(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData);

HI_S32 Hisnet_CMD_SysTimeCfg(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData);
HI_S32 Hisnet_CMD_GetVersion(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData);
HI_S32 Hisnet_CMD_GetSDStatus(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData);
HI_S32 Hisnet_CMD_SetSDFormat(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData);
HI_S32 Hisnet_CMD_GetGPSInfo(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData);

HI_S32 Hisnet_CMD_TimeIntervalCfg(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData);
HI_S32 Hisnet_CMD_ExposureAttrCfg(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData);
HI_S32 Hisnet_CMD_ExposureModeCfg(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData);
HI_S32 Hisnet_CMD_SetStartSnapTask(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData);
HI_S32 Hisnet_CMD_SetStopSnapTask(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData);
HI_S32 Hisnet_CMD_SetSnapCalibration(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData);
HI_S32 Hisnet_CMD_SetStartSnapOnce(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData);

HI_S32 Hisnet_CMD_GetRemoteFileList(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData);


static cgi_cmd_trans_s cgi_cmd [MAX_LEN] =
{
	{"getsystime.cgi",              "getsystime&-act=get&",         CGI_METHOD_GET,     Hisnet_CMD_SysTimeCfg},
    {"setsystime.cgi",              "setsystime&-act=set&",         CGI_METHOD_SET,     Hisnet_CMD_SysTimeCfg},    
    {"getversion.cgi",              "getversion&-act=get&",         CGI_METHOD_GET,     Hisnet_CMD_GetVersion},
	{"getsdstatus.cgi",				"getsdstatus&-act=get&", 		CGI_METHOD_GET, 	Hisnet_CMD_GetSDStatus},
	{"setsdformat.cgi",             "setsdformat&",                 CGI_METHOD_SET,     Hisnet_CMD_SetSDFormat},
	{"getgpsinfo.cgi", 				"getgpsinfo&-act=get&",			CGI_METHOD_GET, 	Hisnet_CMD_GetGPSInfo},

	/* Photoset cgi */
	{"gettimeinterval.cgi",         "gettimeinterval&-act=get&",    CGI_METHOD_GET,     Hisnet_CMD_TimeIntervalCfg},
	{"settimeinterval.cgi",         "settimeinterval&-act=set&",    CGI_METHOD_SET,     Hisnet_CMD_TimeIntervalCfg},
	{"getexposureattr.cgi", 		"getexposureattr&-act=get&",	CGI_METHOD_GET, 	Hisnet_CMD_ExposureAttrCfg},
	{"setexposureattr.cgi",         "setexposureattr&-act=set&",    CGI_METHOD_SET,     Hisnet_CMD_ExposureAttrCfg},
	{"getexposuremode.cgi",         "getexposuremode&-act=get&",    CGI_METHOD_GET,     Hisnet_CMD_ExposureModeCfg},
	{"setexposuremode.cgi",         "setexposuremode&-act=set&",    CGI_METHOD_SET,     Hisnet_CMD_ExposureModeCfg},
	{"setstartsnaptask.cgi",        "setstartsnaptask&-act=set&",   CGI_METHOD_SET,     Hisnet_CMD_SetStartSnapTask},
	{"setstopsnaptask.cgi",         "setstopsnaptask&-act=set&",    CGI_METHOD_SET,     Hisnet_CMD_SetStopSnapTask},
	{"setsnapcalibration.cgi", 		"setsnapcalibration&-act=set&", CGI_METHOD_SET,     Hisnet_CMD_SetSnapCalibration},
	{"setstartsnaponce.cgi", 		"setsnapcalibration&-act=set&", CGI_METHOD_SET,     Hisnet_CMD_SetStartSnapOnce},	
	
	
	/* Filemng cgi */
    {"getfilelist.cgi",             "getfilelist&-act=get&",        CGI_METHOD_GET,     Hisnet_CMD_GetRemoteFileList},
	
};

static HISNET_SVR_CB_S s_SYS_stSvrCB;

HI_S32 Hisnet_SYS_GetCallBackFunction(HISNET_SVR_CB_S *pstSvrCB)
{
    if(HI_NULL == pstSvrCB)
    {
        HISNET_LOG(HISNET, LOG_LEVEL_ERROR, "****ERROR:Hisnet_SYS_GetCallBackFunction Parameter is NULL****\n");
        return HISNET_ERROR_PARAM;
    }
	
    memcpy(pstSvrCB, &s_SYS_stSvrCB, sizeof(HISNET_SVR_CB_S));
    return HI_SUCCESS;
}

HI_S32 Hisnet_CMD_SysTimeCfg(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData)
{
    HI_S32  s32Ret = 0;
    HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};
	HI_CHAR _time[HISNET_MAX_TIMESIZE] = {0};
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act),HISNET_MAX_ACT_LEN},
            
		{(HI_CHAR *)"time",    ARG_TYPE_NO_OPT|ARG_TYPE_STRING, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n",(HI_VOID*)(_time),HISNET_MAX_TIMESIZE},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };

	HISNET_TIME_ATTR_S stSysTime;

    memset(&stSysTime, 0, sizeof(stSysTime));
	
    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        char * json = "{\"cmd\":\"capture\",\"shutter\":\"%s---%s\"}";
        char result[1024];
        snprintf(result,1024, json,"000000:124:0000","my name");
        pfnStringProc(pData,result);
        return HI_FAILURE;
    }
	
    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
    	if(0 == strncmp(_act, "set", strlen("set")))
    	{
    		if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("time", opts))
			{
			   strncpy(stSysTime.aszTime, _time, HISNET_MAX_TIMESIZE-1);
			}
			else
			{
				HISNET_LOG(HISNET, LOG_LEVEL_INFO, "time is null\n");
			}

			s32Ret = MscCmdExecute(CMD_SET_PARAM_SYS_TIME, 0, (HI_VOID *)&stSysTime, HI_NULL);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData, "Success\r\n");
			}
			else
			{
				pfnStringProc(pData, "SvrFuncResult=\"%d\"", HISNET_SVR_RTN_ERROR);
				return HI_FAILURE;
			}
    	}
		else if(0 == strncmp(_act, "get", strlen("get")))
		{
			s32Ret = MscCmdExecute(CMD_GET_PARAM_SYS_TIME, 0, HI_NULL, (HI_VOID *)&stSysTime);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData, "var time=\"%s\"\r\n", stSysTime.aszTime);
			}
			else
			{
				pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
				return HI_FAILURE;
			}
		}
		else
        {
            pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 Hisnet_CMD_GetVersion(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData)
{
	HI_S32  s32Ret = 0;
    HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act),HISNET_MAX_ACT_LEN},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };

	HISNET_VERSION_ATTR_S stVersion = {};
	
    memset(&stVersion, 0, sizeof(HISNET_VERSION_ATTR_S));

    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }
	
    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
        if(0 == strncmp(_act, "get", strlen("get")))
        {
        	s32Ret = MscCmdExecute(CMD_GET_PARAM_SYS_VERSION, 0, HI_NULL, (HI_VOID *)&stVersion);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData, (HI_CHAR *)"var version=\"%s\"\r\n", stVersion.aszVersion);
			}
			else
			{
				pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
				return HI_FAILURE;
			}
        }
        else
        {
            pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }
	
    return HI_SUCCESS;
}

HI_S32 Hisnet_CMD_GetSDStatus(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData)
{
    HI_S32 s32Ret = 0;
    HI_CHAR _act[HISNET_MAX_ACT_LEN]={0};
    HI_CHAR aszOut[64] = {0};
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act),HISNET_MAX_ACT_LEN},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };

	HISNET_SDSTATUS_S stSDStatus = {};
	
    memset(&stSDStatus, 0, sizeof(HISNET_SDSTATUS_S));

    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, "SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }
	
    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("act", opts))
    {
        if(0 == strncmp(_act, "get", strlen("get")))
        {
			s32Ret = MscCmdExecute(CMD_GET_SD_STATE, 0, HI_NULL, (HI_VOID *)&stSDStatus);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData,"var sdstatus=\"%d\";\r\n"
				  "var sdfreespace=\"%d\";\r\n"
				  "var sdtotalspace=\"%d\";\r\n"
				  "var partitionnum=\"%d\";\r\n",
				  stSDStatus.enSdStatus,\
				  stSDStatus.s32SDFreeSpace,\
				  stSDStatus.s32TotalSpace,\
				  stSDStatus.s32PartitionNum);
			}
			else if(69 == s32Ret)
			{
				pfnStringProc(pData, "SD NO EXIST\r\n");
				return HI_FAILURE;				
			}
			else
			{
				pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
				return HI_FAILURE;
			}
        }
        else
        {
	        pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
	        return HI_FAILURE;
        }
    }
	
    return HI_SUCCESS;
}

HI_S32 Hisnet_CMD_SetSDFormat(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData)
{
	DEBUG_LINE
    HI_S32 s32Ret = 0;
	HI_BOOL _format = HI_FALSE;
	HISNET_SDSTATUS_S stSDStatus;

	ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", HI_NULL, 0},
        
        {(HI_CHAR *)"format",  ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(&_format), sizeof(HI_BOOL)},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", HI_NULL, 0},
    };
#if 0
DEBUG_LINE	
    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, "SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }
DEBUG_LINE
	if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("format", opts))
	{
		if(HI_TRUE == _format)
			{
			s32Ret = MscCmdExecute(CMD_FORMAT_SD, 0, HI_NULL, HI_NULL);
		    if(HI_SUCCESS != s32Ret)
			{
				pfnStringProc(pData,"SvrFuncResult=\"%d\"", HISNET_SVR_RTN_ERROR);
				return HI_FAILURE;
		    }
		}
	}
#endif
	
    memset(&stSDStatus, 0, sizeof(HISNET_SDSTATUS_S));
DEBUG_LINE
	s32Ret = MscCmdExecute(CMD_FORMAT_SD, 0, HI_NULL,HI_NULL);
	printf("CMD_FORMAT_SD s32Ret=%d\n",s32Ret);
	if(HI_SUCCESS == s32Ret)
	{
DEBUG_LINE
		pfnStringProc(pData,"format success;\r\n");
	}
	else
	{
DEBUG_LINE	
		pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
		return HI_FAILURE;
	}	
DEBUG_LINE
    return HI_SUCCESS;
}

HI_S32 Hisnet_CMD_GetGPSInfo(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData)
{
	HI_S32  s32Ret = 0;
    HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act),HISNET_MAX_ACT_LEN},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };

	HISNET_GPSINFO_S stGPSInfo = {};
	
    memset(&stGPSInfo, 0, sizeof(HISNET_GPSINFO_S));

    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }

	
    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
        if(0 == strncmp(_act, "get", strlen("get")))
        {
        	s32Ret = MscCmdExecute(CMD_GET_GPS_STATE, 0, HI_NULL, (HI_VOID *)&stGPSInfo);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData,"var speed=\"%f\";\r\n"
			      "var altitude=\"%f\";\r\n"
			      "var latitude=\"%f\";\r\n"
			      "var NShemi=\"%c\";\r\n"
		          "var longitude=\"%f\";\r\n"
			      "var EWhemi=\"%c\";\r\n"
			      "var pitch=\"%f\";\r\n"
			      "var roll=\"%f\";\r\n"
			      "var yaw=\"%f\";\r\n",
		       	  stGPSInfo.f32Speed, stGPSInfo.f32Altitude,\
			      stGPSInfo.f32Latitude, stGPSInfo.u8NShemi,\
			      stGPSInfo.f32Longitude, stGPSInfo.u8EWhemi,\
			      stGPSInfo.f32Pitch, stGPSInfo.f32Roll, stGPSInfo.f32Yaw);
			}
			else
			{
				pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
	            return HI_FAILURE;
			}
        }
        else
        {
            pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }
	
    return HI_SUCCESS;	
}


HI_S32 Hisnet_CMD_TimeIntervalCfg(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData)
{
    HI_S32  s32Ret = 0;
    HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};
	HI_S32 interval = 0;
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act), HISNET_MAX_ACT_LEN},     

		{(HI_CHAR *)"interval",    ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(&interval), sizeof(HI_S32)},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };
	
    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }

    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
    	if(0 == strncmp(_act, "set", strlen("set")))
    	{
    		/*
			switch(interval)
			{
				case 100:
					interval = PHOTO_TIME_INTERVAL_0x1S;
					break;
				case 500:
					interval = PHOTO_TIME_INTERVAL_0x5S;
					break;	
				case 1000:
					interval = PHOTO_TIME_INTERVAL_1S;
					break;
				default:
					interval = PHOTO_TIME_INTERVAL_0x1S;
					break;
			}
		*/

			s32Ret = MscCmdExecute(CMD_SET_PARAM_SNAP_TIME_INTERVAL, interval, HI_NULL, HI_NULL);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData, "Success\r\n");
			}
			else
			{
				pfnStringProc(pData, "SvrFuncResult=\"%d\"", HISNET_SVR_RTN_ERROR);
				return HI_FAILURE;
			}
    	}
		else if(0 == strncmp(_act, "get", strlen("get")))
		{
			s32Ret = MscCmdExecute(CMD_GET_PARAM_SNAP_TIME_INTERVAL, 0, HI_NULL, (HI_VOID *)&interval);
			if(HI_SUCCESS == s32Ret)
			{
				switch(interval)
	            {
	            	case PHOTO_TIME_INTERVAL_0x1S:
	                    pfnStringProc(pData,(HI_CHAR *)"var interval=\"%d\"\r\n", 100);
	                  	break;
	                case PHOTO_TIME_INTERVAL_0x5S:
	                    pfnStringProc(pData,(HI_CHAR *)"var interval=\"%d\"\r\n", 500);
	                   	break;
		            case PHOTO_TIME_INTERVAL_1S:
	                    pfnStringProc(pData,(HI_CHAR *)"var interval=\"%d\"\r\n", 1000);
	                  	break;
	                default:
                        pfnStringProc(pData,(HI_CHAR *)"var interval=\"%d\"\r\n", 100);
						break;
				}
			}
			else
			{
				pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
				return HI_FAILURE;
			}
		}
		else
        {
            pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}
 static HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};
HI_S32 Hisnet_CMD_ExposureAttrCfg(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData)
{
	printf("%s called!\n",__func__);
    HI_S32  s32Ret = 0;
   
	HI_U8 index = 0;
	HI_S32 exptime = 0;
	HI_S32 expgain = 0;
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act), HISNET_MAX_ACT_LEN},     

		{(HI_CHAR *)"index",   ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(&index), sizeof(HI_U8)},
		
		{(HI_CHAR *)"exptime", ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(&exptime), sizeof(HI_S32)},

		{(HI_CHAR *)"expgain", ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(&expgain), sizeof(HI_S32)},
			
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };

	HISNET_EXPOSURE_ATTR_S stExposureAttr = {};

    memset(&stExposureAttr, 0, sizeof(HISNET_EXPOSURE_ATTR_S));
	
    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
	//printf("Hisnet_Server_ARG_Parser s32Ret=%d, s32Ret,opts[1].pValue=%s\n",s32Ret,opts[1].pValue);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }
DEBUG_LINE	
    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
DEBUG_LINE    
	printf("_act=%s,exptime=%d\n",_act,exptime);
    	if(0 == strncmp(_act, "set", strlen("set")))
    	{
DEBUG_LINE    	
    		if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("index", opts))
			{
DEBUG_LINE			
				stExposureAttr.u8Index = index;
			}
			else
			{
DEBUG_LINE			
				HISNET_LOG(HISNET, LOG_LEVEL_INFO, "index is null\n");
				return HI_FAILURE;
			}

			if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("exptime", opts))
			{
DEBUG_LINE			
				stExposureAttr.u32ExpTime = exptime;
			}
			else
			{
				HISNET_LOG(HISNET, LOG_LEVEL_INFO, "exptime is null\n");
				return HI_FAILURE;
			}

			if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("expgain", opts))
			{
				stExposureAttr.u32Gain = expgain;
			}
			else
			{
				HISNET_LOG(HISNET, LOG_LEVEL_INFO, "expgain is null\n");
				return HI_FAILURE;
			}

			s32Ret = MscCmdExecute(CMD_SET_PARAM_EXPOSURE_ATTR, 0, (HI_VOID *)&stExposureAttr, HI_NULL);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData, "Success\r\n");
			}
			else
			{
				pfnStringProc(pData, "SvrFuncResult=\"%d\"", HISNET_SVR_RTN_ERROR);
				return HI_FAILURE;
			}
    	}
		else if(0 == strncmp(_act, "get", strlen("get")))
		{
			if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("index", opts))
			{
				stExposureAttr.u8Index = index;
			}
			else
			{
				HISNET_LOG(HISNET, LOG_LEVEL_INFO, "index is null\n");
				return HI_FAILURE;
			}
			
			s32Ret = MscCmdExecute(CMD_GET_PARAM_EXPOSURE_ATTR, 0, HI_NULL, (HI_VOID *)&stExposureAttr);
			if(HI_SUCCESS == s32Ret)
			{			
				pfnStringProc(pData, "var exptime=\"%d\";\r\n"
					"var expgain=\"%d\";\r\n",
					stExposureAttr.u32ExpTime, stExposureAttr.u32Gain);
			}
			else
			{
				pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
				return HI_FAILURE;
			}	
		}
		else
        {
            pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 Hisnet_CMD_ExposureModeCfg(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData)
{
    HI_S32  s32Ret = 0;
    HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};
	HI_S32 expmode = 0;
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act), HISNET_MAX_ACT_LEN},     

		{(HI_CHAR *)"expmode", ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(&expmode), sizeof(HI_S32)},
			
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };
	
    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }
	
    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
    	if(0 == strncmp(_act, "set", strlen("set")))
    	{
    		if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("expmode", opts))
			{
				
			}
			else
			{
				HISNET_LOG(HISNET, LOG_LEVEL_INFO, "expmode is null\n");
				return HI_FAILURE;
			}
			
    		switch(expmode)
    		{
    			case 0:
					expmode = PHOTO_EXPOSURE_MODE_AUTO;
					break;
				case 1:
					expmode = PHOTO_EXPOSURE_MODE_MANUAL;
					break;	
    		}
			
			s32Ret = MscCmdExecute(CMD_SET_PARAM_EXPOSURE_MODE, expmode, HI_NULL, HI_NULL);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData, "Success\r\n");
			}
			else
			{
				pfnStringProc(pData, "SvrFuncResult=\"%d\"", HISNET_SVR_RTN_ERROR);
				return HI_FAILURE;
			}
    	}
		else if(0 == strncmp(_act, "get", strlen("get")))
		{		
			s32Ret = MscCmdExecute(CMD_GET_PARAM_EXPOSURE_MODE, 0, HI_NULL, &expmode);
			if(HI_SUCCESS == s32Ret)
			{		
				switch(expmode)
				{
					case PHOTO_EXPOSURE_MODE_AUTO:
						expmode = 0;
						break;
					case PHOTO_EXPOSURE_MODE_MANUAL:
						expmode = 1;
						break;
				}
				pfnStringProc(pData, "var expmode=\"%d\";\r\n", expmode);
			}
			else
			{
				pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
				return HI_FAILURE;
			}	
		}
		else
        {
            pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 Hisnet_CMD_SetStartSnapOnce(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData)
{
	HI_S32  s32Ret = 0;
    HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};

	printf("%s\n", __FUNCTION__);
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act),HISNET_MAX_ACT_LEN},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };

    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }

    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
        if(0 == strncmp(_act, "set", strlen("set")))
        {
        	s32Ret = MscCmdExecute(CMS_SNAP_ONCE_ALL, 0, HI_NULL, HI_NULL);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData, "Success\r\n");
			}
			else
			{
				pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
	            return HI_FAILURE;
			}
        }
        else
        {
            pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }
	
    return HI_SUCCESS;	
}

HI_S32 Hisnet_CMD_SetStartSnapTask(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData)
{
	HI_S32  s32Ret = 0;
    HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};

	printf("%s\n", __FUNCTION__);
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act),HISNET_MAX_ACT_LEN},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };

    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }

    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
        if(0 == strncmp(_act, "set", strlen("set")))
        {
        	s32Ret = MscCmdExecute(CMS_START_SNAP_TASK_ALL, 0, HI_NULL, HI_NULL);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData, "Success\r\n");
			}
			else
			{
				pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
	            return HI_FAILURE;
			}
        }
        else
        {
            pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }
	
    return HI_SUCCESS;	
}

HI_S32 Hisnet_CMD_SetStopSnapTask(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData)
{
	HI_S32  s32Ret = 0;
    HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act),HISNET_MAX_ACT_LEN},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };

    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }

    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
        if(0 == strncmp(_act, "set", strlen("set")))
        {
        	s32Ret = MscCmdExecute(CMS_STOP_SNAP_TASK_ALL, 0, HI_NULL, HI_NULL);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData, "Success\r\n");
			}
			else
			{
				pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
	            return HI_FAILURE;
			}
        }
        else
        {
            pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }
	
    return HI_SUCCESS;		
}

HI_S32 Hisnet_CMD_SetSnapCalibration(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData)
{
	HI_S32  s32Ret = 0;
    HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act),HISNET_MAX_ACT_LEN},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };

    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }

    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
        if(0 == strncmp(_act, "set", strlen("set")))
        {
        	s32Ret = MscCmdExecute(CMS_START_SNAP_CALIBRATION, 0, HI_NULL, HI_NULL);
			if(HI_SUCCESS == s32Ret)
			{
				pfnStringProc(pData, "Success\r\n");
			}
			else
			{
				pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
	            return HI_FAILURE;
			}
        }
        else
        {
            pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }
	
    return HI_SUCCESS;	
}

HI_S32 Hisnet_CMD_GetRemoteFileList(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void* pData)
{
    HI_S32 s32Ret = 0;
    HI_U32 i = 0, j = 0;
	HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};
	HI_U32 u32GroupNum = 0, u32WorkNum = 0;
	HI_U32 u32FileNum[5] = {0};
	GROUP_WORK_S stGroupWork; 
	
    HISNET_REMOTEFILENAME_S *pstFileList = NULL;

    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act), HISNET_MAX_ACT_LEN},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };

    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }

	memset(&stGroupWork, 0, sizeof(GROUP_WORK_S));
	
    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
        if(0 == strncmp(_act, "get", strlen("get")))
        {
			s32Ret = MscCmdExecute(CMD_GET_GROUP_NUMBER, 0, &stGroupWork, &u32GroupNum);
			printf("u32GroupNum=%d,ret=%d\n",u32GroupNum,s32Ret);
			if(HI_SUCCESS == s32Ret)
			{		
				pfnStringProc(pData, "{\"groupcount\":\"%d\",\"group\":[", u32GroupNum);
				
				for(i = 0; i < u32GroupNum; i++)
				{	
					stGroupWork.GroupIndex = i;
					s32Ret = MscCmdExecute(CMD_GET_WORK_NUMBER, 0, &stGroupWork, &u32WorkNum);
					if(HI_SUCCESS == s32Ret)
					{		
						pfnStringProc(pData, "{\"workcount\":\"%d\",\"work\":[", u32WorkNum);		
						printf("u32WorkNum=%d,ret=%d\n",u32WorkNum,s32Ret);
						for(j = 0; j < u32WorkNum; j++)
						{
							stGroupWork.WorkIndex = j;
							
							//s32Ret = MscCmdExecute(CMD_GET_FILE_NUMBER, 0, &stGroupWork, &u32FileNum);
							HI_S32 w = 0;
							for(w = 0; w < 5; w++)
							{
								stGroupWork.CamID = w;
								s32Ret |= MscCmdExecute(CMD_GET_FILE_NUMBER_CAMID, 0, &stGroupWork, u32FileNum+w);
								printf("u32FileNum=%d,ret=%d\n",u32FileNum[w],s32Ret);							
							}
							if(HI_SUCCESS == s32Ret)
							{
								if(j == u32WorkNum - 1)
								{
									if(i == u32GroupNum - 1)
									{
										pfnStringProc(pData, "{\"filecount\":\"%d,%d,%d,%d,%d\"}]}", u32FileNum[0], u32FileNum[1], u32FileNum[2], u32FileNum[3], u32FileNum[4]);
									}
									else
									{
										pfnStringProc(pData, "{\"filecount\":\"%d,%d,%d,%d,%d\"}]},", u32FileNum[0], u32FileNum[1], u32FileNum[2], u32FileNum[3], u32FileNum[4]);
									}
								}
								else
								{
									pfnStringProc(pData, "{\"filecount\":\"%d,%d,%d,%d,%d\"},", u32FileNum[0], u32FileNum[1], u32FileNum[2], u32FileNum[3], u32FileNum[4]);
								}
							}
							else
							{
								pfnStringProc(pData,"SvrFuncResult=\"%d\"\r\n",HISNET_SVR_RTN_ERROR);
								return HI_FAILURE;
							}
						}
					}
					else
					{
						pfnStringProc(pData,"SvrFuncResult=\"%d\"\r\n",HISNET_SVR_RTN_ERROR);
						return HI_FAILURE;
					}
				}
				pfnStringProc(pData, "]}");
			}
			else
			{
				pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
			}
        }
		else
        {
            pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }
	
    return HI_SUCCESS;
}


#if 0
HI_S32 Hisnet_CMD_SetSensor(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData)
{
    HI_S32  s32Ret = 0;
    HI_CHAR _act[HISNET_MAX_ACT_LEN] = {0};
    HI_S32 _sensor0 = 0, _sensor1 = 0, _sensor2 = 0, _sensor3 = 0, _sensor4 = 0, _sensor5 = 0,;
	
    ARG_OPT_S opts[] =
    {
        {(HI_CHAR *)"?",       ARG_TYPE_NO_PARA|ARG_TYPE_SINGLE, HI_NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", NULL,0},
        
        {(HI_CHAR *)"act",     ARG_TYPE_MUST|ARG_TYPE_NO_OPT|ARG_TYPE_STRING, (HI_CHAR *)"get|set", HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"print help msg\r\n", (HI_VOID*)(_act),HISNET_MAX_ACT_LEN},
            
		{(HI_CHAR *)"sensor0", ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n",(HI_VOID*)(_sensor0), sizeof(_sensor0)},

		{(HI_CHAR *)"sensor1", ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n",(HI_VOID*)(_sensor1), sizeof(_sensor1)},

		{(HI_CHAR *)"sensor2", ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n",(HI_VOID*)(_sensor2), sizeof(_sensor2)},
			
		{(HI_CHAR *)"sensor3", ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n",(HI_VOID*)(_sensor3), sizeof(_sensor3)},
			
		{(HI_CHAR *)"sensor4", ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n",(HI_VOID*)(_sensor4), sizeof(_sensor4)},
			
		{(HI_CHAR *)"sensor5", ARG_TYPE_NO_OPT|ARG_TYPE_INT, HI_NULL, HI_FALSE, {0},
			(HI_CHAR *)ARG_S4"print help msg\r\n",(HI_VOID*)(_sensor5), sizeof(_sensor5)},
		
        {(HI_CHAR *)"END",     ARG_TYPE_END, NULL, HI_FALSE, {0},
            (HI_CHAR *)ARG_S4"END\n", NULL,0},
    };

    HISNET_SVR_CB_S stSvrCB = {};

    memset(&stSvrCB, 0, sizeof(HISNET_SVR_CB_S));
	
    s32Ret = Hisnet_Server_ARG_Parser(argc, argv, opts);
    if(HI_SUCCESS != s32Ret)
    {
        pfnStringProc(pData, (HI_CHAR *)"SvrFuncResult=\"%d\"", HISNET_SVR_ARGPARSER_ERROR);
        return HI_FAILURE;
    }

	Hisnet_SYS_GetCallBackFunction(&stSvrCB);
	
    if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet((HI_CHAR *)"act", opts))
    {
    	if(0 == strncmp(_act, "set", strlen("set")))
    	{
    		if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("sensor0", opts))
			{
			   stMscCamInfo.bSnapStart[0] = HI_TRUE;
			}
			else if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("sensor1", opts))
			{
				stMscCamInfo.bSnapStart[1] = HI_TRUE;
			}
			else if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("sensor2", opts))
			{
				stMscCamInfo.bSnapStart[2] = HI_TRUE;
			}
			else if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("sensor3", opts))
			{
				stMscCamInfo.bSnapStart[3] = HI_TRUE;
			}
			else if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("sensor4", opts))
			{
				stMscCamInfo.bSnapStart[4] = HI_TRUE;
			}
			else if (HI_SUCCESS == Hisnet_Server_ARG_OptIsSet("sensor5", opts))
			{
				stMscCamInfo.bSnapStart[5] = HI_TRUE;
			}

			if(HI_NULL != stSvrCB.pfnSetSensor)
			{
				s32Ret = stSvrCB.pfnSetSensor(&stMscCamInfo);
				if(HI_SUCCESS == s32Ret)
				{
					pfnStringProc(pData, "Success\r\n");
				}
				else
				{
					pfnStringProc(pData, "SvrFuncResult=\"%d\"", HISNET_SVR_RTN_ERROR);
					return HI_FAILURE;
				}
			}
			else
			{
				pfnStringProc(pData, "SvrFuncResult=\"%d\"", HISNET_SVR_REGFUNC_NULL);
				return HI_FAILURE;
			}
    	}
		else if(0 == strncmp(_act, "get", strlen("get")))
		{
			if(HI_NULL != stSvrCB.pfnGetSysTime)
			{
				s32Ret = stSvrCB.pfnGetSysTime(&stSysTime);
				if(HI_SUCCESS == s32Ret)
				{
					pfnStringProc(pData, "var time=\"%s\"\r\n", stSysTime.aszTime);
				}
				else
				{
					pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_RTN_ERROR);
					return HI_FAILURE;
				}
			}
			else
			{
				pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_REGFUNC_NULL);
				return HI_FAILURE;
			}
		}
		else
        {
            pfnStringProc(pData, "SvrFuncResult=\"%d\"\r\n", HISNET_SVR_CMD_INVALID);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}
#endif

//#endif

HI_S32 HISNET_Occasional(struct timeval *nowP)
{
    occasional(JunkClientData, nowP);
    return HI_SUCCESS;
}

void THTTPD_strdecode(char* to, char* from)
{
    for ( ; *from != '\0'; ++to, ++from )
    {
        if ( from[0] == '%' && hi_isxdigit( from[1] ) && hi_isxdigit( from[2] ) )
        {
            *to = hexit( from[1] ) * 16 + hexit( from[2] );
            from += 2;
        }
        else if( (*from) == '+')
        {
            *to = ' ';
        }
        else
        {
            *to = *from;
        }
    }
    *to = '\0';
}

int CGI_StrCommandParse(char* strcmdin, int* argc, char** argv, int maxCnt, int maxLen)
{
    char* pszQuery;
    char* pszAndTemp = NULL;
    char* pszEqualTemp = NULL;
    char* pszMinTemp = NULL;
    int s32Num = *argc;
    int s32StrLen = 0;

    pszQuery = strcmdin;
    while( (NULL != pszQuery)&&(*pszQuery != '\0'))
    {
        pszAndTemp = strchr(pszQuery,'&');
        pszEqualTemp = strchr(pszQuery,'=');
        if((pszAndTemp != NULL)&&(pszEqualTemp != NULL))
        {
            if( strlen(pszAndTemp) > strlen(pszEqualTemp) )
            {
                pszMinTemp = pszAndTemp;
            }
            else
            {
                pszMinTemp = pszEqualTemp;
            }
        }
        else if((pszAndTemp != NULL))
        {
            pszMinTemp = pszAndTemp;
        }
        else if((pszEqualTemp != NULL))
        {
            pszMinTemp = pszEqualTemp;
        }
        else if((pszAndTemp == NULL)&&(pszEqualTemp == NULL))
        {
            s32StrLen= MIN( strlen(pszQuery), maxLen-1 );
            strncpy(argv[s32Num],pszQuery,s32StrLen);
            *(argv[s32Num]+s32StrLen)='\0';
            s32Num++;
            break;
        }
        if((strlen(pszQuery)-strlen(pszMinTemp))>0)
        {
            s32StrLen= MIN((strlen(pszQuery)-strlen(pszMinTemp)), maxLen-1 );
            strncpy(argv[s32Num],pszQuery,s32StrLen);
            *(argv[s32Num]+ s32StrLen )='\0';
            s32Num++;
            if( s32Num >= maxCnt)
            {
                break;
            }
        }
        pszQuery = pszMinTemp+1;
        pszAndTemp = NULL;
        pszEqualTemp = NULL;
        pszMinTemp = NULL;
    }
    (*argc) = s32Num;
    return 0;
}

int HI_CGI_CommandProc(char* strcmdin, PfnCgiStringProc pfnCgiStringGet, void* pData)
{
DEBUG_LINE
    int i;
    int ret;
    int argc = 0;
    char* argv[MAX_NUM_CMD] = {0};
    char* pszAllMalloc = NULL;
    CGI_CMD_CmdProc pFunCgiCmdProc = NULL;
    char szCommand[MAX_CGI_INTER] = {0};
    char* pCommand = NULL;
    char* pCgiLabel = NULL;     //'.cgi' symbol
    char* pAndLabel = NULL;     //'&' symbol
    char method[8] = {0};
    int commandFlag = 0;
    HI_CHAR aszRequest[MAX_CNT_CMD] = {0};
    if(strcmdin == NULL)
    {
        HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[error]strcmdin is NULL\n");
        return -1;
    }
DEBUG_LINE
	printf("%s\n", strcmdin);
	
    //parse command from strcmdin, the substring before ".cgi"  is the right command.
    pCgiLabel = strstr(strcmdin,CGI_POSTFIX);
    if(NULL == pCgiLabel)
    {
DEBUG_LINE    
        pAndLabel = strchr(strcmdin,'&');
        if(pAndLabel != NULL)
        {
            if((pAndLabel - strcmdin) > (MAX_CGI_INTER-1))
            {
                HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[error]input cgi cmd length too long: %d\n", (pAndLabel - strcmdin));
                return -1;
            }
            strncpy(szCommand, strcmdin,  pAndLabel - strcmdin);
            pCommand = pAndLabel + 1;
        }
        else
        {
            HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[error]invalid szCommand\n");
            return -1;
        }
    }
    else
    {
DEBUG_LINE    
        if((pCgiLabel - strcmdin) > (MAX_CGI_INTER-1))
        {
            HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[error]input cgi cmd length too long: %d\n", (pAndLabel - strcmdin));
            return -1;
        }
        strncpy(szCommand, strcmdin,  pCgiLabel - strcmdin);
        pCommand = pCgiLabel + sizeof(CGI_POSTFIX) - 1;
    }
DEBUG_LINE	
	pszAllMalloc = (char *)malloc(MAX_NUM_CMD * MAX_CNT_EVERYCMD * sizeof(char));
    if(pszAllMalloc == NULL)
    {
        HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[error] malloc the argv size is fail\n");
        return -1;
    }
DEBUG_LINE	
    memset(pszAllMalloc, 0x00, (MAX_NUM_CMD * MAX_CNT_EVERYCMD * sizeof(char)));
    for(i = 0; i < MAX_NUM_CMD; i++)
    {
        argv[i] = pszAllMalloc + (i * MAX_CNT_EVERYCMD);
    }
DEBUG_LINE	
    //find the match command in the global array cgi_cmd[]
    for(i = 0; (i < MAX_LEN) && (NULL != cgi_cmd[i].cmdProc); i++)
    {
    	printf("szCommand=%s,cgi_cmd[i].cgi=%s\n",szCommand,cgi_cmd[i].cgi);
        if(strstr(cgi_cmd[i].cgi, szCommand) != NULL)
        {
            pFunCgiCmdProc = cgi_cmd[i].cmdProc;
            strncpy(aszRequest,cgi_cmd[i].cmd,strlen(cgi_cmd[i].cmd));
            strncpy(method, cgi_cmd[i].method, strlen(cgi_cmd[i].method));
            commandFlag = 1;
            break;
        }
    }
DEBUG_LINE	
    if(i >= MAX_LEN)
    {
        HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[Error]can not find the cgi command\n");
        ret = -1;
        goto exit;
    }
DEBUG_LINE	
    if(strlen(pCommand) > (sizeof(aszRequest) - strlen(aszRequest))-1)
    {
        HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[Error]pCommand len is too long: %d\n", strlen(pCommand));
        ret = -1;
        goto exit;
    }
DEBUG_LINE	
    strncat(aszRequest,pCommand,strlen(pCommand));
    if(commandFlag != 1)
    {
        HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[error]invalid interface\n");
        ret = -1;
        goto exit;
    }
DEBUG_LINE
    ret = CGI_StrCommandParse(aszRequest, &argc, argv, MAX_NUM_CMD, MAX_CNT_EVERYCMD);
    if(ret != 0)
    {
        ret = -1;
        goto exit;
    }
DEBUG_LINE
	printf("argc = %d\n", argc);
	HI_U8 k = 0;
	for(k = 0; k < argc; k++)
	{
		printf("argv[%d] = %s\n", k, argv[k]);
	}
DEBUG_LINE
    if(pFunCgiCmdProc != NULL)
    {
DEBUG_LINE    
        ret = pFunCgiCmdProc(argc, (const char **)argv, pfnCgiStringGet, pData);
        if(ret != 0)
        {
            HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[error]pFunCgiCmdProc fail \n" );
            ret = -1;
            goto exit;
        }
    }
DEBUG_LINE	
    ret = 0;
exit:
    if(pszAllMalloc != NULL)
    {
        free(pszAllMalloc);
    }
    pszAllMalloc = NULL;
DEBUG_LINE    
    return ret;
}

HI_S32 HISNET_ICGI_CommandProc(httpd_conn* hc, PfnIcgiDataProc pfnProc, HTTPD_RESPONSE_S *pstReponse)
{
DEBUG_LINE
    HI_S32 s32Ret = HI_SUCCESS;
    HI_CHAR* pCommand = NULL;
    HI_CHAR szCommand[MAX_CGI_INTER] = {0};
    HI_CHAR aszMethod[MAX_HTTP_METHOD_LEN] = {0};
    HI_CHAR aszAccessLog[MAX_ACCESSLOG_LEN] = {0};
	
    if(hc == NULL || pstReponse == NULL)
    {
        printf("pstRequest & pstReponse can not be null\n\n\n");
        return HI_FAILURE;
    }
    if(hc->origfilename == NULL)
    {
        printf("origfilename can not be null\n\n\n");
        return HI_FAILURE;
    }
    if( hc->method == METHOD_GET )
    {
        strncpy(aszMethod,"GET", MAX_HTTP_METHOD_LEN-1);
    }
    else
    {
        strncpy(aszMethod,"POST", MAX_HTTP_METHOD_LEN-1);
    }
DEBUG_LINE	
    pCommand = strrchr(hc->origfilename,'/');
    if((pCommand != NULL)&&(strlen(pCommand) < MAX_CGI_INTER))
    {
        strncpy(szCommand, pCommand + 1, MAX_CGI_INTER-1);
        szCommand[MAX_CGI_INTER-1] = '\0';
    }
    else
    {
        printf("ICGI [Error]the command is invalide \n");
        return HI_FAILURE;
    }
DEBUG_LINE    
    HI_CHAR query[MAX_CNT_CGI] = {0};
    HI_CHAR pszRequest[MAX_CNT_CMD] = {0};
    strncpy(pszRequest,szCommand,strlen(szCommand));
    if(hc->method == METHOD_GET)
    {
DEBUG_LINE    
        if(hc->query[0] != '\0')
        {
            if(strlen(hc->query) >= MAX_CNT_CGI)
            {
                *(hc->query + MAX_CNT_CGI) = '\0';
            }
            THTTPD_strdecode(query,hc->query);
            if(strlen(query)  > (sizeof(pszRequest) - strlen(pszRequest))-1)
            {
                HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[error]HI_CGI_CommandProc pszRequest len is too short\n");
                return HI_FAILURE;
            }
            strncat(pszRequest,query,strlen(query));
        }
        strncpy(aszMethod,"GET", MAX_HTTP_METHOD_LEN-1);
DEBUG_LINE		
    }
    else if(hc->method == METHOD_POST)
    {
DEBUG_LINE    
        strncpy(aszMethod,"POST", MAX_HTTP_METHOD_LEN-1);
        HI_S32 c,retnum,cpynum = 0,r;
        HI_CHAR buf[MAX_CNT_CGI]={0};
        c = hc->read_idx - hc->checked_idx;
        if(c > 0)
        {
            strncpy(buf, &(hc->read_buf[hc->checked_idx]), MIN(c,MAX_CNT_CGI-1));
            cpynum = MIN(c,MAX_CNT_CGI-1);
        }
        while ((c < hc->contentlength ) && (c < MAX_CNT_CGI-1))
        {
            r = read(hc->conn_fd, buf+c, MIN( hc->contentlength - c, MAX_CNT_CGI-1 - c));
            if (r < 0 && (errno == EINTR || errno == EAGAIN ))
            {
                sleep( 1 );
                continue;
            }
            if ( r <= 0 )
            {
                break;
            }
            c += r;
            cpynum += r;
        }
        buf[cpynum] = '\0';
        retnum = strlen(buf);
        if(retnum > 0)
        {
            THTTPD_strdecode(query, buf);
            if(strlen(query)  > (sizeof(pszRequest) - strlen(pszRequest))-1)
            {
                HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[error]HI_CGI_CommandProc pszRequest len is too short\n");
                return HI_FAILURE;
            }
            strncat(pszRequest,query,strlen(query));
        }
DEBUG_LINE		
    }
    else
    {
        DEBUG_CGI_PRINT("CGI", LOG_LEVEL_ERROR, "[error]invalide request method \n" );
        return HI_FAILURE;
    }
DEBUG_LINE    
    if(pszRequest[0] != '\0')
    {
        memset(aszAccessLog,0,sizeof(aszAccessLog));
        snprintf(aszAccessLog,sizeof(aszAccessLog), "[%s] %s %s \n", httpd_ntoa(&hc->client_addr),aszMethod,pszRequest);
    }
    else
    {
        memset(aszAccessLog, 0, sizeof(aszAccessLog));
        snprintf(aszAccessLog,sizeof(aszAccessLog), "[%s] %s %s \n", httpd_ntoa(&hc->client_addr),aszMethod,szCommand);
    }
DEBUG_LINE	
    s32Ret = HI_CGI_CommandProc(pszRequest, pfnProc, hc);
    if(s32Ret != HI_SUCCESS)
    {
        HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"[error]HI_CGI_CommandProc fail\n");
#if  defined (HI_LINUX_SUPPORT_UCLIBC)
        pstReponse->status = HTTPD_RESPONSE_STATUS_INTERNALERR;
        strncpy(pstReponse->statusDesc,"Internal Server Error", MAX_HTTP_STATUS_CODE_LEN-1);
        pstReponse->extrahead[0] = '\0';
        strncpy(pstReponse->type, "text/html; charset=%s", MAX_HTTP_MIME_TYPE_LEN-1);
        pstReponse->bMIME = 1;
        pstReponse->bSize = 0;
#else
        pstReponse->status = HTTPD_RESPONSE_STATUS_INTERNALERR;
        strncpy(pstReponse->statusDesc,"Internal Server Error", MAX_HTTP_STATUS_CODE_LEN-1);
        pstReponse->extrahead[0] = '\0';
        strncpy(pstReponse->type, "text/html; charset=%s", MAX_HTTP_MIME_TYPE_LEN-1);
        pstReponse->bMIME = 1;
        pstReponse->bSize = 0;
#endif
        return HI_SUCCESS;//s32Ret;
    }
DEBUG_LINE	
#if  defined (HI_LINUX_SUPPORT_UCLIBC)
    pstReponse->status = HTTPD_RESPONSE_STATUS_OK;
    strncpy(pstReponse->statusDesc,"OK", MAX_HTTP_STATUS_CODE_LEN-1);
    pstReponse->extrahead[0] = '\0';
    strncpy(pstReponse->type, "text/html; charset=%s", MAX_HTTP_MIME_TYPE_LEN-1);
    pstReponse->bMIME = 1;
    pstReponse->bSize = 0;
#else
    pstReponse->status = HTTPD_RESPONSE_STATUS_OK;
    strncpy(pstReponse->statusDesc,"OK", MAX_HTTP_STATUS_CODE_LEN-1);
    pstReponse->extrahead[0] = '\0';
    strncpy(pstReponse->type, "text/html; charset=%s", MAX_HTTP_MIME_TYPE_LEN-1);
    pstReponse->bMIME = 1;
    pstReponse->bSize = 0;
#endif
DEBUG_LINE    
    return HI_SUCCESS;
}

HI_S32 Hisnet_Server_ARG_Parser(HI_S32 argc, const HI_CHAR** argv, ARG_OPT_S* opts)
{
/*
	printf("Hisnet_Server_ARG_Parser\n");
	{
		int i =0;
		for(i=0;i<argc;i++)
			printf("%s, ", argv[i]);

		printf("\n");
	}
*/	
    HI_S32      ret;
    HI_S32      iArgIdx     = 0;
    const HI_CHAR*    pCurrArg    = NULL;
    ARG_OPT_S*   pCurrOpt   = NULL;
    
    HI_BOOL     bHasParseArg    = HI_FALSE;
    //HI_S32      iNO_OPT_Cnt     = 0;
    HI_S32      s32I            = 0;
    HI_U8       u8ISSetSingle   = HI_FALSE;
    HI_U8       u8ISSetOTHER    = HI_FALSE;
    if (NULL == argv || NULL == opts)
    {
        printf("argv or opts is null\n\n");
        return HI_ERR_ARG_NO_OPT;
    }
    
    //argv[0] is execute command
    for (iArgIdx = 1; iArgIdx < argc ; iArgIdx++)
    {
        pCurrArg = argv[iArgIdx];
        if ((*pCurrArg == '/' || *pCurrArg == '-') && ((48 > *(pCurrArg + 1)) || (57 < * (pCurrArg + 1))))
        {
            if (*pCurrArg == '-')
            {
               if ((48 <= *(pCurrArg + 1)) && (57 >= *(pCurrArg + 1)))
                {
                    //printf("value is - \n");
                    pCurrOpt = Hisnet_ARGFindOpt_NoArg(opts);
                    
                    if (pCurrOpt == NULL)
                    {
                        HISNET_LOG(HISNET,LOG_LEVEL_ERROR,"Wrong Args z: %s\n",argv[iArgIdx]);
                        return HI_ERR_ARG_NOT_DEFINE_NO_OPT;
                    }
                    
                    ret = Hisnet_ARG_SetValue(pCurrOpt, argv[iArgIdx]);
                    
                    if ( ret != HI_SUCCESS)
                    {
                    DEBUG_LINE
                        HISNET_LOG(HISNET, LOG_LEVEL_ERROR, "Wrong Args z: %s\n", argv[iArgIdx]);
                        return ret;
                    }
                    
                    continue;
                }
                
            }
            
            pCurrArg++;
            
            /*the argv[n] as '-' */
            if (*pCurrArg == 0)
            {
                HISNET_LOG(HISNET, LOG_LEVEL_ERROR, "Wrong Args x: %s\n", argv[iArgIdx]);
                return HI_ERR_ARG_WRONG_FMT;
            }
            
            pCurrOpt = Hisnet_ARGFindOptStr(opts, pCurrArg);
            
            if (pCurrOpt == NULL)
            {
            DEBUG_LINE
                HISNET_LOG(HISNET, LOG_LEVEL_ERROR, "Wrong Args y: %s\n", argv[iArgIdx]);
                return HI_ERR_ARG_WRONG_FMT;
            }
            
            if ((pCurrOpt->u32Type & ARG_TYPE_NO_PARA) == ARG_TYPE_NO_PARA)
            {
                bHasParseArg = HI_FALSE;
                pCurrOpt->s8Isset = HI_TRUE;
            }
            else
            {
                bHasParseArg = HI_TRUE;
            }
            
            continue;
        }
        else if (bHasParseArg == HI_FALSE)
        {
       
            pCurrOpt = Hisnet_ARGFindOpt_NoArg(opts);
            
            if (pCurrOpt == NULL)
            {
                HISNET_LOG(HISNET, LOG_LEVEL_ERROR, "Wrong Args z: %s\n", argv[iArgIdx]);
                return HI_ERR_ARG_NOT_DEFINE_NO_OPT;
            }
            
            bHasParseArg = HI_TRUE;
        }
        
        if (HI_TRUE == bHasParseArg )
        {
         //printf("pCurrOpt = %s, set value %d = %s\n",pCurrOpt->pszOpt ,iArgIdx, argv[iArgIdx]);

            ret = Hisnet_ARG_SetValue(pCurrOpt, argv[iArgIdx]);
			//printf("%s=%s\n", pCurrOpt->pszOpt, pCurrOpt->pValue);

            
            if ( ret != HI_SUCCESS)
            {
                HISNET_LOG(HISNET, LOG_LEVEL_ERROR, "Wrong Args z: %s\n", argv[iArgIdx]);
                return ret;
            }
            
            bHasParseArg = HI_FALSE;
        }
        
    }
    
    /* Check Multi SINGLE option or SINGLE and non-SINGLE option  set at the same time */
    for (s32I = 0; (opts[s32I].u32Type & ARG_TYPE_END) != ARG_TYPE_END; s32I++)
    {
        if (opts[s32I].s8Isset == HI_TRUE)
        {
            if (((opts[s32I].u32Type)&ARG_TYPE_SINGLE) == ARG_TYPE_SINGLE)
            {
                if ((HI_FALSE == u8ISSetSingle) && (HI_FALSE == u8ISSetOTHER))
                {
                    
                    u8ISSetSingle = HI_TRUE;
                }
                else
                {
                DEBUG_LINE
                    HISNET_LOG(HISNET, LOG_LEVEL_ERROR, "[%d]\n", __LINE__);
                    return HI_FAILURE;
                }
            }
            else
            {
                if (HI_FALSE == u8ISSetSingle)
                {
                    
                    u8ISSetOTHER = HI_TRUE;
                }
                else
                {
                DEBUG_LINE
                    HISNET_LOG(HISNET, LOG_LEVEL_ERROR, "[%d]\n", __LINE__);
                    return HI_FAILURE;
                }
            }
        }
    }
    
    if (HI_FALSE == u8ISSetSingle)
    {
        for (s32I = 0; (opts[s32I].u32Type & ARG_TYPE_END) != ARG_TYPE_END; s32I++)
        {
            if ((opts[s32I].s8Isset == HI_FALSE) && (((opts[s32I].u32Type)&ARG_TYPE_MUST) == ARG_TYPE_MUST))
            {
            DEBUG_LINE
                HISNET_LOG(HISNET, LOG_LEVEL_ERROR, "[%d]\n", __LINE__);
                return HI_ERR_ARG_MUSTOPT_NOTSET;
            }
        }
    }
    
    return HI_SUCCESS;
}

static ARG_OPT_S* Hisnet_ARGFindOpt_NoArg(ARG_OPT_S* opts)
{
    HI_S32 i = 0;
    
    while (opts[i].u32Type < ARG_TYPE_END)
    {
        if (((opts[i].u32Type & ARG_TYPE_NO_OPT) == ARG_TYPE_NO_OPT) && (opts[i].s8Isset == HI_FALSE))
        {
            return &opts[i];
        }
        
        i++;
    }
    
    return NULL;
}

HI_S32 Hisnet_ARG_SetValue(ARG_OPT_S* pOpt, const char* szArgv)
{
    
    if ((HI_NULL_PTR == pOpt->pValue) || (0 == pOpt->u32ValueBuffersize) || (HI_NULL_PTR == szArgv))
    {
        return HI_FAILURE;
    }
    
    switch (pOpt->u32Type & (ARG_TYPE_FLOAT | ARG_TYPE_INT | ARG_TYPE_CHAR | ARG_TYPE_STRING))
    {
        case ARG_TYPE_STRING:
            if (Hisnet_ARG_CheckRegion(pOpt, szArgv) == HI_SUCCESS)
            {
                strncpy((char*)pOpt->pValue, szArgv, pOpt->u32ValueBuffersize);
				//printf("pOpt->pszOpt=%s, pOpt->pValue=%s, szArgv=%s\n",pOpt->pszOpt, pOpt->pValue,szArgv);
                pOpt->s8Isset = HI_TRUE;
            }
            else
            {
                return HI_FAILURE;
            }
            
            break;
            
        case ARG_TYPE_CHAR:
            if (Hisnet_ARG_CheckRegion(pOpt, szArgv) == HI_SUCCESS)
            {
                *((HI_CHAR*)pOpt->pValue) = szArgv[0];
                pOpt->s8Isset = HI_TRUE;
            }
            else
            {
                return HI_FAILURE;
            }
            
            break;
            
        case ARG_TYPE_INT:
            if (Hisnet_ARG_CheckRegion(pOpt, szArgv) == HI_SUCCESS)
            {
                *((HI_S32*)pOpt->pValue) = atoi(szArgv);
                pOpt->s8Isset = HI_TRUE;
            }
            else
            {
                return HI_FAILURE;
            }
            
            break;
            
        case ARG_TYPE_FLOAT:
            if (Hisnet_ARG_CheckRegion(pOpt, szArgv) == HI_SUCCESS)
            {
                *((float*)pOpt->pValue) = (float)atof(szArgv);
                pOpt->s8Isset = HI_TRUE;
            }
            else
            {
                return HI_FAILURE;
            }
            
            break;
            
        default:
        case ARG_TYPE_MUST:
        case ARG_TYPE_NO_OPT:
        case ARG_TYPE_NO_PARA:
        case ARG_TYPE_SINGLE:
        case ARG_TYPE_HELP:
        case ARG_TYPE_END:
        case ARG_TYPE_BUTT:
        case ARG_TYPE_4BYTE:
            pOpt->s8Isset = HI_FALSE;
            //printf("error opt type: %u\n",pOpt->type );
            return HI_ERR_ARG_DEFINE;
    }
    
    return HI_SUCCESS;
}

HI_S32 Hisnet_Server_ARG_OptIsSet(HI_CHAR* opt_name, ARG_OPT_S* opts)
{
    HI_S32 iOptsIdx = 0;
    
    if (NULL == opt_name || NULL == opts)
    {
        //WRITE_LOG_ERROR("NO OPT!!!\n");
        return HI_ERR_ARG_NO_OPT;
    }
    
    while (opts[iOptsIdx].u32Type < ARG_TYPE_END)
    {
        //WRITE_LOG_INFO("nnn: %d, %s  %s \n",iOptsIdx,opts[iOptsIdx].sz_opt, opt_name );
        if (strcmp(opts[iOptsIdx].pszOpt, opt_name) == 0)
        {
            if (opts[iOptsIdx].s8Isset)
            {
                return HI_SUCCESS;
            }
            
            iOptsIdx++;
        }
        else
        {
            iOptsIdx++;
        }
    }
    
    return HI_ERR_ARG_OPT_NOTSET;
}

static ARG_OPT_S* Hisnet_ARGFindOptStr(ARG_OPT_S* opts, const HI_CHAR* strOpt)
{
    HI_S32 i = 0;
    
    while (opts[i].u32Type < ARG_TYPE_END)
    {
        if (strcmp(opts[i].pszOpt, strOpt) == 0 )
        {
            return &opts[i];
        }
        
        i++;
    }
    
    return NULL;
}
HI_S32 Hisnet_ARG_Get_DwnAndUp(HI_CHAR* pszSHead, HI_CHAR* pszSTail, HI_CHAR** ppszDWN, HI_CHAR** ppszUP)
{
    HI_CHAR*    pszVRBegin     = pszSHead;      /* String begin address */
    HI_CHAR*    pszVREnd       = pszVRBegin;    /* String end address */
    HI_U8       u8FRegion       = 0;            /*bit0 : 0, Single value; 1, not single value;
                                                 bit1 : 0, no up limit ;1, exist up limit;
                                                 bit3 : 0, no down limit;1,exist down limit*/
    HI_U32  u32DwnLen = 0;
    HI_U32  u32UpLen = 0;
    HI_CHAR*    pszVRSplit = HI_NULL_PTR;
    
    *ppszDWN = *ppszUP = HI_NULL_PTR;
    
    while (pszVREnd != pszSTail)
    {
        if (*pszVREnd != '~')
        {
            pszVREnd++;
        }
        else    /* *pszVREnd== '~' */
        {
            pszVRSplit = pszVREnd;
            break;
        }
    }
    
    if ((NULL == pszVRSplit) || (pszSHead == pszSTail )) // single value "..." or "~"
    {
        u32DwnLen = pszSTail - pszSHead + 1;
        *ppszDWN = (HI_CHAR*)malloc(u32DwnLen + 1);
        
        if (*ppszDWN)
        {
            memcpy(*ppszDWN, pszSHead, u32DwnLen);
            (*ppszDWN)[u32DwnLen] = 0;
        }
        
        ARG_CLR_REGION(u8FRegion);
        return 1;
    }
    else
    {
        ARG_SET_REGION(u8FRegion);
        
        u32DwnLen = pszVRSplit - pszSHead;
        
        if (u32DwnLen > 0)     //"..~"
        {
            *ppszDWN = (HI_CHAR*)malloc(u32DwnLen + 1);
            
            if (*ppszDWN)
            {
                memcpy(*ppszDWN, pszSHead, u32DwnLen);
                (*ppszDWN)[u32DwnLen] = 0;
            }
            
            ARG_SET_DWNLMT(u8FRegion);
        }
        else
        {
            ARG_CLR_DWNLMT(u8FRegion);
        }
        
        u32UpLen = pszSTail - pszVRSplit;
        
        if (u32UpLen > 0)   //"~.."
        {
            *ppszUP = (HI_CHAR*)malloc(u32UpLen + 1);
            
            if (*ppszUP)
            {
                memcpy(*ppszUP, pszVRSplit+1, u32UpLen);
                (*ppszUP)[u32UpLen] = 0;
            }
            
            ARG_SET_UPLMT(u8FRegion);
        }
        else
        {
            ARG_CLR_UPLMT(u8FRegion);
        }
        
        return 0;
    }
}
HI_S32 Hisnet_ARG_CheckRegion(ARG_OPT_S* pOpt, const char* pszArgv)
{
    HI_CHAR*    pszVRBegin     = pOpt->pszValueRegion;     /* Region String begin address */
    HI_CHAR*    pszVREnd       = pszVRBegin;               /* Region String end address */
    HI_S32      s32CRLT         = 0;                       /* String Compare Return Value */
    HI_U32      u32Count        = 0;
    HI_CHAR*    pszDWN          = HI_NULL_PTR;              /* Down String*/
    HI_CHAR*    pszUP           = HI_NULL_PTR;              /* Up String */
    HI_U32      u32Rtn          = 0;                        /* CallBack Return Value */
    
    if (pOpt->pszValueRegion == HI_NULL_PTR)
    {
        return HI_SUCCESS;
    }
    
    switch (pOpt->u32Type & (ARG_TYPE_FLOAT | ARG_TYPE_INT | ARG_TYPE_CHAR | ARG_TYPE_STRING))
    {
        case ARG_TYPE_STRING:
        {
            while (pszVRBegin != HI_NULL_PTR)
            {
                u32Count = 0;
                
                while ((*pszVREnd != '|') && (*pszVREnd != '\0'))
                {
                    pszVREnd++;
                    u32Count++;
                }
                
                if (strlen(pszArgv) == u32Count)
                {
                    s32CRLT = memcmp(pszVRBegin, pszArgv, u32Count);
                    
                    if (0 == s32CRLT)
                    {
                        break;
                    }
                }
                
                if (*pszVREnd != '\0')
                {
                    pszVREnd++;
                    pszVRBegin = pszVREnd;
                }
                else
                {
                    pszVRBegin = HI_NULL_PTR;
                }
            }
            
            if (pszVRBegin == HI_NULL_PTR)
            {
                return HI_ERR_ARG_NOT_IN_REGION;
            }
            
            break;
        }
            
        case ARG_TYPE_CHAR:
        {
            while (pszVRBegin != HI_NULL_PTR)
            {
                u32Count = 0;
                
                pszDWN = pszUP = HI_NULL_PTR;
                
                while ((*pszVREnd != '|') && (*pszVREnd != '\0'))
                {
                    pszVREnd++;
                    u32Count++;
                }
                
                u32Rtn = Hisnet_ARG_Get_DwnAndUp(pszVRBegin, pszVREnd, &pszDWN, &pszUP);
                
                if (0 == u32Rtn) /* Region */
                {
                    if ((pszDWN == HI_NULL_PTR) || (pszArgv[0] >= pszDWN[0]))   /* Input Param >= Down Limit */
                    {
                        if ((pszUP == HI_NULL_PTR) || (pszArgv[0] <= pszUP[0])) /* Input Param >= Up Limit */
                        {
                            break;
                        }
                    }
                }
                else /* Single Value */
                {
                    if ((pszDWN == HI_NULL_PTR) || (pszArgv[0] == pszDWN[0]))
                    {
                        break;
                    }
                }
                
                if (*pszVREnd != '\0')
                {
                    pszVREnd++;
                    pszVRBegin = pszVREnd;
                }
                else
                {
                    pszVRBegin = HI_NULL_PTR;
                }
                
                if (HI_NULL_PTR != pszDWN)
                {
                    free(pszDWN);
                    pszDWN = HI_NULL_PTR;
                }
                
                if (HI_NULL_PTR != pszUP)
                {
                    free(pszUP);
                    pszUP = HI_NULL_PTR;
                }
            }
            
            if (pszVRBegin == HI_NULL_PTR)
            {
                return HI_ERR_ARG_NOT_IN_REGION;
            }
            
            break;
        }
            
        case ARG_TYPE_INT:
        {
            HI_S32 s32Vlue = atoi(pszArgv);
            
            while (pszVRBegin != HI_NULL_PTR)
            {
                u32Count = 0;
                
                pszDWN = pszUP = HI_NULL_PTR;
                
                while ((*pszVREnd != '|') && (*pszVREnd != '\0'))
                {
                    pszVREnd++;
                    u32Count++;
                }
                
                u32Rtn = Hisnet_ARG_Get_DwnAndUp(pszVRBegin, pszVREnd, &pszDWN, &pszUP);
                
                if (0 == u32Rtn) /* Region */
                {
                    if ((pszDWN == HI_NULL_PTR) || (s32Vlue >= atoi(pszDWN)))   /* Input Param >= Down Limit */
                    {
                        if ((pszUP == HI_NULL_PTR) || (s32Vlue <= atoi(pszUP))) /* Input Param >= Up Limit */
                        {
                            break;
                        }
                    }
                }
                else /* Single Value */
                {
                    if ((pszDWN == HI_NULL_PTR) || (s32Vlue == atoi(pszDWN)))
                    {
                        break;
                    }
                }
                
                if (*pszVREnd != '\0')
                {
                    pszVREnd++;
                    pszVRBegin = pszVREnd;
                }
                else
                {
                    pszVRBegin = HI_NULL_PTR;
                }
                
                if (HI_NULL_PTR != pszDWN)
                {
                    free(pszDWN);
                    pszDWN = HI_NULL_PTR;
                }
                
                if (HI_NULL_PTR != pszUP)
                {
                    free(pszUP);
                    pszUP = HI_NULL_PTR;
                }
            }
            
            if (pszVRBegin == HI_NULL_PTR)
            {
                return HI_ERR_ARG_NOT_IN_REGION;
            }
            
            break;
        }
            
        case ARG_TYPE_FLOAT:
        {
            HI_FLOAT eVlue = (HI_FLOAT)atof(pszArgv);
            
            while (pszVRBegin != HI_NULL_PTR)
            {
                u32Count = 0;
                
                pszDWN = pszUP = HI_NULL_PTR;
                
                while ((*pszVREnd != '|') && (*pszVREnd != '\0'))
                {
                    pszVREnd++;
                    u32Count++;
                }
                
                u32Rtn = Hisnet_ARG_Get_DwnAndUp(pszVRBegin, pszVREnd, &pszDWN, &pszUP);
                
                if (0 == u32Rtn) /* Region */
                {
                    if ((pszDWN == HI_NULL_PTR) || (eVlue >= (HI_FLOAT)atof(pszDWN)))  /* Input Param >= Down Limit */
                    {
                        if ((pszUP == HI_NULL_PTR) || (eVlue <= (HI_FLOAT)atof(pszUP))) /* Input Param >= Up Limit */
                        {
                            break;
                        }
                    }
                }
                else /* Single Value */
                {
                    if ((pszDWN == HI_NULL_PTR) || (eVlue == (HI_FLOAT)atof(pszDWN)))
                    {
                        break;
                    }
                    
                    //return HI_ERR_ARG_DEFINE;
                }
                
                if (*pszVREnd != '\0')
                {
                    pszVREnd++;
                    pszVRBegin = pszVREnd;
                }
                else
                {
                    pszVRBegin = HI_NULL_PTR;
                }
                
                if (HI_NULL_PTR != pszDWN)
                {
                    free(pszDWN);
                    pszDWN = HI_NULL_PTR;
                }
                
                if (HI_NULL_PTR != pszUP)
                {
                    free(pszUP);
                    pszUP = HI_NULL_PTR;
                }
            }
            
            if (pszVRBegin == HI_NULL_PTR)
            {
                return HI_ERR_ARG_NOT_IN_REGION;
            }
            
            break;
        }
            
        default:
        case ARG_TYPE_MUST:
        case ARG_TYPE_NO_OPT:
        case ARG_TYPE_NO_PARA:
        case ARG_TYPE_SINGLE:
        case ARG_TYPE_HELP:
        case ARG_TYPE_END:
        case ARG_TYPE_BUTT:
        case ARG_TYPE_4BYTE:
            pOpt->s8Isset = HI_FALSE;
            //WRITE_LOG_ERROR("error opt type: %u\n",pOpt->type );
            return HI_ERR_ARG_DEFINE;
    }
    
    if (HI_NULL_PTR != pszDWN)
    {
        free(pszDWN);
    }
    
    if (HI_NULL_PTR != pszUP)
    {
        free(pszUP);
    }
    return HI_SUCCESS;
}

int main(void)
{

 printf("-----CGI COMING!!\n");

    HI_S32 s32Rtn;
    //注册CGI
    HI_THTTPD_RegisterInternalCgiProc(HISNET_ICGI_CommandProc);

	#if 0
	Hisnet_SYS_RegCb_SetSensor(Product_ICGI_SetSensor);
	#endif
	
    //开启http线程
    s32Rtn = pthread_create(&g_webserveru32Thread,HI_NULL_PTR,thttpd_start_main,NULL);
    if(s32Rtn != HI_SUCCESS )
    {
        printf("pthread_create error\n");
    }
	while(1)
	{
		sleep(1);
	}
    return s32Rtn;
}

void textCGI(int argc, const char* argv[], PfnCgiStringProc pfnStringProc, void *pData)
{
    printf("-----CGI COMING!!\n");
}

