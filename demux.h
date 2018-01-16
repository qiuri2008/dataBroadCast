#ifndef __DEMUX_H__
#define __DEMUX_H__
#include "type_.h"

/* TS过滤器输出回调函数 */
typedef void (*PTsFilterCallback)( void *handle, void* filter, unsigned char *buf, int32_t size, uint32_t lParam );

/* 创建解复用器 */
void *Demux_Create();

/* 销毁解复用器 */
void Demux_Destroy( void *handle );

/* 创建一个过滤器 */
void* Demux_CreateFilter( void *handle, uint16_t pid, unsigned char *pattern, unsigned char *mask, int32_t len, 
							int32_t qsize, PTsFilterCallback pProc, uint32_t lParam  );

/* 用另外一种方式创建一个过滤器 */
void* Demux_CreateFilterEx( void *handle, uint16_t pid, unsigned char tid, unsigned char tid_mask,
							uint16_t tid_ext, uint16_t tid_ext_mask,
							int32_t qsize, PTsFilterCallback pProc, uint32_t lParam  );
/* 销毁一个过滤器 */
void Demux_DestroyFilter( void *handle, void* filter );

/* 送入TS流数据 */
void Demux_OnData( void *handle, unsigned char *buf, int32_t size );

/* SI-SECTION的解复用器 */
typedef struct tagFilterNode
{
    /* TS包计数器 */
    unsigned char continuity_counter;
    /* TS数据包是否连续 */
    int discontinuity;
    /* SECTION还缺多少数据(byte) */
    int need;
    /* 是否处理完头部的3个字节 */
    int complete_header;

    /* SECTION数据缓冲区 */
    unsigned char *buf;
    /* SECTION数据缓冲区大小 */
    int bufsize;
    /* SECTION数据缓冲区中数据量大小 */
    int datsize;

    /* 过滤条件 */
    uint16_t pid;
    unsigned char pattern[16];
    unsigned char mask[16];
    int len;

    /* SECTION输出函数 */
    PTsFilterCallback pOutputProc;
    /* SECTION输出函数所带的用户参数 */
    uint32_t lParam;

    /* 下一个FILTER */
    struct tagFilterNode *next;
}SFilterNode;

typedef struct
{
    /* 过滤器链表 */
    SFilterNode *list;
    /* 保留上次残留TS数据的缓冲区 */
    unsigned char tsbuf[188];
    /* 保留上次残留TS数据大小 */
    int tssize;
}SDemux;

typedef struct tag_dvb
{
    void *m_pDemux;         //指向一个解复用器
    void *m_pFilter[50];    //过滤器
    int32_t size;
}dvb, *pdvb;




#endif /* __DEMUX_H__ */
