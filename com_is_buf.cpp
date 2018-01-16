
#include "com_is_buf.h"
#include "batParse.h"
//#include "com_is_filter.h"


FileList_t* pgFileList = NULL;
FileList_t FileBuffer[IS_MAX_FILE_NUM];
static uint32_t* gs_pFileBufferSpace[IS_MAX_FILE_NUM];


Stack_t			stack_file;


/**
 * @brief  create stack
 * @param  ppStackSpace : stack space address
 * @param  nSize : stack size
 * @param  pStack : created stack info
 * @return  NULL
 */
void create_stack(uint32_t **ppStackSpace, size_t nSize,
				  Stack_t* pStack)
{
    Q_ASSERT(ppStackSpace != NULL && pStack != NULL);

    pStack->m_pBuf = (void**)ppStackSpace;

    pStack->m_nBottom    = 0;
    pStack->m_nTop       = nSize-1;
    pStack->m_nInterator = 0;
    pStack->m_nSize      = nSize;
}

 /**
 * @brief  pop stack
 * @param  pStack : stack to pop
 * @return  NULL
 */
void* pop_stack(Stack_t * pStack )
{
	uint32_t  i       = 0;
	void *pResult = NULL;

    Q_ASSERT( pStack != NULL );
	if( pStack->m_nInterator > 0 )
	{
		i = (--pStack->m_nInterator) + pStack->m_nBottom;

		pResult = (void*)(pStack->m_pBuf[i]); 

		return pResult;
	}

	// no more stack to alloc
	return NULL;
}

 /**
 * @brief  push data into stack
 * @param  pStack : stack to push
 * @param  pBuf : the pointer of data
 * @return  NULL
 */
void push_stack(Stack_t * pStack, void* pBuf )
{
    uint32_t i   = 0;

    Q_ASSERT(pStack != NULL && pBuf != NULL);

    if( pStack->m_nInterator < pStack->m_nSize )
    {
        i = (pStack->m_nInterator++)+pStack->m_nBottom;
        pStack->m_pBuf[i] = pBuf;
    }
}



uint32_t com_stack_file_initialize( void )
{
	uint32_t i = 0;
	uint32_t err = 0;

	create_stack(gs_pFileBufferSpace, IS_MAX_FILE_NUM, &stack_file);

	for( i=0; i<IS_MAX_FILE_NUM; i++ )
	{
		push_stack( &stack_file, (void*)&FileBuffer[i] );
	}
	return err;
}

FileList_t* com_is_malloc_file(void)
{
	FileList_t* pFileList = NULL;	
	pFileList = (FileList_t*)pop_stack( &stack_file );
	if( pFileList!=NULL )
	{
		memset( pFileList, 0, sizeof(FileList_t) );
	}

	return pFileList;
}


uint32_t com_is_free_file( FileList_t *buf )
{
/*begin 明显的问题 by uu_dou 20121225*/
	//MEMSET(buf,0,sizeof(FileList_t*));
	memset(buf,0,sizeof(FileList_t));
/*end*/
	push_stack( &stack_file, (void*) buf);

/*begin 同时将该单元从单链表pgFileList中踢出来，一般是链表的最后一个单元 by uu_dou 20121225*/
	FileList_t  *pFileList = pgFileList;
	FileList_t  *pPreFileList = pgFileList;

	if(pgFileList == buf)
	{
		pgFileList = NULL;
		buf = NULL;
		return 0;
	}

	if( NULL != pFileList )
	{
		while( pFileList->next != NULL )
		{
			pPreFileList = pFileList;
			pFileList= pFileList->next;
		}

		if(pFileList == buf)
		{
			buf = NULL;
			pPreFileList->next = NULL;
		}
	}
/*end*/	
	return 0;
}

void com_is_add_file( FileList_t** pFileOut)                   
{
	FileList_t  *pFileList = pgFileList;
	FileList_t *pFileNew = NULL;

	pFileNew = com_is_malloc_file();
	if(pFileNew == NULL)
	{
		return ;
	}
	pFileList = pgFileList;
	if( NULL != pFileList )
	{
		while( pFileList->next != NULL )
		{
			pFileList= pFileList->next;
		}

		pFileList->next = pFileNew;
		pFileNew->next = NULL;
	}
	else
	{
		pFileNew->next = NULL;
		pgFileList = pFileNew;
	}	
	*pFileOut = pFileNew;
	return ;

}

void com_is_destroy_filelist(void )
{
	FileList_t *pFile = pgFileList;
	FileList_t *pFile_temp = NULL;

	for(pFile = pgFileList;pFile != NULL; )
	{
/*begin 需要将链表的单元一个一个的释放 by uu_dou 20121225*/
		//com_is_free_file(pFile);

		pFile_temp = pFile->next;
		memset(pFile,0,sizeof(FileList_t));
		push_stack( &stack_file, (void*) pFile);
		pFile = pFile_temp;
/*end*/
	}
	pgFileList = NULL;
}







FileState_t com_is_update_file_type(FileList_t *pFile)
{
	uint16_t len = 0;
	FileState_t state = FILE_OK;
	len = strlen((char*)pFile->m_pchFileName);
	if(120 == pFile->m_pchFileName[len-3]//x
		&&109 == pFile->m_pchFileName[len-2]//m
		&&108 == pFile->m_pchFileName[len-1])//l
	{
		pFile->m_FileType = FILE_XML;
	}
	else if(98 == pFile->m_pchFileName[len-3]//b
		&&109 == pFile->m_pchFileName[len-2]//m
		&&112 == pFile->m_pchFileName[len-1])//p
	{
		pFile->m_FileType = FILE_BMP;
	}
	else if(109 == pFile->m_pchFileName[len-3]//m
		&&112 == pFile->m_pchFileName[len-2]//p
		&&103 == pFile->m_pchFileName[len-1])//g
	{
		pFile->m_FileType = FILE_MPG;
	}
	else
	{
        pFile->m_FileType = FILE_ERR;
		state = FILE_ERROR;
	}
	return state;
}





FileList_t* com_is_get_file_list(void)
{
    return pgFileList;
}


