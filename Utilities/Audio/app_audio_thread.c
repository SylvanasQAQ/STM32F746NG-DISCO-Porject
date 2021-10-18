/**
 * @Brief   用于显示麦克风语音波形的线程
 * @Dependency  ADC3 IN0，TIM2 CH2，DMA2 stream 1
 */


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "DIALOG.h"
#include <math.h>


/* Functions prototypes ---------------------------------------------*/
static void AudioThread(void *argument);




/* Static data -----------------------------------------------------------*/
uint16_t audio_record_buffer[1024];




/* Private variables -----------------------------------------------------------*/
extern GRAPH_DATA_Handle hGraphData_AudioWindow;      // AudioWindow 绑定的 GraphData
extern uint16_t Audio_DMA_Ready;                        // DMA 就绪标志

// audio 线程的 Handle
osThreadId_t app_audioTaskHandle;
const osThreadAttr_t app_audioTask_attributes = {
    .name = "Audio Task",
    .stack_size = 1024,
    .priority = (osPriority_t)osPriorityAboveNormal,
};





/* Function implementations -----------------------------------------------------------*/
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

    static uint32_t sum, ave, tmp;
    static uint32_t offset = 2140;


    HAL_ADC_Start_IT(&hadc3);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)audio_record_buffer, 1024);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);


    for(;;)
    {   
        if(Audio_DMA_Ready)
        {
            sum = 0;
            for(int i = 0; i < 1024; i += 32)
            {
                for(int j = i; j < i+32; j++)
                    ave += audio_record_buffer[j];
                ave /= 32;

                // 剔除坏值
                if((tmp = (ave + 100 - offset) * 20 / 30 + 100 /3) < 3000){
                    GRAPH_DATA_YT_AddValue(hGraphData_AudioWindow, tmp);
                }
                sum += ave; 
            }
            offset = (offset + sum / 32) / 2;
            Audio_DMA_Ready = 0;
        }
        if(hCurrentWindow != hAudioWindow){
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
            HAL_ADC_Stop_DMA(&hadc3);
            HAL_ADC_Stop_IT(&hadc3);
            osThreadExit();
        }
        else
            osDelay(20);
    }
}