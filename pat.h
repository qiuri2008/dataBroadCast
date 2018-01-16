#ifndef PAT_H
#define PAT_H

#include "sction.h"

typedef struct Spat
{
	unsigned short transport_stream_id;//区别网络中其他复用流
	unsigned short program_number[10];//program_number==0标识NIT_PID
	unsigned short pmt_pid[10];//PMT_PID
	unsigned short size;
}pat, *ppat;

#endif
