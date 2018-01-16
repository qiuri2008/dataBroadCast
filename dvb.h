#ifndef DVB_H
#define DVB_H

#include "sction.h"
#include "pat.h"
#include "demux.h"
#include "crc.h"



void *dvb_create();

void dvb_destroy( pdvb ths);

/* 过滤器回调函数 */
void pat_filter(void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam);
void pmt_filter(void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam);
void sdt_filter(void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam);
void nit_filter(void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam);

/*打印结果*/
void print_reslut();

/*释放内存*/
void free_all();


#endif
