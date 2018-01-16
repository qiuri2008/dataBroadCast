#ifndef __BMP8_TO_16_H__
    #define __BMP8_TO_16_H__

int bmp_8bpp_to_16bpp(void *data, int length, char *filename);
int bmp_8bpp_to_16bpp1(void *data, int length, char *filename);
unsigned char * bmp_8bpp_to_16bppBuff(void *data, int length, unsigned char *filename);
unsigned char * bmp_8bpp_to_16bppBuff3(void *data, unsigned char *bmpBuffer);
#endif /*__BMP8_TO_16_H__*/

