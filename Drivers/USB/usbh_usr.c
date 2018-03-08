/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
//#include "lcd_log.h"
#include "ff.h"       /* FATFS */
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"

#include "ff_gen_drv.h"
#include "usbh_diskio.h"

/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR
* @brief    This file includes the usb host stack user callbacks
* @{
*/

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/
/**
* @}
*/


/** @defgroup USBH_USR_Private_Defines
* @{
*/
//#define IMAGE_BUFFER_SIZE    512
/**
* @}
*/


/** @defgroup USBH_USR_Private_Macros
* @{
*/
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
/**
* @}
*/

/** @defgroup USBH_USR_MAIN_Private_Variables
* @{
*/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core __ALIGN_END;

#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USBH_HOST                USB_Host __ALIGN_END;
/**
* @}
*/

/** @defgroup USBH_USR_Private_Variables
* @{
*/
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
uint8_t filenameString[15]  = {0};

FATFS fatfs;
FIL file;
char USBPath[4];
uint8_t line_idx = 0;

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError

};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]        = "> Host Library Initialized\n";
const uint8_t MSG_DEV_ATTACHED[]     = "> Device Attached \n";
const uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected\n";
const uint8_t MSG_DEV_ENUMERATED[]   = "> Enumeration completed \n";
const uint8_t MSG_DEV_HIGHSPEED[]    = "> High speed device detected\n";
const uint8_t MSG_DEV_FULLSPEED[]    = "> Full speed device detected\n";
const uint8_t MSG_DEV_LOWSPEED[]     = "> Low speed device detected\n";
const uint8_t MSG_DEV_ERROR[]        = "> Device fault \n";

const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected\n";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected\n";
const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes: \n";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:\n";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...\n";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected\n";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE\n";

/** @defgroup USBH_USR_Private_Functions
* @{
*/


/**
* @brief  USBH_USR_Init
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{

}

/**
* @brief  USBH_USR_DeviceAttached
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
    FATFS_LinkDriver(&USBH_Driver, USBPath);
    printf("%s, usb path:%s!\r\n", __FUNCTION__, USBPath);
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
    printf("%s!\r\n", __FUNCTION__);
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void)
{
    FATFS_UnLinkDriver(USBPath);
    USBH_USR_ApplicationState = USH_USR_FS_INIT;
    printf("%s!\r\n", __FUNCTION__);
}
/**
* @brief  USBH_USR_ResetUSBDevice
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
    printf("%s!\r\n", __FUNCTION__);
  /* callback for USB-Reset */
}


/**
* @brief  USBH_USR_DeviceSpeedDetected
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
    const char *pDeviceSpeed[] = {
        "high speed",
        "full speed",
        "low speed"
    };
    
    printf("%s, speed:%s!\r\n", __FUNCTION__, pDeviceSpeed[DeviceSpeed]);
}

/**
* @brief  USBH_USR_Device_DescAvailable
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
    USBH_DevDesc_TypeDef *hs = (USBH_DevDesc_TypeDef *)DeviceDesc;

    printf("%s, vendor id:0x%04x, product id:0x%04x!\r\n", __FUNCTION__, hs->idVendor, hs->idProduct);
}

/**
* @brief  USBH_USR_DeviceAddressAssigned
*         USB device is successfully assigned the Address
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
    printf("%s!\r\n", __FUNCTION__);
}


/**
* @brief  USBH_USR_Conf_Desc
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{

    USBH_InterfaceDesc_TypeDef *id = itfDesc;
    printf("%s, interface number:%d, endpoint number:%d, class:0x%02x!\r\n", __FUNCTION__, id->bInterfaceNumber, id->bNumEndpoints, id->bInterfaceClass);
    // bInterfaceClass 08:MSC 03:HID
}

/**
* @brief  USBH_USR_Manufacturer_String
*         Displays the message on LCD for Manufacturer String
* @param  Manufacturer String
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
    printf("%s, manufacture:%s!\r\n", __FUNCTION__, (const char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
    printf("%s, product:%s!\r\n", __FUNCTION__, (const char *)ProductString);
}

/**
* @brief  USBH_USR_SerialNum_String
*         Displays the message on LCD for SerialNum_String
* @param  SerialNum_String
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
    printf("%s, serial:%s!\r\n", __FUNCTION__, (char *)SerialNumString);
}

/**
* @brief  EnumerationDone
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
    printf("%s!\r\n", __FUNCTION__);
}


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
    printf("%s!\r\n", __FUNCTION__);
}

/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
    printf("%s!\r\n", __FUNCTION__);
    USBH_USR_Status usbh_usr_status = USBH_USR_RESP_OK;

    return usbh_usr_status;
}

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void)
{
    printf("%s!\r\n", __FUNCTION__);
}

/**
* @brief  USBH_USR_MSC_Application
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void)
{
  //FRESULT res;

  switch(USBH_USR_ApplicationState)
  {
    case USH_USR_FS_INIT:
    printf("%s!\r\n", __FUNCTION__);
    /* Initialises the File System*/
    if ( f_mount(&fatfs, USBPath, 0) != FR_OK )
    {
      /* efs initialisation fails*/
      ////LCD_ErrLog("> Cannot initialize File System.\n");
      return(-1);
    }
//    //LCD_UsrLog("> File System initialized.\n");
//    //LCD_UsrLog("> Disk capacity : %d Bytes\n", USBH_MSC_Param.MSCapacity * \
      USBH_MSC_Param.MSPageLength);

    if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
    {
//      //LCD_ErrLog((void *)MSG_WR_PROTECT);
    }

    USBH_USR_ApplicationState = USH_USR_FS_OK;
    break;

    case USH_USR_FS_OK:

    break;
  }
  return(0);
}

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
    USBH_USR_ApplicationState = USH_USR_FS_INIT;
}

void Marlin_USB_Init(void)
{
  USBH_Init(&USB_OTG_Core,
#ifdef USE_USB_OTG_FS
            USB_OTG_FS_CORE_ID,
#else
            USB_OTG_HS_CORE_ID,
#endif
            &USB_Host,
            &USBH_MSC_cb,
            &USR_cb);
}

void Marlin_USB_Process(void)
{
  USBH_Process(&USB_OTG_Core, &USB_Host);
}

/**
  * @brief  OTG_FS_IRQHandler
  *          This function handles USB-On-The-Go FS global interrupt request.
  *          requests.
  * @param  None
  * @retval None
  */

#include "usb_hcd_int.h"

#ifdef USE_USB_OTG_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
  USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

/**
  *****************************************************************************
  * @Name   : 获取USB连接状态
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : 0: 未连接
  *           1: 已经连接
  *****************************************************************************
**/
uint32_t USB_disk_status(void)
{
  return HCD_IsDeviceConnected(&USB_OTG_Core);
}

/**
  *****************************************************************************
  * @Name   : USB 读取数据
  *
  * @Brief  : none
  *
  * @Input  : sector: 读取数据扇区起始地址
  *           count:  扇区数目
  *
  * @Output : *buff:  读取数据缓冲
  *
  * @Return : 0:    OK
  *           其他: 出错
  *****************************************************************************
**/
int USB_disk_read(uint8_t *buff, uint32_t sector, uint32_t count)
{
  int status = 1;

  if (HCD_IsDeviceConnected(&USB_OTG_Core))
  {
    do
    {
      status = USBH_MSC_Read10(&USB_OTG_Core, buff, sector, 512*count);
      USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);

      if (!HCD_IsDeviceConnected(&USB_OTG_Core))
      {
        return 1;  //读取错误
      }
    }while (status == USBH_MSC_BUSY);
  }
  else
  {
    status = 1;
  }

  if (status == USBH_MSC_OK)  return 0;

  return status;
}

/**
  *****************************************************************************
  * @Name   : USB 写入数据
  *
  * @Brief  : none
  *
  * @Input  : *buff:  写入数据缓冲
  *           sector: 写入数据扇区起始地址
  *           count:  扇区数目
  *
  * @Output : none
  *
  * @Return : 0:    OK
  *           其他: 出错
  *****************************************************************************
**/
int USB_disk_write(uint8_t *buff, uint32_t sector, uint32_t count)
{
  int status = 1;

  if (HCD_IsDeviceConnected(&USB_OTG_Core))
  {
    do
    {
      status = USBH_MSC_Write10(&USB_OTG_Core, (uint8_t *)buff, sector, 512*count);
      USBH_MSC_HandleBOTXfer(&USB_OTG_Core, &USB_Host);

      if (!HCD_IsDeviceConnected(&USB_OTG_Core))
      {
        return 1;  //写入出错
      }
    }while (status == USBH_MSC_BUSY);
  }
  else
  {
    status = 1;
  }

  if (status == USBH_MSC_OK)  return 0;

  return status;
}

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

