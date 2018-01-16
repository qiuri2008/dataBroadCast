#include "dvb.h"

/* 用来存放指向section的指针 */
section *ppat_section[256];
section *ppmt_section[10][256];
section *pnit_section[256];
section *psdt_section[256];

/* 用来标记对应该的section是否找到, 元素值为0时表示没找到，为1时表示已找到 */
static short hash_pat[256] = {0};
static short hash_pmt[10][256];
static short hash_nit[256] = {0};
static short hash_sdt[256] = {0};

/* 用来标记已找到的section的数目 */
static short num_pat_section = 0;
static short num_pmt_section[10] = {0};
static short num_nit_section = 0;
static short num_sdt_section = 0;

/* 用来存放一个pat的transport_stream_id和节目等信息 */
ppat pat_msg = NULL;

/* 创建并初始化一个dvb结构体 */
void *dvb_create()
{
    //pdvb pTmp = (pdvb)malloc(sizeof(dvb));  //分配空间
    pdvb pTmp = new dvb();
	
	pTmp->m_pDemux  = NULL;
	memset(pTmp->m_pFilter, 0, sizeof(pTmp->m_pFilter));
	return pTmp;
}
/* 销毁一个dvb结构体 */
void dvb_destroy(pdvb ths)
{
    if(NULL != ths)
	{
        //free(ths);
        delete ths;
		ths = NULL;
	}
}
/* pat表的回调函数 */
void pat_filter( void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam )
{
    unsigned char section_number;
	dvb *pTdvb = ( dvb *)lParam;
	psection pat_section = NULL; 
    //unsigned char *pmt_pid;
	pdvb pPmtdvb = NULL;
	int i = 0;
	uint32_t crc321, crc322;
	
	/* CRC校验 */
	crc321 = GetCrc32(buf, size-4);
	memcpy(&crc322, buf+size-4, 4);
    //if(crc322 != htonl(crc321))
    //{
        //printf("CRC校验出错!\n");
        //return;
    //}
	
	/* 检验current_net_indicator的值,为1时该sction有效,无效丢弃 */
	if((buf[5] & 0x1) == 1)
	{
		section_number = buf[6];
		if(0 == hash_pat[section_number])  //判断该section是否已收集
		{
			hash_pat[section_number] = 1; 
			
            //pat_section = (psection)malloc(sizeof(section));
            pat_section = new section();
			pat_section->size = size;                        //将section的长度赋给section成员size
			memcpy(&pat_section->cInfo, buf, size);          //将section的信息存入到pat_section->cInfo中
			
			ppat_section[num_pat_section++] = pat_section;   //将section结构体指针存放到数组中,并把计数器加1
			
			if(num_pat_section == buf[7] + 1)                //如果所有的pat_section已收集到
			{
                //printf("pat 已收集满\n");
				pat_msg = (ppat)get_pat_msg(buf, size);      //获取pat相关信息
				
				/* 对每一个PMT，创建Filter */
                pPmtdvb = (pdvb)dvb_create();
				pPmtdvb->size = pat_msg->size;
				for(i = 0; i < pat_msg->size; i++)
				{
                    //pmt_pid = malloc(2 * sizeof(byte));
					pPmtdvb->m_pDemux = pTdvb->m_pDemux;
					pPmtdvb->m_pFilter[i] = Demux_CreateFilterEx( pTdvb->m_pDemux, pat_msg->
						pmt_pid[i], 0x02, 0xff, 0, 0, 1024, pmt_filter, (uint32_t)pPmtdvb );
				}
				
				Demux_DestroyFilter(handle, filter);        //销毁pat的过滤器
			}
		}
	}
}
/* pmt_section回调函数 */
void pmt_filter( void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam )
{
	
	dvb *pTdvb = ( dvb *)lParam;
	psection pmt_section = NULL; 
    //byte *presult = NULL;
    unsigned char section_number;
	unsigned char *p = buf;
    unsigned short i = 0, total_cottection = 0;
	uint32_t crc321, crc322;
	
	/* CRC校验 */
	crc321 = GetCrc32(buf, size-4);
	memcpy(&crc322, buf+size-4, 4);
    //if(crc322 != htonl(crc321))
    //{
        //printf("CRC校验出错!\n");
        //return;
    //}
	
	for(i = 0; i < pat_msg->size; i++)
	{
		if (filter == pTdvb->m_pFilter[i]) //判断该filter是否为第i个pmt的过滤器。
		{
			/* 检验current_net_indicator的值,为1时该sction有效，无效丢弃 */
            if(((p[5] & 0x01) == 1))
			{
				section_number = p[6];
				if(0 == hash_pmt[i][section_number])
				{
					hash_pmt[i][section_number] = 1;
					
					/* 存储pmt_section */
                    //pmt_section = (psection)malloc(sizeof(section));
                    pmt_section = new section();
					pmt_section->size = size;
					memcpy(&pmt_section->cInfo, buf, size);
					
					ppmt_section[i][num_pmt_section[i]++] = pmt_section;
					
					/* 某一个节目的pmt收集已满，销毁过滤器 */
					if(num_pmt_section[i] == p[7] + 1)
					{
                        //printf("pmt %d 已收集满\n", i);
						total_cottection++;
						Demux_DestroyFilter(handle, filter);
					}
				}
			}
			break;
		}
	}
	/* 所有的pmt全部收集满，销毁dvb */
	if(total_cottection == pat_msg->size)
	{	
		dvb_destroy(pTdvb);
	}
}

/*sdt_section回调函数*/
void sdt_filter( void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam )
{
	
    //dvb *pTdvb = ( dvb *)lParam;
    lParam = lParam;
    unsigned char section_number;
	psection sdt_section = NULL; 
	uint32_t crc321, crc322;
	
	/* CRC校验 */
	crc321 = GetCrc32(buf, size-4);
	memcpy(&crc322, buf+size-4, 4);
    //if(crc322 != htonl(crc321))
    //{
        //printf("CRC校验出错!\n");
        //return;
    //}

	/* 检验current_net_indicator的值,为1时该sction有效，无效丢弃 */
	if((buf[5] & 0x1) == 1)
	{
		section_number = buf[6];
		if( 0 == hash_sdt[section_number] )
		{
			hash_sdt[section_number] = 1;

			/* 存储sdt_section */
            //sdt_section = (psection)malloc(sizeof(section));
            sdt_section = new section();
			sdt_section->size = size;
			memcpy(&sdt_section->cInfo, buf, size);
			
			psdt_section[num_sdt_section++ ] = sdt_section;
			
			/* sdt收集已满，销毁过滤器 */
			if(num_sdt_section == buf[7] + 1)
			{
                //printf("sdt 已收集满\n");
				Demux_DestroyFilter(handle, filter);
			}
		}
	}
}
/*nit_section的回调函数*/
void nit_filter( void *handle, void *filter, unsigned char *buf, int32_t size, uint32_t lParam )
{
    //dvb *pTdvb = ( dvb *)lParam;
    lParam = lParam;
    unsigned char section_number;
	psection nit_section = NULL; 
    //int i = 0;
	uint32_t crc321, crc322;
	
	/* CRC校验 */
	crc321 = GetCrc32( buf, size-4 );
	memcpy( &crc322, buf+size-4, 4 );
    //if( crc322 != htonl(crc321) )
    //{
        //printf("CRC校验出错!\n");
        //return;
    //}
	
	section_number = buf[6];
	if( 0 == hash_nit[section_number] )
	{
		/*检验current_net_indicator的值,为1时该sction有效*/
		if((buf[5] & 0x1) == 1)
		{
			hash_nit[section_number] = 1;

			/* 存储nit_section */
            //nit_section = (psection)malloc(sizeof(section));
            nit_section = new section();
			nit_section->size = size;	
			memcpy(&nit_section->cInfo, buf, size);	
			
			pnit_section[num_nit_section++] = nit_section;

			/* nit_section收集满，销毁过滤器 */
			if(num_nit_section == buf[7] + 1)
			{
                //printf("nit 已收集满\n");
				Demux_DestroyFilter(handle, filter);
			}
		}
	}
}

/* 释放空间 */
void free_all()
{
	int i = 0, j = 0;

	/* pat_section存放空间隔*/
	for(i = 0; i < num_pat_section; i++)
	{
		if(ppat_section[i] != NULL)
		{
            //free(ppat_section[i]);
            delete ppat_section[i];
			ppat_section[i] = NULL;
		}
	}

	/* nit_section存放空间 */
	for(i = 0; i < num_nit_section; i++)
	{
		if(ppat_section[i] != NULL)
		{
            //free(pnit_section[i]);
            delete pnit_section[i];
			pnit_section[i] = NULL;
		}
	}

	/* sdt_section存放空间 */
	for(i = 0; i < num_sdt_section; i++)
	{
		if(ppat_section[i] != NULL)
		{
            //free(psdt_section[i]);
            delete psdt_section[i];
			psdt_section[i] = NULL;
		}
	}

	/* pmt_section存放空间 */
    if(pat_msg)
	for(i = 0; i < pat_msg->size; i++)
	{
		for(j = 0; j < num_pmt_section[i]; i++)
		{
			if(ppmt_section[i][j] != NULL)
			{
                //free(ppmt_section[i][j]);
                delete ppmt_section[i][j];
				ppmt_section[i][j] = NULL;
			}
			
		}
	}

	/* pat信息结构体空间 */
	if(pat_msg != NULL)
	{
        //free(pat_msg);
        delete pat_msg;
		pat_msg = NULL;
	}
	
}

/* 输出结果 */
void print_reslut()
{
	
	int i = 0, j = 0;
	psection pat1 = ppat_section[0];
	byte *pnit = NULL, *psdt = NULL, *psdt1 = NULL, *ppmt = NULL;
	double frequency = 0, symbol_rate = 0;
    //char qam[10][20] = {"not defined","16 QAM","32 QAM","64 QAM","128 QAM","256 QAM"};
	unsigned short network_id = 0, v_pid = 0, a_pid = 0;
	
	
	if(pat_msg == NULL)
	{
        //pat_msg = (ppat)malloc(sizeof(pat));
        pat_msg = new pat();
        pat_msg = (ppat)get_pat_msg(pat1->cInfo, pat1->size);
	}
	
	/*找到频点信息*/
	for(i = 0; i < num_nit_section; i++)
	{
        pnit = (byte *)get_network(((psection)pnit_section[j])->cInfo, pat_msg->transport_stream_id ,986);
		if(pnit != NULL)
		{
			break;
		}
	}
	
	if(pnit != NULL)
	{
		network_id = (pnit[0] << 8) | pnit[1];
		
		/*将BCD码转化为十进制数*/
		for(i = 2; i <= 5; i++)
		{
			frequency = frequency * 100 + ((pnit[i] & 0xf0) >> 4) * 10 + (pnit[i] & 0x0f);
		}
		frequency /= 10000;
		
		for(i = 7; i <= 9; i++)
		{
			symbol_rate = symbol_rate * 100 + ((pnit[i] & 0xf0) >> 4) * 10 + (pnit[i] & 0x0f);
		}
		symbol_rate = symbol_rate * 10 + ((pnit[10] & 0xf0) >> 4);
		symbol_rate /= 10000;
	}
	
    //printf("service_id\tservice_name\tPMT_pid\tts_id\tvideo_pid\taudio_pid\n");
	for(i = 0; i < pat_msg->size; i++)
	{
		v_pid = 0; 
		a_pid = 0;

		/* 查找对应的节目名称 */
		for(j = 0; j < num_sdt_section; j++)
		{
			psdt = (byte *)get_service_name(((psection)psdt_section[j])->cInfo, 
				pat_msg->program_number[i], pat_msg->transport_stream_id, 
				((psection)psdt_section[j])->size);
			if(psdt != NULL)
			{
				break;
			}
		}

		/* 查找对应的video_pid, audio_pid  */
		for(j = 0; j < num_pmt_section[i]; j++)
		{
            ppmt = (byte *)get_program_msg(((psection)ppmt_section[i][j])->cInfo,
				((psection)ppmt_section[i][j])->size);
			if(ppmt != NULL)
			{
				break;
			}
		}

		if(ppmt != NULL)
		{
			/* 将得到的信息进行转化，存入到v_pid，a_pid中 */
			for(j = 0; ppmt[j] != '\0'; j += 3)
			{
				if(ppmt[j] == 1 || ppmt[j] == 2)
				{
					v_pid = (ppmt[j + 1] << 8) | ppmt[j + 2];
				}else if(ppmt[j] == 3 || ppmt[j] == 4)
				{
					a_pid = (ppmt[j + 1] << 8) | ppmt[j + 2];
				}
			}
		}
		
		/*按格式，输出信息*/
        //qDebug()<<pat_msg->program_number[i];
		if(psdt != NULL)
		{
			psdt1 = psdt + 1;
			if(psdt[0] < 8)
			{
                ;//printf("\t%s\t", psdt1);
			}
			else{
                ;//printf("\t%s", psdt1);
			}
		}else{
            ;//printf("\tnot found");
		}
        //printf("\t%d\t%d\t", pat_msg->pmt_pid[i], pat_msg->transport_stream_id);
		if(v_pid == 0)
		{
           ; //printf("not found\t");
		}else{
            ;//printf("0x%02x (%d)\t", v_pid, v_pid);
		}
		if(a_pid == 0)
		{
            ;//printf("not found\n");
		}else{
            ;//printf("0x%02x (%d)\n", a_pid, a_pid);
		}
	}
	
	if(pnit != NULL)
	{
		/* 输出频点相关信息 */
        //printf("\nnetwork_id\tfrequency\tsymbol_rate\tmodulation\n");
		for(i = 0; i < pat_msg->size; i++)
		{
            ;//printf("    %d    \t%.4lfmhz\t%.4lfmbl/s\t%s\n",
                //network_id, frequency, symbol_rate, qam[pnit[6]]);
		}
	}else{
        ;//printf("\n没有找到频点相关信息！\n\n");
	}
	
}
