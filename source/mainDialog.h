#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include "getFdrInfoClient.h"
#include "widgetlogin.h"
#include "mainwinpreference.h"
#include <QDialog>
#include <QThread>
#include <QDebug>
#include <QScreen>
#include <QRect>
#include <QUdpSocket>
#include <QtXml>
#include <QDomDocument>
#include <QMessageBox>
#include <QThread>
#include <QLabel>
#include <QBitmap>
#include <QSize>
#include <QTimer>

//系统托盘用
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();

    /************************UI管理***************************/
    Ui::Dialog *ui;         //UI界面
    //窗口字体大小辅助类
    QFont ft;
    //窗口大小
    QSize desktop;
    int iDesktopWidth;
    int iDesktopHigh;
    WidgetLogin* widgetLogin = new WidgetLogin;
    MainWinPreference* mainWinPreference = new MainWinPreference;    //首选项配置窗口
    /************************UI管理***************************/

    //线程管理
    QThread* qtWorkThread1 = new QThread;

    QTimer* timer1000 = new QTimer;     //1s定时器
    GetFdrInfoClient* getFdrInfoHttpClient1 = new GetFdrInfoClient;

    int iErrCount;                      //报警类型总数
    uint uiDisplayTopSize;              //顶部字体大小显示
    uint uiDisplayCenterSize;           //正中字体大小显示
    QStringList fdrErrList;             //报警信息列表

    bool bIpGetted = false;             //接收ip状态
    bool bTimeOut = false;              //数据接收超时状态
    short usReciveTimeOutCounter = 0;   //接收超时计数
    short usStep = 0;                   //当前执行步骤
    short usLastStep = 0;               //上一执行步骤
    short usRecoverCounter = 0;         //恢复状态计数

    //公有函数
    void initUi();
    void initPar();
    bool checkIp(QString qStrCheckIp);

    //系统托盘
    QSystemTrayIcon *m_sysTrayIcon;
    QMenu *qmTryMenu;               //托盘菜单
    QAction *qActionLogin;          //登录动作
    QAction *qActionPerference;     //首选项动作
    QAction *qActionAboutSystem;    //关于系统动作
    QAction *qActionExit;           //退出动作
    void createActions();
    void createMenu();
    void closeEvent (QCloseEvent *event) override;
    void initSysTrayIcon();

    QString buildDateTime();

signals:

public slots:
    void solProcessData(double dbFdrStatus);    //接收数据处理
    void sloTimerProcess();                     //定时器循环处理
    void solCommunciationChange();              //通讯配置发生过改变

    void sloTryActLogin();                      //托盘动作登录
    void sloTryActPreference();                 //托盘动作首选项

    //系统托盘
    void sloTryActAboutSys();
    void sloTryActExit();

private slots:


};
#endif // MAINDIALOG_H
