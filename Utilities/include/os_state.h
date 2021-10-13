#ifndef __OS_STATE_H
#define __OS_STATE_H

#include "main.h"
//#include "cmsis_os2.h"
//#include "task.h"

#define CALCULATION_PERIOD    1000

/* Exported functions ------------------------------------------------------- */
uint16_t osGetCPUUsage (void);
uint16_t osGetMemUsage(void);

#endif