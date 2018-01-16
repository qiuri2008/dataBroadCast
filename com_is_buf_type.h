#ifndef __COM_IS_BUF_TYPE_H__
#define __COM_IS_BUF_TYPE_H__
#include "type_.h"


#define IS_MAX_FILE_NAME			(50)
#define IS_MAX_SECTION_NUM		(700)    // TODO: 2.5MB->640 Section
#define IS_MAX_SECTION_LEN			(4100) //4096


#define ADDR_ALIGN(data)		\
    ((((data) & 0x3) == 0)? (data) : ((data) + 0x4 - ((data) & 0x3)))

/*IS_DATA_PARSE*/

typedef struct Stack_s
{
	uint32_t     m_nBottom;
	uint32_t     m_nTop;
    int32_t     m_nInterator;
	uint32_t     m_nSize;
	void**  m_pBuf;
}Stack_t;

typedef enum FileType_e
{
	FILE_ALL,
	FILE_BMP,
    FILE_XML,
	FILE_MPG,
	FILE_ERR,
}FileType_t;

typedef enum FileState_e
{
	FILE_NOT_EXIST = 1,
    FILE_EXIST,
    FILE_ERROR,
    FILE_OK
}FileState_t;

typedef struct FileList_s
{
	FileType_t m_FileType;
	uint16_t m_FileId;
	uint8_t m_version;
	uint8_t m_chFileNameLen;
	int8_t m_pchFileName[IS_MAX_FILE_NAME +1];
/*begin 记录每张表的最大section num by uu_dou 20121225*/
	uint8_t m_nLastSection;
/*end*/
	uint32_t m_nFileAddr;
	uint32_t m_nFileLength;
/*begin 记录每个section的地址，长度；从section 0开始放 by uu_dou 20121225*/
	uint32_t m_nSubFileAddr[256];
	uint32_t m_nSubFileLength[256];
/*end*/
	
	struct FileList_s *next;
}FileList_t;

#endif


