
#include "uial_info_service.h"
extern XmlRoot_t *pSecondPage;

static int sMultiPage = 0;
//static int sCurPage = 0;
static int SecondServiceType = 0;
//static const char* sIndexFocusWidget = NULL;
//static int sSecondCurPage;
//static int sSecondTotalPage;


int create_sevice_second(void)
{
    int i = 0;
    int iElementType = 0;
    uint32_t * pRoot;
    static char title_text[10][50];
    static char rect[30], name[50], rect1[30];
    if(pSecondPage == NULL)
        qDebug()<<"secondPage error";
    qDebug()<<"secondPage right";
    pRoot = (uint32_t *)pSecondPage;
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
                case ELEMENT_MENU:
                    {
                        iElementType = ((pElementMenu_t)pRoot)->m_NextElementType;

                        //listview
                        if(((pElementMenu_t)pRoot)->m_pnChildElement)
                        {
                            sMultiPage ++;
                            if(((pElementMenu_t)pRoot)->m_pchMenuName!=NULL)
                            {
                                memset(title_text[sMultiPage-1], 0, 50);
                                memcpy(title_text[sMultiPage-1],  ((pElementMenu_t)pRoot)->m_pchMenuName, 49);
                            }
                            else
                            {
                                memset(title_text[sMultiPage-1], 0, 50);
                                //qDebug("[DATACAST]---------second title addr NULL!!-------------\n");
                            }
                            qDebug("[subPage]-%s",title_text[sMultiPage-1]);
                            if( ((pElementMenu_t)pRoot)->m_ChildElementType == ELEMENT_TEXT)
                            {
                                pElementText_t pText;
                                pText =  (pElementText_t)(((pElementMenu_t)pRoot)->m_pnChildElement);

                                SecondServiceType = 0;
                                while(pText!=NULL)
                                {
                                    //qDebug("%s",pText->m_pchTextContext);
                                    pText = (pElementText_t)(pText->m_pnNextElement);
                                }
                            }
                            else if( ((pElementMenu_t)pRoot)->m_ChildElementType == ELEMENT_TABLE)
                            {
                                uint8_t i = 0,j=0;
                                pElementTable_t pTable = NULL;
                                ElementTableTitle_t *pTableTitle = NULL;
                                //ElementTableTitle_t *pTableTitleTemp = NULL;
                                int nColumnNumber = 0;
                                //int nClumnWithSum = 0;
                                int nRowNumber = 0;
                                //int nHeight = 0;
                                pTable = (pElementTable_t)(((pElementMenu_t)pRoot)->m_pnChildElement);
                                pTableTitle = pTable->m_pTableTitle;
                                nColumnNumber = pTable->m_chCol;
                                nRowNumber = pTable->m_chRow;

                                SecondServiceType = 1;


                                {
                                    pElementTableContext_t pTableContext = NULL;
                                    pTableContext = pTable->m_pTableContext;
//                                    for(; n > 0; n--)
//                                    {
//                                        pTableContext = pTableContext->m_pNextTableContext;
//                                    }
                                    for(j=0;j<pTable->m_chRow;j++)
                                    for(i = 0; i<pTable->m_chCol; i++)
                                    {
                                        qDebug("%s",(char*)pTableContext->m_pchTextContext);
                                        pTableContext = pTableContext->m_pNextTableContext;
                                    }
                                }
                            }
                        }
                        else
                        {
                            //sMultiPage ++;
                            if(((pElementMenu_t)pRoot)->m_pchMenuName!=NULL)
                            {
                                sMultiPage ++;
                                memset(title_text[sMultiPage-1], 0, 50);
                                memcpy(title_text[sMultiPage-1],  ((pElementMenu_t)pRoot)->m_pchMenuName, 49);
                            }
                            else
                            {
                                //memset(title_text[sMultiPage-1], 0, 50);
                                printf("[DATACAST]---------second title addr NULL!!-------------\n");
                            }
                        }
                    }

                    pRoot = ((pElementMenu_t)pRoot)->m_pnNextElement;
                    break;
                default:
                    break;
            }
        }
    }
    return 0;
}
