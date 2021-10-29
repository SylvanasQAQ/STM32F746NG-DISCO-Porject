/**
 * @Brief   用于显示麦克风频谱的线程
 * @Dependency  ADC3 IN0，TIM2 CH2，DMA2 stream 1,
 */

/* Includes ------------------------------------------------------------------*/
#include "DIALOG.h"
#include "main.h"



/* Defines ------------------------------------------------------------------*/
#define TIM_CLOCK                   108000000           // APB1 的频率，即 TIM 的时钟频率
#define SAMPLING_FREQUENCY          40000                // ADC 采样频率
#define TIM2_PRESCALER              100                 // TIM2 的预分频系数
#define TIM2_AUTORELOAD             (TIM_CLOCK / TIM2_PRESCALER / SAMPLING_FREQUENCY - 1)       // 在👆上述条件下的 TIM2 autoreload



/* Functions prototypes ---------------------------------------------*/
static void Freq_FFT_Calculation();
static void Freq_MainFreq_Measurement();
static void FreqAnalysisThread(void *argument);


/* Static data -----------------------------------------------------------*/
extern uint16_t audio_record_buffer[1024]; // ADC 采集的语音原始数据
static float32_t audioDataQuart[512];      // 用于频谱显示的语音原始数据
float32_t fftResultMag[256];               // 用于频谱显示的 FFT 频率幅值

extern float32_t audio_fft_data[2048];      // 用于主频率测量的 FFT 原始数据
extern float32_t audio_fft_mag[512];        // 用于主频率测量的 FFT 频率幅值
uint16_t Freq_MainFreq_Index;               // 测量主频率的结果     index of 1024

/* Private variables -----------------------------------------------------------*/
extern uint16_t Audio_DMA_Ready;  // DMA 就绪标志
uint16_t Freq_FFT_Ready = 0;      // 用于频谱显示的 FFT 数据就绪标志
uint16_t Freq_MainFreq_Ready = 0; // 主频率测量就绪标志
uint16_t Freq_FFT_Points = 128;   // FFT 点数


/* Function implementations -----------------------------------------------------------*/
#ifdef CMSIS_V1
TaskHandle_t app_freqAnalysisTaskHandle;
#endif


#ifdef CMSIS_V2
osThreadId_t app_freqAnalysisTaskHandle;
const osThreadAttr_t app_freqAnalysisTask_attributes = {
    .name = "Freq Analysis Task",
    .stack_size = 1024,
    .priority = (osPriority_t)osPriorityAboveNormal1,
};
#endif




/**
  * @brief  用于新建一个 Freq Analysis Task 的线程\任务
  * @param  None
  * @retval None
  */
void vFreqAnalysisTaskCreate()
{
    #ifdef CMSIS_V1
    xTaskCreate(FreqAnalysisThread, "FreqAnalysis Task", 256, NULL, osPriorityNormal, &app_freqAnalysisTaskHandle);
    #endif

    #ifdef CMSIS_V2
    app_freqAnalysisTaskHandle = osThreadNew(FreqAnalysisThread, NULL, &app_audioTask_attributes);
    #endif
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

    __HAL_TIM_SET_PRESCALER(&htim2, TIM2_PRESCALER);
    __HAL_TIM_SET_AUTORELOAD(&htim2, TIM2_AUTORELOAD);
    __HAL_TIM_SetCounter(&htim2, 0);          
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
#ifdef  CMSIS_V1
            vTaskDelete(app_freqAnalysisTaskHandle);
#endif

#ifdef  CMSIS_V2
            osThreadExit();
#endif
        }
        else
            osDelay(20);
    }
}






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
    Freq_FFT_Ready = 1;
}




#define MAIN_FREQ_UPCOUNTER 10              // 每 10 次 DMA 采集完成测量一次主频率
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
            audio_fft_data[i * 2] = ((int)audio_record_buffer[i] - (int)audio_offset);
            audio_fft_data[i * 2 + 1] = 0;
        }

        arm_cfft_f32(&arm_cfft_sR_f32_len1024, audio_fft_data, 0, 1);
        arm_cmplx_mag_f32(audio_fft_data, audio_fft_mag, 512);
        audio_fft_mag[0] /= 2;

        maxValue = audio_fft_mag[Freq_MainFreq_Index = 0];
        for (i = 0; i < 512; i++)
        {
            if (audio_fft_mag[i] > maxValue)
            {
                maxValue = audio_fft_mag[i];
                Freq_MainFreq_Index = i;
            }
        }

        Freq_MainFreq_Ready = 1; // 设置主频率测量完成标志
        counter = 0;             // counter 清零
    }
}