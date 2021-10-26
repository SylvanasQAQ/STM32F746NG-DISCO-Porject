#include "app_wavDecoder.h"
#include <stdio.h>

// ————————————————
// 版权声明：本文为CSDN博主「AnSwEr不是答案」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
// 原文链接：https://blog.csdn.net/u011192270/article/details/54291621

int read_wavheader(FIL *fp, WaveHeader_t *wavheader)
{
    if (fp == NULL)
        return -1;
    uint32_t len = 1;
    f_read(fp, wavheader->riff_id, 4, &len);
    wavheader->riff_id[4] = '\0';
    f_read(fp, (char*)&wavheader->riff_datasize, 4, &len);
    f_read(fp, wavheader->riff_type, 4, &len);
    wavheader->riff_type[4] = '\0';
    f_read(fp, wavheader->fmt_id, 4, &len);
    wavheader->fmt_id[4] = '\0';
    f_read(fp, (char*)&wavheader->fmt_datasize, 4, &len);
    f_read(fp, (char*)&wavheader->fmt_compression_code, 2, &len);
    f_read(fp, (char*)&wavheader->fmt_channels, 2, &len);
    f_read(fp, (char*)&wavheader->fmt_sample_rate, 4, &len);
    f_read(fp, (char*)&wavheader->fmt_avg_bytes_per_sec, 4, &len);
    f_read(fp, (char*)&wavheader->fmt_block_align, 2, &len);
    f_read(fp, (char*)&wavheader->fmt_bit_per_sample, 2, &len);
    f_read(fp, wavheader->data_id, 4, &len);
    wavheader->data_id[4] = '\0';
    f_read(fp, (char*)&wavheader->data_datasize, 4, &len);

    if (strcmp(wavheader->data_id, "data") != 0)
    {
        fp->fptr += wavheader->data_datasize;
        f_read(fp, wavheader->data_id, 4, &len);
        f_read(fp, (char*)&wavheader->data_datasize, 4, &len);
    }
    return 0;
}



void print_wavheader(WaveHeader_t wavheader)
{
    printf("wavheader.riff_id: %c%c%c%c\n", wavheader.riff_id[0], wavheader.riff_id[1], wavheader.riff_id[2], wavheader.riff_id[3]);
    printf("wavheader.riff_datasize: %d\n", wavheader.riff_datasize);
    printf("wavheader.riff_type: %c%c%c%c\n", wavheader.riff_type[0], wavheader.riff_type[1], wavheader.riff_type[2], wavheader.riff_type[3]);
    printf("wavheader.fmt_id: %c%c%c%c\n", wavheader.fmt_id[0], wavheader.fmt_id[1], wavheader.fmt_id[2], wavheader.fmt_id[3]);
    printf("wavheader.fmt_datasize: %d\n", wavheader.fmt_datasize);
    printf("wavheader.fmt_compression_code: %hd\n", wavheader.fmt_compression_code);
    printf("wavheader.fmt_channels: %hd\n", wavheader.fmt_channels);
    printf("wavheader.fmt_sample_rate: %d\n", wavheader.fmt_sample_rate);
    printf("wavheader.fmt_avg_bytes_per_sec: %d\n", wavheader.fmt_avg_bytes_per_sec);
    printf("wavheader.fmt_block_align: %hd\n", wavheader.fmt_block_align);
    printf("wavheader.fmt_bit_per_sample: %hd\n", wavheader.fmt_bit_per_sample);
    printf("wavheader.data_id: %c%c%c%c\n", wavheader.data_id[0], wavheader.data_id[1], wavheader.data_id[2], wavheader.data_id[3]);
    printf("wavheader.data_datasize: %d\n", wavheader.data_datasize);
}