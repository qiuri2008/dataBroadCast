
#include "uial_info_service.h"
#include "bmp8_to_16.h"
#include "bmp16_to_16.h"
#include "bmp24_to_16.h"
#include "bmp32_to_16.h"
#include <math.h>

extern XmlRoot_t *pFirstPage;
extern XmlRoot_t *pSecondPage;

#define BUTTEN_SELECT_COLOR    "[#808080,#ffff00,#808080]"

typedef struct SERVICEBUTTON
{
	int button_x;
	int button_y;
	int button_w;
	int button_h;
	char *pHref;
	char name[30];
	struct SERVICEBUTTON *pNextButton;
}ServiceButton,*pServiceButton;

typedef struct SERVICEBUTTON_GROUP
{
	int total;
	struct SERVICEBUTTON *pFirstbutton;
}ServiceButton_Group,*pServiceButton_Group;

void Button_free(pServiceButton pButton)
{
	if(pButton->pNextButton != NULL)
	{
		Button_free(pButton->pNextButton);
	}
	free(pButton);
	pButton = NULL;
}

void ButtonGroup_free(pServiceButton_Group pButtonGroup)
{
	pButtonGroup->total = 0;
	Button_free(pButtonGroup->pFirstbutton);
	pButtonGroup->pFirstbutton = NULL;
	free(pButtonGroup);
}

int create_sevice_index(void)
{
    char path[256];
    uint32_t * pRoot;
    int i = 0;
    int iElementType = 0;
    int ret = 0;
    int length = 0;

    length = strlen((char*)pFirstPage->m_pchBackgroudName);
    pFirstPage->m_pchBackgroudName[length-1] = 'n';
    pFirstPage->m_pchBackgroudName[length-2] = 'i';
    pFirstPage->m_pchBackgroudName[length-3] = 'b';
    sprintf(path, "%s%s", DATACAST_BMP_PATH, "logo.bin");//pFirstPage->m_pchBackgroudName);
    if(1)
    {
        FILE *fp = NULL;
        fp = fopen(path, "wr");
        if(NULL == fp)
        {
            return (1);
        }
        ret = fwrite((void*)(pFirstPage->m_nBackgroudAddr), pFirstPage->m_nBackgroudLength, 1, fp);
        //ret = fwrite((void*)(pFirstPage->m_nBackgroudAddr), pFirstPage->m_nBackgroudLength, 1, fp);

        fclose(fp);
    }

    pRoot = (uint32_t *)pFirstPage;
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
                        if(0)//!((pElementImage_t)pRoot)->m_pchHref)
                        {
                            sprintf(path, "%s%s", DATACAST_BMP_PATH, ((pElementImage_t)pRoot)->m_pchImageName);
                            bmp_8bpp_to_16bpp1(((pElementImage_t)pRoot)->m_pImageAddr, ((pElementImage_t)pRoot)->m_nImageLength,path);
                            qDebug()<<path;
                        }
                        else if(1)
                        {
                            {
                            sprintf(path, "%s%s", DATACAST_BMP_PATH, ((pElementImage_t)pRoot)->m_pchImageName);
                            //bmp_8bpp_to_16bpp(((pElementImage_t)pRoot)->m_pImageAddr, ((pElementImage_t)pRoot)->m_nImageLength,path);
                            unsigned char *buffer;
                            unsigned char *buffer1;
                            buffer = (unsigned char *)malloc(95536);
                            //buffer1 = bmp_8bpp_to_16bppBuff3(((pElementImage_t)pRoot)->m_pImageAddr,buffer);
                            //sprintf(path, "%s%s", DATACAST_BMP_PATH, ((pElementImage_t)pRoot)->m_pchImageName,"_1");
                            FILE *fp = NULL;
                            fp = fopen(path, "wr");
                            if(NULL == fp)
                            {
                                return (1);
                            }
                            //fwrite(buffer, abs(buffer1-buffer), 1, fp);
                            fwrite(((pElementImage_t)pRoot)->m_pImageAddr, 40*1024, 1, fp);
                            fclose(fp);
                            free(buffer);
                            }
                            {
                                sprintf(path, "%s%s -", DATACAST_BMP_PATH, ((pElementImage_t)pRoot)->m_pchImageName);
                                //bmp_8bpp_to_16bpp(((pElementImage_t)pRoot)->m_pImageAddr, ((pElementImage_t)pRoot)->m_nImageLength,path);
                                unsigned char *buffer;
                                unsigned char *buffer1;
                                buffer = (unsigned char *)malloc(95536);
                                //buffer1 = bmp_8bpp_to_16bppBuff3(((pElementImage_t)pRoot)->m_pImageAddr,buffer);
                                //sprintf(path, "%s%s", DATACAST_BMP_PATH, ((pElementImage_t)pRoot)->m_pchImageName,"_1");
                                FILE *fp = NULL;
                                fp = fopen(path, "wr");
                                if(NULL == fp)
                                {
                                    return (1);
                                }
                                //fwrite(buffer, abs(buffer1-buffer), 1, fp);
                                fwrite(((pElementImage_t)pRoot)->m_pImageAddr, 40*1024, 1, fp);
                                fclose(fp);
                                free(buffer);
                            }
                        }

                        pRoot = ((pElementImage_t)pRoot)->m_pnNextElement;
                    }
                    break;
                default:
                    break;
            }
        }
    }


    return 0;
}


