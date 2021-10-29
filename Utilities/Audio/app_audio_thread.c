/**
 * @Brief   用于显示麦克风语音波形的线程
 * @Dependency  ADC3 IN0，TIM2 CH2，DMA2 stream 1
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "DIALOG.h"
#include <math.h>
#include "arm_math.h"
#include "dialog_window.h"


/* Private defines -----------------------------------------------------------*/
#define TIM_CLOCK                   108000000           // APB1 的频率，即 TIM 的时钟频率
#define SAMPLING_FREQUENCY          16000                // ADC 采样频率
#define TIM2_PRESCALER              100                 // TIM2 的预分频系数
#define TIM2_AUTORELOAD             (TIM_CLOCK / TIM2_PRESCALER / SAMPLING_FREQUENCY - 1)       // 在👆上述条件下的 TIM2 autoreload

#define TIM5_PRESCALER              108         // TIM5 的预分频系数

#define AUDIO_FULL_RECORD_FREQUENCY 40000               // 全音调提取时ADC的采样频率
#define TIM2_FULL_RECORD_AUTORELOAD (TIM_CLOCK / TIM2_PRESCALER / AUDIO_FULL_RECORD_FREQUENCY - 1)
#define TIM5_FULL_RECORD_PRESCALER  1                   // 全音调提取时 TIM5 的预分频系数
#define TIM5_DMA_BATCH              1024


#define ADC_BATCH_POINTS            1024        // 一次 DMA 传送 ADC 采集点数
#define AUDIO_LENGTH                20          // 录音长度，单位：秒 s
#define THREAD_SLEEP_TIME           2           // 线程休眠时间，单位：毫秒 ms






/* Functions prototypes ---------------------------------------------*/
static void AudioThread(void *argument);
static void HardwareInit();
static void AudioSingalProcess();
static void AudioSingalExtract();
static void AudioSingalReplay();




/* Static data -----------------------------------------------------------*/
uint16_t audio_record_buffer[1024];             // ADC 原始音频数据

float32_t audio_fft_data[2048];                 // FFT 原始数据
float32_t audio_fft_mag[512];                   // FFT 频率幅值

uint16_t audio_main_freq[1536];                  // 提取到的音调
uint16_t audio_main_freq_index = 0;            // 当前记录位置 index

uint16_t Audio_Full_Record = 0;
uint16_t  *audio_record_buffer_sdram = (uint16_t*)(AUDIO_RECORD_BUFFER);              // 存储在 sdram 中的语音波形
float32_t *audio_fir_buffer = (float32_t*)(AUDIO_RECORD_BUFFER + 0x1E0000);
float32_t *audio_fir_state_buffer = (float32_t*)(AUDIO_RECORD_BUFFER + 0x1E5000);   
uint32_t  audio_record_buffer_index = 0;
uint32_t  audio_record_buffer_len = 0;
extern uint32_t  uiPuleseBuf[];         // TIM5 CH4 的 PWM 占空比信息
extern uint32_t  uiAutoReload_TIM5; 
extern float32_t fir_filter[];
static arm_fir_instance_f32 S;
uint16_t    firBlockSize = 1024;

// 一些音符的频率
uint16_t music_frequencies[] = {65, 69, 73, 78, 82, 87, 92, 98, 103, 116, 123, 
                            131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,
                            262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
                            523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
                            1046, 1109, 1175, 1244, 1318, 1397, 1480, 1568, 1661, 1760,
                            1865, 1926, 2089, 2160, 2288, 2422, 2565, 2716, 2877, 3047,
                            3226, 3417, 3618, 3832};




/* Private variables -----------------------------------------------------------*/
extern GRAPH_DATA_Handle hGraphData_AudioWindow;      // AudioWindow 绑定的 GraphData

extern uint16_t Audio_DMA_Ready;                        // 采集的音频数据 DMA 就绪标志
uint16_t Audio_Record_OnOff = 0;            // 音调提取开启标志
uint16_t Audio_Record_Ready = 0;            // 音调提取完成标志
uint16_t Audio_Record_Replay = 0;           // 音调重放标志
uint16_t Audio_Thread_Exist = 0;
uint16_t Audio_Thread_Terminate = 0;


extern ADC_HandleTypeDef hadc3;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;

extern U16               Music_Play_On;               // 音乐播放中标志



/* Function implementations -----------------------------------------------------------*/
#ifdef CMSIS_V1
TaskHandle_t app_audioTaskHandle;
#endif


#ifdef CMSIS_V2
osThreadId_t app_audioTaskHandle;
const osThreadAttr_t app_audioTask_attributes = {
    .name = "Audio Task",
    .stack_size = 1024,
    .priority = (osPriority_t)osPriorityAboveNormal,
};
#endif



/**
  * @brief  用于新建一个 Audio Task 的线程\任务
  * @param  None
  * @retval None
  */
void vAudioTaskCreate()
{
    #ifdef CMSIS_V1
    xTaskCreate(AudioThread, "Audio Task", 256, NULL, osPriorityNormal, &app_audioTaskHandle);
    #endif

    #ifdef CMSIS_V2
    app_audioTaskHandle = osThreadNew(AudioThread, NULL, &app_audioTask_attributes);
    #endif
}




/**
 * @brief  Audio thread
 * @param  void* argument : pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void AudioThread(void *argument)
{
    extern GUI_HWIN          hCurrentWindow;
    extern GUI_HWIN          hAudioWindow;

    Audio_Thread_Exist = 1;

    HardwareInit();

    for(;;)
    {   
        if(Audio_DMA_Ready)
        {
            Audio_DMA_Ready = 0;
            AudioSingalProcess();           // 处理 ADC 采样到的数据
            AudioSingalExtract();           // 提取主音调
        }

        AudioSingalReplay();                                // 播放采集到的音调


        if(hCurrentWindow != hAudioWindow || Audio_Thread_Terminate){
            Audio_Thread_Terminate = 0;

            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);        // 退出界面时停止 ADC 采集
            HAL_ADC_Stop_DMA(&hadc3);
            HAL_ADC_Stop_IT(&hadc3);

            if(Audio_Record_Replay == 0)             // 没有音乐在播放，结束线程
            {
                Audio_Thread_Exist = 0;

                Audio_Record_OnOff = 0;
                audio_main_freq_index = 0;
                Audio_Record_Ready = 0;
                audio_record_buffer_index = 0;
                audio_record_buffer_len = 0;
#ifdef CMSIS_V1
                vTaskDelete(app_audioTaskHandle);
#endif
#ifdef CMSIS_V2
                osThreadTerminate(app_audioTaskHandle);
#endif
            }
        }

        osDelay(THREAD_SLEEP_TIME);
    }
}




/**
 * @brief  初始化需要的硬件
 * @param  None
 * @retval None
 */
static void HardwareInit()
{
    /**
     * 开启 ADC3 IN0 的准备
     *  1. 设置 ADC3 采集的触发信号 TIM2
     *  2. 开启 ADC3 采集完成中断 (DMA 方式)
     */
    if(Audio_Full_Record)
        __HAL_TIM_SetAutoreload(&htim2, TIM2_FULL_RECORD_AUTORELOAD);       // Fs = 40K Hz  
    else
        __HAL_TIM_SetAutoreload(&htim2, TIM2_AUTORELOAD);                   // Fs = 8K Hz
    __HAL_TIM_SET_PRESCALER(&htim2, TIM2_PRESCALER);
    __HAL_TIM_SetCounter(&htim2, 0);

    HAL_ADC_Start_IT(&hadc3);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)audio_record_buffer, 1024);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    

    /**
     *  1. 如果后台有音乐播放则停止
     */
    if(Music_Play_On)
    {
        Music_Play_On = 0;                          // 关闭在播放的音乐
        HAL_TIM_PWM_Stop_DMA(&htim5, TIM_CHANNEL_4);
    }

    arm_fir_init_f32(&S, 62, fir_filter, audio_fir_state_buffer, firBlockSize);
}







#define SCREEN_OFFSET       90
/**
 * @brief  对 ADC 采样的信号进行处理以便于显示
 * @param  None
 * @retval None
 */
static void AudioSingalProcess()
{
    static long sum, ave;
    static uint16_t i, j;
    static long audio_offset = 2140;
    static signed short tmp, minTmp;
    
    

    sum = 0;
    minTmp = 150;
    for(i = 0; i < 1024; i += 32)
    {
        if(Audio_Record_OnOff == 0)                 // 音调提取关闭
            for(int j = i; j < i+32; j++)
                ave += audio_record_buffer[j];
        else
        {
            if(Audio_Full_Record)                       // 录音
                for(j = i; j < i + 32; j++){
                    ave += audio_record_buffer[j];
                    tmp = ((int)audio_record_buffer[j] - audio_offset)*10;
                    //audio_record_buffer_sdram[audio_record_buffer_len++] = tmp*50;
                    audio_fft_data[j] = tmp;
                }
            else
                for(int j = i; j < i+32; j++)           // 音调提取开启
                {
                    ave += audio_record_buffer[j];

                    audio_fft_data[j*2] = (int)audio_record_buffer[j] - audio_offset;
                    audio_fft_data[j*2 + 1] = 0;
                }
        }
            
            
        ave /= 32;
        

        // 剔除坏值
        tmp = (ave + SCREEN_OFFSET - audio_offset) ;
        if((tmp > 0)){
            GRAPH_DATA_YT_AddValue(hGraphData_AudioWindow, tmp);
            if(tmp < minTmp)
                minTmp = tmp;
        }
        else
            GRAPH_DATA_YT_AddValue(hGraphData_AudioWindow, minTmp);
        sum += ave;
    }
    audio_offset = (audio_offset + sum / 32) / 2;
}



uint16_t _abs(int a)
{
    return a > 0 ? a : -a;
}
/**
 * @brief  对 ADC 采样的信号进行音调提取
 * @param  None
 * @retval None
 */
static void AudioSingalExtract()
{
    static float32_t maxValue;
    static uint16_t i, index, usMainFreq;
    static uint16_t low, high, mid;
    static int a, b, c;
    

    // 寻找主频率
    if(Audio_Record_OnOff && Audio_Full_Record == 0)
    {
        arm_cfft_f32(&arm_cfft_sR_f32_len1024, audio_fft_data, 0, 1);
        arm_cmplx_mag_f32(audio_fft_data, audio_fft_mag, 512);
        audio_fft_mag[0] /= 2;

        maxValue = audio_fft_mag[index = 0];
        for (i = 0; i < 512; i++)
        {
            if (audio_fft_mag[i] > maxValue)
            {
                maxValue = audio_fft_mag[i];
                index = i;
            }
        }
        usMainFreq = SAMPLING_FREQUENCY * index / 1024;


        // 二分查找寻找最接近的频率值
        low = 0, high = 71;
        while(low < high)
        {
            mid = (low + high) >> 1;
            if(usMainFreq <= music_frequencies[mid])
                high = mid;
            else
                low = mid+1;
        }


        if(low > 0 && low < 70)
        {
            a = _abs((int)usMainFreq - music_frequencies[low]);
            b = _abs((int)usMainFreq - music_frequencies[low-1]);
            c = _abs((int)usMainFreq - music_frequencies[low-2]);
            if(a <= b && a <= c)
                usMainFreq = music_frequencies[low];
            else if(b <= a && b <= c)
                usMainFreq = music_frequencies[low-1];
            else
                usMainFreq = music_frequencies[low-2];
        }

        // 将提取到的频率写入到数组
        audio_main_freq[audio_main_freq_index++] = usMainFreq;

        if(audio_main_freq_index > SAMPLING_FREQUENCY / ADC_BATCH_POINTS * AUDIO_LENGTH)             // 只提取 20s
        {
            Audio_Record_OnOff = 0;
            Audio_Record_Ready = 1;
            CreateAlarmDialog_Self("Notice", "Main frequency capture\n completes!", DIALOG_NOTHING);

            __HAL_TIM_SET_PRESCALER(&htim5, TIM5_PRESCALER - 1); // 音调提取
            __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, 0);
            HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4);
        }
    }
    else if(Audio_Record_OnOff && Audio_Full_Record == 1)
    {
        for (int i = 0; i < 1; i++)
            arm_fir_f32(&S, audio_fft_data + (i * firBlockSize), audio_fir_buffer + (i * firBlockSize), firBlockSize);
        for (int i = 0; i < 1024; i++)
            audio_record_buffer_sdram[audio_record_buffer_len++] = _abs(audio_fir_buffer[i] + 1200)*5;

        if(audio_record_buffer_len > AUDIO_FULL_RECORD_FREQUENCY * AUDIO_LENGTH)
        {
            Audio_Record_OnOff = 0;
            Audio_Record_Ready = 1;
            audio_record_buffer_index = 0;
            CreateAlarmDialog_Self("Notice", "Main frequency capture\n completes!", DIALOG_NOTHING);

            __HAL_TIM_SET_PRESCALER(&htim5, TIM5_FULL_RECORD_PRESCALER - 1);
        }
    }
}




/**
 * @brief  对提取到的音调进行播放
 * @param  None
 * @retval None
 */
static void AudioSingalReplay()
{
    static uint16_t             i = 0;
    static uint16_t             autoReload, timeElapsed;
    static uint16_t             tim5_ch4_on = 0;


    if(Audio_Record_Replay)
    {
        if(Audio_Full_Record == 0)
        {
            timeElapsed += THREAD_SLEEP_TIME;
            if (timeElapsed >= 1000 / (SAMPLING_FREQUENCY / ADC_BATCH_POINTS))
            {
                if (audio_main_freq[i] == 0)
                    __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 0);
                else if (i > 0 && audio_main_freq[i] != audio_main_freq[i - 1])
                {
                    autoReload = TIM_CLOCK / TIM5_PRESCALER / audio_main_freq[i];
                    __HAL_TIM_SetAutoreload(&htim5, autoReload);
                    __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 10);
                    __HAL_TIM_SetCounter(&htim5, 0);
                }
                i++;

                if (i >= audio_main_freq_index)
                {
                    Audio_Record_Replay = 0;
                    i = 0;
                    __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 0);
                }
                timeElapsed = 0;
            }
        }
        else if(Audio_Full_Record == 1)
        {
            if(tim5_ch4_on == 0)
            {
                tim5_ch4_on = 1;

                uiAutoReload_TIM5 = TIM_CLOCK / AUDIO_FULL_RECORD_FREQUENCY / TIM5_FULL_RECORD_PRESCALER;
                __HAL_TIM_SET_AUTORELOAD(&htim5, uiAutoReload_TIM5);
                for (uint32_t i = 0; i < TIM5_DMA_BATCH; i++)
                    uiPuleseBuf[i] = uiAutoReload_TIM5 * audio_record_buffer_sdram[audio_record_buffer_index++] / 0xfff;

                HAL_TIM_PWM_Stop_DMA(&htim5, TIM_CHANNEL_4);
                HAL_TIM_PWM_Start_DMA(&htim5, TIM_CHANNEL_4, uiPuleseBuf, TIM5_DMA_BATCH);       // 启动 PWM
            }

            if (audio_record_buffer_index > audio_record_buffer_len)
            {
                tim5_ch4_on= 0;
                audio_record_buffer_index = 0;
                Audio_Record_Replay = 0;

                HAL_TIM_PWM_Stop_DMA(&htim5, TIM_CHANNEL_4);
            }
        }
    }
}