/**
 * @Brief   用于显示麦克风频谱的线程
 * @Dependency  ADC3 IN0，TIM2 CH2，DMA2 stream 1
 */

/* Includes ------------------------------------------------------------------*/
#include "DIALOG.h"
#include "main.h"
#include <math.h>

/* Functions prototypes ---------------------------------------------*/
static void FreqAnalysisThread(void *argument);




/* Static data -----------------------------------------------------------*/
float32_t fftResult[1024];
float32_t audioData[1024];
extern uint16_t audio_record_buffer[1024];




/* Private variables -----------------------------------------------------------*/
// 频谱显示样式，0 -- 幅度谱， 1 -- 频率谱
uint16_t spectrumForm = 0;
extern uint16_t Audio_DMA_Ready; // DMA 就绪标志

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



/**
 * @brief  频谱显示
 * @param  uint16_t x   频谱x坐标
 * @param  uint16_t y   频谱y坐标
 * @retval None
 */
static void DisWave(uint16_t x, uint16_t y)
{
    static uint16_t topValueArr[32] = {0}; /* 频谱顶值表 */
    static uint16_t curValueArr[32] = {0}; /* 频谱当前值表 */
    static uint8_t timeArr[32] = {0};      /* 顶值停留时间表 */
    const uint16_t maxVal = 128;           /* 高度固定为128个像素 */
    uint16_t i;
    uint16_t temp;
    float ufTempValue;

    /* 显示32条频谱 */
    for (i = 0; i < 32; i++)
    {
        if (spectrumForm == 1)
        {
            /* 对数谱，对幅值取对数 */
            ufTempValue = fftResult[i] / 128;
            if (ufTempValue < 1)
            {
                ufTempValue = 0;
            }
            temp = 80 * log10(ufTempValue);
        }
        else
        {
            /*
               1. 幅值谱
               2. 将得到的FFT模值除以32，方便界面显示
            */
            temp = fftResult[i] / 32;
        }

        /* 2. 更新频谱数值 */
        if (curValueArr[i] < temp)
        {
            curValueArr[i] = temp;
        }
        else
        {
            if (curValueArr[i] > 1)
            {
                curValueArr[i] -= 2;
            }
            else
            {
                curValueArr[i] = 0;
            }
        }

        /* 3. 更新频谱顶值 */
        if (timeArr[i])
        {
            timeArr[i]--;
        }
        else
        {
            /* 峰值减小1 */
            if (topValueArr[i])
            {
                topValueArr[i]--;
            }
        }

        /* 4. 重设频谱顶值 */
        if (curValueArr[i] > topValueArr[i])
        {
            topValueArr[i] = curValueArr[i];

            /* 重设峰值停顿时间 */
            timeArr[i] = 10;
        }

        /* 5. 防止超出频谱值和顶值范围，高度固定为128个像素 */
        if (curValueArr[i] > maxVal)
            curValueArr[i] = maxVal;

        if (topValueArr[i] > maxVal)
            topValueArr[i] = maxVal;
    }

    /* 6. 绘制得到的频谱 */
    for (i = 0; i < 32; i++)
    {
        /* 显示频谱 */
        GUI_DrawGradientV(x,
                          y + maxVal - curValueArr[i],
                          x + 10,
                          y + maxVal,
                          GUI_YELLOW,
                          GUI_GREEN);

        /* 显示顶值 */
        GUI_SetColor(GUI_RED);
        GUI_DrawHLine(y + maxVal - topValueArr[i] - 1, x, x + 10);
        x += 15;
    }
}