#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int _convert_32_to_16(unsigned char *src, FILE *fp, unsigned int size)
{
    int i = 0, ret = 0;
    unsigned int offset = 0, r = 0, g = 0, b = 0;
    unsigned short color = 0;
    unsigned char tail = 0xFF;
    
    offset = (((size * 16) + 31) >> 5) << 2;

    for(i = 0; i < size; i++)
    {
	b = src[i * 4];
	g = src[i * 4 + 1];
	r = src[i * 4 + 2];

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

int bmp_32bpp_to_16bpp(void *data, int length, char *filename)
{
    int ret = 0, i = 0;
    unsigned short bpp = 0;
    unsigned int width = 0, height = 0, hoffset = 0, size = 0, data_size = 0, offset = 0;
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
    if(32 != bpp)
    {
	printf("The source BMP is not 32 bit per pixel BMP file, please check!\n");
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

    offset = ptr[0xA] | (ptr[0xB] << 8) | (ptr[0xC] << 16) | (ptr[0xD] << 24);
    fseek(fp, 0x46, SEEK_SET); 

    ptr = ptr + offset;
    hoffset = (((width * 32) + 31) >> 5) << 2;

    for(i = 0; i < height; i++)
    {
	_convert_32_to_16(ptr + i * hoffset, fp, width);	
    }

    fclose(fp);

    return (ret);
}

