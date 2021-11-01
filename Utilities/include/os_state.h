#ifndef __OS_STATE_H
#define __OS_STATE_H

#include "main.h"
//#include "cmsis_os2.h"
//#include "task.h"

#define CALCULATION_PERIOD    1000

/* Exported functions ------------------------------------------------------- */
uint16_t osGetCPUUsage (void);
uint16_t osGetMemUsage(void);


struct os_reboot_info
{
  uint64_t magic;
  uint32_t uiWavPlayIndex;
  char     musicPath[100];
  U16      Music_Item_Current;
  U16      Music_Play_Restart;
  uint16_t musicListLen;
  char     musicPathList[100][128];
  char     musicDurationList[100][16];
};

typedef struct os_reboot_info os_reboot_info_t;

#endif