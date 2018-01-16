#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "type_.h"

static int _convert_8_to_16(unsigned char *src, FILE *fp, unsigned int size, unsigned int *pal)
{
    int i = 0, ret = 0;
    unsigned int pal_color = 0, offset = 0, r = 0, g = 0, b = 0;
    unsigned short color = 0;
    unsigned char tail = 0xFF, index = 0;

    offset = (((size * 16) + 31) >> 5) << 2;

    for(i = 0; i < size; i++)
    {
        index = src[i];
        pal_color = pal[index];
        r = (pal_color >> 16) & 0x000000FF;
        g = (pal_color >> 8) & 0x000000FF;
        b = (pal_color) & 0x000000FF;

        color = ( ((r >> 3 ) << 11 )  |  ((g >> 2 ) << 5 )  |  (b >> 3 ) );
        fwrite(&color, 2, 1, fp);
    }

    if(offset % (size * 2))
    {
        offset = offset % (size * 2);
        for(i = 0; i < offset; i++)
        {
            fwrite(&tail, 1, 1, fp);
        }
    }

    return (ret);
}

int bmp_8bpp_to_16bpp1(void *data, int length, char *filename)
{
    int ret = 0, i = 0;
    unsigned int width = 0, height = 0, hoffset = 0, size = 0, data_size = 0, offset = 0;
    unsigned int pal[256] = {0}, src_width = 0;
    unsigned short bpp = 0;
    FILE *fp = NULL;
    unsigned char *ptr = NULL;
    char temp[10] = {0};

    ptr = (unsigned char *)data;

    if(('B' != ptr[0]) || ('M' != ptr[1]))
    {
    printf("The source File is not BMP file.\n");
    return (1);
    }

    bpp = ptr[0x1C] | (ptr[0x1D] << 8);
    if(8 != bpp)
    {
    printf("The source BMP is not 8 bit per pixel BMP file, please check!\n");
    return (2);
    }

    fp = fopen(filename, "wr");
    if(NULL == fp)
    {
    return (1);
    }

    /*file tag (word)*/
    temp[0] = 'B';
    temp[1] = 'M';
    fwrite(ptr, 30872, 1, fp);
    fclose(fp);
    return 1;
    fwrite(temp, 2, 1, fp);

    width = ptr[0x12] | (ptr[0x13] << 8) | (ptr[0x14] << 16) | (ptr[0x15] << 24);
    height = ptr[0x16] | (ptr[0x17] << 8) | (ptr[0x18] << 16) | (ptr[0x19] << 24);
    hoffset = (((width * 16) + 31) >> 5) << 2;
    data_size = hoffset * height;
    size = 0x45 + data_size;

    /*file size (dword)*/
    fwrite(&size, 4, 1, fp);

    /*reserved (dword)*/
    memset(temp, 0, 10);
    fwrite(temp, 4, 1, fp);

    /*data offset (dword)*/
    offset = 0x46;
    fwrite(&offset, 4, 1, fp);

    /*header size (dword)*/
    offset = 0x38;
    fwrite(&offset, 4, 1, fp);

    /*width (dword)*/
    fwrite(&width, 4, 1, fp);

    /*height (dword)*/
    fwrite(&height, 4, 1, fp);

    /*planes (word)*/
    bpp = 1;
    fwrite(&bpp, 2, 1, fp);

    /*bpp (word)*/
    bpp = 8;
    fwrite(&bpp, 2, 1, fp);

    /*compress (dword)*/
    offset = 0x3; /*Windows format*/
    fwrite(&offset, 4, 1, fp);

    /*data size (dword)*/
    fwrite(&data_size, 4, 1, fp);

    /*h resolution (dword)*/
    fwrite(ptr + 0x26, 4, 1, fp);

    /*v resolution (dword)*/
    fwrite(ptr + 0x2A, 4, 1, fp);

    /*colors (dword)*/
    offset = 0;
    fwrite(&offset, 4, 1, fp);

    /*important color (dword)*/
    offset = 0;
    fwrite(&offset, 4, 1, fp);

    /*mask code to determine the format is RGB565*/
//    offset = 0x0000F800;
//    fwrite(&offset, 4, 1, fp);
//    offset = 0x000007E0;
//    fwrite(&offset, 4, 1, fp);
//    offset = 0x0000001F;
//    fwrite(&offset, 4, 1, fp);
//    offset = 0x00000000;
//    fwrite(&offset, 4, 1, fp);

    /*data*/
    memcpy(pal,(ptr + 0x36), 256 * sizeof(int));
    src_width = (((width * 8) + 31) >> 5) << 2;

    offset = ptr[0xA] | (ptr[0xB] << 8) | (ptr[0xC] << 16) | (ptr[0xD] << 24);
    fseek(fp, 0x46, SEEK_SET);

    ptr = ptr + offset;
    fwrite(ptr, data_size, 1, fp);

//    for(i = 0; i < height; i++)
//    {
//    _convert_8_to_16(ptr + i * src_width, fp, width, pal);
//    }

    fclose(fp);

    return (ret);
}

int bmp_8bpp_to_16bpp(void *data, int length, char *filename)
{
    int ret = 0, i = 0;
    unsigned int width = 0, height = 0, hoffset = 0, size = 0, data_size = 0, offset = 0;
    unsigned int pal[256] = {0}, src_width = 0;
    unsigned short bpp = 0;
    FILE *fp = NULL;
    unsigned char *ptr = NULL;
    char temp[10] = {0};

    ptr = (unsigned char *)data;

    if(('B' != ptr[0]) || ('M' != ptr[1]))
    {
        printf("The source File is not BMP file.\n");
        return (1);
    }

    bpp = ptr[0x1C] | (ptr[0x1D] << 8);
    if(8 != bpp)
    {
        printf("The source BMP is not 8 bit per pixel BMP file, please check!\n");
        return (2);
    }

    fp = fopen(filename, "wr");
    if(NULL == fp)
    {
        return (1);
    }

    /*file tag (word)*/
    temp[0] = 'B';
    temp[1] = 'M';
    fwrite(temp, 2, 1, fp);

    width = ptr[0x12] | (ptr[0x13] << 8) | (ptr[0x14] << 16) | (ptr[0x15] << 24);
    height = ptr[0x16] | (ptr[0x17] << 8) | (ptr[0x18] << 16) | (ptr[0x19] << 24);
    hoffset = (((width * 16) + 31) >> 5) << 2;
    data_size = hoffset * height;
    size = 0x45 + data_size;

    /*file size (dword)*/
    fwrite(&size, 4, 1, fp);

    /*reserved (dword)*/
    memset(temp, 0, 10);
    fwrite(temp, 4, 1, fp);


    /*data offset (dword)*/
    offset = 0x46;
    fwrite(&offset, 4, 1, fp);

    /*header size (dword)*/
    offset = 0x38;
    fwrite(&offset, 4, 1, fp);

    /*width (dword)*/
    fwrite(&width, 4, 1, fp);
    //qDebug("bmp w [%d]",width);
    /*height (dword)*/
    fwrite(&height, 4, 1, fp);

    /*planes (word)*/
    bpp = 1;
    fwrite(&bpp, 2, 1, fp);

    /*bpp (word)*/
    bpp = 16;
    fwrite(&bpp, 2, 1, fp);

    /*compress (dword)*/
    offset = 0x3; /*Windows format*/
    fwrite(&offset, 4, 1, fp);

    /*data size (dword)*/
    fwrite(&data_size, 4, 1, fp);

    /*h resolution (dword)*/
    fwrite(ptr + 0x26, 4, 1, fp);

    /*v resolution (dword)*/
    fwrite(ptr + 0x2A, 4, 1, fp);

    /*colors (dword)*/
    offset = 0;
    fwrite(&offset, 4, 1, fp);

    /*important color (dword)*/
    offset = 0;
    fwrite(&offset, 4, 1, fp);

    /*mask code to determine the format is RGB565*/
    offset = 0x0000F800;
    fwrite(&offset, 4, 1, fp);
    offset = 0x000007E0;
    fwrite(&offset, 4, 1, fp);
    offset = 0x0000001F;
    fwrite(&offset, 4, 1, fp);
    offset = 0x00000000;
    fwrite(&offset, 4, 1, fp);

    /*data*/
    memcpy(pal,(ptr + 0x36), 256 * sizeof(int));
    src_width = (((width * 8) + 31) >> 5) << 2;

    offset = ptr[0xA] | (ptr[0xB] << 8) | (ptr[0xC] << 16) | (ptr[0xD] << 24);
    fseek(fp, 0x46, SEEK_SET);

    ptr = ptr + offset;

    for(i = 0; i < height; i++)
    {
        _convert_8_to_16(ptr + i * src_width, fp, width, pal);
    }

    fclose(fp);

    return (ret);
}

static unsigned char * _convert_8_to_16Buff(unsigned char *src, unsigned char *buffer, unsigned int size, unsigned int *pal)
{
    int i = 0;
    unsigned int pal_color = 0, offset = 0, r = 0, g = 0, b = 0;
    unsigned short color = 0;
    unsigned char tail = 0xFF, index = 0;

    offset = (((size * 16) + 31) >> 5) << 2;

    for(i = 0; i < size; i++)
    {
        index = src[i];
        pal_color = pal[index];
        r = (pal_color >> 16) & 0x000000FF;
        g = (pal_color >> 8) & 0x000000FF;
        b = (pal_color) & 0x000000FF;

        color = ( ((r >> 3 ) << 11 )  |  ((g >> 2 ) << 5 )  |  (b >> 3 ) );
        //fwrite(&color, 2, 1, fp);
        memcpy(buffer,&color,2);
        buffer += 2;
    }

    if(offset % (size * 2))
    {
        offset = offset % (size * 2);
        for(i = 0; i < offset; i++)
        {
            //fwrite(&tail, 1, 1, fp);
            memcpy(buffer,&tail,1);
            buffer += 1;
        }
    }

    return (buffer);
}


unsigned char * bmp_8bpp_to_16bppBuff(void *data, int length, unsigned char *filename)
{
    int i = 0;
    unsigned int width = 0, height = 0, hoffset = 0, size = 0, data_size = 0, offset = 0;
    unsigned int pal[256] = {0}, src_width = 0;
    unsigned short bpp = 0;
    unsigned char *ptr = NULL;
    char temp[10] = {0};

    ptr = (unsigned char *)data;

    unsigned char *buffer;
    buffer = filename;

    if(('B' != ptr[0]) || ('M' != ptr[1]))
    {
        printf("The source File is not BMP file.\n");
        return (0);
    }

    bpp = ptr[0x1C] | (ptr[0x1D] << 8);
    if(8 != bpp)
    {
        printf("The source BMP is not 8 bit per pixel BMP file, please check!\n");
        return (0);
    }

    if(NULL == filename)
    {
        return (0);
    }

    /*file tag (word)*/
    temp[0] = 'B';
    temp[1] = 'M';
    //fwrite(temp, 2, 1, fp);
    memcpy(filename,temp,2);
    filename += 2;

    width = ptr[0x12] | (ptr[0x13] << 8) | (ptr[0x14] << 16) | (ptr[0x15] << 24);
    height = ptr[0x16] | (ptr[0x17] << 8) | (ptr[0x18] << 16) | (ptr[0x19] << 24);
    hoffset = (((width * 16) + 31) >> 5) << 2;
    data_size = hoffset * height;
    size = 0x45 + data_size;

    /*file size (dword)*/
    //fwrite(&size, 4, 1, fp);
    memcpy(filename,&size,4);
    filename += 4;

    /*reserved (dword)*/
    memset(temp, 0, 10);
    //fwrite(temp, 4, 1, fp);
    memcpy(filename,temp,4);
    filename += 4;

    /*data offset (dword)*/
    offset = 0x46;
    //fwrite(&offset, 4, 1, fp);
    memcpy(filename,&offset,4);
    filename += 4;

    /*header size (dword)*/
    offset = 0x38;
    //fwrite(&offset, 4, 1, fp);
    memcpy(filename,&offset,4);
    filename += 4;

    /*width (dword)*/
    //fwrite(&width, 4, 1, fp);
    memcpy(filename,&width,4);
    filename += 4;

    /*height (dword)*/
    //fwrite(&height, 4, 1, fp);
    memcpy(filename,&height,4);
    filename += 4;

    /*planes (word)*/
    bpp = 1;
    //fwrite(&bpp, 2, 1, fp);
    memcpy(filename,&bpp,2);
    filename += 2;

    /*bpp (word)*/
    bpp = 16;
    //fwrite(&bpp, 2, 1, fp);
    memcpy(filename,&bpp,2);
    filename += 2;

    /*compress (dword)*/
    offset = 0x3; /*Windows format*/
    //fwrite(&offset, 4, 1, fp);
    memcpy(filename,&offset,4);
    filename += 4;

    /*data size (dword)*/
    //fwrite(&data_size, 4, 1, fp);
    memcpy(filename,&data_size,4);
    filename += 4;

    /*h resolution (dword)*/
    //fwrite(ptr + 0x26, 4, 1, fp);
    memcpy(filename,ptr + 0x26,4);
    filename += 4;

    /*v resolution (dword)*/
    //fwrite(ptr + 0x2A, 4, 1, fp);
    memcpy(filename,ptr + 0x2A,4);
    filename += 4;

    /*colors (dword)*/
    offset = 0;
    //fwrite(&offset, 4, 1, fp);
    memcpy(filename,&offset,4);
    filename += 4;

    /*important color (dword)*/
    offset = 0;
    //fwrite(&offset, 4, 1, fp);
    memcpy(filename,&offset,4);
    filename += 4;

    /*mask code to determine the format is RGB565*/
    offset = 0x0000F800;
    //fwrite(&offset, 4, 1, fp);
    memcpy(filename,&offset,4);
    filename += 4;
    offset = 0x000007E0;
    //fwrite(&offset, 4, 1, fp);
    memcpy(filename,&offset,4);
    filename += 4;
    offset = 0x0000001F;
    //fwrite(&offset, 4, 1, fp);
    memcpy(filename,&offset,4);
    filename += 4;
    offset = 0x00000000;
    //fwrite(&offset, 4, 1, fp);
    memcpy(filename,&offset,4);
    filename += 4;

    /*data*/
    memcpy(pal,(ptr + 0x36), 256 * sizeof(int));
    src_width = (((width * 8) + 31) >> 5) << 2;

    offset = ptr[0xA] | (ptr[0xB] << 8) | (ptr[0xC] << 16) | (ptr[0xD] << 24);
    buffer += 0x46;

    ptr = ptr + offset;
    unsigned char *addr = 0;
    for(i = 0; i < height; i++)
    {
        addr = _convert_8_to_16Buff(ptr + i * src_width, buffer, width, pal);
        buffer = addr;
    }
    qDebug()<<width<<src_width;
    return (addr);
}

static unsigned char * _convert_8_to_16Buff3(unsigned char *src, unsigned char *buffer, unsigned int size, unsigned int *pal)
{
    int i = 0;
    unsigned int pal_color = 0, offset = 0, r = 0, g = 0, b = 0;
    unsigned short color = 0;
    unsigned char tail = 0xFF, index = 0;

    offset = (((size * 16) + 31) >> 5) << 2;

    for(i = 0; i < size; i++)
    {
        index = src[i];
        pal_color = pal[index];
        r = (pal_color >> 16) & 0x000000FF;
        g = (pal_color >> 8) & 0x000000FF;
        b = (pal_color) & 0x000000FF;

        color = ( ((r >> 3 ) << 11 )  |  ((g >> 2 ) << 5 )  |  (b >> 3 ) );
        memcpy(buffer,&color,2);
        buffer += 2;
    }

    if(offset % (size * 2))
    {
        offset = offset % (size * 2);
        for(i = 0; i < offset; i++)
        {
            memcpy(buffer,&tail,1);
            buffer += 1;
        }
    }

    return (buffer);
}

unsigned char *MApp_BmpDataReverse(unsigned char *pu8BmpData, unsigned char *bmpdata,  unsigned short bmpW, unsigned short bmpH)
{
    unsigned short u16picth = 0;
    unsigned int  i = 0,j = 0;
    u16picth = 2;
    for(i = 0;i < (bmpH/2);i++)
    {
        for(j = 0;j < (bmpW*u16picth);j++)
        {
            bmpdata[j] = *(pu8BmpData + (i*bmpW*u16picth) + j);
            *(pu8BmpData + (i*bmpW*u16picth) + j) = *(pu8BmpData + ((bmpH -i -1)*bmpW*u16picth) + j);
            *(pu8BmpData + ((bmpH -i -1)*bmpW*u16picth) + j) = bmpdata[j];
        }
    }
    return pu8BmpData;
}



unsigned char * bmp_8bpp_to_16bppBuff3(void *data, unsigned char *bmpBuffer)
{
    int i = 0;
    unsigned int width = 0, height = 0, hoffset = 0, size = 0, data_size = 0, offset = 0;
    unsigned int pal[256] = {0}, src_width = 0;
    unsigned short bpp = 0;
    unsigned char *ptr = NULL;
    char temp[10] = {0};

    ptr = (unsigned char *)data;

    unsigned char *buffer;
    buffer = bmpBuffer;

    if(('B' != ptr[0]) || ('M' != ptr[1]))
    {
        printf("The source File is not BMP file.\n");
        return (0);
    }

    bpp = ptr[0x1C] | (ptr[0x1D] << 8);
    if(8 != bpp)
    {
        printf("The source BMP is not 8 bit per pixel BMP file, please check!\n");
        return (0);
    }

    if(NULL == bmpBuffer)
    {
        return (0);
    }

    /*file tag (word)*/
    temp[0] = 'B';
    temp[1] = 'M';
    memcpy(bmpBuffer,temp,2);
    bmpBuffer += 2;

    width = ptr[0x12] | (ptr[0x13] << 8) | (ptr[0x14] << 16) | (ptr[0x15] << 24);
    height = ptr[0x16] | (ptr[0x17] << 8) | (ptr[0x18] << 16) | (ptr[0x19] << 24);
    hoffset = (((width * 16) + 31) >> 5) << 2;
    data_size = hoffset * height;
    size = 0x45 + data_size;

    /*file size (dword)*/
    memcpy(bmpBuffer,&size,4);
    bmpBuffer += 4;

    /*reserved (dword)*/
    memset(temp, 0, 10);
    memcpy(bmpBuffer,temp,4);
    bmpBuffer += 4;

    /*data offset (dword)*/
    offset = 0x46;
    memcpy(bmpBuffer,&offset,4);
    bmpBuffer += 4;

    /*header size (dword)*/
    offset = 0x38;
    memcpy(bmpBuffer,&offset,4);
    bmpBuffer += 4;

    /*width (dword)*/
    //width = ((++width)>>1)<<1;
    memcpy(bmpBuffer,&width,4);
    bmpBuffer += 4;

    /*height (dword)*/
    memcpy(bmpBuffer,&height,4);
    bmpBuffer += 4;

    /*planes (word)*/
    bpp = 1;
    memcpy(bmpBuffer,&bpp,2);
    bmpBuffer += 2;

    /*bpp (word)*/
    bpp = 16;
    memcpy(bmpBuffer,&bpp,2);
    bmpBuffer += 2;

    /*compress (dword)*/
    offset = 0x3; /*Windows format*/
    memcpy(bmpBuffer,&offset,4);
    bmpBuffer += 4;

    /*data size (dword)*/
    memcpy(bmpBuffer,&data_size,4);
    bmpBuffer += 4;

    /*h resolution (dword)*/
    memcpy(bmpBuffer,ptr + 0x26,4);
    bmpBuffer += 4;

    /*v resolution (dword)*/
    memcpy(bmpBuffer,ptr + 0x2A,4);
    bmpBuffer += 4;

    /*colors (dword)*/
    offset = 0;
    memcpy(bmpBuffer,&offset,4);
    bmpBuffer += 4;

    /*important color (dword)*/
    offset = 0;
    memcpy(bmpBuffer,&offset,4);
    bmpBuffer += 4;

    /*mask code to determine the format is RGB565*/
    offset = 0x0000F800;
    memcpy(bmpBuffer,&offset,4);
    bmpBuffer += 4;
    offset = 0x000007E0;
    memcpy(bmpBuffer,&offset,4);
    bmpBuffer += 4;
    offset = 0x0000001F;
    memcpy(bmpBuffer,&offset,4);
    bmpBuffer += 4;
    offset = 0x00000000;
    memcpy(bmpBuffer,&offset,4);
    bmpBuffer += 4;

    /*data*/
    memcpy(pal,(ptr + 0x36), 256 * sizeof(int));
    src_width = (((width * 8) + 31) >> 5) << 2;

    offset = ptr[0xA] | (ptr[0xB] << 8) | (ptr[0xC] << 16) | (ptr[0xD] << 24);
    buffer += 0x46;
    bmpBuffer = buffer;
    qDebug()<<width<<src_width;
    ptr = ptr + offset;

    for(i = 0; i < height; i++)
    {
        buffer = _convert_8_to_16Buff3(ptr + i * src_width, buffer, width, pal);
    }
    width = ((++width)>>1)<<1;
    unsigned char *lineTemp = (unsigned char *)malloc(width*2);
    memset(lineTemp,0,width*2);
    MApp_BmpDataReverse(bmpBuffer,lineTemp,width,height);
    free(lineTemp);

    return (buffer);
}



