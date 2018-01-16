#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "dvb.h"
#include "Demux.h"
#include "sction.h"
#include "type_.h"
#include "batParse.h"
#include "comParse.h"
#include <QLabel>
#include <QImage>
#include "com_is_buf.h"
#include "uial_info_service.h"
#include <QTextBrowser>
#include <QLineEdit>
#include <QTextEdit>
#include <QTimer>

char g_strPathBmp[256];
char g_strPathXml[256];
char g_strPathIFrema[256];
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(TimerTsParseOk()));
    timer->start(500);
    g_bPaseOk = false;
    QDir directory;
    QString mPath = directory.currentPath();
    sprintf(g_strPathBmp,"%s%s",mPath.toLatin1().data(),"/absBmp/");
    sprintf(g_strPathXml,"%s%s",mPath.toLatin1().data(),"/absXml/");
    sprintf(g_strPathIFrema,"%s%s",mPath.toLatin1().data(),"/absIframe/");
}


void MainWindow::on_openFile_clicked()
{
    QString path;
    path = QFileDialog::getOpenFileName(this,tr("选择码流"),mPath + "ts",tr("TS File(*.ts)"));

    if(path.isEmpty())
        return ;
    unsigned char buf[65535];
    QFile pTSFile(path);

    if(!pTSFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(NULL, tr("提示"), tr("文件载入错误!"));
        return ;
    }

    QDataStream file(&pTSFile);

    pdvb pBatdvb;
    void *pDemux;
    int size;
    //=========================
    //重复载入TS文件时,要先释放资源
    DataCastInit();
    gx_exit_IS();
    com_is_close();
    //=========================
    pBatdvb = (dvb *)dvb_create();
    pDemux = (SDemux *)Demux_Create();
    pBatdvb->m_pDemux = pDemux;

    //创建的过滤器，全部加入到Demux的SFilterNode链表内.返回当前创建的过滤器节点地址，赋予dvb的m_pFilter[0],返回节点暂无用处
    pBatdvb->m_pFilter[0] = Demux_CreateFilterEx(pBatdvb->m_pDemux, 0x11, 0x4A, 0xff, 0, 0, 4096, bat_filter, (uint32_t)pBatdvb);

    while(!file.atEnd())
    {
        //如果过滤器为空,则不过滤任何数据
        if(((SDemux *)(pBatdvb->m_pDemux))->list == NULL)
            break;
        size = file.readRawData((char*)buf,sizeof(buf));
        if( size <= 0 )
        {
            break;
        }
        Demux_OnData(pBatdvb->m_pDemux, buf, size);
    }

    //销毁全部过滤器及解复用器
    Demux_Destroy(pBatdvb->m_pDemux);
    dvb_destroy(pBatdvb);

    pTSFile.close();
}

void MainWindow::displayBmp()
{
    QLabel *lable = new QLabel(this);
    QImage *img = new QImage;
    if(!img->load("C:/002_Entrance_01.bmp"))
        QMessageBox::information(this,tr("打开图像失败"), tr("打开图像失败！"));
    lable->setPixmap(QPixmap::fromImage(*img));
}

void MainWindow::on_pushButton_clicked()
{
    QString filename;
    filename=QFileDialog::getOpenFileName(this,tr("选择BMP"),"",tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
    if(filename.isEmpty())
    {
        return;
    }
    else
    {
//        QLabel *lable = new QLabel(this);

//        QImage *img = new QImage();
//        if(!img->load(filename))
//            QMessageBox::information(NULL, tr("提示"), tr("文件载入错误"));
//        lable->setGeometry(150,100,img->size().width(),img->size().height());
//        lable->setPixmap(QPixmap::fromImage(*img));
//        lable->show();

    }

}

void MainWindow::on_pushBtnIframe_clicked()
{
    char buf[65536];
    int size= 0;
    QString path;
    path = QFileDialog::getOpenFileName(this,tr("选择I帧"),"",tr("IFrme(*.bin)"));
    if(path.isEmpty())
        return ;
    QFile iFrameFile(path);
    if(!iFrameFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(NULL,tr("提示"),tr("文件载入错误"));
        return;
    }
    QDataStream file(&iFrameFile);
    size = file.readRawData(buf,sizeof(buf));
    if(size)
    {
        QFile saveFile(path+"1");
        saveFile.open(QIODevice::WriteOnly);
        QDataStream file(&saveFile);
        for(int j=0;j<=10;j++)
            file.writeRawData(buf,size);
    }
}

void MainWindow::on_pushBtnXML_clicked()
{
    if(!g_bPaseOk)
        return ;

    QFile xmlFile(mPath + lineEdit->text());
    if(!xmlFile.open(QIODevice::WriteOnly))
        return ;
    QDataStream file(&xmlFile);
    int size = 0;
    FileList_t *pFile = NULL;

    gx_find_IS_file((int8_t *)(lineEdit->text().toLatin1().data()), FILE_XML , &pFile);
    size = file.writeRawData((char *)pFile->m_nFileAddr,pFile->m_nFileLength);
}

void MainWindow::on_index_clicked()
{
    if(!g_bPaseOk)
        return ;
    FileList_t *pFile = NULL;
    gx_find_IS_file((int8_t *)(lineEdit->text().toLatin1().data()),FILE_XML,&pFile);
    if(pFile)
    {
        char buf[8192];
        memcpy(buf,(char *)pFile->m_nFileAddr,pFile->m_nFileLength);
        buf[pFile->m_nFileLength] = 0;
        QString xml(buf);
        textBrowser->setWordWrapMode(QTextOption::NoWrap);//设置部件宽度不够时不能自动换行
        textBrowser->setText(xml);
    }
}

void MainWindow::TimerTsParseOk()
{
    if(g_bPaseOk)
    {
        timer->stop();
        lineEdit->setText("0_index.xml");
        textBrowser->setText(tr("单击“索引”，从主页开始解析。。。"));
        on_index_clicked();
    }
}

void MainWindow::on_secondPage_clicked()
{
    create_sevice_third();
}

void MainWindow::on_homePage_clicked()
{
    if(!g_bPaseOk)
        return ;
    FileList_t *pFile = NULL;
    lineEdit->setText("0_index.xml");
    gx_find_IS_file((int8_t *)(lineEdit->text().toLatin1().data()),FILE_XML,&pFile);
    if(pFile)
    {
        char buf[8192];
        memcpy(buf,(char *)pFile->m_nFileAddr,pFile->m_nFileLength);
        buf[pFile->m_nFileLength] = 0;
        QString xml(buf);
        textBrowser->setWordWrapMode(QTextOption::NoWrap);//设置部件宽度不够时不能自动换行
        textBrowser->setText(xml);
    }

}
