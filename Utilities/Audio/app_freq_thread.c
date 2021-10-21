/**
 * @Brief   用于显示麦克风频谱的线程
 * @Dependency  ADC3 IN0，TIM2 CH2，DMA2 stream 1,
 */

/* Includes ------------------------------------------------------------------*/
#include "DIALOG.h"
#include "main.h"
#include <math.h>

/* Defines ------------------------------------------------------------------*/

/* Functions prototypes ---------------------------------------------*/
static void Freq_FFT_Calculation();
static void Freq_MainFreq_Measurement();

/* Static data -----------------------------------------------------------*/
extern uint16_t audio_record_buffer[1024]; // ADC 采集的语音原始数据
static float32_t audioDataQuart[512];      // 用于频谱显示的语音原始数据
float32_t fftResultMag[256];               // 频谱显示 FFT 结果

static float32_t audioDataFull[2048];   // 用于主频率测量的语音原始数据
static float32_t fftResultMagFull[512]; // 测量主频率 FFT 结果
uint16_t Freq_MainFreq_Index;           // 测量主频率的结果     index of 1024

/* Private variables -----------------------------------------------------------*/
extern uint16_t Audio_DMA_Ready;  // DMA 就绪标志
uint16_t Freq_FFT_Ready = 0;      // 用于频谱显示的 FFT 数据就绪标志
uint16_t Freq_MainFreq_Ready = 0; // 主频率测量就绪标志
uint16_t Freq_FFT_Points = 128;   // FFT 点数

/* Function implementations -----------------------------------------------------------*/
#ifdef CMSIS_V1
// osThreadId app_freqAnalysisTaskHandle;
TaskHandle_t app_freqAnalysisTaskHandle;

static void FreqAnalysisThread(void *argument);
/**
 * @brief  Frequency Analysis Thread 的包装函数，用于 osThreadNew()
 * @param  void *argument
 * @retval None
 */
void app_freqAnalysis_thread(void *argument)
{
    FreqAnalysisThread(argument);
}

/**
 * @brief  Frequency Analysis Thread
 * @param  void* argument : pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void FreqAnalysisThread(void *argument)
{
    extern ADC_HandleTypeDef hadc3;
    extern TIM_HandleTypeDef htim2;
    extern GUI_HWIN hCurrentWindow;
    extern GUI_HWIN hFreqAnalysisWindow;

    __HAL_TIM_SET_AUTORELOAD(&htim2, 27-1);
    __HAL_TIM_SetCounter(&htim2, 0);           // Fs = 8K Hz
    HAL_ADC_Start_IT(&hadc3);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)audio_record_buffer, 1024);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

    for (;;)
    {
        if (Audio_DMA_Ready) // ADC 采样的 DMA 数据传送完成
        {
            Audio_DMA_Ready = 0;
            Freq_FFT_Calculation();
            Freq_MainFreq_Measurement();
        }

        if (hCurrentWindow != hFreqAnalysisWindow)
        {
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
            HAL_ADC_Stop_DMA(&hadc3);
            HAL_ADC_Stop_IT(&hadc3);
            vTaskDelete(app_freqAnalysisTaskHandle);
        }
        else
            osDelay(20);
    }
}
#endif

#ifdef CMSIS_V2
osThreadId_t app_freqAnalysisTaskHandle;
const osThreadAttr_t app_freqAnalysisTask_attributes = {
    .name = "Freq Analysis Task",
    .stack_size = 1024,
    .priority = (osPriority_t)osPriorityAboveNormal1,
};

static void FreqAnalysisThread(void *argument);
void app_freqAnalysis_thread(void *argument)
{
    FreqAnalysisThread(argument);
}

/**
 * @brief  Frequency Analysis Thread
 * @param  void* argument : pointer that is passed to the thread function as start argument.
 * @retval None
 */
static void FreqAnalysisThread(void *argument)
{
    extern ADC_HandleTypeDef hadc3;
    extern TIM_HandleTypeDef htim2;
    extern GUI_HWIN hCurrentWindow;
    extern GUI_HWIN hFreqAnalysisWindow;

    HAL_ADC_Start_IT(&hadc3);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)audio_record_buffer, 1024);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

    for (;;)
    {
        if (Audio_DMA_Ready) // ADC 采样的 DMA 数据传送完成
        {
            Audio_DMA_Ready = 0;
            Freq_FFT_Calculation();
            Freq_MainFreq_Measurement();
        }

        if (hCurrentWindow != hFreqAnalysisWindow)
        {
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
            HAL_ADC_Stop_DMA(&hadc3);
            HAL_ADC_Stop_IT(&hadc3);
            osThreadExit();
        }
        else
            osDelay(20);
    }
}
#endif

static uint32_t audio_offset = 2140; // 语音信号的基频幅度
/**
 * @brief  计算用于频谱显示的 FFT 数据
 * @note   在完成时设置 `Freq_MainFreq_Ready = 1`, 同时得到主频率测量结果 `Freq_MainFreq_Index`
 * @param  None
 * @retval None
 */
static void Freq_FFT_Calculation()
{
    static uint32_t sum;
    static uint16_t i;
    extern float32_t hamming256[256];
    extern float32_t hamming128[128];
    extern float32_t hamming64[64];

    // 计算用于频谱显示的数据
    switch (Freq_FFT_Points)
    {
    case 128:
        sum = 0;
        for (i = 0; i < 256; i++)
        {
            sum += audio_record_buffer[i];

            audioDataQuart[i * 2] = ((int)audio_record_buffer[i] - (int)audio_offset) * hamming256[i];
            audioDataQuart[i * 2 + 1] = 0;
        }
        audio_offset = (audio_offset + sum / 256) >> 1;

        arm_cfft_f32(&arm_cfft_sR_f32_len256, audioDataQuart, 0, 1);
        arm_cmplx_mag_f32(audioDataQuart, fftResultMag, 128);
        break;
    case 64:
        sum = 0;
        for (i = 0; i < 128; i++)
        {
            sum += audio_record_buffer[i];

            audioDataQuart[i * 2] = ((int)audio_record_buffer[i] - (int)audio_offset) * hamming128[i];
            audioDataQuart[i * 2 + 1] = 0;
        }
        audio_offset = (audio_offset + sum / 128) >> 1;

        arm_cfft_f32(&arm_cfft_sR_f32_len128, audioDataQuart, 0, 1);
        arm_cmplx_mag_f32(audioDataQuart, fftResultMag, 64);
        break;
    case 32:
        sum = 0;
        for (i = 0; i < 64; i++)
        {
            sum += audio_record_buffer[i];

            audioDataQuart[i * 2] = ((int)audio_record_buffer[i] - (int)audio_offset) * hamming64[i];
            audioDataQuart[i * 2 + 1] = 0;
        }
        audio_offset = (audio_offset + sum / 64) >> 1;

        arm_cfft_f32(&arm_cfft_sR_f32_len64, audioDataQuart, 0, 1);
        arm_cmplx_mag_f32(audioDataQuart, fftResultMag, 32);
        break;
    }
    fftResultMag[0] /= 2;
    // for (int i = 0; i < NPT; i++)
    //     fftResultMag[i] = fftResultMag[i] / NPT * 2;
    Freq_FFT_Ready = 1;
}

#define MAIN_FREQ_UPCOUNTER 10
/**
 * @brief  测量主频率
 * @note   在完成时设置 `Freq_MainFreq_Ready = 1`, 同时得到主频率测量结果 `Freq_MainFreq_Index`
 * @param  None
 * @retval None
 */
static void Freq_MainFreq_Measurement()
{
    static uint16_t counter = 0;
    static uint16_t i;
    static float32_t maxValue;

    counter++;
    if (counter == MAIN_FREQ_UPCOUNTER)
    {
        for (i = 0; i < 1024; i++)
        {
            audioDataFull[i * 2] = ((int)audio_record_buffer[i] - (int)audio_offset);
            audioDataFull[i * 2 + 1] = 0;
        }

        arm_cfft_f32(&arm_cfft_sR_f32_len1024, audioDataFull, 0, 1);
        arm_cmplx_mag_f32(audioDataFull, fftResultMagFull, 512);
        fftResultMagFull[0] /= 2;

        maxValue = fftResultMagFull[Freq_MainFreq_Index = 0];
        for (i = 0; i < 512; i++)
        {
            if (fftResultMagFull[i] > maxValue)
            {
                maxValue = fftResultMagFull[i];
                Freq_MainFreq_Index = i;
            }
        }

        Freq_MainFreq_Ready = 1; // 设置主频率测量完成标志
        counter = 0;             // counter 清零
    }
}