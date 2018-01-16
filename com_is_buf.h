#ifndef __COM_IS_BUF_H__
#define __COM_IS_BUF_H__

#include"com_is_buf_type.h"


uint32_t com_stack_file_initialize( void );
uint32_t com_stack_file_free(void);
FileList_t* com_is_malloc_file(void);
uint32_t com_is_free_file( FileList_t *buf );
void com_is_add_file( FileList_t** pFileOut);
void com_is_destroy_filelist(void );
FileState_t com_is_update_file_type(FileList_t *pFile);

FileList_t* com_is_get_file_list(void);

#endif


