#include "uial_xmlparser.h"
#include "type_.h"
int g_nNodeCount = 0;       //节点数目

/******************************************************************************
是空否
******************************************************************************/
static int isSpace(int c)
{
    switch (c)
    {
        case 0x20:
        case 0x0D:
        case 0x0A:
        case 0x09:
            return 1;
    }
    return 0;
}

/******************************************************************************
有效的名称前导符
******************************************************************************/
static int isNameChar(int c)
{
    return (c>='A' && c<='Z') || (c>='a' && c<='z') || (c=='_') ;
}

/******************************************************************************
删除前导空
******************************************************************************/
char * DelSpace( char *buf)
{
    char *p ;

    for(p=buf ; isSpace(*p) ; p++); /* 去掉空字符 */
    return p;
}

/******************************************************************************

******************************************************************************/
int getNodeName(char * buf , int len , XString * pNodeName)
{
    char *p = NULL , *p1 = NULL;

    pNodeName->m_chText = NULL;
    for (p = buf ; p < (buf+len) ; p++)
    {
        if(*p == '<' && (isNameChar(*(p+1))))	// 找到'<' 且后一字符为字母
        {
            p++;
            pNodeName->m_chText = p;
            for(p1 = p ; p1 < (buf+len) ; p1++)
            {
                if(isSpace(*p1) || ('>' == *p1))
                {
                    pNodeName->m_nLen = (int)(p1 - p);
                    break;
                }
            }
            break;
        }
    }
    return 0;
}
/******************************************************************************

******************************************************************************/
int getContent(char * buf , int len , XNode *pNode)
{
    char *chNodeHead = NULL , *chNodeTail = NULL;
    char *p = NULL;
    int nStrLen = 0;

    if(NULL == ((pNode->m_strName).m_chText))
    {
        return -1;
    }
    /*chNodeHead = "<" + "节点名"*/
    nStrLen = (pNode->m_strName).m_nLen + 2;
    //chNodeHead = (char *)calloc(nStrLen,sizeof(char));
    chNodeHead = new char[nStrLen];
    if(NULL == chNodeHead )
    {
        qDebug("calloc error\n");
    }
    memset(chNodeHead , 0x00 , nStrLen);
    chNodeHead[0] = '<';
    memcpy(chNodeHead+1 , (pNode->m_strName).m_chText , nStrLen - 2);

    /*查找节点头*/
    p = strstr(buf , chNodeHead);
    if(NULL == p)
    {
        return -1;//没找到
    }
    //free(chNodeHead);
    delete [] chNodeHead;
    p = p+nStrLen-1;
    if(!(isSpace(*p) || '>' == (*p)))
    {
        return -1;//没找到
    }
    for(; *p != '>' ; p++)
    {
        if(p > (buf + len))
        {
            return -1;
        }
    }
    pNode->m_strBuf.m_chText = p+1;

    /*查找节点尾*/
    if( '/' == *(p-1))
    {
        pNode->m_strBuf.m_nLen = 0;//空节点
    }
    else
    {
        /*chNodeTail = "</" + "节点名"*/
        nStrLen = (pNode->m_strName).m_nLen + 3;
        //chNodeTail = (char *)calloc(nStrLen , sizeof(char));
        chNodeTail = new char[nStrLen];
        if(NULL == chNodeTail )
        {
            qDebug("calloc error\n");
        }
        memset(chNodeTail , 0x00 , nStrLen);
        chNodeTail[0] = '<';
        chNodeTail[1] = '/';
        memcpy(chNodeTail+2 , (pNode->m_strName).m_chText , nStrLen - 3);

        p = buf;
        do
        {
            p = strstr(p , chNodeTail);
            if((p > (buf + len)) || (p == NULL))
            {
                //free(chNodeTail);
                delete [] chNodeTail;
                return -1;
            }
            p = p+nStrLen-1;
        } while(!(isSpace(*p) || ('>' == *p)));
        //free(chNodeTail);
        delete [] chNodeTail;
        (pNode->m_strBuf).m_nLen = (int)(p
            - ((pNode->m_strBuf).m_chText)
            - nStrLen
            + 1);
    }
    return 0;
}

/******************************************************************************

******************************************************************************/
int getAttr(char * buf , int len , XNode *pNode)
{
    char *chNodeHead = NULL , *p = NULL, *p1 = NULL, *p2 = NULL;
    int nStrLen = 0;
    int nAttrCount = 0;
    XAttr *pAttr = NULL , **pPreAttr = NULL;
    char chHaveEqualSign = 0 ;

    if(NULL == ((pNode->m_strName).m_chText))
    {
        return -1;
    }

    /*chNodeHead = "<" + "节点名"*/
    nStrLen = (pNode->m_strName).m_nLen + 2;
    //chNodeHead = (char *)calloc(nStrLen,sizeof(char));
    chNodeHead = new char[nStrLen];
    if(NULL == chNodeHead )
    {
        qDebug("calloc error\n");
    }
    memset(chNodeHead , 0x00 , nStrLen);
    chNodeHead[0] = '<';
    memcpy(chNodeHead+1 , (pNode->m_strName).m_chText , nStrLen - 2);

    /*查找字符串*/
    p = strstr(buf , chNodeHead);
    if(NULL == p)
    {
        return -1;//没找到
    }
    //free(chNodeHead);
    delete [] chNodeHead;
    p = p+nStrLen-1;
    if(!isSpace(*p))
    {
        return -1;//没找到
    }

    /*取属性个数*/
    p1 = DelSpace(p);
    for(p = p1 ; *p != '>' ; p++)
    {
        if('"' == *p)
        {
            nAttrCount++;
        }
    }
    /*双引号不对称*/
    if(nAttrCount % 2 )
    {
        return -1;
    }
    /*取属性*/
    nAttrCount = nAttrCount / 2;
    pPreAttr = &(pNode->m_pAttr);
    p2 = p;
    while(nAttrCount != 0)
    {
        p1 = DelSpace(p1);
        //pAttr = calloc(1,sizeof(XAttr));
        pAttr = new XAttr();
        if(NULL == pAttr )
        {
            qDebug("calloc error\n");
        }
        if(NULL == pAttr)
        {
            qDebug("申请节点存储空间失败!");
            return -2;
        }
        memset(pAttr, 0x00 , sizeof(XAttr));
        /*取属性名*/
        pAttr->m_strAttrName.m_chText = p1;
        for(p = p1 ; !(isSpace(*p) || ('=' == *p)) ; p++)
        {
            if(p == p2)
            {
                //free(pAttr);
                delete pAttr;
                return -1;
            }
        }
        pAttr->m_strAttrName.m_nLen = (int)(p - p1);
        /*取属性值*/
        chHaveEqualSign = 0;
        for(; *p != '"' ;p++)
        {
            if('=' == *p)
            {
                chHaveEqualSign = 1;
            }
        }
        if(0 == chHaveEqualSign)
        {
            //free(pAttr);
            delete pAttr;
            return -1;
        }
        p ++;
        pAttr->m_strAttrValue.m_chText = p;
        p1 = p;
        for(; *p != '"';p++)
        {
            if(p == p2)
            {
                //free(pAttr);
                delete pAttr;
                return -1;
            }
        }
        pAttr->m_strAttrValue.m_nLen = (int)(p - p1);
        p ++;
        p1 = p;
        *pPreAttr = pAttr;
        pPreAttr = &(pAttr->m_pNextAttr);
        /*下一个属性*/
        nAttrCount--;
    }
    return 0;
}




/******************************************************************************
printf for debug
******************************************************************************/
int XPrintf(XString strval)
{
    char *p;

    //p = calloc(strval.m_nLen + 1 , sizeof(char));
    p = new char[strval.m_nLen + 1];
    if(NULL == p )
    {
        qDebug("calloc error\n");
    }
    memcpy(p,strval.m_chText,strval.m_nLen);
    qDebug("%s\n",p);
    //free(p);
    delete [] p;
    return strval.m_nLen;
}
int showxml(XNode *pXmltree)
{
#if 0
    int i;
    XAttr *pAttr;

    for(i = 0; i < g_nNodeCount; i++)
    {
        printf("\n==============================================================================\n");
        printf("xmltree[%02d]=0x%08X========================================================\n",i,&pXmltree[i]);
        printf("==============================================================================\n");
        printf("m_strName = ");
        XPrintf(pXmltree[i].m_strName);

        printf("m_nIndex    = %d\n", pXmltree[i].m_nIndex);
        printf("m_nLevel    = %d\n", pXmltree[i].m_nLevel);
        printf("m_nTagCount = %d\n", pXmltree[i].m_nTagCount);

        printf("m_pParentNode = 0x%08X\n", pXmltree[i].m_pParentNode);
        printf("m_pChildNode  = 0x%08X\n", pXmltree[i].m_pChildNode);
        printf("m_pNextNode   = 0x%08X\n", pXmltree[i].m_pNextNode);

        pAttr = pXmltree[i].m_pAttr;
        printf("--------ATTR-------\n");
        while(pAttr)
        {
            XPrintf(pAttr->m_strAttrName);
            printf("=\n");
            XPrintf(pAttr->m_strAttrValue);
            pAttr = pAttr->m_pNextAttr;
            printf("\n");
        }
        printf("-------------------\n");

        printf("m_strBuf = ");
        XPrintf(pXmltree[i].m_strBuf);

    }
#endif
    return 0;
}

/******************************************************************************
读xml并解析到X树
******************************************************************************/
int XmlParserRead(char *xmlbuf, int xmlsize, XNode **pXmltree)
{
    char *p = NULL, *p1 = NULL, *pend = xmlbuf + xmlsize;
    int nNodeCount = 0;  //XML中节点总个数
    XString strXmlStore; //所要解析的存储
    XString strNodeName; //节点名
    int i = 0 , nCount = 0;
    XNode *xmltree = NULL; //XML结构首地址

    /*XML简单合法性校验*/
    if(memcmp(xmlbuf,"<?", 2) != 0)
    {
        qDebug("无XML内容头!\n");
        return -1;
    }
    p = strstr(xmlbuf , "?>");
    if (NULL == p)
    {
        qDebug("XML内容头无结尾!\n");
        return -1;
    }
    //TODO: 有空时补上GB2312 编码 校验

    /*计算节点数*/
    p1 = DelSpace(p+2);//p1=正文开始处
    for (p = p1, nNodeCount = 0; p < pend; p++)
    {
        if ((*p == '/') && (*(p-1) == '<')) // "</"
        {
            nNodeCount++;
        }
        if ((*p == '>') && (*(p-1) =='/'))  //"/>"
        {
            nNodeCount++;
        }
    }

    /*申请节点存储空间*/
    //*pXmltree = (XNode *)calloc(nNodeCount , sizeof(XNode));/*申请节点存储空间*/
    *pXmltree = new XNode[nNodeCount]();
    if(NULL == *pXmltree )
    {
        qDebug("calloc error\n");
    }
    if(NULL == (*pXmltree))
    {
        qDebug("申请节点存储空间失败!");
        return -2;
    }
    xmltree = *pXmltree;
    g_nNodeCount = nNodeCount;
    memset(xmltree , 0x00, nNodeCount * sizeof(XNode));//初始化节点数据

    /*解析XML根节点*/
    strXmlStore.m_chText = p1;
    strXmlStore.m_nLen = (int)(pend-p1);

    xmltree[0].m_nIndex = 0;
    xmltree[0].m_nLevel = 1;
    xmltree[0].m_pParentNode = NULL;
    xmltree[0].m_pNextNode = NULL;
    getNodeName(strXmlStore.m_chText ,
        strXmlStore.m_nLen ,
        &(xmltree[0].m_strName));
    getAttr(strXmlStore.m_chText , strXmlStore.m_nLen , &xmltree[0]);
    getContent(strXmlStore.m_chText , strXmlStore.m_nLen , &xmltree[0]);

    nCount++;
    /*按层遍历*/
    for(i = 0 ; i < g_nNodeCount-1 ; i++)
    {
        /*查询没解析的节点*/
        if(NULL == xmltree[i].m_pChildNode)
        {
            strXmlStore = xmltree[i].m_strBuf;
            getNodeName(strXmlStore.m_chText ,
                strXmlStore.m_nLen ,
                &strNodeName);
            if(strNodeName.m_chText) //是否存在没有解析过的节点
            {
                xmltree[i].m_pChildNode = &xmltree[nCount];
                /*找第一个子节点*/
                xmltree[nCount].m_pParentNode = &xmltree[i];
                xmltree[nCount].m_strName = strNodeName;
                getAttr(strXmlStore.m_chText ,
                    strXmlStore.m_nLen ,
                    &xmltree[nCount]);
                getContent(strXmlStore.m_chText ,
                    strXmlStore.m_nLen ,
                    &xmltree[nCount]);
                xmltree[nCount].m_nLevel = xmltree[i].m_nLevel + 1;
                xmltree[nCount].m_nIndex = nCount;
                xmltree[i].m_nTagCount ++;

                /*找子节点的兄弟节点*/
                while(1)
                {
                    strXmlStore.m_chText = xmltree[nCount].m_strBuf.m_chText
                        + xmltree[nCount].m_strBuf.m_nLen
                        + xmltree[nCount].m_strName.m_nLen
                        + 2;
                    strXmlStore.m_nLen = (int)(xmltree[i].m_strBuf.m_chText
                        + xmltree[i].m_strBuf.m_nLen
                        - strXmlStore.m_chText);

                    getNodeName(strXmlStore.m_chText ,
                        strXmlStore.m_nLen ,
                        &strNodeName);
                    nCount++;
                    if(NULL == strNodeName.m_chText)
                    {
                        break;
                    }
                    xmltree[nCount - 1].m_pNextNode = &xmltree[nCount];
                    xmltree[nCount].m_pParentNode = &xmltree[i];
                    xmltree[nCount].m_pNextNode = NULL;
                    xmltree[nCount].m_strName = strNodeName;
                    getAttr(strXmlStore.m_chText ,
                        strXmlStore.m_nLen ,
                        &xmltree[nCount]);
                    getContent(strXmlStore.m_chText ,
                        strXmlStore.m_nLen ,
                        &xmltree[nCount]);
                    xmltree[nCount].m_nLevel = xmltree[i].m_nLevel + 1;
                    xmltree[nCount].m_nIndex = nCount;
                    xmltree[i].m_nTagCount ++;
                }
            }
        }
    }
    return nNodeCount;
}

/******************************************************************************
资源释放
******************************************************************************/
int XmlParserFree(XNode *xmltree)
{
    XAttr * pAttr = NULL;
    int i = 0;

    for(i = 0 ; i < g_nNodeCount ; i++)
    {
        while(xmltree[i].m_pAttr)
        {
            pAttr = xmltree[i].m_pAttr;
            pAttr = pAttr->m_pNextAttr;
            //free( xmltree[i].m_pAttr);
            delete xmltree[i].m_pAttr;
            xmltree[i].m_pAttr = pAttr;
        }
    }
    if(g_nNodeCount > 0)
    {
        //free(xmltree);
        delete xmltree;
    }
    g_nNodeCount = 0;
    return 0;
}



/*******************************************************************************
会动态申请空间而不回收,慎用
*******************************************************************************/
int8_t *XmlXString2Char(XString *strXString)
{
    int8_t *pStr = NULL;

    if(NULL == strXString)
    {
        return NULL;
    }
    //pStr = calloc((strXString->m_nLen)+1 , sizeof(int8_t));
    pStr = new int8_t[(strXString->m_nLen)+1];
    if(NULL == pStr)
    {
        qDebug("calloc error\n");
    }
    if(NULL == pStr)
    {
        return NULL;
    }
    memcpy(pStr , strXString->m_chText , strXString->m_nLen);
    pStr[strXString->m_nLen] = '\0';
    return pStr;
}

/*******************************************************************************
字符串比较,如果strXString == strChar返回1,不相等返回0
*******************************************************************************/
uint8_t Xmlgx_strsame(XString strXString , int8_t *strChar)
{
    int32_t i;

    if(strXString.m_nLen == strlen((char *)strChar))
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
XString *XmlgetAttrValue(XAttr *m_pAttr , int8_t * chAttrName)
{
    while(m_pAttr != NULL)
    {
        if(Xmlgx_strsame(m_pAttr->m_strAttrName , chAttrName))
        {
            return(&m_pAttr->m_strAttrValue);
            break;
        }
        m_pAttr = m_pAttr->m_pNextAttr;
    }
    return NULL;
}
