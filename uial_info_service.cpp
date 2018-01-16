/* Includes ----------------------------------------------------------------- */
#include "uial_info_service.h"
#include "uial_xmlparser.h"
#include <stdlib.h>

//#include "sys_printf.h"
/* Private Types ------------------------------------------------------------ */
/*BMP文件头*/
typedef struct tagBITMAPFILEHEADER { 
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;
/*位图信息头*/
typedef struct tagBITMAPINFOHEADER{
	uint32_t biSize;
	uint32_t biWidth;
	uint32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	uint32_t biXPelsPerMeter;
	uint32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} BITMAPINFOHEADER,*PBITMAPINFOHEADER;
/* Private Constants -------------------------------------------------------- */

/* Private Variables (static)------------------------------------------------ */ 

/* Global Variables --------------------------------------------------------- */
static FileList_t *filelist = NULL;    //文件
XmlRoot_t *pFirstPage  = NULL;  //第一级页面数据结构指针
XmlRoot_t *pSecondPage = NULL;  //第二级页面数据结构指针
XmlRoot_t *pThirdPage  = NULL;  //第三级页面数据结构指针
/* Private Macros ----------------------------------------------------------- */
//#define ELEMENT_MENU "cdbs:RootMenu"		//菜单
//"cdbs:Background"
//"cdbs:Name"
//"cdbs:Text"								//文本  
//"cdbs:Href"
//"cdbs:Image"								//图	
#define REV_ORDER4B(a) (a)//(((a&0x000000FF)<<24) | ((a&0x0000FF00)<<8) | ((a&0x00FF0000)>>8) | ((a&0xFF000000)>>24))
/*begin 高地位翻转错误 modify by uu_dou 20121225*/
#define REV_ORDER2B(b) (b)//(((b&0x00FF)<<8) | ((b&0xFF00)>>8))
/*end by uu_dou 20121225*/

/* Private Function prototypes ---------------------------------------------- */
/*******************************************************************************
释放资源
*******************************************************************************/
uint8_t gx_free_is_xml(XmlRoot_t * page)
{
	void *pElementaddr = NULL;
	void *pMenuElementaddr = NULL;
	void *pChileElementaddr = NULL;
	void *pNextElementaddr = NULL;
    XmlElementType_e ElementType = ELEMENT_NONE , NextElementType = ELEMENT_NONE;

	pNextElementaddr = page->m_pnNextElement;
	ElementType = page->m_NextElementType;
	while(pNextElementaddr)
	{
		if(ELEMENT_TEXT == ElementType)
		{     
			pElementaddr = pNextElementaddr;
			pNextElementaddr = ((ElementText_t *)pElementaddr)->m_pnNextElement;
			NextElementType = ((ElementText_t *)pElementaddr)->m_NextElementType;

			if(((ElementText_t *)pElementaddr)->m_pchHref)
			{
				free(((ElementText_t *)pElementaddr)->m_pchHref);
			}
			if(((ElementText_t *)pElementaddr)->m_pchTextContext)
			{
				free(((ElementText_t *)pElementaddr)->m_pchTextContext);
			}
			if(pElementaddr)
			{
				free(pElementaddr);
			}
		}	
		else if(ELEMENT_IMAGE == ElementType)
		{
			pElementaddr = pNextElementaddr;
			pNextElementaddr =((ElementImage_t *)pElementaddr)->m_pnNextElement;
			NextElementType = ((ElementImage_t *)pElementaddr)->m_NextElementType;

			if(((ElementImage_t *)pElementaddr)->m_pchHref)
			{
				free(((ElementImage_t *)pElementaddr)->m_pchHref);
			}
			if(((ElementImage_t *)pElementaddr)->m_pchImageName)
			{
				free(((ElementImage_t *)pElementaddr)->m_pchImageName);
			}
			if(pElementaddr)
			{
				free(pElementaddr);
			}

		}
		else if(ELEMENT_TABLE == ElementType)
		{
			pElementaddr = pNextElementaddr;
			pNextElementaddr = ((ElementTable_t *)pElementaddr)->m_pnNextElement;
			NextElementType = ((ElementTable_t *)pElementaddr)->m_NextElementType;

			pChileElementaddr = ((ElementTable_t *)pElementaddr)->m_pTableTitle;
			while(pChileElementaddr)
			{
				((ElementTable_t *)pElementaddr)->m_pTableTitle = 
					((ElementTable_t *)pElementaddr)->m_pTableTitle->m_pNextTableTitle;
				free(pChileElementaddr);
				pChileElementaddr = ((ElementTable_t *)pElementaddr)->m_pTableTitle;
			}

			pChileElementaddr = ((ElementTable_t *)pElementaddr)->m_pTableContext;
			while(pChileElementaddr)
			{
				((ElementTable_t *)pElementaddr)->m_pTableContext = 
					((ElementTable_t *)pElementaddr)->m_pTableContext->m_pNextTableContext;
				free(pChileElementaddr);
				pChileElementaddr = ((ElementTable_t *)pElementaddr)->m_pTableContext;
			}
			if(pElementaddr)
			{
				free(pElementaddr);
			}

		}

		else if(ELEMENT_MENU == ElementType)
		{

			pMenuElementaddr = pNextElementaddr;
			pNextElementaddr = ((ElementMenu_t *)pMenuElementaddr)->m_pnChildElement;
			ElementType = ((ElementMenu_t *)pMenuElementaddr)->m_ChildElementType;

			while(pNextElementaddr)
			{
				if(ELEMENT_TEXT == ElementType)
				{     
					pElementaddr = pNextElementaddr;

					pNextElementaddr = ((ElementText_t *)pElementaddr)->m_pnNextElement;
					NextElementType = ((ElementText_t *)pElementaddr)->m_NextElementType;

					if(((ElementText_t *)pElementaddr)->m_pchHref)
					{
						free(((ElementText_t *)pElementaddr)->m_pchHref);
					}
					if(((ElementText_t *)pElementaddr)->m_pchTextContext)
					{
						free(((ElementText_t *)pElementaddr)->m_pchTextContext);
					}
					if(pElementaddr)
					{
						free(pElementaddr);
					}
				}	
				else if(ELEMENT_IMAGE == ElementType)
				{
					pElementaddr = pNextElementaddr;

					pNextElementaddr =((ElementImage_t *)pElementaddr)->m_pnNextElement;
					NextElementType = ((ElementImage_t *)pElementaddr)->m_NextElementType;

					if(((ElementImage_t *)pElementaddr)->m_pchHref)
					{
						free(((ElementImage_t *)pElementaddr)->m_pchHref);
					}
					if(((ElementImage_t *)pElementaddr)->m_pchImageName)
					{
						free(((ElementImage_t *)pElementaddr)->m_pchImageName);
					}
					if(pElementaddr)
					{
						free(pElementaddr);
					}
				}
				else if(ELEMENT_TABLE == ElementType)
				{
					pElementaddr = pNextElementaddr;

					pNextElementaddr = ((ElementTable_t *)pElementaddr)->m_pnNextElement;
					NextElementType = ((ElementTable_t *)pElementaddr)->m_NextElementType;
					
					pChileElementaddr = ((ElementTable_t *)pElementaddr)->m_pTableTitle;				
					while(pChileElementaddr)
					{
						((ElementTable_t *)pElementaddr)->m_pTableTitle = 
							((ElementTable_t *)pElementaddr)->m_pTableTitle->m_pNextTableTitle;
						free(pChileElementaddr);
						pChileElementaddr = ((ElementTable_t *)pElementaddr)->m_pTableTitle;
					}

					pChileElementaddr = ((ElementTable_t *)pElementaddr)->m_pTableContext;
					while(pChileElementaddr)
					{
						((ElementTable_t *)pElementaddr)->m_pTableContext = 
							((ElementTable_t *)pElementaddr)->m_pTableContext->m_pNextTableContext;
						free(pChileElementaddr);
						pChileElementaddr = ((ElementTable_t *)pElementaddr)->m_pTableContext;
					}
					free(pElementaddr);        
				}
				ElementType = NextElementType;
			}

			pNextElementaddr = ((ElementMenu_t *)pMenuElementaddr)->m_pnNextElement;
			NextElementType = ((ElementMenu_t *)pMenuElementaddr)->m_NextElementType;

			free(((ElementMenu_t *)pMenuElementaddr)->m_pchMenuName);
			free(pMenuElementaddr);            
		}
		ElementType = NextElementType;
	}

	free(page->m_pchBackgroudName);
	free(page);

	return 0;
}


uint8_t gx_get_bmp_info(FileList_t *ISFile , ElementImage_t **element)
{
	uint8_t *BmpFileAddr = NULL;
	BITMAPFILEHEADER bmpFileHead;
	BITMAPINFOHEADER bmpInfoHead;

	BmpFileAddr = (uint8_t *)(ISFile->m_nFileAddr);
	memcpy(&(bmpFileHead.bfType) , BmpFileAddr , 2);
	memcpy(&(bmpFileHead.bfSize),BmpFileAddr + 2 , 4);
	memcpy(&(bmpFileHead.bfOffBits),BmpFileAddr + 10 , 4);	
	memset(&bmpInfoHead , 0x00 , sizeof(BITMAPINFOHEADER));
	memcpy(&bmpInfoHead , (void *)(BmpFileAddr + 14) , sizeof(BITMAPINFOHEADER));
	
    if((('M'<<8) | 'B') == bmpFileHead.bfType)//((('B'<<8) | 'M') == bmpFileHead.bfType)
	{
		(*element)->m_nWith = REV_ORDER4B(bmpInfoHead.biWidth);		
		(*element)->m_nHeight = REV_ORDER4B(bmpInfoHead.biHeight);
		(*element)->m_nBytesPerLine = (*element)->m_nWith;
/*begin 从实际头信息中获取bmp的位数 modify by uu_dou 20121225*/
		(*element)->m_nBitsPerPixel = REV_ORDER2B(bmpInfoHead.biBitCount);//8;
/*end*/
        //APP_Printf("[%s]bit:%#x-->>%#x\n", __FUNCTION__, bmpInfoHead.biBitCount, (*element)->m_nBitsPerPixel);
		(*element)->m_pImageAddr = BmpFileAddr/* + REV_ORDER4B(bmpFileHead.bfOffBits)*/;
		(*element)->m_pPalletteStruct = NULL;
		(*element)->m_pMethods = NULL;
		(*element)->m_nImageLength = 0;
		return 0;
	}
	else
	{
		return 1;
	}
}

/*******************************************************************************
查询指定文件
*******************************************************************************/
uint8_t gx_find_IS_file(int8_t * chfilename , FileType_t filetype , FileList_t **pFile)
{
	FileList_t *file = filelist;
	uint8_t nFilenamelen = (uint8_t)strlen((char*)chfilename);

	while(NULL != file)	
	{
		if(filetype == FILE_MPG)
		{
			if(0 == strcmp((char*)file->m_pchFileName , "001_Background_00.mpg"))
			{
				*pFile = file;
				return 1;
			}		
		}
		if(nFilenamelen == file->m_chFileNameLen)//文件名长度比较
		{
			if(FILE_ALL != filetype)
			{
				if(filetype == file->m_FileType)//文件类型比较
				{
					if(0 == strcmp((char*)chfilename,(char*)file->m_pchFileName))//文件名比较
					{
						*pFile = file;
						return 1;
					}
				}
			}
			else
			{
				if(0 == strcmp((char*)chfilename,(char*)file->m_pchFileName))
				{
					*pFile = file;
					return 1;
				}
			}
		}
		file = file->next;
	}
	return 0;
}

/*******************************************************************************
UNICODE转为UTF8
*******************************************************************************/
int Unicode2Utf8_size(const char* in, int inLength)
{
	int i = 0;
	int totalNum = 0;
	char unicode;
	
	if(in == NULL || inLength <0)
	{
		return -1;
	}
	for(i=0; i<inLength; i++)
	{
		unicode = in[i];
		if(unicode >= 0x0000 && unicode <=0x007f)
		{
			totalNum+=1;
		}
		else if(unicode >= 0x0080 && unicode <=0x07ff)
		{
			totalNum+=2;
		}
		else if(unicode >= 0x0800 && unicode <=0xffff)
		{
			totalNum+=3;
		}
	}
	return totalNum;
}

int Unicode2Utf8(char* out, int outLength, const char* in, int inLength)
{
	int i = 0;
	int outsize = 0;
	char *tmp = out;
	char unicode;
	
	if(out == NULL || in == NULL || inLength <0)
	{
		return -1;
	}
	for(i=0; i<inLength; i++)
	{
		if(outsize>outLength)
		{
			return -1;
		}
		unicode = in[i];
		if(unicode >= 0x0000 && unicode <=0x007f)
		{
			*tmp = unicode;
			tmp+=1;
			outsize += 1;
		}
		else if(unicode >= 0x0080 && unicode <=0x07ff)
		{
			*tmp = 0xc0 | (unicode >> 6);
			tmp+=1;
			*tmp = 0x80 | (unicode & (0xff>>2));
			tmp+=1;
			outsize +=2;
		}
		else if(unicode >= 0x0800 && unicode <=0xffff)
		{
			*tmp = 0xe0 | (unicode >> 12);
			tmp+=1;
			*tmp = 0x80 | (unicode >> 6 & 0x003f);
			tmp+=1;
			*tmp = 0x80 | (unicode & (0xff>>2));
			tmp+=1;
			outsize +=3;
		}
	}
	return outsize;
}


/*******************************************************************************
会动态申请空间而不回收,慎用
*******************************************************************************/
int8_t *XString2Char(XString *strXString)
{
	int8_t *pStr = NULL;
	int32_t Utf8Size = 0;
	int32_t ret = 0;

	if(NULL == strXString)
	{
		return NULL;
	}	
	Utf8Size = Unicode2Utf8_size(strXString->m_chText, strXString->m_nLen);
	if(Utf8Size == -1)
	{
		printf("UTF8 change size error!\n");
		return NULL;
	}
	//pStr = calloc((strXString->m_nLen)+1 , sizeof(int8_t));
    pStr = (int8_t *)calloc(Utf8Size+1 , sizeof(int8_t));
	if(NULL == pStr)
	{
        //APP_Printf("calloc error\n");
	}
	if(NULL == pStr)
	{
		return NULL;
	}
	ret = Unicode2Utf8((char*)pStr, Utf8Size, strXString->m_chText, strXString->m_nLen);
	if(ret != Utf8Size)
	{
		printf("UTF8 change error!\n");
		return NULL;
	}
	//memcpy(pStr , strXString->m_chText , strXString->m_nLen);
	//pStr[strXString->m_nLen] = '\0';
	pStr[Utf8Size] = '\0';
	return pStr;
}

/*******************************************************************************
会动态申请空间而不回收,慎用(table专用)
*******************************************************************************/
int8_t *XString2Char_t(XString *strXString)
{
	int8_t *pStr = NULL;
	int32_t Utf8Size = 0;
	int32_t ret = 0;

	if(NULL == strXString)
	{
		return NULL;
	}	
    if(('\n' == strXString->m_chText[strXString->m_nLen - 1])
    	||('\r' == strXString->m_chText[strXString->m_nLen - 2])) //add 0822
    {
    	Utf8Size = Unicode2Utf8_size(strXString->m_chText, strXString->m_nLen);
		if(Utf8Size == -1)
		{
			printf("UTF8 change size error!\n");
			return NULL;
		}
    	//pStr = calloc(strXString->m_nLen , sizeof(int8_t));
        pStr = (int8_t *)calloc(Utf8Size , sizeof(int8_t));
        if(NULL == pStr)
        {
            //APP_Printf("calloc error\n");
        }
    	if(NULL == pStr)
    	{
    		return NULL;
    	}
    	if('\r' == strXString->m_chText[strXString->m_nLen - 2])//add 0822
    	{	
    		ret = Unicode2Utf8((char*)pStr, Utf8Size, strXString->m_chText, strXString->m_nLen-2);
			if(ret != Utf8Size)
			{
				printf("UTF8 change error!\n");
				return NULL;
			}
  	    	//memcpy(pStr , strXString->m_chText , strXString->m_nLen-2);
	    	//pStr[strXString->m_nLen-2] = '\0';  	
	    	pStr[Utf8Size-2] = '\0';
    	}
    	else
    	{
    		ret = Unicode2Utf8((char*)pStr, Utf8Size, strXString->m_chText, strXString->m_nLen-1);
			if(ret != Utf8Size)
			{
				printf("UTF8 change error!\n");
				return NULL;
			}
	    	//memcpy(pStr , strXString->m_chText , strXString->m_nLen-1);
	    	//pStr[strXString->m_nLen-1] = '\0';
	    	pStr[Utf8Size-1] = '\0';
    	}

    }
    else
    {
    	Utf8Size = Unicode2Utf8_size(strXString->m_chText, strXString->m_nLen);
		if(Utf8Size == -1)
		{
			printf("UTF8 change size error!\n");
			return NULL;
		}
    	//pStr = calloc((strXString->m_nLen)+1 , sizeof(int8_t));
        pStr = (int8_t *)calloc(Utf8Size+1 , sizeof(int8_t));
    			if(NULL == pStr)
        {
            //APP_Printf("calloc error\n");
        }
    	if(NULL == pStr)
    	{
    		return NULL;
    	}
		ret = Unicode2Utf8((char*)pStr, Utf8Size, strXString->m_chText, strXString->m_nLen);
		if(ret != Utf8Size)
		{
			printf("UTF8 change error!\n");
			return NULL;
		}
    	//memcpy(pStr , strXString->m_chText , strXString->m_nLen);
    	//pStr[strXString->m_nLen] = '\0';
    	pStr[Utf8Size] = '\0';
    }
	return pStr;
}


/*******************************************************************************
字符串比较,如果strXString == strChar返回1,不相等返回0
*******************************************************************************/
uint8_t gx_strsame(XString strXString , int8_t *strChar)
{
	int32_t i;

    if(strXString.m_nLen == strlen((char*)strChar))
	{
		for(i = 0 ; i < strXString.m_nLen ; i++)
		{
			if(strXString.m_chText[i] != strChar[i])
			{
				return 0;
			}
		}
		return 1;
	}
	return 0;
}


/*******************************************************************************
获取chAttrName指定的属性值
*******************************************************************************/
XString *getAttrValue(XAttr *m_pAttr , int8_t * chAttrName)
{
	while(m_pAttr != NULL)
	{
		if(gx_strsame(m_pAttr->m_strAttrName , chAttrName))
		{
			return(&m_pAttr->m_strAttrValue);
			break;
		}
		m_pAttr = m_pAttr->m_pNextAttr;
	}
	return NULL;
}

/*******************************************************************************
TABELE元素赋值
*******************************************************************************/
int8_t gx_table_element_evaluate( XNode *pXmlNode 
						  , XmlElementType_e **pNextElementType
						  , uint32_t ***pNextElementAddr)
{
	XString *strAttrString = NULL;
	int8_t *chAttrString = NULL;
	ElementTable_t *element = NULL;
	XNode *pChildNode = NULL;
	XNode *pUnitNode = NULL; //TABLE中最小单元节点
	ElementTableTitle_t *pTitle = NULL; 
	ElementTableTitle_t *pTableTitle = NULL; 
	ElementTableContext_t *pContext = NULL;
	ElementTableContext_t *pTableContext = NULL;
	//ElementTableContext_t **pTransposeContext = NULL; //倒置用的缓冲
	//uint32_t nNextSub = 0;
	//uint8_t i = 0;

	**pNextElementType = ELEMENT_TABLE;
	/*申请根节点空间*/
    **pNextElementAddr = (uint32_t *)calloc(1,sizeof(ElementTable_t));
    if(NULL == **pNextElementAddr)
    {
        //APP_Printf("calloc error\n");
    }
	element = (ElementTable_t *)**pNextElementAddr;
	if(NULL == element)
	{
		return -1;
	}
	/*节点清零*/
	memset(element , 0x00 , sizeof(ElementTable_t));
	/*节点附值*/
	strAttrString = getAttrValue(pXmlNode->m_pAttr , (int8_t*)"Col");
	chAttrString = XString2Char(strAttrString);
	element->m_chCol = atoi((char*)chAttrString);
	free(chAttrString);

	pChildNode = pXmlNode->m_pChildNode;
	while(pChildNode)
	{
		pUnitNode = pChildNode->m_pChildNode;
		if(gx_strsame(pChildNode->m_strName , (int8_t*)"cdbs:Title"))
		{
			while(pUnitNode)
			{
				/*取每个标题数据*/
				pTitle = 
					(ElementTableTitle_t *)
					calloc(1 , sizeof(ElementTableTitle_t));
					if(NULL == pTitle)
                {
                    //APP_Printf("calloc error\n");
                }
				if(NULL == pTitle)
				{
					return -1;
				}		
				memset(pTitle , 0x00 , sizeof(ElementTableTitle_t));

				strAttrString = getAttrValue(pUnitNode->m_pAttr , (int8_t*)"Len");
				chAttrString = XString2Char(strAttrString);
				pTitle->m_chColWidth = atoi((char*)chAttrString);
				free(chAttrString);

				pTitle->p_chTitleName = XString2Char(&pUnitNode->m_strBuf);

				if(element->m_pTableTitle)
				{
					pTableTitle->m_pNextTableTitle = pTitle;
					pTableTitle = pTableTitle->m_pNextTableTitle;
				}
				else
				{				
					element->m_pTableTitle = pTitle;
					pTableTitle = pTitle;
				}
				pUnitNode = pUnitNode->m_pNextNode;
			}
		}//if(gx_strsame(pChildNode->m_strName , "cdbs:Title"))
		else if(gx_strsame(pChildNode->m_strName , (int8_t*)"cdbs:Content"))
		{
			while(pUnitNode)
			{
				/*取每个数据*/
				pContext = 
					(ElementTableContext_t *)
					calloc(1 , sizeof(ElementTableContext_t));
									if(NULL == pContext)
		{
                    //APP_Printf("calloc error\n");
                }
				if(NULL == pContext)
				{
					return -1;
				}		
				memset(pContext , 0x00 , sizeof(ElementTableContext_t));

				strAttrString = getAttrValue(pUnitNode->m_pAttr , (int8_t*)"cdbs:Href");
				pContext->m_pchHref = XString2Char(strAttrString);

				pContext->m_pchTextContext = XString2Char_t(&pUnitNode->m_strBuf);//eidt by lijq 0808

				if(element->m_pTableContext)
				{
					pTableContext->m_pNextTableContext = pContext;
					pTableContext = pTableContext->m_pNextTableContext;
				}
				else
				{				
					element->m_pTableContext = pContext;
					pTableContext = pContext;
				}
				pUnitNode = pUnitNode->m_pNextNode;
			}
		} // else if(gx_strsame(pChildNode->m_strName , "cdbs:Content"))
		pChildNode = pChildNode->m_pNextNode;
		element->m_chRow++;
	}
	element->m_chRow--; //去除表标题行数1
	/*表数据行竖转置*/
	/*if(element->m_chRow > 1)
	{
		pTransposeContext = calloc( element->m_chRow * element->m_chCol
			, sizeof(ElementTableContext_t *));
		if(NULL == pTransposeContext)
		{
			return -1;
		}
		pTableContext = element->m_pTableContext;
		for(i = 0 ; i<(element->m_chRow * element->m_chCol) ; i++)
		{
            pTransposeContext[i] = pTableContext;
			pTableContext = pTableContext->m_pNextTableContext;
		}
        
		for(i = 0 ; i<(element->m_chRow * element->m_chCol - 1) ; i++)//转置
		{
			if(i < (element->m_chRow - 1) * element->m_chCol)
			{
                nNextSub = i + element->m_chCol;
			}
			else
			{
				nNextSub = i - (element->m_chRow - 1) * element->m_chCol + 1;
			}
			pTransposeContext[i]->m_pNextTableContext 
				= pTransposeContext[nNextSub];
		}

		free(pTransposeContext);
	}*/

	*pNextElementType = &(element->m_NextElementType);
	*pNextElementAddr = &(element->m_pnNextElement);
	return 0;
}

/*******************************************************************************
IMAGE元素赋值
*******************************************************************************/
int8_t gx_image_element_evaluate( XNode *pXmlNode 
						  , XmlElementType_e **pNextElementType
						  , uint32_t ***pNextElementAddr)
{
	FileList_t *ISFile = NULL;
	ElementImage_t *element = NULL;
	/*属性值转换用的变量*/
	XString *strAttrString = NULL;
	int8_t *chAttrString = NULL;

	**pNextElementType = ELEMENT_IMAGE;
	/*申请根节点空间*/
    **pNextElementAddr = (uint32_t *)calloc(1,sizeof(ElementImage_t));
    if(NULL == **pNextElementAddr )
    {
        //APP_Printf("calloc error\n");
    }
	element = (ElementImage_t *)**pNextElementAddr;
	if(NULL == element)
	{
		return -1;
	}
	/*节点清零*/
	memset(element , 0x00 , sizeof(ElementImage_t));
	/*节点附值*/
	element->m_pchImageName = XString2Char(&(pXmlNode->m_strBuf));

	strAttrString = getAttrValue(pXmlNode->m_pAttr ,(int8_t*)"top_left_x");
	chAttrString = XString2Char(strAttrString);
	element->m_nTop_left_x = atoi((char*)chAttrString);
	free(chAttrString);

	strAttrString = getAttrValue(pXmlNode->m_pAttr , (int8_t*)"top_left_y");
	chAttrString = XString2Char(strAttrString);
	element->m_nTop_left_y = atoi((char*)chAttrString);
	free(chAttrString);

	strAttrString = getAttrValue(pXmlNode->m_pAttr , (int8_t*)"cdbs:Href");
	chAttrString = XString2Char(strAttrString);
	element->m_pchHref = chAttrString;

    //APP_Printf("element->m_pchImageName:%s\n", element->m_pchImageName);
	gx_find_IS_file(element->m_pchImageName , FILE_BMP , &ISFile);

	gx_get_bmp_info(ISFile,&element);

	*pNextElementType = &(element->m_NextElementType);
	*pNextElementAddr = &(element->m_pnNextElement);
	return 0;
}

/*******************************************************************************
TEXT元素赋值
*******************************************************************************/
int8_t gx_text_element_evaluate( XNode *pXmlNode 
						 , XmlElementType_e **pNextElementType
						 , uint32_t ***pNextElementAddr)
{
	ElementText_t *element = NULL;
	/*属性值转换用的变量*/
	XString *strAttrString = NULL;

	**pNextElementType = ELEMENT_TEXT;
	/*申请根节点空间*/
    **pNextElementAddr = (uint32_t *)calloc(1,sizeof(ElementText_t));
	if(NULL == **pNextElementAddr )
	{
        //APP_Printf("calloc error\n");
	}
	element = (ElementText_t *)**pNextElementAddr;
	if(NULL == element)
	{
		return -1;
	}
	/*节点清零*/
	memset(element , 0x00 , sizeof(ElementText_t));
	/*节点附值*/
	element->m_pchTextContext =	XString2Char(&(pXmlNode->m_strBuf));
	strAttrString = getAttrValue(pXmlNode->m_pAttr , (int8_t*)"cdbs:Href");
	element->m_pchHref = XString2Char(strAttrString);
	*pNextElementType = &(element->m_NextElementType);
	*pNextElementAddr = &(element->m_pnNextElement);
	return 0;
}

/*******************************************************************************
MENU元素赋值
*******************************************************************************/
int8_t gx_menu_element_evaluate( XNode *pXmlNode 
							, XmlElementType_e **pNextElementType
							, uint32_t ***pNextElementAddr)
{
	ElementMenu_t *element = NULL;
	/*属性值转换用的变量*/
	XString *strAttrString = NULL;
	int8_t *chAttrString = NULL;
	/*子节点类型及地址*/
	XmlElementType_e *pChildElementType = NULL; 
	uint32_t **pChildElementAddr = NULL;

	**pNextElementType = ELEMENT_MENU;
	/*申请根节点空间*/
    **pNextElementAddr = (uint32_t *)calloc(1,sizeof(ElementMenu_t));
    if(NULL == **pNextElementAddr )
    {
        //APP_Printf("calloc error\n");
    }
	element = (ElementMenu_t *)**pNextElementAddr;
	if(NULL == element)
	{
		return -1;
	}
	/*节点清零*/
	memset(element , 0x00 , sizeof(ElementMenu_t));
	/*节点附值*/
	strAttrString = getAttrValue(pXmlNode->m_pAttr , (int8_t*)"cdbs:Name");
	chAttrString = XString2Char(strAttrString);
	element->m_pchMenuName = chAttrString;

	pChildElementType = &(element->m_ChildElementType);
	pChildElementAddr = &(element->m_pnChildElement);

	pXmlNode = pXmlNode->m_pChildNode;
	while(pXmlNode)
	{
		if(gx_strsame(pXmlNode->m_strName , (int8_t*)"cdbs:Text"))
		{
			gx_text_element_evaluate(pXmlNode 
				, &pChildElementType 
				, &pChildElementAddr);
		}
		else if(gx_strsame(pXmlNode->m_strName ,(int8_t*) "cdbs:Image"))
		{
			gx_image_element_evaluate(pXmlNode
				, &pChildElementType 
				, &pChildElementAddr);
		}
		else if(gx_strsame(pXmlNode->m_strName , (int8_t*)"cdbs:Table"))
		{
			gx_table_element_evaluate(pXmlNode
				, &pChildElementType 
				, &pChildElementAddr);
		}
		pXmlNode = pXmlNode->m_pNextNode;
	}

	*pNextElementType = &(element->m_NextElementType);
	*pNextElementAddr = &(element->m_pnNextElement);
	return 0;
}

/*******************************************************************************
解析XML
*******************************************************************************/
int32_t gx_parse_XML(int8_t *xmlbuf , uint32_t xmlsize , XmlRoot_t **pISxml)
{
	XNode *pXmlNodeRoot = NULL; //XML节点根节点
	int32_t nNodeNumber = 0 ,nResult = 0 , i = 0; 
	XmlRoot_t *pISxmlRoot = NULL;//IS模块中XML的根节点
	FileList_t *ISFile;
	/*下一个节点类型及地址*/
	XmlElementType_e *pNextElementType = NULL; 
	uint32_t **pNextElementAddr = NULL;
	/*属性值转换用的变量*/
	XString *strAttrString = NULL;

	nNodeNumber = XmlParserRead((char*)xmlbuf , xmlsize , &pXmlNodeRoot);
	if(nNodeNumber <= 0)//解析失败
	{
        //APP_Printf("####XmlParserRead error 1####\n");
		return -1;
	}

	if(gx_strsame(pXmlNodeRoot[0].m_strName , (int8_t*)"cdbs:RootMenu") 
		|| gx_strsame(pXmlNodeRoot[0].m_strName , (int8_t*)"cdbs:Root"))
	{
		/*申请根节点空间*/
        pISxmlRoot = (XmlRoot_t *)calloc(1,sizeof(XmlRoot_t));
		if(NULL == pISxmlRoot )
		{
            //APP_Printf("calloc error\n");
		}
		/*节点数据清零*/
		memset(pISxmlRoot , 0x00 , sizeof(XmlRoot_t));
		/*根节点附值*/
		strAttrString = getAttrValue(pXmlNodeRoot[0].m_pAttr , (int8_t*)"cdbs:Background");
		pISxmlRoot->m_pchBackgroudName = XString2Char(strAttrString);

		gx_find_IS_file(pISxmlRoot->m_pchBackgroudName , FILE_MPG , &ISFile);
		if(ISFile->m_nFileAddr < 0x80000000)
		{
            //APP_Printf("####XmlParserRead error 2####\n");
			;//return -1;
		}
		pISxmlRoot->m_nBackgroudAddr = ISFile->m_nFileAddr;
		pISxmlRoot->m_nBackgroudLength = ISFile->m_nFileLength;

		pNextElementType = &(pISxmlRoot->m_NextElementType);
		pNextElementAddr = &(pISxmlRoot->m_pnNextElement);

		*pISxml = pISxmlRoot;
	}
	else
	{
        //APP_Printf("####XmlParserRead error 3####\n");
		return -1;
	}

	for(i = 1 ; i < nNodeNumber ; i++)
	{
        if(pXmlNodeRoot[i].m_nLevel != 2)
		{
			continue;
		}
		if(gx_strsame(pXmlNodeRoot[i].m_strName , (int8_t*)"cdbs:RootMenu"))
		{
			/*子节点提层*/
			((pXmlNodeRoot[i].m_pChildNode)->m_nLevel)--;
			continue;
		}
		if(gx_strsame(pXmlNodeRoot[i].m_strName , (int8_t*)"cdbs:Text"))
		{
			gx_text_element_evaluate(&pXmlNodeRoot[i] 
				, &pNextElementType 
				, &pNextElementAddr);
		}
		else if(gx_strsame(pXmlNodeRoot[i].m_strName , (int8_t*)"cdbs:Image"))
		{
			gx_image_element_evaluate(&pXmlNodeRoot[i] 
				, &pNextElementType 
				, &pNextElementAddr);
		}
		else if(gx_strsame(pXmlNodeRoot[i].m_strName , (int8_t*)"cdbs:Table"))
		{
			gx_table_element_evaluate(&pXmlNodeRoot[i] 
				, &pNextElementType 
				, &pNextElementAddr);
		}
		else if(gx_strsame(pXmlNodeRoot[i].m_strName , (int8_t*)"cdbs:Menu"))
		{
			gx_menu_element_evaluate(&pXmlNodeRoot[i] 
				, &pNextElementType 
				, &pNextElementAddr);
		}
	}//for(i = 1 ; i < nNodeNumber ; i++)
    
	nResult = XmlParserFree(pXmlNodeRoot);
	if(nResult)//释放失败
	{
        //APP_Printf("####XmlParserRead error 4####\n");
		return -1;
	}
	return 0;
}

/* Public Functions --------------------------------------------------------- */ 

/*******************************************************************************
进入页面
*******************************************************************************/
uint8_t gx_enter_url(int8_t *chXmlfile)
{
	FileList_t *pFile = NULL;

	/*查找文件*/
	gx_find_IS_file(chXmlfile , FILE_XML , &pFile);
	/*判断XML文件在IS中的级数*/
	if(0 == (strcmp((char*)chXmlfile , "0_index.xml")))
	{
		if(NULL == pFirstPage)
		{
			/*XML解析*/
			if(-1 == gx_parse_XML((int8_t *)(pFile->m_nFileAddr),pFile->m_nFileLength, &pFirstPage))
			{
				return 1;
			}
		}
	}
	else
	{
		if(strstr((char*)chXmlfile , "_000.xml"))
		{
			/*释放相应级XML数据*/
			if(NULL != pSecondPage)
			{
				gx_free_is_xml(pSecondPage);
				pSecondPage = NULL;
			}
			/*XML解析*/
			gx_parse_XML((int8_t *)(pFile->m_nFileAddr),pFile->m_nFileLength, &pSecondPage);
		}
		else
		{
			/*释放相应级XML数据*/
			if(NULL != pThirdPage)
			{
				gx_free_is_xml(pThirdPage);
				pThirdPage = NULL;
			}
			/*XML解析*/
			gx_parse_XML((int8_t *)(pFile->m_nFileAddr),pFile->m_nFileLength, &pThirdPage);
		}
	}
	/*向GUI返回消息*/
	return 0;
}
extern char g_strPathBmp[256];
extern char g_strPathXml[256];
extern char g_strPathIFrema[256];
uint8_t tide_up_file_list(FileList_t *file_list)
{
	FileList_t *pfilelist = NULL;
	uint8_t *file_buf = NULL;
	uint32_t i, offset= 0;

	if(NULL == file_list)return 1;

	pfilelist = file_list;
	while(NULL != pfilelist)
	{
        file_buf = (uint8_t *)malloc(pfilelist->m_nFileLength+0xf);
		if(NULL == file_buf)
		{
			return 1;
		}
		memset(file_buf, 0, pfilelist->m_nFileLength+0xf);

		offset = 0;
		for(i = 0; i <= pfilelist->m_nLastSection; i++)
		{

			memcpy((uint8_t *)(file_buf + offset)
				, (uint8_t *)(pfilelist->m_nSubFileAddr[i])
				, pfilelist->m_nSubFileLength[i]);

			offset += pfilelist->m_nSubFileLength[i];
		}
		pfilelist->m_nFileAddr = pfilelist->m_nSubFileAddr[0];


		if(pfilelist->m_nFileLength!=offset)
		{
            //APP_Printf("test########%#x <<--- %#x offset:%#x len:%#x\n", pfilelist->m_nFileAddr, pfilelist->m_nSubFileAddr[0], offset, pfilelist->m_nFileLength);
		}
		for(i = 0; i <= pfilelist->m_nLastSection; i++)
		{
			if(pfilelist->m_nFileAddr>pfilelist->m_nSubFileAddr[i])
			{
                //APP_Printf("test====test\n");
                //APP_Printf("i=%d pfilelist->m_nFileAddr:%#x--->>%#x\n",i , pfilelist->m_nFileAddr
                    //, pfilelist->m_nSubFileAddr[i]);
				pfilelist->m_nFileAddr = pfilelist->m_nSubFileAddr[i];
			}
		}

		memcpy((uint8_t *)(pfilelist->m_nFileAddr), (uint8_t *)file_buf, pfilelist->m_nFileLength);
        /*
        //========================
                //对图像进行转换和翻转
                if(pfilelist->m_FileType == FILE_BMP)
                {
                    extern uint32_t InfoBufAddrOffset;
                    uint32_t bmpAddr = ADDR_ALIGN(InfoBufAddrOffset);
                    if((bmpAddr +  pfilelist->m_nFileLength*2) > IS_BUF0_END)
                    {
                        printf("bmp buffer overflow protect...\n");
                        continue ;
                    }
                    InfoBufAddrOffset = (uint32_t)bmp_8bpp_to_16bppBuff((void *)pfilelist->m_nFileAddr, (unsigned char *)bmpAddr);
                    pfilelist->m_nFileAddr = bmpAddr;
                    pfilelist->m_nFileLength = abs(InfoBufAddrOffset-bmpAddr);
                }
        //========================
*/


        FILE *fp = NULL;
        char path[256];
        if(pfilelist->m_FileType == FILE_MPG)
            sprintf(path, "%s%s", g_strPathIFrema, pfilelist->m_pchFileName);
        else if(pfilelist->m_FileType == FILE_BMP)
            sprintf(path, "%s%s", g_strPathBmp, pfilelist->m_pchFileName);
        else
            sprintf(path, "%s%s", g_strPathXml, pfilelist->m_pchFileName);
        //存储目录必须存在，不存在会异常，此处省去检查
        fp = fopen(path, "wb");
        if(fp)
        {
            fwrite((void*)(pfilelist->m_nFileAddr), pfilelist->m_nFileLength, 1, fp);
            fclose(fp);
        }
        else
            ;//qDebug("%s,0x%x",pfilelist->m_pchFileName,pfilelist->m_nFileLength);

		free(file_buf);
		file_buf = NULL;
		offset = 0;

		pfilelist = pfilelist->next;
	}

	return 0;
}
/*******************************************************************************
进入IS模块
*******************************************************************************/
uint8_t gx_enter_IS(void)
{
	filelist = com_is_get_file_list();

	if(1 == tide_up_file_list(filelist))
	{
		return 0;
	}

	/*解析0_index.xml*/
	if(1 == gx_enter_url((int8_t*)"0_index.xml"))
	{
		return 0;
	}

    if(pFirstPage == NULL)
	{
		return 0;
	}

	return 0;
}

/*******************************************************************************
退出IS模块,释放资源
*******************************************************************************/
uint8_t gx_exit_IS(void)
{
	/*释放XML解析时所用的资源(共三级页面)*/
	if(pFirstPage)
	{
		gx_free_is_xml(pFirstPage);
		pFirstPage = NULL;
	}
	if(pSecondPage)
	{
		gx_free_is_xml(pSecondPage);
		pSecondPage = NULL;
	}
	if(pThirdPage)
	{
		gx_free_is_xml(pThirdPage);
		pThirdPage = NULL;
	}
    /*释放IS文件资源*/
    //com_is_close();

    return 0;
}



void uial_info_service_loading(void)
{
	/*static StbAppMsg_t tStbAppMsg;
	
	tStbAppMsg.m_AppType = MSG_ENTER_INFO_SERVICE;
	uial_os_mbox_put(HandleMboxStbApp, (void *)&tStbAppMsg);*/
}


