/*******************************************************************************
* 						   CONFIDENTIAL
*        Hangzhou GuoXin Science and Technology Co., Ltd.
*                      (C)2008, All right reserved
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
* File Name :	uial_info_service.h
* Author    : 	wuwj
* Project   :   GX1501
* Type      :   Driver
--------------------------------------------------------------------------------
* Purpose   :	data definition statement of information service middle layer
--------------------------------------------------------------------------------
* Release History:
  VERSION	Date			  AUTHOR         Description
   0.1  	2008.3.18	      wuwj   	     creation
*******************************************************************************/
#ifndef _UIAL_INFO_SERVICE_H
#define _UIAL_INFO_SERVICE_H

/*******************************************************************************
Include files List
*******************************************************************************/
#include "com_is_buf_type.h"
#include "com_is_buf.h"
#include "type_.h"

/*******************************************************************************
macro function
*******************************************************************************/

/*******************************************************************************
Enumerate List
*******************************************************************************/
#define DATACAST_BMP_PATH				("C:/")

/*元素类型*/
typedef enum XmlElementType
{	
	ELEMENT_NONE = 0,
	ELEMENT_TEXT = 1,	//文本    
	ELEMENT_IMAGE,		//图	
	ELEMENT_TABLE,		//表格	
	ELEMENT_MENU,		//菜单	
}XmlElementType_e;

/*******************************************************************************
structure List
*******************************************************************************/

/*图片调色板参数结构体*/
typedef struct LogPalette_s
{
	uint32_t  NumEntries;                   //调色板颜色个数
	int8_t   HasTrans;                     //透明度,默认=0
	uint32_t  *pPalEntries;				   //调色板数据入口地址
} LogPalette_t; 

/*XML解析数据入口结构体*/
typedef struct XmlRoot_s
{	
	int8_t *m_pchBackgroudName;				//IS背景文件名
	uint32_t m_nBackgroudAddr;				//背景文件的数据首地址
	uint32_t m_nBackgroudLength;				//背景文件的数据长度
	XmlElementType_e m_NextElementType;	//下一个元素的类型	
	uint32_t *m_pnNextElement;				//下一个元素的数据结构指针
} XmlRoot_t , *pXmlRoot_t;

/*文本元素的结构体*/
typedef struct ElementText_s
{
	int8_t *m_pchHref;						//链接网址，如"098_233_007.xml"
	int8_t *m_pchTextContext;				//文本内容
	XmlElementType_e m_NextElementType;	//下一个元素的类型	
	uint32_t *m_pnNextElement;				//下一个元素的数据结构指针
} ElementText_t , *pElementText_t;

/*图片元素的结构体*/
typedef struct ElementImage_s
{	
	/*以下7个数据次序,数据类型不能任意更改.因GUI要使用到*/
	uint16_t m_nWith;						//图宽度
	uint16_t m_nHeight;						//图高度	
	uint16_t m_nBytesPerLine;				//每行有多少字节,默认=m_nWith
	uint16_t m_nBitsPerPixel;				//几位色
	uint8_t  *m_pImageAddr;					//图片数据首地址
	LogPalette_t *m_pPalletteStruct;	//调色板结构指针
	uint8_t  *m_pMethods;
	/*---------------------------------*/
	uint16_t m_nTop_left_x;					//坐标，X	
	uint16_t m_nTop_left_y;					//坐标，Y
	int8_t *m_pchHref;						//链接网址，如"098_233_007.xml"	
	int8_t *m_pchImageName;					//图片文件名	
	uint32_t m_nImageLength;					//图片数据长度
										//(数据是从左到右，从下到上的像素数据)
	XmlElementType_e m_NextElementType;	//下一个元素的类型	
	uint32_t *m_pnNextElement;				//下一个元素的数据结构指针
} ElementImage_t , *pElementImage_t;

/*表格元素的结构体*/
typedef struct ElementTable_s
{
	uint8_t m_chCol;										//表列数	
	uint8_t m_chRow;										//表行数
	struct ElementTableTitle_s *m_pTableTitle;		//表标题指针
	struct ElementTableContext_s *m_pTableContext;	//表内容指针
	XmlElementType_e m_NextElementType;				//下一个元素的类型
	uint32_t *m_pnNextElement;							//下一个元素的数据结构指针
} ElementTable_t , *pElementTable_t;	
/*表标题的结构体*/
typedef struct ElementTableTitle_s
{
	uint8_t m_chColWidth;					//列宽度，单位：每汉字的像素宽	
	int8_t *p_chTitleName;					//标题名	
	struct ElementTableTitle_s *m_pNextTableTitle;//横向数据的单向列表
												  //(注意：横向的)
} ElementTableTitle_t , *pElementTableTitle_t;
/*表内容的结构体*/
typedef struct ElementTableContext_s
{
	int8_t *m_pchHref;								//链接网址，如"098_233_007.xml"	
	int8_t *m_pchTextContext;							  //文本内容	
	struct ElementTableContext_s *m_pNextTableContext;//竖向数据的单向列表
													  //(注意：竖向的)
} ElementTableContext_t , *pElementTableContext_t;

/*菜单元素的结构体*/
typedef struct ElementMenu_s
{
	int8_t *m_pchMenuName;					//菜单名
	XmlElementType_e m_ChildElementType;//当前菜单中的元素类型	
	uint32_t *m_pnChildElement;				//当前菜单中的元素指针
	XmlElementType_e m_NextElementType;	//下一个元素的类型	
	uint32_t *m_pnNextElement;				//下一个元素的数据结构指针
} ElementMenu_t , *pElementMenu_t;

/*******************************************************************************
variable List
*******************************************************************************/



/*******************************************************************************
function
*******************************************************************************/
int32_t gx_parse_XML(int8_t *xmlbuf , uint32_t xmlsize , XmlRoot_t **pISxml);
uint8_t gx_enter_url(int8_t *chXmlfile);
uint8_t gx_enter_IS(void);
uint8_t gx_exit_IS(void);
extern uint8_t gx_find_IS_file(int8_t * chfilename , FileType_t filetype , FileList_t **pFile);

void uial_info_service_loading(void);

extern int create_sevice_second(void);
extern int create_sevice_third(void);

#endif//#ifndef _INFO_SERVICE_H





