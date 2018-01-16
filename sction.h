#ifndef SCTION_H
#define SCTION_H

#include "pat.h"
#include "type_.h"

typedef struct Ssection
{
	int32_t size;
	unsigned char cInfo[1024];
}section, *psection;

/* 在section中查找transport_stream_id相对应的频点信息 */
void *get_network(unsigned char *buf, uint16_t transport_stream_id, int32_t size);

/* 在sdt_section中对应的service_id的节目名称 */
void *get_service_name(unsigned char *buf, uint16_t service_id, uint16_t transport_stream_id, int32_t size);

/* 在pmt_section中，查找video_pid和audio_pid */
void *get_program_msg(unsigned char *buf, int32_t size);

/* 提取pat中的相关信息，包括transport_stream_id,program_number, pmt_pid */
void *get_pat_msg(unsigned char *buf, int32_t size);

#endif
