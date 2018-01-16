#include "sction.h"

/* 在section中查找transport_stream_id相对应的频点信息 */
void *get_network(unsigned char *buf, uint16_t transport_stream_id, int32_t size)
{
	unsigned char *p = buf, *temp = NULL;
	uint16_t ts_id = 0;
	uint16_t network_des_len = 0;
	uint16_t transport_des_len = 0;
	uint16_t descriptor_tag = 0;
	uint16_t descript_length = 0;
	int flag = 0;
	byte *presult = NULL;
	
	network_des_len = ((p[8] & 0x0f) << 8) | p[9];
	p += network_des_len + 12;
	
    while(p < buf + size - 4)
	{
		temp = p;
		ts_id = (temp[0] << 8) | temp[1];
		transport_des_len = ((temp[4] & 0x0f) << 8) | temp[5];
		
		if(ts_id == transport_stream_id)
		{
			/*存储相应的频点信息*/
			while(temp < p + 6 + transport_des_len)
			{
				descriptor_tag = temp[6];
				descript_length = temp[7];
				if(descriptor_tag == 0x44)
				{
					
                    //presult = malloc(11 * sizeof(byte));
                    presult = new byte[11];
					presult[0] = buf[3];    //network_id的信息
					presult[1] = buf[4];
					
					presult[2] = temp[8];   //frequecy的信息
					presult[3] = temp[9];
					presult[4] = temp[10];
					presult[5] = temp[11];
					
					presult[6] = temp[14];  //modulation的信息
					
					presult[7] = temp[15];  //sysbol_rate的信息
					presult[8] = temp[16];
					presult[9] = temp[17];
					presult[10] = temp[18] & 0xf0;
					flag = 1;
					break;
					
				}
				temp += descript_length + 6;
			}
		}
		if(flag == 1) //找到了，则返回
		{
			break;
		}
		p += 6 + transport_des_len;
	}
	return presult;
}

/* 在sdt_section中对应的service_id的节目名称 */
void *get_service_name(unsigned char *buf, uint16_t service_id, uint16_t transport_stream_id, int32_t size)
{
	unsigned char *p = buf, *temp = NULL;
	uint16_t ser_id = 0;
	uint16_t descriptor_tag = 0;
	uint16_t descriptor_loop_length = 0;
	uint16_t service_name_len = 0, service_provider_nlen = 0;	
	uint16_t ts_id = 0;
	int i, flag = 0;
	byte *presult = NULL;
	p += 11;
	
	ts_id = (buf[3] << 8) | buf[4];
	if(ts_id == transport_stream_id)
	{
		while(p < buf + size - 4)
		{
			
			temp = p;
			descriptor_loop_length = ((temp[3] & 0x0f) << 8) | temp[4]; 
			ser_id = (temp[0] << 8) | temp[1];
			if(ser_id == service_id)  //section中的service_id与要找的相等
			{
				while(temp < p + 5 + descriptor_loop_length)
				{
					descriptor_tag = temp[5];
					
					if(descriptor_tag == 0x48)  //判断描述的的类型
					{
						service_provider_nlen = temp[8];
						temp += 9 + service_provider_nlen;
						service_name_len = temp[0];
                        //presult = malloc((service_name_len + 2) * sizeof(byte));
                        presult = new byte[service_name_len + 2];
						
						for(i = 0; i <= service_name_len; i++)  //存储名称的长度及名称
						{
							presult[i] = temp[i];
						}
						presult[i] = '\0';
						flag = 1;
						break;
					}
					
					temp += 1 + service_name_len;
				}
				if(flag == 1)
				{
					break;
				}
			}
			p += descriptor_loop_length + 5; 
		}
	}
	
	return presult;
}

/* 在pmt_section中，查找video_pid和audio_pid,(无需判断pmt_pid,经过滤后pmt_id已一一对应) */
void *get_program_msg(unsigned char *buf, int32_t size) 
{
	uint16_t pro_info_length = 0, ES_info_length = 0;
	unsigned char *p = buf;
    uint16_t stream_type = 0;
	int i = 0;
	byte *presult = NULL;
	
	pro_info_length = ((p[10] & 0x0f) << 8) | p[11];
	p += 12 + pro_info_length;
    //presult = malloc(20 * sizeof(byte));
    presult = new byte[20];
	while(p < buf + size - 4)
	{
		stream_type = p[0];
		/*如果流类型为音频或视频则存储*/
		if(stream_type == 1 || stream_type == 2 || stream_type == 3 || stream_type == 4)
		{
			presult[i] = p[0];
			presult[i + 1] = p[1] & 0x1f;
			presult[i + 2] = p[2];
			i += 3;
		}
		ES_info_length = ((p[3] & 0x0f) << 8) | p[4];
		p += 5 + ES_info_length;
	}
	presult[i] = '\0';
	return presult;
}

/* 提取pat中的相关信息，包括transport_stream_id,program_number, pmt_pid */
void *get_pat_msg(unsigned char *buf, int32_t size)
{
	ppat pat_msg = NULL;
	uint16_t program_number = 0, pro_map_pid = 0;
	uint16_t transport_stream_id = 0;
	unsigned char *p = buf;
	uint16_t pro_num = 0;
	
    //pat_msg = (ppat)malloc(sizeof(pat));
    pat_msg = new pat();
	
	/* 提取transport_stream_id */
	transport_stream_id = (p[3] << 8 | p[4]);
	pat_msg->transport_stream_id = transport_stream_id;
	p += 8;
	
	/* 提取program_number和pmt_pid */
	while(p < buf + size - 4)
	{
		program_number = (p[0] << 8 | p[1]);
		if(program_number != 0) //寻找MAP_ID
		{
			pro_map_pid = ((p[2] & 0x1f) << 8) | p[3];
			pat_msg->program_number[pro_num] = program_number;
			pat_msg->pmt_pid[pro_num] = pro_map_pid;
			pro_num++;
		}
		p += 4;
	}
	pat_msg->size = pro_num;
	
	return pat_msg;
}
