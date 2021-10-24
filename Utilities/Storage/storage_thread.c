/**
 * @Brief   用于SD卡大容量存储设备的线程
 * @Dependency  
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"


/* Defines ------------------------------------------------------------------*/
#define SD_BLOCKSIZE 512




/* Functions prototypes ---------------------------------------------*/
static void StorageThread(void *argument);
static FRESULT scan_files(char* path);




/* Private variables -----------------------------------------------------------*/
FRESULT ret[4];
char pwd[20];



/* Function implementations -----------------------------------------------------------*/
#ifdef CMSIS_V1
TaskHandle_t storageTaskHandle;
#endif

#ifdef CMSIS_V2
osThreadId_t storageTaskHandle;
const osThreadAttr_t storageTask_attributes = {
	.name = "Storage Task",
	.stack_size = 1024,
	.priority = (osPriority_t)osPriorityLow,
};
#endif


/**
 * @brief  Storage Thread 的包装函数，用于 osThreadNew()
 * @param  None
 * @retval None
 */
void vStorageTaskCreate()
{
#ifdef CMSIS_V1
	xTaskCreate(StorageThread, "Storage Task", 256, NULL, osPriorityLow, &storageTaskHandle);
#endif

#ifdef CMSIS_V2
	guiTaskHandle = osThreadNew(StorageThread, NULL, &storageTask_attributes);
#endif
}


#include "app_wavDecoder.h"

static void StorageThread(void *argument)
{
    MX_USB_DEVICE_Init();
    MX_FATFS_Init();
	  
	ret[0] = f_mount(&SDFatFS, "", 0);
    if(ret[0] == FR_OK)
    {
        ret[1] = f_open(&SDFile, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
        if(ret[1] == FR_OK)
        {
            ret[2] = f_printf(&SDFile, "Hello %d\n", 112);
            ret[3] = f_close(&SDFile);
            printf("FAT fs OK!\n");
        }
    }

// #ifdef CMSIS_V1
//     vTaskDelete(storageTaskHandle);
// #endif
// #ifdef CMSIS_V2
//     osThreadTerminate(storageTaskHandle);
// #endif
    

    ret[1] = f_open(&SDFile, "The_Dawn.wav", FA_READ);
    WaveHeader_t wavHeader;
    read_wavheader(&SDFile, &wavHeader);
    print_wavheader(wavHeader);

    for(;;)
    {
        osDelay(100);
    }
}



static FRESULT scan_files(char* path)
{
    char        path_other[255] = {0}; //目录 长度
    
    FRESULT     res;
    FILINFO     fno;
    DIR         dir;
    uint16_t     i = 0;
    char        *fn;

    res = f_opendir(&dir, path);
    if (res == FR_OK)
    {
        for (;;)
        {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0)
                break;

            strcpy(path_other, fno.fname);
            path_other[6] = '\0';
            if (fno.fname[0] == '.' || !strcmp(path_other, "SYSTEM") || !strcmp(path_other, "SPOTLI") || !strcmp(path_other, "FSEVEN"))
                continue;

            fn = fno.fname;

            if ((fno.fattrib & AM_DIR) && fn[0] != '.')
            {
                memset(path_other, 0, sizeof(path_other));
                sprintf(path_other, "%s/%s", path, fno.fname);
                printf("%s/%s--%ld\n", path, fn, fno.fsize);

                scan_files(path_other);

                i++;
            }
            else
                printf("%s/%s--%ld\n", path, fn, fno.fsize);
        }
        res = f_closedir(&dir);
    }
    return res;
}
