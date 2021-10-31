/**
 * @Brief   用于SD卡大容量存储设备的线程
 * @Dependency
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"



/* Defines ------------------------------------------------------------------*/
#define SD_BLOCKSIZE                    512
#define SD_SCAN_FILES_ENABLE            1           // 是否开机扫描 sd 卡文件




/* Functions prototypes ---------------------------------------------*/
static void StorageThread(void *argument);
static FRESULT ScanFiles(char *path);
static void InitFatFS();
static void ReadFile();



/* Private variables -----------------------------------------------------------*/
extern U16      Music_Thread_Exist;          // 音乐线程启动标志

uint16_t        Fatfs_OK = 0;

FIL*            Storage_Read_pFile;         // 文件指针
uint8_t*        Storage_Read_pBuffer;       // 缓冲区地址
uint32_t        Storage_Read_uiSize;        // 要读的块大小
uint32_t        Storage_Read_uiNum;         // 实际读到的字节数
uint16_t        Storage_Read_Request;       // 读请求
uint32_t        Storage_Read_fptr;          // 【用于读失败重启文件系统后恢复文件位置指针】

uint16_t        Storage_Thread_Exist = 0;       // Storage 线程是否正在运行



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
    if(Storage_Thread_Exist == 1)
        return;
        
#ifdef CMSIS_V1
    #if SD_SCAN_FILES_ENABLE
        xTaskCreate(StorageThread, "Storage Task", 512, NULL, (UBaseType_t)osPriorityLow, &storageTaskHandle);
    #else
        xTaskCreate(StorageThread, "Storage Task", 512, NULL, (UBaseType_t)osPriorityLow, &storageTaskHandle);
    #endif
#endif

#ifdef CMSIS_V2
    guiTaskHandle = osThreadNew(StorageThread, NULL, &storageTask_attributes);
#endif
}





static void StorageThread(void *argument)
{
    Storage_Thread_Exist = 1;

    for (;;)
    {
        InitFatFS();
        ReadFile();

        if (Storage_Read_Request == 0 && Music_Thread_Exist == 0)
        {
            Storage_Thread_Exist = 0;
#ifdef CMSIS_V1
            vTaskDelete(storageTaskHandle);
#endif
#ifdef CMSIS_V2
            osThreadTerminate(storageTaskHandle);
#endif
        }

        osDelay(10);
    }
}




/**
 * @brief  初始化 FAT 文件系统
 * @param  None
 * @retval None
 */
static void InitFatFS()
{
    static FRESULT fatfs_ret[4];
    extern SD_HandleTypeDef hsd1;
    extern HAL_StatusTypeDef HAL_SD_DeInit(SD_HandleTypeDef *hsd);

    if (Fatfs_OK == 0)
    {
        HAL_SD_DeInit(&hsd1);
        MX_FATFS_Init();
        MX_USB_DEVICE_Init();

        fatfs_ret[0] = f_mount(&SDFatFS, "", 1);
        if (fatfs_ret[0] == FR_OK)
        {
            fatfs_ret[1] = f_open(&SDFile, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
            if (fatfs_ret[1] == FR_OK)
            {
                fatfs_ret[2] = (FRESULT)f_printf(&SDFile, "Hello %d\n", 112);
                fatfs_ret[3] = f_close(&SDFile);
                if(fatfs_ret[3] == FR_OK)
                {
                    printf("FAT fs OK!\n");
                    f_unlink("test.txt");

                    Fatfs_OK = 1;
                    if (SD_SCAN_FILES_ENABLE)
                        ScanFiles("");
                }
                else
                    printf("FATfs error!\n");
            }
            else
                printf("FATfs init error while opening a file!\n");
        }
    }
}



/**
 * @brief  读取 SD 卡某个文件的内容到 `Storage_Read_pBuffer` 指定的区域
 * @param  None
 * @retval None
 */
static void ReadFile()
{
    static FRESULT  ret;
    static uint16_t Fatfs_restart = 0;          // 文件系统重启指示
    extern char     currentMusicPath[100];

    if (Storage_Read_Request)
    {
        if(Fatfs_restart == 1)
        {
            if(Fatfs_OK == 1)
            {
                ret = f_open(Storage_Read_pFile, currentMusicPath, FA_READ);
                if(ret == FR_OK)
                {
                    Fatfs_restart = 0;          // 重启成功
                    printf("Fatfs restart ok!\n");
                    f_lseek(Storage_Read_pFile, Storage_Read_fptr);     // 移动文件指针
                }
                else
                {
                    printf("Fatfs restart failed!\n");
                    return;                 // 继续重启
                }
            }
            else
                return;                     // 继续重启
        }

        ret = f_read(Storage_Read_pFile, Storage_Read_pBuffer, Storage_Read_uiSize, &Storage_Read_uiNum);
        if (ret != FR_OK) // 读失败，表示文件系统损坏，需要重新初始化
        {
            Fatfs_OK = 0;
            Fatfs_restart = 1;
        }
        else
            Storage_Read_Request = 0; // 读成功，清零请求标志
    }
}

/**
 * @brief  扫描 SD 卡指定目录下所有文件
 * @param  char* path
 * @retval None
 */
static FRESULT ScanFiles(char *path)
{
    char path_other[70] = {0}; //目录

    FRESULT     res;
    FILINFO     fno;
    DIR         dir;
    uint16_t    i = 0;
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
            if (fno.fname[0] == '.' || !strcmp(path_other, "System") || !strcmp(path_other, "SPOTLI") || !strcmp(path_other, ".fseve"))
                continue;

            fn = fno.fname;

            if ((fno.fattrib & AM_DIR) && fn[0] != '.')
            {
                memset(path_other, 0, sizeof(path_other));
                sprintf(path_other, "%s/%s", path, fno.fname);
                printf("%s/%s--%ld\n", path, fn, fno.fsize);

                ScanFiles(path_other);

                i++;
            }
            else
                printf("%s/%s--%ld\n", path, fn, fno.fsize);
        }
        res = f_closedir(&dir);
    }
    return res;
}