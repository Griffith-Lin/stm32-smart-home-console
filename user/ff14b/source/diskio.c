/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

//完成了下述的修改，还需要修改堆栈空间，因为长文件支持需要占用堆空间。

//在diskio.c中加入添加rtc.h 文件 并加入RTC函数
#include "rtc.h"
DWORD get_fattime (void)
{		
	DWORD date = 0;
	
	RTC_GetTimeDate();
	date =
	( (
	(RTC_DateStruct.RTC_Year+20) << 25) |
	(RTC_DateStruct.RTC_Month << 21 ) |
	(RTC_DateStruct.RTC_Date << 16 ) |
	(RTC_TimeStruct.RTC_Hours << 11 ) |
	(RTC_TimeStruct.RTC_Minutes << 5 ) |
	(RTC_TimeStruct.RTC_Seconds )
	);
	return date;
}
//其中RTC_GetTimeDate()函数如下、在RTC中书写即可
RTC_DateTypeDef RTC_DateStruct;
RTC_TimeTypeDef RTC_TimeStruct;
void RTC_GetTimeDate(void)
{
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
}







/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/


//该函数主要是用来获取磁盘状态。现在未用到
//DSTATUS disk_status (
//	BYTE pdrv		/* Physical drive nmuber to identify the drive */
//)
//{
//	DSTATUS stat;
//	int result;

//	switch (pdrv) {
//	case DEV_RAM :
//		result = RAM_disk_status();

//		// translate the reslut code here

//		return stat;

//	case DEV_MMC :
//		result = MMC_disk_status();

//		// translate the reslut code here

//		return stat;

//	case DEV_USB :
//		result = USB_disk_status();

//		// translate the reslut code here

//		return stat;
//	}
//	return STA_NOINIT;
//}


DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_SD :
//		result = RAM_disk_status();

		// translate the reslut code here
		return 0;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/


//添加SD卡的初始化，其他不用到的代码直接删掉
//DSTATUS disk_initialize (
//	BYTE pdrv				/* Physical drive nmuber to identify the drive */
//)
//{
//	DSTATUS stat;
//	int result;

//	switch (pdrv) {
//	case DEV_RAM :
//		result = RAM_disk_initialize();

//		// translate the reslut code here

//		return stat;

//	case DEV_MMC :
//		result = MMC_disk_initialize();

//		// translate the reslut code here

//		return stat;

//	case DEV_USB :
//		result = USB_disk_initialize();

//		// translate the reslut code here

//		return stat;
//	}
//	return STA_NOINIT;
//}

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	switch (pdrv) {
	case DEV_SD :
		stat = SD_Initialize();
		return stat;
		// translate the reslut code here
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/


//修改disk_read函数，加入SD卡读任意扇区的函数(需要用户自己提供)，其他不用到的选项可以删掉
//DRESULT disk_read (
//	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
//	BYTE *buff,		/* Data buffer to store read data */
//	LBA_t sector,	/* Start sector in LBA */
//	UINT count		/* Number of sectors to read */
//)
//{
//	DRESULT res;
//	int result;

//	switch (pdrv) {
//	case DEV_RAM :
//		// translate the arguments here

//		result = RAM_disk_read(buff, sector, count);

//		// translate the reslut code here

//		return res;

//	case DEV_MMC :
//		// translate the arguments here

//		result = MMC_disk_read(buff, sector, count);

//		// translate the reslut code here

//		return res;

//	case DEV_USB :
//		// translate the arguments here

//		result = USB_disk_read(buff, sector, count);

//		// translate the reslut code here

//		return res;
//	}

//	return RES_PARERR;
//}

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_SD:
		res = SD_ReadDisk(buff,sector,count);
		return res;
	}

	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

//修改disk_write 函数，添加写扇区函数：
//DRESULT disk_write (
//	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
//	const BYTE *buff,	/* Data to be written */
//	LBA_t sector,		/* Start sector in LBA */
//	UINT count			/* Number of sectors to write */
//)
//{
//	DRESULT res;
//	int result;

//	switch (pdrv) {
//	case DEV_RAM :
//		// translate the arguments here

//		result = RAM_disk_write(buff, sector, count);

//		// translate the reslut code here

//		return res;

//	case DEV_MMC :
//		// translate the arguments here

//		result = MMC_disk_write(buff, sector, count);

//		// translate the reslut code here

//		return res;

//	case DEV_USB :
//		// translate the arguments here

//		result = USB_disk_write(buff, sector, count);

//		// translate the reslut code here

//		return res;
//	}

//	return RES_PARERR;
//}


DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_SD:
		res = SD_WriteDisk((u8*)buff,sector,count);
		while(res != 0)
		{
			res = SD_WriteDisk((u8*)buff,sector,count);
		}
		return res;
	}

	return RES_PARERR;
}


#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

//修改disk_ioctl 函数，填充ioctl命令功能。这些功能是标准的命令，在diskio.h有定义
//DRESULT disk_ioctl (
//	BYTE pdrv,		/* Physical drive nmuber (0..) */
//	BYTE cmd,		/* Control code */
//	void *buff		/* Buffer to send/receive control data */
//)
//{
//	DRESULT res;
//	int result;

//	switch (pdrv) {
//	case DEV_RAM :

//		// Process of the command for the RAM drive

//		return res;

//	case DEV_MMC :

//		// Process of the command for the MMC/SD card

//		return res;

//	case DEV_USB :

//		// Process of the command the USB drive

//		return res;
//	}

//	return RES_PARERR;
//}

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_SD:
		switch(cmd)
		{
			case CTRL_SYNC:
				res = RES_OK;
				break;
			case GET_SECTOR_SIZE:
				*(DWORD*)buff = 512;
				res = RES_OK;
				break;
			case GET_BLOCK_SIZE:
				*(WORD*)buff = 4096;
				res = RES_OK;
				break;
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = 1989632;
				res = RES_OK;
				break;
			default:
				res = RES_PARERR;
				break;
		}
		return res;
	}

	return RES_PARERR;
}