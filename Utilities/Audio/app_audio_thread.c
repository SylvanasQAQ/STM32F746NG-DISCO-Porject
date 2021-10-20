/**
 * @Brief   用于显示麦克风语音波形的线程
 * @Dependency  ADC3 IN0，TIM2 CH2，DMA2 stream 1
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "DIALOG.h"
#include <math.h>
#include "arm_math.h"


/* Functions prototypes ---------------------------------------------*/
static void AudioSingalProcess();
static void AudioSingalExtract();
static void AudioSingalReplay();




/* Static data -----------------------------------------------------------*/
uint16_t audio_record_buffer[1024];             // ADC 原始音频数据

float32_t audio_fft_data[2048];                 // FFT 原始数据
float32_t audio_fft_mag[512];                   // FFT 频率幅值

uint16_t audio_main_freq[200];                  // 提取到的音调
uint16_t audio_main_freq_index = 0;            // 当前 index

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
extern uint16_t Audio_Record_OnOff;                 // 音调提取开启标志
extern uint16_t Audio_Record_Ready;                     // 音调提取完成标志
extern uint16_t Audio_Record_Replay;                // 音调重放标志






/* Function implementations -----------------------------------------------------------*/
#ifdef CMSIS_V1
//osThreadId app_audioTaskHandle;
TaskHandle_t app_audioTaskHandle;



static void AudioThread(void *argument);
/**
  * @brief  Audio Thread 的包装函数，用于 osThreadNew()
  * @param  void *argument
  * @retval None
  */
void app_audio_thread(void *argument)
{
    AudioThread(argument);
}



/**
 * @brief  Audio thread
 * @param  void* argument : pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void AudioThread(void *argument)
{
    extern ADC_HandleTypeDef hadc3;
    extern TIM_HandleTypeDef htim2;
    extern GUI_HWIN hCurrentWindow;
    extern GUI_HWIN hAudioWindow;

    __HAL_TIM_SetAutoreload(&htim2, 27*5-1);
    __HAL_TIM_SetCounter(&htim2, 0);           // Fs = 8K Hz
    HAL_ADC_Start_IT(&hadc3);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)audio_record_buffer, 1024);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);


    for(;;)
    {   
        if(Audio_DMA_Ready)
        {
            Audio_DMA_Ready = 0;
            AudioSingalProcess();
            AudioSingalExtract();
        }

        AudioSingalReplay();


        if(hCurrentWindow != hAudioWindow){
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
            HAL_ADC_Stop_DMA(&hadc3);
            HAL_ADC_Stop_IT(&hadc3);

            

            if(Audio_Record_Replay == 0)             // 没有音乐在播放，结束线程
            {
                Audio_Record_OnOff = 0;
                audio_main_freq_index = 0;
                Audio_Record_Ready = 0;
                vTaskDelete(app_audioTaskHandle);
            }
                
            osDelay(25);
        }
        else{
            osDelay(25);
        }
    }
}
#endif



#ifdef CMSIS_V2
osThreadId_t app_audioTaskHandle;
const osThreadAttr_t app_audioTask_attributes = {
    .name = "Audio Task",
    .stack_size = 1024,
    .priority = (osPriority_t)osPriorityAboveNormal,
};



static void AudioThread(void *argument);
void app_audio_thread(void *argument)
{
    AudioThread(argument);
}


/**
 * @brief  Audio thread
 * @param  void* argument : pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void AudioThread(void *argument)
{
    extern ADC_HandleTypeDef hadc3;
    extern TIM_HandleTypeDef htim2;
    extern GUI_HWIN hCurrentWindow;
    extern GUI_HWIN hAudioWindow;


    __HAL_TIM_SET_COUNTER(&htim2, 27*5-1);
    __HAL_TIM_SetCounter(&htim2, 0);           // Fs = 8K Hz
    HAL_ADC_Start_IT(&hadc3);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)audio_record_buffer, 1024);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);


    for(;;)
    {   
        if(Audio_DMA_Ready)
        {
            Audio_DMA_Ready = 0;
            AudioSingalProcess();
            AudioSingalExtract();
            AudioSingalReplay();
        }


        if(hCurrentWindow != hAudioWindow){
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
            HAL_ADC_Stop_DMA(&hadc3);
            HAL_ADC_Stop_IT(&hadc3);
            osThreadExit();
        }
        else
            osDelay(25);
    }
}
#endif






#define SCREEN_OFFSET       90
/**
 * @brief  对 ADC 采样的信号进行处理以便于显示
 * @param  None
 * @retval None
 */
static void AudioSingalProcess()
{
    static long sum, ave;
    static uint16_t i;
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
            for(int j = i; j < i+32; j++)           // 音调提取开启
            {
                ave += audio_record_buffer[j];

                audio_fft_data[j*2] = (int)audio_record_buffer[j] - audio_offset;
                audio_fft_data[j*2 + 1] = 0;
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



uint16_t ind = 0;
uint16_t freqArr[200];
inline uint16_t abs(uint16_t a)
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
    static uint16_t low, high, mid, a, b, c;

    // 寻找主频率
    if(Audio_Record_OnOff)
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
        usMainFreq = 8000 * index / 1024;
        freqArr[ind++] = usMainFreq;



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
            a = abs(usMainFreq - music_frequencies[low-1]);
            b = abs(usMainFreq - music_frequencies[low]);
            c = abs(usMainFreq - music_frequencies[low-2]);
            if(a <= b && a <= c)
                usMainFreq = music_frequencies[low-1];
            else if(b <= a && b <= c)
                usMainFreq = music_frequencies[low];
            else
                usMainFreq = music_frequencies[low-2];
        }

        // 将提取到的频率写入到数组
        audio_main_freq[audio_main_freq_index++] = usMainFreq;

        if(audio_main_freq_index > 160)             // 只提取 20s
        {
            Audio_Record_OnOff = 0;
            Audio_Record_Ready = 1;
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
    extern TIM_HandleTypeDef    htim5;


    if(Audio_Record_Replay)
    {
        timeElapsed += 25;
        if(timeElapsed >= 125)
        {
            // if(audio_main_freq[i] == 0)
            //     __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 0);
            // else
            {
                autoReload = 1000000 / audio_main_freq[i] - 1;
                __HAL_TIM_SetAutoreload(&htim5, autoReload);
                __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, autoReload /2);
                __HAL_TIM_SetCounter(&htim5, autoReload/3);
            }
            i++;

            if(i >= audio_main_freq_index)
            {
                Audio_Record_Replay = 0;
                i = 0;
                __HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_4, 0);
            }
            timeElapsed = 0;
        }
    }
}