#include "cmdproc.h"
#include "hi_type.h"
//#include "sample_comm.h"
#include "SPS_HTTPManager.h"
//#include "hi_product_log.h"
//#include "hi_product_storagemng.h"
//#include "StorageManager.h"

#if 0
HI_S32 set_param_sys_time(HI_VOID *retVal)
{
	printf("Set System Time: %s\n", ((HISNET_TIME_ATTR_S *)retVal)->aszTime);

	return HI_SUCCESS;
}

HI_S32 set_param_snap_time_interval(HI_S32 param)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32TimeInterval = 0;
	
	switch(param)
	{
		case PHOTO_TIME_INTERVAL_0x1S:
			s32TimeInterval = 100;
			break;
		case PHOTO_TIME_INTERVAL_0x5S:
			s32TimeInterval = 500;
			break;
		case PHOTO_TIME_INTERVAL_1S:
			s32TimeInterval = 1000;
			break;
	}
	
	extern HI_S32 HI_SNAP_TIME_INTERVAL_SET(HI_S32 s32TimeInterval);
	s32Ret = HI_SNAP_TIME_INTERVAL_SET(s32TimeInterval);

	return s32Ret;
}

HI_S32 set_param_exposure_mode(HI_S32 param)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U8 u8Index = 0;
	ISP_EXPOSURE_ATTR_S stExpAttr = {0};

	for(u8Index = 0; u8Index < MAX_CAM_NUM; u8Index++)
	{
		s32Ret = HI_MPI_ISP_GetExposureAttr(u8Index, &stExpAttr);
		if(HI_SUCCESS != s32Ret)
	    {
	        MLOGE("HI_MPI_ISP_GetExposureAttr Failed\n");
	        return HI_FAILURE;
	    }

		switch(param)
		{
			case PHOTO_EXPOSURE_MODE_AUTO:
				stExpAttr.enOpType = OP_TYPE_AUTO;
				break;
			case PHOTO_EXPOSURE_MODE_MANUAL:
				stExpAttr.enOpType = OP_TYPE_MANUAL;
				break;
		}

		s32Ret = HI_MPI_ISP_SetExposureAttr(u8Index, &stExpAttr);
		if(HI_SUCCESS != s32Ret)
		{
		    MLOGE("HI_MPI_ISP_SetExposureAttr Failed\n");
		    return HI_FAILURE;
		}
	}

	return HI_SUCCESS;
}

HI_S32 set_param_exposure_attr(HI_VOID *retVal)
{
	HI_S32 s32Ret = HI_SUCCESS;
	ISP_EXPOSURE_ATTR_S stExpAttr = {0};
	
	s32Ret = HI_MPI_ISP_GetExposureAttr(((HISNET_EXPOSURE_ATTR_S *)retVal)->u8Index, &stExpAttr);
	if(HI_SUCCESS != s32Ret)
    {
        MLOGE("HI_MPI_ISP_GetExposureAttr Failed\n");
        return HI_FAILURE;
    }

	if(OP_TYPE_MANUAL == stExpAttr.enOpType)
	{
		stExpAttr.stManual.u32ExpTime = ((HISNET_EXPOSURE_ATTR_S *)retVal)->u32ExpTime;
		stExpAttr.stManual.u32AGain = ((HISNET_EXPOSURE_ATTR_S *)retVal)->u32Gain * 1024 / 100;

		s32Ret = HI_MPI_ISP_SetExposureAttr(((HISNET_EXPOSURE_ATTR_S *)retVal)->u8Index, &stExpAttr);
		if(HI_SUCCESS != s32Ret)
	    {
	        MLOGE("HI_MPI_ISP_SetExposureAttr Failed\n");
	        return HI_FAILURE;
	    }
	}

	return HI_SUCCESS;
}


HI_S32 get_param_sys_time(HI_VOID *retVal)
{
	((HISNET_TIME_ATTR_S *)retVal)->enTimeFormat = HISNET_TIMEFORMAT_DEFAULT;
	((HISNET_TIME_ATTR_S *)retVal)->enTimeZone = HISNET_Asia_Shanghai;
	memcpy(((HISNET_TIME_ATTR_S *)retVal)->aszTime, "20181112152223", sizeof("20181112152223"));
	
	return HI_SUCCESS;
}

HI_S32 get_param_sys_version(HI_VOID *retVal)
{
	memcpy(((HISNET_VERSION_ATTR_S *)retVal)->aszVersion, SW_VERSION, HISNET_MAX_VERSIONSIZE);
	return HI_SUCCESS;
}

HI_S32 get_param_snap_time_interval(HI_VOID *retVal)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_S32 s32TimeInterval = 0;

	extern HI_S32 HI_SNAP_TIME_INTERVAL_GET(HI_S32 *ps32TimeInterval);
	s32Ret = HI_SNAP_TIME_INTERVAL_GET(&s32TimeInterval);
	switch(s32TimeInterval)
	{
		case 100:
			*(HI_S32*)retVal = PHOTO_TIME_INTERVAL_0x1S;
			break;
		case 500:
			*(HI_S32*)retVal = PHOTO_TIME_INTERVAL_0x5S;
			break;
		case 1000:
			*(HI_S32*)retVal = PHOTO_TIME_INTERVAL_1S;
			break;
	}

	return s32Ret;
}

HI_S32 get_param_exposure_mode(HI_VOID *retVal)
{
	HI_S32 s32Ret = HI_SUCCESS;
	HI_U8 u8Index = 0;
	HI_U8 u8ExpAutoCount = 0;
	HI_U8 u8ExpManaulCount = 0;
	HI_U8 u8ExpButtCount = 0;
	
	ISP_EXPOSURE_ATTR_S stExpAttr[MAX_CAM_NUM] = {0};

	for(u8Index = 0; u8Index < MAX_CAM_NUM; u8Index++)
	{
		s32Ret = HI_MPI_ISP_GetExposureAttr(u8Index, &stExpAttr[u8Index]);
		if(HI_SUCCESS != s32Ret)
	    {
	        MLOGE("HI_MPI_ISP_GetExposureAttr Failed\n");
	        return HI_FAILURE;
	    }

		if(OP_TYPE_AUTO == stExpAttr[u8Index].enOpType)
		{
			u8ExpAutoCount++;
		}
		else if(OP_TYPE_MANUAL == stExpAttr[u8Index].enOpType)
		{
			u8ExpManaulCount++;
		}
		else
		{
			u8ExpButtCount++;
		}
	}

	if(MAX_CAM_NUM == u8ExpAutoCount)
	{
		*(HI_S32 *)retVal = PHOTO_EXPOSURE_MODE_AUTO;
	}
	else if(MAX_CAM_NUM == u8ExpManaulCount)
	{
		*(HI_S32 *)retVal = PHOTO_EXPOSURE_MODE_MANUAL;
	}
	else
	{
		return HI_FAILURE;
	}

	return HI_SUCCESS;
}

HI_S32 get_param_exposure_attr(HI_VOID *retVal)
{
	HI_S32 s32Ret = HI_SUCCESS;
	ISP_EXPOSURE_ATTR_S stExpAttr = {0};

	s32Ret = HI_MPI_ISP_GetExposureAttr(((HISNET_EXPOSURE_ATTR_S *)retVal)->u8Index, &stExpAttr);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE("HI_MPI_ISP_GetExposureAttr Failed\n");
        return HI_FAILURE;
    }
	else
	{
		if(OP_TYPE_AUTO == stExpAttr.enOpType)
		{
			return HI_SUCCESS;
		}
		else if(OP_TYPE_MANUAL == stExpAttr.enOpType)
		{
			((HISNET_EXPOSURE_ATTR_S *)retVal)->u32ExpTime = stExpAttr.stManual.u32ExpTime;
			((HISNET_EXPOSURE_ATTR_S *)retVal)->u32Gain = stExpAttr.stManual.u32AGain;
		}
		else
		{
			return HI_FAILURE;
		}
	}
}

static HI_S32 MscCmdDoExecute_Set_Hardware(MSC_DV_CMD cmd, HI_S32 param, HI_VOID *retVal)
{
	HI_S32 s32Ret = HI_SUCCESS;
	
	switch(cmd)
	{
		case CMD_SET_LED:
			break;
		case CMD_SET_SD_STATE:
			break;
		case CMD_SET_WIFI_STATE:
			break;
		case CMD_SET_BLUETOOTH_STATE:
			break;
		case CMD_SET_LIGHT_MODULE_STATE:
			break;
		case CMD_SHUT_DOWN:
			break;
		case CMD_REBOOT:
			break;
	}
	
	return s32Ret;
}

static HI_S32 MscCmdDoExecute_Set_SysParam(MSC_DV_CMD cmd, HI_S32 param, HI_VOID *retVal)
{
	HI_S32 s32Ret = HI_SUCCESS;

	switch(cmd)
	{
		case CMD_SET_PARAM_SYS_TIME:
			s32Ret = set_param_sys_time(retVal);
			break;
	}
	
	return s32Ret;
}

static HI_S32 MscCmdDoExecute_Set_SnapParam(MSC_DV_CMD cmd, HI_S32 param, HI_VOID *retVal)
{
	HI_S32 s32Ret = HI_SUCCESS;

	switch(cmd)
	{
		case CMD_SET_PARAM_ENABLE_SENSOR:
			break;
		case CMD_SET_PARAM_SNAP_MODE:
			break;
		case CMD_SET_PARAM_SNAP_TIME_INTERVAL:	
			s32Ret = set_param_snap_time_interval(param);
			break;
		case CMD_SET_PARAM_SNAP_DISTANCE:
			break;
		case CMD_SET_PARAM_EXPOSURE_MODE:
			s32Ret = set_param_exposure_mode(param);
			break;
		case CMD_SET_PARAM_EXPOSURE_ATTR:
			s32Ret = set_param_exposure_attr(retVal);
			break;
	}
	
	return s32Ret;
}



static HI_S32 MscCmdDoExecute_Get_SysParam(MSC_DV_CMD cmd, HI_VOID *retVal)
{
	HI_S32 s32Ret = HI_SUCCESS;

	switch(cmd)
	{
		case CMD_GET_PARAM_SYS_VERSION:
			s32Ret = get_param_sys_version(retVal);
			break;
		case CMD_GET_PARAM_SYS_TIME:
			s32Ret = get_param_sys_time(retVal);
			break;
	}
	
	return s32Ret;
}

static HI_S32 MscCmdDoExecute_Get_SnapParam(MSC_DV_CMD cmd, HI_VOID *retVal)
{
	HI_S32 s32Ret = HI_SUCCESS;

	switch(cmd)
	{
		case CMD_GET_PARAM_SNAP_MODE:
			break;
		case CMD_GET_PARAM_SNAP_TIME_INTERVAL:
			s32Ret = get_param_snap_time_interval(retVal);
			break;
		case CMD_GET_PARAM_SNAP_DISTANCE:
			break;
		case CMD_GET_PARAM_EXPOSURE_MODE:
			s32Ret = get_param_exposure_mode(retVal);
			break;
		case CMD_GET_PARAM_EXPOSURE_ATTR:
			s32Ret = get_param_exposure_attr(retVal);
			break;
	}

	return s32Ret;
}







HI_S32 get_hardware_sd_state(HI_VOID *retVal)
{
	HI_S32 s32Ret = HI_SUCCESS;
    HI_U64 u64TotalSize = 0;
    HI_U64 u64RemainSize = 0;

    HI_PDT_STORAGE_STATE_E enState;
    HI_PDT_STORAGE_DISKATTR_S stDiskAttr;
    HI_CHAR *aszMountPath = SD_MOUNT_PATH;

    s32Ret = HI_PDT_Storage_GetState(aszMountPath, &enState);
    if(HI_SUCCESS != s32Ret)
    {
        MLOGE("HI_Product_GetSDState err \n");
        return HI_FAILURE;
    }

    s32Ret = HI_PDT_Storage_GetDiskAttr(aszMountPath, &stDiskAttr);
    if(HI_SUCCESS != s32Ret)
    {
        return HI_FAILURE;
    }

    u64TotalSize = stDiskAttr.u64TotalSpace;
    u64RemainSize = stDiskAttr.u64FreeSpace;

    switch(enState)
    {
	    case PDT_STORAGE_STATE_DEVOUT:
	        ((HISNET_SDSTATUS_S *)retVal)->enSdStatus = HISNET_DEVICE_SD_STATUS_OUT;
	        break;
	    case PDT_STORAGE_STATE_UNINITED:
	        ((HISNET_SDSTATUS_S *)retVal)->enSdStatus = HISNET_DEVICE_SD_STATUS_NOPARTITION;
	        break;
	    case PDT_STORAGE_STATE_UNMOUNTED:
	        ((HISNET_SDSTATUS_S *)retVal)->enSdStatus = HISNET_DEVICE_SD_STATUS_UMOUNT;
	        break;
	    case PDT_STORAGE_STATE_WRITEPROTECT:
	        ((HISNET_SDSTATUS_S *)retVal)->enSdStatus = HISNET_DEVICE_SD_STATUS_READONLY;
	        break;
	    case PDT_STORAGE_STATE_MOUNTED:
	        ((HISNET_SDSTATUS_S *)retVal)->enSdStatus = HISNET_DEVICE_SD_STATUS_MOUNT;
	        break;
	    default:
	        ((HISNET_SDSTATUS_S *)retVal)->enSdStatus = HISNET_DEVICE_SD_STATUS_BUTT;
	        break;
    }

	((HISNET_SDSTATUS_S *)retVal)->s32PartitionNum = 0;
    ((HISNET_SDSTATUS_S *)retVal)->s32SDFreeSpace = u64RemainSize/1024/1024; /*unit turn to MB*/
    ((HISNET_SDSTATUS_S *)retVal)->s32TotalSpace = u64TotalSize/1024/1024;   /*unit turn to MB*/

	return s32Ret;
}

HI_S32 get_hardware_gps_state(HI_VOID *retVal)
{
	return HI_SUCCESS;
}

static HI_S32 MscCmdDoExecute_Get_Hardware(MSC_DV_CMD cmd, HI_VOID *retVal)
{
	HI_S32 s32Ret = HI_SUCCESS;

	switch(cmd)
	{
		case CMD_GET_SD_STATE:
			s32Ret = get_hardware_sd_state(retVal);
			break;
		case CMD_GET_GPS_STATE:
			s32Ret = get_hardware_gps_state(retVal);
			break;
			#if 0
		case CMD_GET_WIFI_STATE:
			break;
		case CMD_GET_BLUETOOTH_STATE:
			break;
		case CMD_GET_LIGHT_MODULE_STATE:
			break;
			#endif
	}
	
	return s32Ret;
}
#endif

pfCallBack pfn = NULL;



HI_S32 MscCmdExecute(MSC_DV_CMD cmd, HI_S32 param, HI_VOID *setVal,HI_VOID *retVal)
{

	HI_S32 s32Ret = HI_SUCCESS;

	s32Ret = pfn(cmd, param, setVal, retVal);

	printf("cmd=%d, param=%d, s32Ret=%d\n",cmd,param,s32Ret);	
	#if 0
    
	if(cmd < MAX_MSC_CMD_SET_HARDWARE)
	{
		s32Ret = MscCmdDoExecute_Set_Hardware(cmd, param, retVal);
	}
	else if(cmd < MAX_MSC_CMD_SET_SYS_PARAM)
	{
		s32Ret = MscCmdDoExecute_Set_SysParam(cmd, param, retVal);
	}
	else if(cmd < MAX_MSC_CMD_SET_SNAP_PARAM)
	{
		s32Ret = MscCmdDoExecute_Set_SnapParam(cmd, param, retVal);
	}
	else if(cmd < MAX_MSC_CMD_GET_HARDWARE)
	{
		s32Ret = MscCmdDoExecute_Get_Hardware(cmd, retVal);
	}
	else if(cmd < MAX_MSC_CMD_GET_SYS_PARAM)
	{
		s32Ret = MscCmdDoExecute_Get_SysParam(cmd, retVal);
	}
	else if(cmd < MAX_MSC_CMD_GET_SNAP_PARAM)	 
	{
		s32Ret = MscCmdDoExecute_Get_SnapParam(cmd, retVal);
	}
	else
	{
		MLOGE("ERR MSC DV CMD\n");
		s32Ret = HI_FAILURE;
	}
	#endif
	return s32Ret;
}
	

HI_S32 CmdProcInit(pfCallBack pCallBack)
{
	pfn = pCallBack;
}

