/* Copyright (c) 2008 吴卫军wuwj  */
#ifndef XML_PARSE_INCLUDED
#define XML_PARSE_INCLUDED 

typedef struct
{
	char *m_chText;    /* 文本,不一定\0为字串结束标志   */
	int  m_nLen;       /* 文本长度                      */
} XString;             /* X字串, 只定义指针,不申请空间  */


struct XAttr_s;
typedef struct XAttr_s XAttr;
struct XAttr_s 
{
    XString m_strAttrName;
	XString m_strAttrValue;
	XAttr   *m_pNextAttr;
};

struct XNode_s;
typedef struct XNode_s XNode;
struct XNode_s
{
	XString m_strBuf ;        /* 节点源存储     */
	XString m_strName ;       /* 节点名         */ 
	XAttr   *m_pAttr;         /* 节点属性       */
	XNode   *m_pParentNode;   /* 父节点         */ 
	XNode   *m_pChildNode;    /* 子节点         */ 
	XNode   *m_pNextNode;     /* 下一个兄弟节点 */ 
	int     m_nIndex ;        /* 索引           */
	int     m_nLevel ;        /* 级别           */
	int     m_nTagCount ;     /* 子节点数       */
};

int XmlParserRead(char *xmlbuf, int xmlsize, XNode **pXmltree);
int XmlParserFree(XNode *xmltree);
int showxml(XNode *pXmltree);
signed char *XmlXString2Char(XString *strXString);
unsigned char Xmlgx_strsame(XString strXString , signed char *strChar);
XString *XmlgetAttrValue(XAttr *m_pAttr , signed char * chAttrName);
#endif 

