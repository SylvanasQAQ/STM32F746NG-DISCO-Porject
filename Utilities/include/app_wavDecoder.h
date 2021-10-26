#ifndef __APP_WAVDECODER_H
#define __APP_WAVDECODER_H

#include "fatfs.h"
#include "string.h"

// ————————————————
// 版权声明：本文为CSDN博主「AnSwEr不是答案」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
// 原文链接：https://blog.csdn.net/u011192270/article/details/54291621

typedef struct
{
    char riff_id[5];   //"RIFF"
    int riff_datasize; // RIFF chunk data size,exclude riff_id[4] and riff_datasize,total - 8

    char riff_type[5]; // "WAVE"

    char fmt_id[5];             // "fmt "
    int fmt_datasize;           // fmt chunk data size,16 for pcm
    short fmt_compression_code; // 1 for PCM
    short fmt_channels;         // 1(mono) or 2(stereo)
    int fmt_sample_rate;        // samples per second
    int fmt_avg_bytes_per_sec;  // sample_rate * channels * bit_per_sample / 8
    short fmt_block_align;      // number bytes per sample, bit_per_sample * channels / 8
    short fmt_bit_per_sample;   // bits of each sample(8,16,32).

    char data_id[5];   // "data"
    int data_datasize; // data chunk size,pcm_size - 44
} WaveHeader_t;

int read_wavheader(FIL *fp, WaveHeader_t *wavheader);
void print_wavheader(WaveHeader_t wavheader);

#endif