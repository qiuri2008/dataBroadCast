#ifndef BATPARSE_H
#define BATPARSE_H
#include "type_.h"

void bat_filter(void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam);
void DataCastInit(void);
#endif // BATPARSE_H
