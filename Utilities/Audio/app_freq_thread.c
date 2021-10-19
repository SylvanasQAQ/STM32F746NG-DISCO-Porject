/**
 * @Brief   用于显示麦克风频谱的线程
 * @Dependency  ADC3 IN0，TIM2 CH2，DMA2 stream 1
 */

/* Includes ------------------------------------------------------------------*/
#include "DIALOG.h"
#include "main.h"
#include <math.h>


/* Defines ------------------------------------------------------------------*/
#define NPT 256


/* Functions prototypes ---------------------------------------------*/
static void FreqAnalysisThread(void *argument);




/* Static data -----------------------------------------------------------*/
float32_t fftResultMag[1024];
float32_t audioData[2048];
extern uint16_t audio_record_buffer[1024];




/* Private variables -----------------------------------------------------------*/

extern uint16_t Audio_DMA_Ready; // DMA 就绪标志
uint16_t Freq_FFT_Ready;       // FFT 数据就绪标志



// audio 线程的 Handle
osThreadId_t app_freqAnalysisTaskHandle;
const osThreadAttr_t app_freqAnalysisTask_attributes = {
    .name = "Freq Analysis Task",
    .stack_size = 1024,
    .priority = (osPriority_t)osPriorityAboveNormal,
};




/* Function implementations -----------------------------------------------------------*/
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
uint32_t offset = 2140;
    uint32_t sum;
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
        if (Audio_DMA_Ready)
        {
            Audio_DMA_Ready = 0;
            
            sum = 0;
            for(int i = 0; i < 1024; i++)
            {
                sum += audio_record_buffer[i];
                audioData[i*2] = ((int)audio_record_buffer[i] - (int)offset);
                audioData[i*2 + 1] = 0;
            }
            offset = (offset + sum/1024) >> 1;

            arm_cfft_f32(&arm_cfft_sR_f32_len256, audioData, 0, 1);
            arm_cmplx_mag_f32(audioData, fftResultMag, NPT);

            arm_cfft_f32(&arm_cfft_sR_f32_len512, (float32_t*)audioData+1024, 0, 1);
            arm_cmplx_mag_f32(audioData+1024, (float32_t*)fftResultMag+512, 512);
            // for (int i = 0; i < NPT; i++)
            //     fftResultMag[i] = fftResultMag[i] / NPT * 2;

            Freq_FFT_Ready = 1;
        }

        if(hCurrentWindow != hFreqAnalysisWindow){
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
            HAL_ADC_Stop_DMA(&hadc3);
            HAL_ADC_Stop_IT(&hadc3);
            osThreadExit();
        }
        else
            osDelay(20);
    }
}
