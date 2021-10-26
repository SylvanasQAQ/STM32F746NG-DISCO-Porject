/**
 * @Brief   用于音乐播放的线程
 * @Dependency
 * @Note    仅支持 8-bit wav 音乐播放
 * @Note    建议歌曲采样率在 20k Hz 以上，以避免高频噪声
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_wavDecoder.h"
#include "fatfs.h"


/* Defines ------------------------------------------------------------------*/
#define TIM_CLOCK           108000000       // APB1 的频率，即 TIM 的时钟频率
#define DMA_BATCH           4096            // DMA 一次传送的数据量
#define WAV_HEADER_PRINT    0               // 打印 wav 文件头信息
#define SD_READ_BATCH       (2*1024*512)    // 一次读写 SD 卡 1 MB 的内容





/* Functions prototypes ---------------------------------------------*/
static void MusicThread(void *argument);
static void PlayMusic();
static void PlayWavMusic(char * fileName);
static void Storage_Thread_Read(FIL *fp, uint64_t offset, void *buff, uint32_t size);
static void WavCacheUpdate();



/* Private variables -----------------------------------------------------------*/
extern TIM_HandleTypeDef htim5;         // TIM5 用于产生 PWM

U16             Music_Play_Start = 0;            // 音乐开始标志
U16             Music_Play_On  = 0;              // 音乐播放中标志
U16             Music_Thread_Exist = 0;          // 音乐线程启动标志
extern U16      Music_Item_Current;             // 当前播放音乐 index
char            currentMusicPath[100];              // 当前正在播放音乐的文件路径
extern char     musicPath[100];                     // 音乐文件路径
extern WM_HWIN  hListView;


FIL             wavFile;
uint8_t*        ucWavData = (uint8_t *)(SDRAM_WRITE_READ_ADDR + 0x2000);            // 在 sdram 中存储歌曲 PCM 数据
uint32_t        uiPuleseBuf[DMA_BATCH];         // TIM5 CH4 的 PWM 占空比信息
uint32_t        autoReload;                     // TIM5 的 autoreload
uint32_t        uiWavPlayIndex;                 // ucWavData 的 index，即当前播放位置
uint64_t        ulWavPcmStart;                  // wav 文件 PCM 数据的起始位置
uint32_t        uiWavDataLength;                // wav 文件的 PCM 数据长度
uint32_t        uiWavSampleDepth;               // wav 文件的采样位数
uint32_t        uiWavSampleRate;                // wav 文件的采样频率
uint16_t        usWavCacheHalfUsed;             // wav 文件的缓存一半已用标志
uint16_t        usWavCacheInvalid;              // wav 文件的缓存失效标志


uint32_t        uiMusicVolumeN = 1;             // 音量系数——分子
uint32_t        uiMusicVolumeD = 1;             // 音量系数——分母
uint32_t        uiMusicCofficient;              // 计算 PWM 占空比时需要除去的系数

uint32_t        uiMusicCurrentMinute = 0;       // 音乐播放进度——分钟
uint32_t        uiMusicCurrentSecond = 0;       // 音乐播放进度——秒
uint32_t        uiMusicCurrentProgress = 0;     // 音乐播放进度——百分比







/* Function implementations -----------------------------------------------------------*/
#ifdef CMSIS_V1
TaskHandle_t app_musicTaskHandle;
#endif

#ifdef CMSIS_V2
osThreadId_t app_musicTaskHandle;
const osThreadAttr_t app_musicTask_attributes = {
    .name = "Storage Task",
    .stack_size = 1024,
    .priority = (osPriority_t)osPriorityLow,
};
#endif



/**
 * @brief  Music Thread 的包装函数，用于 osThreadNew()
 * @param  None
 * @retval None
 */
void vMusicTaskCreate()
{
#ifdef CMSIS_V1
    xTaskCreate(MusicThread, "Music Task", 512, NULL, osPriorityNormal, &app_musicTaskHandle);
#endif

#ifdef CMSIS_V2
    guiTaskHandle = osThreadNew(MusicThread, NULL, &app_musicTask_attributes);
#endif
}




/**
 * @brief  Music thread
 * @param  void* argument : pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void MusicThread(void *argument)
{
    extern GUI_HWIN hCurrentWindow;
    extern GUI_HWIN hMusicWindow;


    for (;;)
    {
        
        PlayMusic();
        WavCacheUpdate();

        if(hCurrentWindow != hMusicWindow && Music_Play_On == 0)
        {
            Music_Thread_Exist = 0;
#ifdef CMSIS_V1
            vTaskDelete(app_musicTaskHandle);
#endif
#ifdef CMSIS_V2
            osThreadTerminate(app_musicTaskHandle);
#endif
        }

        osDelay(2);
    }
}


/**
 * @brief  处理音乐相关事件——暂停\继续
 * @param  None
 * @retval None
 */
static void PlayMusic()
{
    if (Music_Play_Start)
    {
        Music_Play_Start = 0;
        Music_Play_On = 1;

        if (strcmp(musicPath, currentMusicPath) != 0) // 新歌曲播放
        {
            strcpy(currentMusicPath, musicPath);
            PlayWavMusic(currentMusicPath);
        }
        else // 暂停的歌曲继续播放
        {
            if (uiWavPlayIndex < uiWavDataLength)
                HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_4, uiPuleseBuf, DMA_BATCH);
            else
                PlayWavMusic(currentMusicPath);
        }
    }
}




/**
 * @brief  播放 wav 格式的音乐
 * @param  char * fileName：音乐文件路径（必须是全路径
 * @retval None
 */
static void PlayWavMusic(char * fileName)
{
    FRESULT             fatfs_ret;
    WaveHeader_t        wavHeader;
    
    uiWavPlayIndex = 0;
    usWavCacheHalfUsed = 0;

    fatfs_ret = f_open(&wavFile, fileName, FA_READ);
    if (fatfs_ret == FR_OK)
    {
        if (read_wavheader(&wavFile, &wavHeader) != 0) // 读取 wav 文件头
        {
            printf("Invalid music file %s!\n", fileName);
            f_close(&wavFile);
            return;
        }
        uiWavDataLength = wavHeader.data_datasize;
        uiWavSampleDepth = wavHeader.fmt_bit_per_sample;
        uiWavSampleRate = wavHeader.fmt_sample_rate;
        ulWavPcmStart = wavFile.fptr;


        if (WAV_HEADER_PRINT)               // 打印 wav 文件头
            print_wavheader(wavHeader);

        Storage_Thread_Read(&wavFile, ulWavPcmStart, ucWavData, SD_READ_BATCH);
    }


    autoReload = TIM_CLOCK / wavHeader.fmt_sample_rate;         // 计算 TIM5 的 autoreload
    // 设置好 TIM5 的寄存器
    __HAL_TIM_SET_PRESCALER(&htim5, 0);
    __HAL_TIM_SetAutoreload(&htim5, autoReload);
    __HAL_TIM_SetCounter(&htim5, 0);


    // 为第一次 DMA 启动准备数据
    uiMusicCofficient = ((1 << uiWavSampleDepth) - 1) * uiMusicVolumeN / uiMusicVolumeD;
    for (uint32_t i = 0; i < DMA_BATCH; i++)
        uiPuleseBuf[i] = autoReload * ucWavData[uiWavPlayIndex++  % SD_READ_BATCH] / uiMusicCofficient;

    HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_4, uiPuleseBuf, DMA_BATCH);       // 启动 PWM
}



/**
 * @brief  更新 Wav 文件的缓存
 * @param  None
 * @retval None
 */
static void WavCacheUpdate()
{
    static uint64_t     offset;
    static uint64_t     size;
    extern uint16_t     Storage_Read_Request;

    if (Music_Play_On)
    {
        if (usWavCacheInvalid == 1)         // 缓存失效事件，立即更新所有缓存
        {
            usWavCacheInvalid = 0;
            if((uiWavPlayIndex % SD_READ_BATCH) * 2 / SD_READ_BATCH > 0)
                usWavCacheHalfUsed = 1;
            else
                usWavCacheHalfUsed = 0;
            offset = uiWavPlayIndex - (uiWavPlayIndex % SD_READ_BATCH);
            Storage_Thread_Read(&wavFile, offset, ucWavData, SD_READ_BATCH);
        }
        else                                // 正常双缓存策略
        {
            if (usWavCacheHalfUsed == 0 && (uiWavPlayIndex % SD_READ_BATCH) * 2 / SD_READ_BATCH > 0)
            {
                usWavCacheHalfUsed = 1;
                offset = uiWavPlayIndex - (uiWavPlayIndex % SD_READ_BATCH) + SD_READ_BATCH;
                if(offset + SD_READ_BATCH / 2 > uiWavDataLength)
                    size = uiWavDataLength - offset;
                else
                    size = SD_READ_BATCH / 2;
                Storage_Thread_Read(&wavFile, offset, ucWavData, size);
            }

            if (usWavCacheHalfUsed == 1 && (uiWavPlayIndex % SD_READ_BATCH) * 2 / SD_READ_BATCH == 0)
            {
                usWavCacheHalfUsed = 0;
                offset = uiWavPlayIndex - (uiWavPlayIndex % SD_READ_BATCH) + SD_READ_BATCH / 2;
                if(offset + SD_READ_BATCH / 2 > uiWavDataLength)
                    size = uiWavDataLength - offset;
                else
                    size = SD_READ_BATCH / 2;
                Storage_Thread_Read(&wavFile, offset, ucWavData + SD_READ_BATCH / 2, size);
            }
        }
    }
}




/**
 * @brief  DMA 传输完成时的中断
 * @param  TIM_HandleTypeDef *htim
 * @retval None
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM5)
    {
        if (uiWavPlayIndex < uiWavDataLength && Music_Play_On)
            HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_4, uiPuleseBuf, DMA_BATCH);
        else
        {
            HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_4);
            if(Music_Play_On)
            {
                f_close(&wavFile);
                Music_Play_Start = 1;
                Music_Item_Current = (Music_Item_Current + 1) % LISTVIEW_GetNumRows(hListView);
                LISTVIEW_GetItemText(hListView, 0, Music_Item_Current, musicPath, 100);        // 下一首
            }
        }

        uiMusicCofficient = ((1 << uiWavSampleDepth) - 1) * uiMusicVolumeN / uiMusicVolumeD;
        for (uint32_t i = DMA_BATCH / 2; i < DMA_BATCH; i++)
            uiPuleseBuf[i] = autoReload * ucWavData[(uiWavPlayIndex++) % SD_READ_BATCH]/ uiMusicCofficient;
    }
}


/**
 * @brief  DMA 传输一半时的中断
 * @param  TIM_HandleTypeDef *htim
 * @retval None
 */
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM5)
    {
        for (uint32_t i = 0; i < DMA_BATCH / 2; i++)
            uiPuleseBuf[i] = autoReload * ucWavData[uiWavPlayIndex++  % SD_READ_BATCH ] / uiMusicCofficient;
    }
}



static void Storage_Thread_Read(FIL *fp, uint64_t offset, void *buff, uint32_t size)
{
    extern FIL *Storage_Read_pFile;
    extern uint8_t *Storage_Read_pBuffer;
    extern uint32_t Storage_Read_uiSize;
    extern uint32_t Storage_Read_uiNum;
    extern uint16_t Storage_Read_Request;

    Storage_Read_pBuffer = buff;
    Storage_Read_pFile = fp;
    f_lseek(fp, ulWavPcmStart + offset);
    Storage_Read_uiSize = size;
    Storage_Read_Request = 1;
}