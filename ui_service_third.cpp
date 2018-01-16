#include "uial_info_service.h"

extern XmlRoot_t *pThirdPage;

#define THIRD_CONTENT_BACKCOLOR			"[#BEBEBF,#BEBEBF,#BEBEBF]"
#define THIRD_CONTENT_COLOR			"[#FF00FF,#FF00FF,#FF00FF]"
//*********color define*********//
#define THIRD_TITLE_TEXT_COLOR            			"[#0000ff,#0000ff,#0000ff]"
#define THIRD_TEXT_COLOR  (0x303030)
#define THIRD_TABLE_LINE   (0x303030)//(0x06E3EA)
#define THIRD_TABLE_TITLE  (0xFF0000)
//***************************//

static const char* sSecondFocusWidget = NULL;
static int sContentElementNum;

typedef struct UpdateRect
{
    int m_nStartPage;
    int m_nEndPage;
    int m_nStartRow;
    int m_nEndRow;	//和开始行不一定同一页
    XmlElementType_e m_ElementType;	//元素的类型
    uint32_t *m_pElement;//元素的地址
    uint32_t *m_pnNextUpdateRect;
}UpdateRect_t;

typedef struct UpdateContent_s
{
    int m_nTotalPage;
    uint16_t m_nElementTextCount;
    uint16_t m_nCharCountOnePage[256];
    UpdateRect_t *m_pFirstUpdateRect;
}UpdateContent_t;

static UpdateContent_t UpdateContent;
static char sTextBuf[53*8+1];
static int sThirdCurPage;


int create_sevice_third(void)
{
    uint32_t * pRoot;
    int i = 0;
    int m = 0;
    int nElementTextCount = 0;
    int iElementType = 0;
    int nTitleTextFlag = 1;
    static char rect[30], name[50];

    if(pThirdPage == NULL)
        qDebug()<<"third page is null";
    else
        qDebug()<<"third page is ok";

    pRoot = (uint32_t *)pThirdPage;
    for(i = 0;pRoot != NULL;i++)
    {
        if(i == 0)
        {
            iElementType = ((pXmlRoot_t)pRoot)->m_NextElementType;
            pRoot = ((pXmlRoot_t)pRoot)->m_pnNextElement;
        }
        else
        {
            switch(iElementType)
            {
                case ELEMENT_IMAGE:
                    {
                        iElementType = ((pElementImage_t)pRoot)->m_NextElementType;
                        if(0 == ((pElementImage_t)pRoot)->m_nWith)
                        {
                            pRoot = ((pElementImage_t)pRoot)->m_pnNextElement;
                            continue;
                        }
                    }
                    pRoot = ((pElementImage_t)pRoot)->m_pnNextElement;
                    break;
                case ELEMENT_TEXT:
                    {
                        iElementType = ((pElementText_t)pRoot)->m_NextElementType;
                        if(nTitleTextFlag == 1)
                        {
                            nTitleTextFlag = 0;
                            sprintf(name, "third_text0");
                            sprintf(rect, "[%d,%d,%d,%d]",80,80+30,560, 30);
                        }
                        else
                        {
                            //text_conext
                            int nStartPage = 0, nEndPage = 0;
                            int nStartRow = 0, nEndRow = 0;
                            uint8_t *pTextString = NULL;
                            int nStringCount = 0;
                            UpdateRect_t* UpdateRect = NULL;
                            UpdateRect_t* UpdateRectTemp = NULL;
                            UpdateRect = (UpdateRect_t*)(UpdateContent.m_pFirstUpdateRect);

                            sContentElementNum++;
                            if(UpdateRect==NULL)
                            {
                                nStartRow = 1;
                                nStartPage = 1;
                            }
                            else
                            {
                                while(UpdateRect->m_pnNextUpdateRect!=NULL)
                                {
                                    UpdateRect = (UpdateRect_t*)UpdateRect->m_pnNextUpdateRect;
                                }
                                nStartRow = UpdateRect->m_nEndRow;
                                nStartPage = UpdateRect->m_nEndPage;
                                if(nStartRow == 8)
                                {
                                    nStartRow = 1;
                                    nStartPage+=1;
                                }
                                else
                                {
                                    nStartRow+=1;
                                }
                            }
                            nStringCount = strlen((char*)(((pElementText_t)pRoot)->m_pchTextContext));
                            pTextString = (uint8_t *)malloc(nStringCount+1);
                            if(pTextString == NULL)
                            {
                                printf("[DATECAST]---%s[%d]---\n",__FILE__,__LINE__);
                                return 0;
                            }
                            memset(pTextString, 0, nStringCount+1);
                            memcpy(pTextString, ((pElementText_t)pRoot)->m_pchTextContext, nStringCount);
                            {
                                //计算字符串显示分页.53*8 = 424  字符10
                                int n = 0, count = 0;
                                int nRow = 1, nPosInRow = 1;
                                int nPageNum = 1;
                                while(pTextString[n]!='\0')
                                {
                                    if(pTextString[n]=='\r'&&pTextString[n+1]=='\n')
                                    {
                                        nRow++;
                                        nPosInRow = 1;
                                        n++;
                                        count++;
                                    }
                                    else
                                    {
                                        nPosInRow++;
                                        if(nPosInRow>53)
                                        {
                                            nRow++;
                                            nPosInRow = 1;
                                        }
                                    }
                                    if(nRow>(8-(nStartRow-1)))
                                    {
                                        if(pTextString[n]!= ' '||pTextString[n]!=','
                                            ||pTextString[n]!=':'||pTextString[n]!=';'
                                            ||pTextString[n]!='.'||pTextString[n]!='!'
                                            ||pTextString[n]!='?')
                                        {
                                            while(1)
                                            {
                                                if(pTextString[n-1]==' '||pTextString[n-1]==','
                                                    ||pTextString[n-1]==':'||pTextString[n-1]==';'
                                                    ||pTextString[n-1]=='.'||pTextString[n-1]=='!'
                                                    ||pTextString[n-1]=='?')
                                                {
                                                    break;
                                                }
                                                n--;
                                                count --;
                                            }
                                        }
                                        while(pTextString[n]== ' ')
                                        {
                                            n++;
                                            count++;
                                        }
                                        UpdateContent.m_nCharCountOnePage[nElementTextCount+nPageNum-1] = count;
                                        UpdateContent.m_nElementTextCount = nElementTextCount+nPageNum;
                                        count = 0;
                                        //if(pTextString[n+1]!='\0')
                                        {
                                            nRow = 1;
                                            nPageNum++;
                                        }
                                    }
                                    n++;
                                    count++;
                                    if(pTextString[n]=='\0')
                                    {
                                        UpdateContent.m_nCharCountOnePage[nElementTextCount+nPageNum-1] = count;
                                        UpdateContent.m_nElementTextCount = nElementTextCount+nPageNum;
                                    }
                                }
                                if(nPageNum>1)
                                {
                                    nEndRow = nRow;
                                }
                                else
                                {
                                    nEndRow = nStartRow+nRow-1;
                                }
                                nEndPage = nStartPage + nPageNum - 1;
                                nElementTextCount+=nEndPage;
                            }
                            free(pTextString);
                            pTextString = NULL;
                            UpdateRectTemp = (UpdateRect_t*)malloc(sizeof(UpdateRect_t));
                            if(UpdateRectTemp == NULL)
                            {
                                printf("[DATECAST]---------malloc---------error!\n");
                            }
                            memset(UpdateRectTemp, 0, sizeof(UpdateRect_t));
                            UpdateRectTemp->m_nStartPage = nStartPage;
                            UpdateRectTemp->m_nEndPage = nEndPage;
                            UpdateRectTemp->m_nStartRow = nStartRow;
                            UpdateRectTemp->m_nEndRow = nEndRow;
                            UpdateRectTemp->m_ElementType = ELEMENT_TEXT;
                            UpdateRectTemp->m_pElement = pRoot;
                            UpdateRectTemp->m_pnNextUpdateRect = NULL;
                            if(UpdateRect == NULL)
                            {
                                UpdateContent.m_pFirstUpdateRect = UpdateRectTemp;
                            }
                            else
                            {
                                UpdateRect->m_pnNextUpdateRect = (uint32_t*)UpdateRectTemp;
                            }
                            UpdateContent.m_nTotalPage = UpdateRectTemp->m_nEndPage;
                        }
                    }
                    pRoot = ((pElementText_t)pRoot)->m_pnNextElement;
                    break;
                case ELEMENT_TABLE:
                    {
                        iElementType = ((pElementTable_t)pRoot)->m_NextElementType;
                        //text_conext
                        int nRow = 0;
                        int nStartPage = 0;//nEndPage = 0;
                        int nStartRow = 0, nEndRow = 0;
                        int nPageNum = 0;
                        UpdateRect_t* UpdateRect = NULL;
                        UpdateRect_t* UpdateRectTemp = NULL;
                        UpdateRect = (UpdateRect_t*)(UpdateContent.m_pFirstUpdateRect);

                        sContentElementNum++;
                        if(UpdateRect==NULL)
                        {
                            nStartRow = 1;
                            nStartPage = 1;
                        }
                        else
                        {
                            while(UpdateRect->m_pnNextUpdateRect!=NULL)
                            {
                                UpdateRect = (UpdateRect_t*)UpdateRect->m_pnNextUpdateRect;
                            }
                            nStartRow = UpdateRect->m_nEndRow;
                            nStartPage = UpdateRect->m_nEndPage;
                            if(nStartRow>=7)
                            {
                                nStartRow = 1;
                                nStartPage+=1;
                            }
                            else
                            {
                                nStartRow+=1;
                            }
                        }
                        nRow = ((pElementTable_t)pRoot)->m_chRow;
                        if((nRow+1)>(8-(nStartRow-1)))
                        {
                            if(((nRow+1)-(8-(nStartRow-1)))%7 == 0)
                            {
                                nPageNum = ((nRow+1)-(8-UpdateRect->m_nEndRow))/7 + 1;
                                nEndRow = 8;
                            }
                            else
                            {
                                nPageNum = ((nRow+1)-(8-UpdateRect->m_nEndRow))/7 + 2;
                                nEndRow = ((nRow+1)-(8-UpdateRect->m_nEndRow))%7+1;
                            }
                        }
                        else
                        {
                            nPageNum = 1;
                            nEndRow = nStartRow + nRow;
                        }
                        UpdateRectTemp = (UpdateRect_t*)malloc(sizeof(UpdateRect_t));
                        if(UpdateRectTemp == NULL)
                        {
                            printf("[DATECAST]---------malloc---------error!\n");
                        }
                        memset(UpdateRectTemp, 0, sizeof(UpdateRect_t));
                        UpdateRectTemp->m_nStartPage = nStartPage;
                        UpdateRectTemp->m_nEndPage = nStartPage + nPageNum -1;
                        UpdateRectTemp->m_nStartRow = nStartRow;
                        UpdateRectTemp->m_nEndRow = nEndRow;
                        UpdateRectTemp->m_ElementType = ELEMENT_TABLE                                                                                                                                                                                                     ;
                        UpdateRectTemp->m_pElement = pRoot;
                        UpdateRectTemp->m_pnNextUpdateRect = NULL;
                        if(UpdateRect == NULL)
                        {
                            UpdateContent.m_pFirstUpdateRect = UpdateRectTemp;
                        }
                        else
                        {
                            UpdateRect->m_pnNextUpdateRect = (uint32_t*)UpdateRectTemp;
                        }
                        UpdateContent.m_nTotalPage = UpdateRectTemp->m_nEndPage;
                    }
                    pRoot = ((pElementTable_t)pRoot)->m_pnNextElement;
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}


