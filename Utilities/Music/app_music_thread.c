/**
 * @Brief   用于音乐播放的线程
 * @Dependency
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_wavDecoder.h"
#include "fatfs.h"


/* Defines ------------------------------------------------------------------*/






/* Functions prototypes ---------------------------------------------*/
void PlayMusic(void);



/* Private variables -----------------------------------------------------------*/




/* Function implementations -----------------------------------------------------------*/
#ifdef CMSIS_V1
TaskHandle_t app_musicTaskHandle;
#endif

#ifdef CMSIS_V2
osThreadId_t storageTaskHandle;
const osThreadAttr_t storageTask_attributes = {
    .name = "Storage Task",
    .stack_size = 1024,
    .priority = (osPriority_t)osPriorityLow,
};
#endif