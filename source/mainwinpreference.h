#ifndef MAINWINPREFERENCE_H
#define MAINWINPREFERENCE_H

#include "messagetips.h"
#include "backGroundData.h"

#include <QMainWindow>
#include <QObject>
#include <QMessageBox>
#include <QSettings>

#include <QtXml>
#include <QDomDocument>

namespace Ui {
class MainWinPreference;
}

class MainWinPreference : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWinPreference(QWidget *parent = nullptr);
    ~MainWinPreference();

    QString qStrSoftwareVersion = "0.0";        //配置文件版本
    QString qStrXmlVersion = "0.0";             //配置文件版本

    bool bStartOnSystem = false;                //开机自启
    bool bDisplayLogo = false;                  //显示LOGO
    bool bEnLoginSystem = false;                //登录程序

    QString qstrAddress = "127.0.0.1";          //IP地址
    int iPort = 10020;                          //端口号
    QString qstrUrl = "http://127.0.0.1:10020/jtsorterservice/getfeederstatus";
    int iTimeOutMs = 1000;                     //超时时间
    int iGetStatusIntervalMs = 5000;            //轮询间隔

    int iLevelNum = 100;                        //分拣机层号
    int iFdrNum = 100;                          //分拣机编号
    int iStatusCodeCount = 14;                  //状态总数

    int iDisplayTopSize = 2;                    //顶部显示字号
    int iDisplayCenterSize = 4;                 //中心显示字号


private:
    QDomDocument qdSystemConfigXml;


    bool configXmlRead();
    void configXmlCreat();
    bool openXmlFile(QString qstrFilePath);
    bool changeXmlFile(QString qstrRootName, QString qstrNodeName, QString qstrElementName, QString qstrNewValue);
    void uiInit();

    void addElement(QDomNode father, QDomDocument qdDoc, QString elementName, QString elementText);

signals:
    void sigShowWindow();
    void sigCommunicationSChange();

private slots:
    void on_btnCancel_clicked();
    void on_btnYes_clicked();

    void on_btnApply_clicked();

private:
    Ui::MainWinPreference *ui;
};

#endif // MAINWINPREFERENCE_H
