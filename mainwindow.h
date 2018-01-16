#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
private slots:
    void on_openFile_clicked();
    void on_pushButton_clicked();

    void on_pushBtnIframe_clicked();

    void on_pushBtnXML_clicked();

    void on_index_clicked();
    void TimerTsParseOk();

    void on_secondPage_clicked();

    void on_homePage_clicked();


public:
    void displayBmp();

private:
    QTimer *timer;
    QString mPath;

};

#endif // MAINWINDOW_H
