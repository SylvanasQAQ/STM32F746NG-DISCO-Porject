/**
 * @Brief   用于SD卡大容量存储设备的线程
 * @Dependency
 */

/* Includes ------------------------------------------------------------------*/
#include "fatfs.h"
#include "main.h"
#include "usb_device.h"



/* Defines ------------------------------------------------------------------*/
#define SD_BLOCKSIZE 512



/* Functions prototypes ---------------------------------------------*/
static void StorageThread(void *argument);
static FRESULT scan_files(char *path);



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





static void StorageThread(void *argument)
{
    MX_USB_DEVICE_Init();
    MX_FATFS_Init();

    //uctsk_lua_init();

    ret[0] = f_mount(&SDFatFS, "", 0);
    if (ret[0] == FR_OK)
    {
        ret[1] = f_open(&SDFile, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
        if (ret[1] == FR_OK)
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
    scan_files("");
    PlayMusic();

    for (;;)
    {
        osDelay(1);
    }
}

#define TIM_CLOCK 108000000
#define DMA_BATCH 4096
#define WAV_VOLUME 1

uint32_t uiPuleseBuf[DMA_BATCH];
uint8_t *usWavData = (uint8_t *)(SDRAM_WRITE_READ_ADDR + 0x2000);
uint32_t autoReload, uiIndex = 0;
uint32_t num = 1;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim3;
WaveHeader_t wavHeader;

void PlayMusic(void)
{

    //ret[1] = f_open(&SDFile, "TKZC.wav", FA_READ);
    // if(ret[1] != FR_OK)
      //   ret[2] = f_open(&SDFile, "TianKong.wav", FA_READ);
    // if(ret[2] != FR_OK)
    //      ret[1] = f_open(&SDFile, "The_Dawn.wav", FA_READ);
    ret[2] = f_open(&SDFile, "The_Dawn.wav", FA_READ);

    read_wavheader(&SDFile, &wavHeader);
    ret[2] = f_read(&SDFile, usWavData, wavHeader.data_datasize, &num);
    print_wavheader(wavHeader);
    if(num == 0)
        return;

    autoReload = TIM_CLOCK / wavHeader.fmt_sample_rate;

    for (uint32_t i = 0; i < DMA_BATCH; i++)
        uiPuleseBuf[i] = autoReload * usWavData[uiIndex++] / (0xff) / WAV_VOLUME;

    __HAL_TIM_SET_PRESCALER(&htim5, 0);
    __HAL_TIM_SetAutoreload(&htim5, autoReload);
    __HAL_TIM_SetCounter(&htim5, 0);

    HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_4, uiPuleseBuf, DMA_BATCH);
}

uint32_t count = 0;
uint16_t *p;
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if (uiIndex < wavHeader.riff_datasize)
        HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_4, uiPuleseBuf, DMA_BATCH);
    else{
        HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_4);
        f_close(&SDFile);
    }
    for (uint32_t i = DMA_BATCH / 2; i < DMA_BATCH; i++)
        uiPuleseBuf[i] = autoReload * usWavData[uiIndex++]/ (0xff) / WAV_VOLUME;

    count++;
}

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    for (uint32_t i = 0; i < DMA_BATCH / 2; i++)
        uiPuleseBuf[i] = autoReload * usWavData[uiIndex++] / (0xff) / WAV_VOLUME;
}

static FRESULT scan_files(char *path)
{
    char path_other[255] = {0}; //目录 长度

    FRESULT res;
    FILINFO fno;
    DIR dir;
    uint16_t i = 0;
    char *fn;

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
