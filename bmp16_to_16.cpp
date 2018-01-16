#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum 
{
    BMP_16BPP_565,
    BMP_16BPP_1555,
    BMP_16BPP_4444
}BMP_16BPP_FORMAT;

unsigned int bmp_mask[3][4] = 
{
    {0x0000F800, 0x000007E0, 0x0000001F, 0x00000000},
    {0x00007C00, 0x000003E0, 0x0000001F, 0x00008000},
    {0x00000F00, 0x000000F0, 0x0000000F, 0x00000000}
};

static int _convert_16_to_16(unsigned short *src, FILE *fp, unsigned int size, unsigned int format)
{
    unsigned short color = 0;
    unsigned int i = 0, ret = 0;
    unsigned int offset = 0, r = 0, g = 0, b = 0;
    unsigned char tail = 0xFF;

    offset = (((size * 16) + 31) >> 5) << 2;

    for(i = 0; i < size; i++)
    {
	color = src[i];
	if(BMP_16BPP_1555 == format)
	{
	    r = (color & 0x00007C00) >> 7;
	    g = (color & 0x000003E0) >> 2;
	    b = (color & 0x0000001F) << 3;
	}
	else if(BMP_16BPP_4444 == format)
	{
	    r = (color & 0x00000F00) >> 4;
	    g = (color & 0x000000F0);
	    b = (color & 0x0000000F) << 4;
	}
	//color = ( (r << 11 )  |  (g << 5 )  |  (b) );	
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

int bmp_16bpp_to_16bpp(void *data, int length, char *filename)
{
    int ret = 0;
    unsigned short bpp = 0;
    FILE *fp = NULL;
    unsigned char *ptr = NULL;
    unsigned int mask_code[4] = {0}, format = 0, offset = 0; 
    unsigned int width = 0, height = 0, i = 0, hoffset = 0, size = 0;
    //char temp[10] = {0};
    
    ptr = (unsigned char *)data;

    if(('B' != ptr[0]) || ('M' != ptr[1]))
    {
	printf("The source File is not BMP file.\n");	
	return (1);
    }

    bpp = ptr[0x1C] | (ptr[0x1D] << 8);
    if(16 != bpp)
    {
	printf("The source BMP is not 16 bit per pixel BMP file, please check!\n");  
	return (2);
    }

    memcpy(mask_code, ptr + 0x36, 16);
    if((bmp_mask[0][0] == mask_code[0]) && 
       (bmp_mask[0][1] == mask_code[1]) &&
       (bmp_mask[0][2] == mask_code[2]) &&
       (bmp_mask[0][3] == mask_code[3]))
    {
	printf("format is 565\n");
	format = BMP_16BPP_565;
    }
    else if((bmp_mask[1][0] == mask_code[0]) && 
	    (bmp_mask[1][1] == mask_code[1]) &&
	    (bmp_mask[1][2] == mask_code[2]) &&
	    (bmp_mask[1][3] == mask_code[3]))
    {
	printf("format is 1555\n");
	format = BMP_16BPP_1555;
    }
    else if((bmp_mask[2][0] == mask_code[0]) && 
	    (bmp_mask[2][1] == mask_code[1]) &&
	    (bmp_mask[2][2] == mask_code[2]) &&
	    (bmp_mask[2][3] == mask_code[3]))
    {	
	printf("format is 4444\n");
	format = BMP_16BPP_4444;
    }

    fp = fopen(filename, "wr");
    if(NULL == fp)
    {
	return (1);
    }

    if(BMP_16BPP_565 == format)
    {
	fwrite(ptr, length, 1, fp);
	return (0);
    }

    fwrite(ptr, 0x36, 1, fp); 

    /*mask*/
    offset = 0x0000F800;    
    fwrite(&offset, 4, 1, fp);    
    offset = 0x000007E0;    
    fwrite(&offset, 4, 1, fp);    
    offset = 0x0000001F;    
    fwrite(&offset, 4, 1, fp);    
    offset = 0x00000000;    
    fwrite(&offset, 4, 1, fp);

    /*data*/
    width = ptr[0x12] | (ptr[0x13] << 8) | (ptr[0x14] << 16) | (ptr[0x15] << 24);
    height = ptr[0x16] | (ptr[0x17] << 8) | (ptr[0x18] << 16) | (ptr[0x19] << 24);
    hoffset = (((width * 16) + 31) >> 5) << 2; 
    size = hoffset * height;
    fseek(fp, 0x22, SEEK_SET);
    fwrite(&size, 4, 1, fp);
    
    size = 0x45 + size;
    fseek(fp, 0x2, SEEK_SET);
    fwrite(&size, 4, 1, fp);

    fseek(fp, 0x46, SEEK_SET);

    ptr = ptr + 0x46;
    
    for(i = 0; i < height; i++)
    {
	_convert_16_to_16((unsigned short *)(ptr + i * hoffset), fp, width, format);
    }
    
    fclose(fp);
    
    return (ret);
}
