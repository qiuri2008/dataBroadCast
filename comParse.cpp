
#include "type_.h"
#include "com_is_buf.h"
#include "demux.h"
#include "uial_info_service.h"

static FileList_t *pFileNew = NULL;
static uint32_t InfoBufAddrOffset = 0 ;
static uint32_t MaxFileId = 0;
uint8_t *pInfoServiceStartAddr = NULL;
extern BAT_FileList_t gBatFileList;
bool g_bPaseOk;

int32_t com_is_check_one_sec_finished(uint16_t FindFileNO)
{
    uint8_t i = 0;

    for(i = 0; i <= gBatFileList.file[FindFileNO].last_section_num; i++)
    {
        if(0 == gBatFileList.file[FindFileNO].section_record[i])
        {
            return IS_FIND_SEC_NO_FINISH;
        }
    }

    return GX_NO_ERROR;
}

int32_t com_is_parse_section_body(uint8_t *pBuf)
{
    uint8_t table_id = 0;
    uint8_t start_indicator = 0;
    uint8_t version = 0;
    uint8_t section_num = 0;
    uint8_t last_section_num = 0;
    uint8_t file_name_length = 0;
    int16_t section_length = 0;
    uint16_t table_id_ext = 0;
    uint16_t FileDataLength = 0;
    uint8_t* data = NULL;
    uint16_t FileNO = 0;
    uint16_t i = 0;
    uint8_t section_syntax_indicator = 0;

    static uint16_t sOldFileNo= 0;

    data = pBuf;

    table_id = data[0];
    start_indicator = (data[1]&0x40)>>6;
    section_length = (((uint16_t)data[1]<<8)|data[2])&0x0FFF;
    table_id_ext = ((uint16_t)data[3]<<4)|(data[4]>>4);
    version = (data[5]>>1)&0x1f;
    section_num = data[6];
    last_section_num = data[7];
    file_name_length = data[8];


    section_syntax_indicator = data[1]&0x80;
    if(0x80 != section_syntax_indicator)
    {
        return IS_FILTER_CRC_ERROR;
    }
    data = data + 9;

    if(MaxFileId<table_id_ext)
    {
        MaxFileId=table_id_ext;
    }
    FileNO = table_id_ext;

    if(0xffff != FileNO)
    {
        if(1 == gBatFileList.file[FileNO].section_record[section_num])
        {
            return IS_FILTER_REPEAT;
        }
    }
    else
    {
        com_is_destroy_filelist();
        return IS_FILTER_BAT_ERROR;
    }

    if(((pFileNew == NULL)&&(0==section_num))||
            ((pFileNew != NULL)&&(pFileNew->m_FileId != table_id_ext )&&(0==section_num)))
    {    
//        if (pFileNew->m_FileId != table_id_ext && pFileNew!=NULL)
//        {
//            if(IS_FIND_SEC_NO_FINISH == com_is_check_one_sec_finished(pFileNew->m_FileId))
//            {
//                return 0;
//            }

//        }

        if(pFileNew != NULL)
        {
            for(i = 0;i<=gBatFileList.file[sOldFileNo].last_section_num;i++)
            {
                if(0 == gBatFileList.file[sOldFileNo].section_record[i]
                    || pFileNew->m_FileType == FILE_ERR)
                {
                    InfoBufAddrOffset  -=  pFileNew->m_nFileLength;
                    memset(gBatFileList.file[sOldFileNo].section_record,0,IS_MAX_SEC_PER_SUB);
                    com_is_free_file(pFileNew);
                    pFileNew = NULL;
                    break;
                }
            }
        }
        //申请空间并加入链表
        com_is_add_file(&pFileNew) ;
        if(NULL == pFileNew )
        {
            com_is_destroy_filelist();
            return IS_FILE_MALLOC_ERROR;
        }

        if(InfoBufAddrOffset>IS_BUF0_END)
        {
            com_is_destroy_filelist();
            return IS_FILTER_MALLOC_ERROR;
        }

        InfoBufAddrOffset = ADDR_ALIGN(InfoBufAddrOffset);
        pFileNew->m_nFileAddr = (uint32_t)InfoBufAddrOffset ;

        pFileNew->m_nLastSection = last_section_num;
        pFileNew->m_FileId = table_id_ext;
        pFileNew->m_version = version;

        gBatFileList.file[FileNO].table_id_ext = table_id_ext;
        gBatFileList.file[FileNO].last_section_num = last_section_num;

        sOldFileNo = FileNO;

    }

    if((pFileNew!= NULL) && (pFileNew->m_FileId == table_id_ext) )
    {
        if(InfoBufAddrOffset>IS_BUF0_END)
        {
            com_is_destroy_filelist();
            return IS_FILTER_MALLOC_ERROR;
        }

        if(0 == section_num)
        {
            if(file_name_length>IS_MAX_FILE_NAME-1)
            {
                file_name_length = IS_MAX_FILE_NAME-1;
            }

            pFileNew->m_chFileNameLen =  file_name_length;
            memcpy(pFileNew->m_pchFileName, data, file_name_length);
            pFileNew->m_pchFileName[file_name_length] = '\0';
            com_is_update_file_type(pFileNew);
        }

        data += file_name_length;
        FileDataLength = (uint16_t)(data[2]<<8)|data[3];

        if(FileDataLength>4095)
        {
            com_is_destroy_filelist();
            return IS_FILTER_MALLOC_ERROR;
        }

        data += 4;

        InfoBufAddrOffset = ADDR_ALIGN(InfoBufAddrOffset);
        pFileNew->m_nFileLength += FileDataLength;
        pFileNew->m_nSubFileAddr[section_num] = (uint32_t)InfoBufAddrOffset;
        pFileNew->m_nSubFileLength[section_num] = FileDataLength;

        memcpy((uint8_t*)InfoBufAddrOffset, data, FileDataLength);
        InfoBufAddrOffset += FileDataLength;

        if(0xffff != FileNO)
        {
            gBatFileList.file[FileNO].section_record[section_num] = 1;
            //qDebug()<<table_id_ext<<section_num<<FileDataLength<<pFileNew->m_pchFileName;
            //qDebug("[%d]-[%d]-[%d]-[%s]",table_id_ext,section_num,FileDataLength,pFileNew->m_pchFileName);

        }
        else
        {
            com_is_destroy_filelist();
            return IS_FILTER_BAT_ERROR;
        }

    }
    return 0;

}

int32_t com_is_check_all_sec_finished(void)
{
    uint16_t FindFileNO = 0;
    uint8_t i = 0;

    for(FindFileNO = 0; FindFileNO <= MaxFileId; FindFileNO++)
    {
        for(i = 0; i <= gBatFileList.file[FindFileNO].last_section_num; i++)
        {
            if(0 == gBatFileList.file[FindFileNO].section_record[i])
            {
                return IS_FIND_SEC_NO_FINISH;
            }
        }
    }

    return GX_NO_ERROR;
}

void com_is_start(void)
{
    uint32_t i = 0;

    com_stack_file_initialize();

    pInfoServiceStartAddr = new byte[_BUF_SIZE_];
    if(pInfoServiceStartAddr)
    {
        memset(pInfoServiceStartAddr, 0, (_BUF_SIZE_));
    }

    for(i = 0; i < IS_MAX_FILE_NUM; i++)
    {
        memset(gBatFileList.file[i].section_record, 0, sizeof(gBatFileList.file[i].section_record));
    }
    pFileNew = NULL;
    InfoBufAddrOffset = IS_BUF0_START;
    MaxFileId=0;

}

void com_is_close( void )
{
    com_is_destroy_filelist();

    if(pInfoServiceStartAddr)
    {
        delete [] pInfoServiceStartAddr;
        pInfoServiceStartAddr = NULL;
    }
}

extern int create_sevice_index(void);
void com_filter(void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam )
{
    uint32_t Err = 0;
    size = size; lParam = lParam;

    Err = com_is_parse_section_body(buf);
    if(IS_FILTER_REPEAT == Err)
    {
        Err = com_is_check_all_sec_finished();
        if(GX_NO_ERROR == Err)
        {
            g_bPaseOk = true;
            gx_enter_IS();
            create_sevice_index();
            //过滤完成后,销毁过滤器
            Demux_DestroyFilter(handle, filter);
            qDebug()<<"all parse ok...";
        }
    }

}


