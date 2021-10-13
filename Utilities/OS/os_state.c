#include "os_state.h"


/************************ block start ************************/
/**
 * @note CPU usage measurement
 * 
 * 
 * 
 * 
 */
xTaskHandle    xIdleHandle = NULL;
__IO uint32_t  osCPU_Usage = 0;
uint32_t       osCPU_IdleStartTime = 0; 
uint32_t       osCPU_IdleSpentTime = 0; 
uint32_t       osCPU_TotalIdleTime = 0; 

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Application Idle Hook
  * @param  None 
  * @retval None
  */
void vApplicationIdleHook(void) 
{
  if( xIdleHandle == NULL )
  {
    /* Store the handle to the idle task. */
    xIdleHandle = xTaskGetCurrentTaskHandle();
  }
}

/**
  * @brief  Application Idle Hook
  * @param  None 
  * @retval None
  */
void vApplicationTickHook (void)
{
  static int tick = 0;
  
  if(tick ++ > CALCULATION_PERIOD)
  {
    tick = 0;
    
    if(osCPU_TotalIdleTime > 1000)
    {
      osCPU_TotalIdleTime = 1000;
    }
    osCPU_Usage = (100 - (osCPU_TotalIdleTime * 100) / CALCULATION_PERIOD);
    osCPU_TotalIdleTime = 0;
  }
}

/**
  * @brief  Start Idle monitor
  * @param  None 
  * @retval None
  */
void StartIdleMonitor (void)
{
  if( xTaskGetCurrentTaskHandle() == xIdleHandle ) 
  {
    osCPU_IdleStartTime = xTaskGetTickCountFromISR();
  }
}

/**
  * @brief  Stop Idle monitor
  * @param  None 
  * @retval None
  */
void EndIdleMonitor (void)
{
  if( xTaskGetCurrentTaskHandle() == xIdleHandle )
  {
    /* Store the handle to the idle task. */
    osCPU_IdleSpentTime = xTaskGetTickCountFromISR() - osCPU_IdleStartTime;
    osCPU_TotalIdleTime += osCPU_IdleSpentTime; 
  }
}

/**
  * @brief  Stop Idle monitor
  * @param  None 
  * @retval None
  */
uint16_t osGetCPUUsage (void)
{
  return (uint16_t)osCPU_Usage;
}


/************************ block end ************************/






/************************ block start ************************/
/**
 * @note Memory usage measurement
 * 
 * 
 * 
 * 
 */
/************************ block end ************************/

size_t os_memory_total = configTOTAL_HEAP_SIZE;
size_t os_memory_free;

uint16_t osGetMemUsage(void)
{
    os_memory_free = xPortGetFreeHeapSize();
    return  100 - os_memory_free*100 / os_memory_total;
}












/************************ block start ************************/
/**
 * @note 
 * 
 * 
 * 
 * 
 */
/************************ block end ************************/