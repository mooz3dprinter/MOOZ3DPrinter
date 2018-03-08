/**
  ******************************************************************************
  * @file    usbh_diskio.c 
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    20-November-2014
  * @brief   USB Key Disk I/O driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
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
#include "ff_gen_drv.h"
#include "usbh_usr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
DSTATUS USBH_initialize (void);
DSTATUS USBH_status (void);
DRESULT USBH_read (BYTE*, DWORD, UINT);

#if _USE_WRITE == 1
  DRESULT USBH_write (const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
  DRESULT USBH_ioctl (BYTE, void*);
#endif /* _USE_IOCTL == 1 */
  
Diskio_drvTypeDef  USBH_Driver =
{
  USBH_initialize,
  USBH_status,
  USBH_read, 
#if  _USE_WRITE == 1
  USBH_write,
#endif /* _USE_WRITE == 1 */  
#if  _USE_IOCTL == 1
  USBH_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS USBH_initialize(void)
{
  DSTATUS stat = STA_NOINIT;

  if (USB_disk_status())
  {
    stat = RES_OK; 
  }
  return stat;
}

/**
  * @brief  Gets Disk Status
  * @param  None
  * @retval DSTATUS: Operation status
  */
DSTATUS USBH_status(void)
{
  return RES_OK;
}

/**
  * @brief  Reads Sector(s) 
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USBH_read(BYTE *buff, DWORD sector, UINT count)
{
  if (!count)  return RES_PARERR;  //²ÎÊý´íÎó
  
  if (USB_disk_read(buff, sector, count))  //²Ù×÷Ê§°Ü
  {
    return RES_ERROR;
  }
  return RES_OK;
}

/**
  * @brief  Writes Sector(s)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USBH_write(const BYTE *buff, DWORD sector, UINT count)
{
  if (!count)  return RES_PARERR;
  
  if (USB_disk_write((uint8_t *)buff, sector, count))
  {
    return RES_ERROR;
  }
  return RES_OK; 
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USBH_ioctl(BYTE cmd, void *buff)
{
  DRESULT res = RES_OK;
  
  //if (pdrv)  return RES_PARERR;
  res = RES_ERROR;
  
  switch (cmd) {
  case CTRL_SYNC :    /* Make sure that no pending write process */
    
    res = RES_OK;
    break;
    
  case GET_SECTOR_COUNT :  /* Get number of sectors on the disk (DWORD) */
    
    *(DWORD*)buff = (DWORD) USBH_MSC_Param.MSCapacity;
    res = RES_OK;
    break;
    
  case GET_SECTOR_SIZE :  /* Get R/W sector size (WORD) */
    *(WORD*)buff = 512;
    res = RES_OK;
    break;
    
  case GET_BLOCK_SIZE :  /* Get erase block size in unit of sector (DWORD) */
    
    *(DWORD*)buff = 512;
    
    break;
    
    
  default:
    res = RES_PARERR;
  }
  
  return res;
}
#endif /* _USE_IOCTL == 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

