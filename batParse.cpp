#include "batParse.h"
#include "dvb.h"
#include "comParse.h"

uint32_t g_BatISVersion;
BAT_FileList_t gBatFileList;
BAT_TSRecord_t gBatTsRecord;

void info_service_parse(uint8_t *pBuf, uint8_t descriptor_length, uint16_t transport_stream_id)
{
    uint8_t table_id = 0;
    uint8_t file_loop_length = 0;
    uint8_t segment_number = 0;
    uint16_t PID = 0;
    uint8_t last_segment_number = 0;
    uint8_t ver_num = 0;
    uint16_t table_id_ext = 0;
    uint8_t last_section_num = 0;
    uint16_t file_NO = 0;
    uint8_t *data = NULL;
    uint8_t i = 0;

    data = pBuf;

    file_loop_length = descriptor_length - 11;
    segment_number = (data[0]&0xe0)>>5;
    PID = (uint16_t)(data[0]&0x1f)<<8 | data[1];
    table_id = data[2];
    last_segment_number = (data[3]&0xe0) >> 5;
    ver_num = data[3] & 0x1f;
    data += 4;

    if(gBatTsRecord.valide_desc_num > BAT_MAX_DESC)
    {
        return;
    }

    for(i = 0; i < gBatTsRecord.valide_desc_num; i++)
    {
        if(gBatTsRecord.TS_ID_record[i] == transport_stream_id)
        {
            if(1 == gBatTsRecord.segment_record[segment_number])
            {
                return;
            }
        }
    }

    gBatTsRecord.TS_ID_record[gBatTsRecord.valide_desc_num] = transport_stream_id;
    gBatTsRecord.valide_desc_num ++;

    gBatTsRecord.last_segment_num = last_segment_number;
    gBatTsRecord.segment_record[segment_number] = 1;


    gBatFileList.segment_number = segment_number;
    gBatFileList.transport_id = transport_stream_id;
    gBatFileList.PID = PID;
    gBatFileList.last_segment_number = last_segment_number;
    gBatFileList.ver_num = ver_num;

    while(file_loop_length > 0)
    {
        table_id_ext = (uint16_t)data[0]<<8 | data[1];
        last_section_num = data[2];
        data += 3;

        file_NO = gBatFileList.file_num;
        gBatFileList.file[file_NO].table_id_ext = table_id_ext;
        gBatFileList.file[file_NO].last_section_num = last_section_num;
        gBatFileList.file_num++;
        file_loop_length -= 3;
    }
}

void linkage_descriptor(void *handle, void *filter, uint8_t chVersionNumber, uint8_t *pBuf)
{
    int16_t bouquet_descriptors_length = 0;

    uint8_t descriptor_tag = 0;
    uint8_t descriptor_length = 0;

    uint16_t transport_stream_id = 0;
    uint16_t original_network_id = 0;
    uint16_t service_id = 0;
    uint8_t linkage_type = 0;

    uint8_t *data = NULL;

    if( NULL != pBuf )
    {
        data = pBuf;

        bouquet_descriptors_length = ((int16_t)data[0]<<8|data[1])&0x0FFF;
        data +=2;
        while(bouquet_descriptors_length>0)
        {
            descriptor_tag = data[0];
            descriptor_length = data[1];
            data += 2;
            if( descriptor_length ==0)
            {
                break;
            }
            bouquet_descriptors_length -= (descriptor_length+2);
            switch( descriptor_tag )
            {
                case 0x4A:
                    transport_stream_id = (uint16_t)data[0]<<8|data[1];
                    original_network_id = (uint16_t)data[2]<<8|data[3];
                    service_id = (uint16_t)data[4]<<8|data[5];
                    linkage_type = data[6];
                    data += 7;
                    switch(linkage_type)
                    {
                        case 0x80:
                            if((chVersionNumber != g_BatISVersion))
                            {
                                g_BatISVersion = chVersionNumber;
                                memset(&gBatTsRecord, 0, sizeof(gBatTsRecord));
                                memset(&gBatFileList, 0, sizeof(gBatFileList));
                            }
                            info_service_parse(data, descriptor_length, transport_stream_id);
                            data = data - 7 + descriptor_length;
                            break;
                        default:
                            data = data - 7 + descriptor_length;
                            break;
                    }
                    break;
                default:
                    data += descriptor_length;
                    break;
            }
        }

        //开始解析数据
        if (gBatFileList.file_num > 0)
        {
            com_is_start();
            //创建文件过滤器
            Demux_CreateFilterEx( handle, gBatFileList.PID, 0x90, 0xff, 0, 0, 4096, com_filter, 0);
            Demux_DestroyFilter(handle, filter);
            qDebug()<<gBatFileList.file_num<<"bat filter finish!";
        }
    }
}

void bat_filter(void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam )
{
    uint8_t* pBuffReadPtr;
    int16_t wSectionLength = 0;
    uint8_t  chTableId = 0;
    uint8_t  chVersionNumber = 0;
    uint8_t  chSectionNumber = 0;
    uint8_t  chLastSectionNumber = 0;
    uint16_t wBouquetId = 0;

    handle = handle; filter = filter; size = size; lParam = lParam;

    Q_ASSERT(NULL != buf);

    /*init*/
    pBuffReadPtr = buf;

    /*check table id*/
    chTableId = pBuffReadPtr[0];
    if(0x4a != chTableId)
    {
        return ;
    }

    /*check section length*/
    wSectionLength = (((uint16_t)pBuffReadPtr[1]<<8) | (uint16_t)pBuffReadPtr[2]) & 0x0fff;

    if(wSectionLength > 1021)
    {
        return ;
    }

    wBouquetId = (uint16_t)pBuffReadPtr[3]<<8 | pBuffReadPtr[4];
    chVersionNumber = (pBuffReadPtr[5]>>1) & 0x1f;
    chSectionNumber = pBuffReadPtr[6];
    chLastSectionNumber = pBuffReadPtr[7];

    if(0x7011 != wBouquetId)
    {
        return;
    }

    pBuffReadPtr += 8;
    linkage_descriptor(handle, filter, chVersionNumber, pBuffReadPtr);
}


void DataCastInit(void)
{
    g_BatISVersion = 0;
    memset(&gBatTsRecord, 0, sizeof(gBatTsRecord));
    memset(&gBatFileList, 0, sizeof(gBatFileList));
    g_bPaseOk = false;
}
