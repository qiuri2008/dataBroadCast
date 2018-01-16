#ifndef __TYPE__H__
#define __TYPE__H__

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <QDebug>
#include <QFile>
#include <QMessageBox>

/* int8_t的定义在LINUX上易造成编译错误，干脆不用int8_t,uint8_t */
typedef unsigned char byte;

#define IS_MAX_FILE_NUM 			(600)
#define IS_MAX_SEC_PER_SUB 		(256)


#define IS_STEP1_FILTER_NUM 		(1)

#define _BUF_SIZE_                  (4*1024*1024)
extern uint8_t *pInfoServiceStartAddr;
#define IS_BUF0_START      	((uint32_t)pInfoServiceStartAddr)
#define IS_BUF0_END        	(IS_BUF0_START+_BUF_SIZE_)

#define BAT_MAX_DESC		(10)
#define BAT_MAX_SEGMENT		(10)
#define GX_NO_ERROR	(0)

typedef struct BAT_TSRecord_s
{
    uint16_t valide_desc_num;
    uint8_t TS_ID_record[BAT_MAX_DESC];
    uint8_t last_segment_num;
    uint8_t segment_record[BAT_MAX_SEGMENT];

}BAT_TSRecord_t;

typedef struct BAT_FilePara_s
{
    uint16_t table_id_ext;
    uint8_t section_record[IS_MAX_SEC_PER_SUB];
    uint8_t last_section_num;

}BAT_FilePara_t;

typedef struct BAT_FileList_s
{
    uint16_t transport_id;
    uint16_t PID;
    uint8_t segment_number;
    uint8_t last_segment_number;
    uint8_t ver_num;

    uint16_t file_num;
    BAT_FilePara_t file[IS_MAX_FILE_NUM];

}BAT_FileList_t;

typedef enum IS_Error_e
{
    IS_FILTER_TIMEOUT = 0x12345,
    IS_FILTER_CRC_ERROR,
    IS_FILTER_REPEAT,
    IS_FILTER_NO_BAT,
    IS_FILTER_BAT_ERROR,
    IS_FILTER_MALLOC_ERROR,
    IS_FIND_SEC_NO_FINISH,
    IS_SEC_ONE_FINISHED,
    IS_SEC_ALL_FINISHED,
    IS_FILE_MALLOC_ERROR,
    IS_TUNER_UNLOCK

}IS_Error_t;



#endif /* __TYPE__H__ */
