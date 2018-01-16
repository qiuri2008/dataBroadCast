#ifndef COMPARSE_H
#define COMPARSE_H

void com_filter(void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam );
void com_is_start(void);
void com_is_close( void );
extern bool g_bPaseOk;


#endif // COMPARSE_H
