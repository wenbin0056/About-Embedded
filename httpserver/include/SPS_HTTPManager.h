//
//  SPS_CMDHandle.h
//  HttpServer
//
//  Created by Space Zhong on 2018/9/18.
//  Copyright © 2018年 Space Zhong. All rights reserved.
//

#ifndef SPS_CMDHandle_h
#define SPS_CMDHandle_h

#include <stdio.h>
#include "SPS_CMDConst.h"
#include "timers.h"

/************************************************************************************/

#define HISNET_MAX_TIMESIZE         (40+1)      /*Time Length*/
#define HISNET_MAX_VERSIONSIZE		(10)
#define FILEMNG_MAX_PATH_LEN        (256)

/************************************************************************************/

#define HISNET_ERROR_CLIENT     (-100)

#define HISNET_ERROR_PARAM                                      (HISNET_ERROR_CLIENT-1)
#define HISNET_ERROR_GETDEVINFO                                 (HISNET_ERROR_CLIENT-2)
#define HISNET_ERROR_POST                                       (HISNET_ERROR_CLIENT-3)
#define HISNET_ERROR_RETURNINFO                                 (HISNET_ERROR_CLIENT-4)
#define HISNET_ERROR_CLIENT_ARGPARSER                           (HISNET_ERROR_CLIENT-5)
#define HISNET_ERROR_SVRFUNC_RTN                                (HISNET_ERROR_CLIENT-6)
#define HISNET_ERROR_SVRFUNC_NULL                               (HISNET_ERROR_CLIENT-7)
#define HISNET_ERROR_SVRFUNC_INVALIDCMD                         (HISNET_ERROR_CLIENT-8)
#define HISNET_ERROR_SVRFUNC_ARGPARSER                          (HISNET_ERROR_CLIENT-9)

/************************************************************************************/

#define HISNET_FREE(pbuf)\
    do{\
        if(NULL != pbuf)\
        {\
            free(pbuf);\
            pbuf = NULL;\
        }\
    }while(0)

/************************************************************************************/

typedef enum hisnet_SDSTATUS_E
{
    HISNET_DEVICE_SD_STATUS_OUT,
    HISNET_DEVICE_SD_STATUS_MOUNT,
    HISNET_DEVICE_SD_STATUS_UMOUNT,
    HISNET_DEVICE_SD_STATUS_READONLY,
    HISNET_DEVICE_SD_STATUS_NOPARTITION,
    HISNET_DEVICE_SD_STATUS_BUTT
} HISNET_SDSTATUS_E;

typedef struct hisnet_SDSTATUS_S
{
    HISNET_SDSTATUS_E   enSdStatus;
    HI_S32 s32SDFreeSpace;
    HI_S32 s32TotalSpace;
    HI_S32 s32PartitionNum;
} HISNET_SDSTATUS_S;

typedef enum hisnet_SYS_TIMEFORMAT_E
{
    HISNET_TIMEFORMAT_DEFAULT,
    HISNET_TIMEFORMAT_WESTERN,
    HISNET_TIMEFORMAT_EASTERN,
    HISNET_TIMEFORMAT_BUTT
} HISNET_SYS_TIMEFORMAT_E;

typedef enum hisnet_SYS_TIMEZONE_E
{
    HISNET_Pacific_Majuro = 1,
    HISNET_Pacific_Midway,
    HISNET_Pacific_Honolulu,
    HISNET_America_Anchorage,
    HISNET_America_Los_Angeles,
    HISNET_America_Tijuana,
    HISNET_America_Phoenix,
    HISNET_America_Chihuahua,
    HISNET_America_Denver,
    HISNET_America_Costa_Rica,
    HISNET_America_Chicago,
    HISNET_America_Mexico_City,
    HISNET_America_Regina,
    HISNET_America_Bogota,
    HISNET_America_New_York,
    HISNET_America_Caracas,
    HISNET_America_Barbados,
    HISNET_America_Manaus,
    HISNET_America_Santiago,
    HISNET_America_St_Johns,
    HISNET_America_Sao_Paulo,
    HISNET_America_Argentina_Buenos_Aires,
    HISNET_America_Godthab,
    HISNET_America_Montevideo,
    HISNET_Atlantic_South_Georgia,
    HISNET_Atlantic_Azores,
    HISNET_Atlantic_Cape_Verde,
    HISNET_Africa_Casablanca,
    HISNET_Europe_London,
    HISNET_Europe_Amsterdam,
    HISNET_Europe_Belgrade,
    HISNET_Europe_Brussels,
    HISNET_Europe_Sarajevo,
    HISNET_Africa_Windhoek,
    HISNET_Africa_Brazzaville,
    HISNET_Asia_Amman,
    HISNET_Europe_Athens,
    HISNET_Asia_Beirut,
    HISNET_Africa_Cairo,
    HISNET_Europe_Helsinki,
    HISNET_Asia_Jerusalem,
    HISNET_Europe_Minsk,
    HISNET_Africa_Harare,
    HISNET_Asia_Baghdad,
    HISNET_Europe_Moscow,
    HISNET_Asia_Kuwait,
    HISNET_Africa_Nairobi,
    HISNET_Asia_Tehran,
    HISNET_Asia_Baku,
    HISNET_Asia_Tbilisi,
    HISNET_Asia_Yerevan,
    HISNET_Asia_Dubai,
    HISNET_Asia_Kabul,
    HISNET_Asia_Karachi,
    HISNET_Asia_Oral,
    HISNET_Asia_Yekaterinburg,
    HISNET_Asia_Calcutta,
    HISNET_Asia_Colombo,
    HISNET_Asia_Katmandu,
    HISNET_Asia_Almaty,
    HISNET_Asia_Rangoon,
    HISNET_Asia_Krasnoyarsk,
    HISNET_Asia_Bangkok,
    HISNET_Asia_Shanghai,
    HISNET_Asia_Hong_Kong,
    HISNET_Asia_Irkutsk,
    HISNET_Asia_Kuala_Lumpur,
    HISNET_Australia_Perth,
    HISNET_Asia_Taipei,
    HISNET_Asia_Seoul,
    HISNET_Asia_Tokyo,
    HISNET_Asia_Yakutsk,
    HISNET_Australia_Adelaide,
    HISNET_Australia_Darwin,
    HISNET_Australia_Brisbane,
    HISNET_Australia_Hobart,
    HISNET_Australia_Sydney,
    HISNET_Asia_Vladivostok,
    HISNET_Pacific_Guam,
    HISNET_Asia_Magadan,
    HISNET_Pacific_Auckland,
    HISNET_Pacific_Fiji,
    HISNET_Pacific_Tongatapu,
    HISNET_TIMEZONE_BUTT
} HISNET_SYS_TIMEZONE_E;

typedef struct hisnet_TIME_ATTR_S
{
    HI_CHAR aszTime[HISNET_MAX_TIMESIZE];       /* eg. "20120608110426"*/
    HISNET_SYS_TIMEFORMAT_E enTimeFormat;
    HISNET_SYS_TIMEZONE_E enTimeZone;
} HISNET_TIME_ATTR_S;

typedef struct hisnet_VERSION_ATTR_S
{
	HI_CHAR aszVersion[HISNET_MAX_VERSIONSIZE];
} HISNET_VERSION_ATTR_S;

typedef struct hisnet_REMOTEFILENAME_S
{
    HI_CHAR aszName[FILEMNG_MAX_PATH_LEN];
} HISNET_REMOTEFILENAME_S;

typedef struct hisnet_GPSINFO_ATTR_S
{
	HI_FLOAT f32Speed;
	HI_FLOAT f32Altitude;
	HI_FLOAT f32Latitude;
	HI_U8  	 u8NShemi;
	HI_FLOAT f32Longitude;
	HI_U8  	 u8EWhemi;
	HI_FLOAT f32Pitch;
	HI_FLOAT f32Roll;
	HI_FLOAT f32Yaw;
	
} HISNET_GPSINFO_S;

typedef enum hisnet_EXPOSURE_MODE_E
{
    EXPOSURE_MODE_AUTO    = 0,
    EXPOSURE_MODE_MANUAL  = 1,
    EXPOSURE_MODE_BUTT
} HISNET_EXPOSURE_MODE_E;

typedef struct hisnet_EXPOSURE_ATTR_S
{
	HI_U8  u8Index;
	HI_U32 u32ExpTime;
    HI_U32 u32Gain;
	
} HISNET_EXPOSURE_ATTR_S;

/************************************************************************************/

#if 0
typedef HI_S32 (*Hisnet_SYS_SetSensor_FN_PTR)(MSC_CAM_INFO_S *pstMscCamInfo);

HI_S32 Hisnet_SYS_RegCb_SetSensor(Hisnet_SYS_SetSensor_FN_PTR pfnStartSensor);
#endif

/************************************************************************************/

typedef struct
{	
	#if 0
	Hisnet_SYS_SetSensor_FN_PTR    pfnSetSensor;
	#endif

} HISNET_SVR_CB_S;

int startHTTP(void);
HI_S32 HISNET_Occasional(struct timeval *nowP);

#endif /* SPS_CMDHandle_h */
