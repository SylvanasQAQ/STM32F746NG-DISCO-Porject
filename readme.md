# 功能测试函数

## DSP 库测试
```c
#include "arm_math.h"
#include "arm_const_structs.h"


float32_t lBufInArray[256*2];
float32_t mag[256];
void testDSP()
{
  uint16_t NPT = 256;
  float32_t PI2 = 6.28318530;
  uint32_t Fs = 44800;
  unsigned short i;
  float32_t fx;
  for (i = 0; i < NPT; i++)
  {
    fx = 1500 * arm_sin_f32(PI2 * i * 350 / Fs) +
        2700 * arm_sin_f32(PI2 * i * 8400 / Fs) +
        4000 * arm_sin_f32(PI2 * i * 18725 / Fs);
    lBufInArray[2*i] = fx;
    lBufInArray[2*i+1] = 0;
  }

  arm_cfft_f32(&arm_cfft_sR_f32_len256, lBufInArray, 0, 1);
  arm_cmplx_mag_f32(lBufInArray, mag, 256);

  for (i = 0; i < NPT; i++)
    mag[i] = mag[i] / NPT * 2;

  return;
}
```


## SD 卡写入

```c
HAL_StatusTypeDef status;
uint8_t txBuf[512];
for(int i = 0; i < 512; i++)
    txBuf[i] = i;
status = HAL_SD_WriteBlocks(&hsd1, txBuf, 0, 1, 10000);
```


## FAT 文件系统测试

```c
MX_FATFS_Init();

FRESULT ret;
ret = f_mount(&SDFatFS, "", 0);
ret = f_open(&SDFile, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
ret = f_printf(&SDFile, "Hello, %d\n", 112);
ret = f_close(&SDFile);
```