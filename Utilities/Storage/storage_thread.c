/**
 * @Brief   用于SD卡大容量存储设备的线程
 * @Dependency
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"
#include "dialog_window.h"
#include "os_state.h"
#include <string.h>



/* Defines ------------------------------------------------------------------*/
#define SD_BLOCKSIZE                    512
#define SD_SCAN_FILES_ENABLE            0           // 是否开机扫描 sd 卡文件




/* Functions prototypes ---------------------------------------------*/
static void StorageThread(void *argument);
static FRESULT ScanFiles(char *path);
static void InitFatFS();
static void ReadFile();
static void vRebootTimerCraete();
static void SaveInfoBeforeReboot();



/* Private variables -----------------------------------------------------------*/
extern U16      Music_Thread_Exist;          // 音乐线程启动标志

uint16_t        Storage_Fatfs_OK = 0;

FIL*            Storage_Read_pFile;         // 文件指针
uint8_t*        Storage_Read_pBuffer;       // 缓冲区地址
uint32_t        Storage_Read_uiSize;        // 要读的块大小
uint32_t        Storage_Read_uiNum;         // 实际读到的字节数
uint16_t        Storage_Read_Request;       // 读请求
uint32_t        Storage_Read_fptr;          // 【用于读失败重启文件系统后恢复文件位置指针】

uint16_t        Storage_Thread_Exist = 0;       // Storage 线程是否正在运行


// 【用于重启】
#ifdef CMSIS_V1
osTimerId timer_reboot;
#endif
#ifdef CMSIS_V2
osTimerId_t timer_reboot;
#endif
uint16_t Timer_Reboot_Exist = 0;




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

        if (Storage_Read_Request == 0 && Music_Thread_Exist == 0)       // 当音乐线程存在时，存储线程不退出
        {
            Storage_Thread_Exist = 0;
#ifdef CMSIS_V1
            vTaskDelete(storageTaskHandle);
#endif
#ifdef CMSIS_V2
            osThreadTerminate(storageTaskHandle);
#endif
        }

        vTaskDelay(10);
    }
}




/**
 * @brief  初始化 FAT 文件系统
 * @note   支持热插拔 SD 卡
 * @param  None
 * @retval None
 */
static void InitFatFS()
{
    static FRESULT fatfs_ret[4];
    extern SD_HandleTypeDef hsd1;

    if (Storage_Fatfs_OK == 0)
    {
        FATFS_UnLinkDriver("0:/");          // 用于支持热插拔
        MX_FATFS_Init();
        MX_USB_DEVICE_Init();

        f_mount(NULL,"",0);
        fatfs_ret[0] = f_mount(&SDFatFS, "", 1);        // 1. 挂载文件系统
        if (fatfs_ret[0] == FR_OK)
        {
            fatfs_ret[1] = f_open(&SDFile, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);        // 2. 打开测试文件
            if (fatfs_ret[1] == FR_OK)
            {
                fatfs_ret[2] = (FRESULT)f_printf(&SDFile, "Hello %d\n", 112);               // 3. 向测试文件中写入
                fatfs_ret[3] = f_close(&SDFile);                                            // 4. 关闭测试文件
                if(fatfs_ret[3] == FR_OK && fatfs_ret[2] != (FRESULT)EOF)           // 检测返回值是否为 FR_OK
                {
                    printf("@@@FAT fs OK!\n");         // 文件系统测试成功
                    f_unlink("test.txt");              // 删除测试文件

                    Storage_Fatfs_OK = 1;                   // 置位文件系统初始化成功标志
                    if (SD_SCAN_FILES_ENABLE)
                        ScanFiles("");          // 打印SD卡中所有文件名【可选】
                }
                else if (Timer_Reboot_Exist == 0)           // 关闭文件的返回值异常
                    CreateAlarmDialog_Self("FATfs Error", "FATfs init error \nwhile closing a file!", DIALOG_NOTHING);     
            }
            else  if (Timer_Reboot_Exist == 0)  // 打开文件的返回值异常
                CreateAlarmDialog_Self("FATfs Error", "FATfs init error \nwhile opening a file!", DIALOG_NOTHING);
        }
        else if (Timer_Reboot_Exist == 0)// 挂载 SD 卡异常
            CreateAlarmDialog_Self("FATfs Error", "FATfs init error \nwhile mounting SD card!", DIALOG_NOTHING);
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
    static WM_HWIN  hDialogWin = 0;

    if (Storage_Read_Request)
    {
        if(Fatfs_restart == 1)          // 文件系统需要重启
        {
            if(Storage_Fatfs_OK == 1)           // 文件系统重启成功
            {
                ret = f_open(Storage_Read_pFile, currentMusicPath, FA_READ);        // 重新打开文件
                if(ret == FR_OK)
                {
                    Fatfs_restart = 0;                  // 重启成功，清空文件系统重启标识
                    GUI_EndDialog(hDialogWin, 0);       // 关闭窗口
                    if(Timer_Reboot_Exist)
                    {
                        osTimerDelete(timer_reboot);
                        Timer_Reboot_Exist = 0;
                        memset((void*)REBOOT_STORAGE_AREA, 0, REBOOT_STORAGE_AREA_SIZE);
                    }
                    

                    f_lseek(Storage_Read_pFile, Storage_Read_fptr);                 // 重新移动文件指针
                }
                else
                    return;
            }
            else                        // 重启失败
            {
                if(Timer_Reboot_Exist == 0)
                    hDialogWin = CreateAlarmDialog_Self("FATfs Error", "FATfs read error \nPlease reboot! \nAuto reboot after 3 seconds", 
                                                        DIALOG_REBOOT);
                SaveInfoBeforeReboot();
            }
        }

        // 读文件
        ret = f_read(Storage_Read_pFile, Storage_Read_pBuffer, Storage_Read_uiSize, &Storage_Read_uiNum);
        if (ret != FR_OK) // 读失败，表示文件系统损坏，需要重新初始化
        {
            Storage_Fatfs_OK = 0;
            Fatfs_restart = 1;
        }
        else
            Storage_Read_Request = 0; // 读成功，清零请求标志
    }
}





/**
 * @brief  发出在 Storage 线程中进行读写的请求
 * @param  FIL*     fp
 * @param  uint64_t offset
 * @param  void*    buff
 * @param  uint32_t size
 * @retval None
 */
void Storage_Thread_Read(FIL *fp, uint64_t offset, void *buff, uint32_t size)
{
    if(Storage_Read_Request)         //等待上一次读完成
        vTaskDelay(1);
    Storage_Read_pFile = fp;
    f_lseek(fp, Storage_Read_fptr = offset);
    Storage_Read_pBuffer = buff;
    Storage_Read_uiSize = size;
    Storage_Read_Request = 1;
}



static void SaveInfoBeforeReboot()
{
    extern U16      Music_Item_Current;
    extern char     musicPath[100];
    extern WM_HWIN  hListView;
    extern uint32_t uiWavPlayIndex;
    extern U16      Music_Play_On;

    static uint16_t i, numRows;
    static char   buf[100];
    static os_reboot_info_t   *pRebootInfo = (os_reboot_info_t*)REBOOT_STORAGE_AREA;


    if (Timer_Reboot_Exist == 0)
    {
        Timer_Reboot_Exist = 1;
        vRebootTimerCraete();

        memset(pRebootInfo, 0, REBOOT_STORAGE_AREA_SIZE);
        pRebootInfo->magic = 0x1234567887654321;
        pRebootInfo->uiWavPlayIndex = uiWavPlayIndex;
        pRebootInfo->Music_Item_Current = Music_Item_Current;
        pRebootInfo->Music_Play_Restart = 2;
        strcpy(pRebootInfo->musicPath, musicPath);

        pRebootInfo->musicListLen = numRows = LISTVIEW_GetNumRows(hListView);
        for (i = 0; i < numRows; i++)
        {
            LISTVIEW_GetItemText(hListView, 0, i, buf, 99);
            strcat((pRebootInfo->musicPathList)[i], buf);
            LISTVIEW_GetItemText(hListView, 1, i, buf, 10);
            strcat((pRebootInfo->musicDurationList)[i], buf);
        }
    }
}




#ifdef CMSIS_V1
/**
  * @brief  Timer callback for `reboot` (5 s)
  * @param  void *n: Timer index 
  * @retval None
  */
static void RebootTimerCallback(const void *n)
{  
  NVIC_SystemReset();
}
#endif


#ifdef CMSIS_V2
/**
  * @brief  Timer callback for `reboot` (5 s)
  * @param  void *n: Timer index 
  * @retval None
  */
static void RebootTimerCallback(void *n)
{  
  NVIC_SystemReset();
}
#endif



/**
 * @brief  创建一个重启定时器，计时 5s
 * @param  None
 * @retval None
 */
static void vRebootTimerCraete()
{
#ifdef CMSIS_V1
  osTimerDef(timer_reboot, RebootTimerCallback);
  timer_reboot = osTimerCreate(osTimer(timer_reboot), osTimerOnce, NULL);
  osTimerStart(timer_reboot, 3000);
#endif

#ifdef CMSIS_V2
  timer_reboot = osTimerNew(RebootTimerCallback, osTimerOnce, NULL, NULL);
  osTimerStart(timer_reboot, 3000);
#endif
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




