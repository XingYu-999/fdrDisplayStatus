#include "mainDialog.h"
#include "ui_MainDialog.h"

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog) {
    qDebug() << "Dialog主线程号：" << QThread::currentThreadId();

    ui->setupUi(this);
    initPar();              //初始化参数值
    initUi();               //初始化Ui界面

    getFdrInfoHttpClient1->moveToThread(qtWorkThread1);

    /************************信号槽连接***********************/
    connect(timer1000, &QTimer::timeout, this, &MainDialog::sloTimerProcess);
    connect(getFdrInfoHttpClient1, &GetFdrInfoClient::sigGetFdrStatus, this, &MainDialog::solProcessData);
    connect(mainWinPreference, &MainWinPreference::sigCommunicationSChange, this, &MainDialog::solCommunciationChange);
    //开启1s定时器
    timer1000->start(1000);
}

MainDialog::~MainDialog()
{
    delete ui;
}

//初始化参数值
void MainDialog::initPar()
{
    //参数初始化或写入初始值
    iErrCount = 14;
    uiDisplayTopSize = 25;
    uiDisplayCenterSize = 40;
    fdrErrList
        << "未知"
        << "运行"
        << "停止"
        << "光幕异常"
        << "邮件间距异常"
        << "邮件超高"
        << "伺服驱动器报警"
        << "违规停机"
        << "邮件纵向超标"
        << "邮件横向超标"
        << "通讯异常"
        << "扫码异常"
        << "相机异常"
        << "邮件超重";

    initSysTrayIcon();      //托盘图标显示
}

//初始化Ui
void MainDialog::initUi()
{
    this->setWindowTitle("供包台状态显示");
    this->setAttribute(Qt::WA_TranslucentBackground,true);                          //设置背景透明
    //弹出式窗口(系统任务栏不显示) | 窗口置顶 | 无边框窗口 | 禁用阴影
    this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    this->resize(400,50);                                                           //设置窗口大小

    //设置label
    this->ui->label->setStyleSheet("color:gray");
    this->ui->label->setText("正在初始化自检");
    QFont ft;
    ft.setPointSize(20);
    this->ui->label->setFont(ft);

    //获取显示器高度和宽度
//    QList<QScreen *> list_screen =  QGuiApplication::screens();                   //多显示器
//    QRect rect = list_screen.at(0)->geometry();                                   //获取显示器大小
//    iDesktopWidth = rect.width();                                                 //获取显示器宽度
//    iDesktopHigh = rect.height();                                                 //获取显示器高度

    //获取可用显示高度和宽度
    desktop = QGuiApplication::primaryScreen()->availableSize();
    iDesktopWidth = desktop.width();
    iDesktopHigh = desktop.height();

    this->move(iDesktopWidth / 2 - 200, -10);                                       //显示置于中上
//    this->move(iDesktopWidth / 2 - 200, iDesktopHigh / 2 - 30);                   //显示置于正中

    //设置窗口图标
    QIcon icon = QIcon(":/icon/fdrPanel.bmp");    //资源文件添加的图标
    this->setWindowIcon(icon);

    //设置窗口字体大小
    ft.setPointSize(uiDisplayTopSize);
    this->ui->label->setFont(ft);
}

//处理数据
void MainDialog::solProcessData(double dbFdrStatus)
{
    //复位接收计时器
    bTimeOut = false;
    usReciveTimeOutCounter = 0;

    int iFdrStatus = dbFdrStatus;
    switch (iFdrStatus)
    {
    case 0:
        this->ui->label->setStyleSheet("color:gray");                               //设置label字体颜色灰色
        this->move(iDesktopWidth / 2 - 200, -10);                                   //显示置于中上
        ft.setPointSize(uiDisplayTopSize);                                          //字体大小修改
        this->ui->label->setFont(ft);

        this->ui->label->setText("未知0");
        break;
    case 1:
        this->ui->label->setStyleSheet("color:green");                              //设置label字体颜色绿色
        this->move(iDesktopWidth / 2 - 200, -10);                                   //显示置于中上
        ft.setPointSize(uiDisplayTopSize);                                          //字体大小修改
        this->ui->label->setFont(ft);

        this->ui->label->setText("供包台运行中");
        break;
    case 2:
        this->ui->label->setStyleSheet("color:yellow");                             //设置label字体颜色红色
        this->move(iDesktopWidth / 2 - 200, -10);                                   //显示置于中上
        ft.setPointSize(uiDisplayTopSize);                                          //字体大小修改
        this->ui->label->setFont(ft);

        this->ui->label->setText("供包台停止");
        break;
    default:

        if(iFdrStatus < iErrCount) {
            this->move(iDesktopWidth / 2 - 200, iDesktopHigh / 2 - 30);                 //显示置于正中
            ft.setPointSize(uiDisplayCenterSize);                                       //字体大小修改
            this->ui->label->setFont(ft);

            this->ui->label->setText(fdrErrList[iFdrStatus]);
        }
        else {
            this->move(iDesktopWidth / 2 - 200, -10);                                   //显示置于中上
            ft.setPointSize(uiDisplayTopSize);                                          //字体大小修改
            this->ui->label->setFont(ft);

            this->ui->label->setText("未知代码:" + QString::number(iFdrStatus));
        }

        this->ui->label->setStyleSheet("color:red");                              //设置label字体颜色
    }
}

//每1s处理一次
void MainDialog::sloTimerProcess()
{
    switch(usStep)
    {
    case 0:
//        qDebug() << "初始检测连接测试";
        if(checkIp(mainWinPreference->qstrAddress)) usStep = 1;    //检测本机是否存在配置的ip
        else usStep = 2;
            usLastStep = 0;
        break;
    case 1:
        if(usLastStep != 1) {
//            qDebug() << "初始检测ip存在";4
            usStep = 3;
            usLastStep = 1;
            //配置和配置文件同步
            getFdrInfoHttpClient1->iLevelNo = mainWinPreference->iLevelNum;
            getFdrInfoHttpClient1->iFeederId = mainWinPreference->iFdrNum;
            getFdrInfoHttpClient1->qStrGetFdrStatusUrl = mainWinPreference->qstrUrl;
            //配置接口超时重试时间
            getFdrInfoHttpClient1->iPostTimeoutTime = mainWinPreference->iTimeOutMs;

            //托盘栏图标显示内容更新
            QString qstrLevelNum = "下";
            if(mainWinPreference->iLevelNum == 1) {
                qstrLevelNum = "上";
            }
            m_sysTrayIcon->setToolTip("供件台报警显示 " + qstrLevelNum + "层:" + QString::number(mainWinPreference->iFdrNum));

            //开启供件台状态获取子线程
            qtWorkThread1->start();
            //开启子线程定时器
            getFdrInfoHttpClient1->timerPostHttp->start(mainWinPreference->iGetStatusIntervalMs);
        }
        break;
    case 2:
        if(usLastStep != 2) {
            qDebug() << "初始检测ip不存在";
            usRecoverCounter = 0;
            this->ui->label->setStyleSheet("color:gray");                               //设置label字体颜色
            this->move(iDesktopWidth / 2 - 200, -10);                                   //显示置于中上
            ft.setPointSize(uiDisplayTopSize);                                          //字体大小修改
            this->ui->label->setFont(ft);

            this->ui->label->setText("本机配置ip缺失");
            usLastStep = 2;
        }
        if (checkIp(mainWinPreference->qstrAddress)) {
            this->ui->label->setText("配置ip已存在，恢复中 " + QString::number(5 - usRecoverCounter));
            usRecoverCounter += 1;
            if(usRecoverCounter >= 5) usStep = 1;
        }
        else {
            usRecoverCounter = 0;
            this->ui->label->setText("本机配置ip缺失");
        }
        break;
    case 3:
        if(usLastStep != 3) {
//            qDebug() << "进入运行步骤";
            if(this->ui->label->text() == "正在初始化自检") {
                this->ui->label->setStyleSheet("color:gray");                           //设置label字体颜色
                this->move(iDesktopWidth / 2 - 200, -10);                               //显示置于中上
                ft.setPointSize(uiDisplayTopSize);                                      //字体大小修改
                this->ui->label->setFont(ft);

                this->ui->label->setText("准备接收数据");
            }
            bTimeOut = false;
            usLastStep = 3;
        }
        if (!checkIp(mainWinPreference->qstrAddress)) usStep = 4;
        //接收数据超时检测
        usReciveTimeOutCounter += 1;
        if(usReciveTimeOutCounter >= (mainWinPreference->iTimeOutMs/1000) && !bTimeOut) {
            bTimeOut = true;
            usReciveTimeOutCounter = 0;
            this->ui->label->setStyleSheet("color:gray");                               //设置label字体颜色
            this->move(iDesktopWidth / 2 - 200, -10);                                   //显示置于中上
            ft.setPointSize(uiDisplayTopSize);                                          //字体大小修改
            this->ui->label->setFont(ft);
            this->ui->label->setText("上位机发送数据超时");
        }
        break;
    case 4:
        if(usLastStep != 4) {
//            qDebug() << "中途断网";
            usRecoverCounter = 0;
            this->ui->label->setStyleSheet("color:gray");                               //设置label字体颜色
            this->move(iDesktopWidth / 2 - 200, -10);                                   //显示置于中上
            ft.setPointSize(uiDisplayTopSize);                                          //字体大小修改
            this->ui->label->setFont(ft);

            this->ui->label->setText("本机配置ip缺失");
            usLastStep = 4;
        }
        if (checkIp(mainWinPreference->qstrAddress)) {
            this->ui->label->setText("配置ip已存在，恢复中 " + QString::number(5 - usRecoverCounter));
            usRecoverCounter += 1;
            if(usRecoverCounter >= 5) {
                bTimeOut = false;
                usReciveTimeOutCounter = 0;
                usStep = 3;
            }
        }
        else {
            usRecoverCounter = 0;
            this->ui->label->setText("网线断开");
        }
    case 5: //通讯参数修改
        if(usLastStep == 0) {
            usStep = 0;
        }
        else {
            usStep = 3;
            usLastStep = 1;

            //配置和配置文件同步
            getFdrInfoHttpClient1->iLevelNo = mainWinPreference->iLevelNum;
            getFdrInfoHttpClient1->iFeederId = mainWinPreference->iFdrNum;
            getFdrInfoHttpClient1->qStrGetFdrStatusUrl = mainWinPreference->qstrUrl;
            //配置接口超时重试时间
            getFdrInfoHttpClient1->iPostTimeoutTime = mainWinPreference->iTimeOutMs;

            //关闭子线程定时器
            getFdrInfoHttpClient1->timerPostHttp->stop();
            //重新开启子线程定时器
            getFdrInfoHttpClient1->timerPostHttp->start(mainWinPreference->iGetStatusIntervalMs);
            //重置超时计数器
            usReciveTimeOutCounter = 0;
            bTimeOut = false;

            //托盘栏图标显示内容更新
            QString qstrLevelNum = "下";
            if(mainWinPreference->iLevelNum == 1) {
                qstrLevelNum = "上";
            }
            m_sysTrayIcon->setToolTip("供件台报警显示 " + qstrLevelNum + "层:" + QString::number(mainWinPreference->iFdrNum));

            getFdrInfoHttpClient1->HttpConnectStatus = -1;

            this->ui->label->setStyleSheet("color:gray");                               //设置label字体颜色
            this->move(iDesktopWidth / 2 - 200, -10);                                   //显示置于中上
            ft.setPointSize(uiDisplayTopSize);                                          //字体大小修改
            this->ui->label->setFont(ft);
            this->ui->label->setText("通讯参数修改中");
        }
        break;
    }
}

//创建系统托盘
void MainDialog::initSysTrayIcon()
{
    //新建QSystemTrayIcon对象
    m_sysTrayIcon = new QSystemTrayIcon(this);
    //设置托盘图标
    QIcon icon = QIcon(":/icon/fdrPanel.bmp");    //资源文件添加的图标
    m_sysTrayIcon->setIcon(icon);

    //给QSystemTrayIcon添加槽函数
    connect(m_sysTrayIcon, &QSystemTrayIcon::activated,
            [=](QSystemTrayIcon::ActivationReason reason)
    {
        switch(reason)
        {
        case QSystemTrayIcon::Trigger:
            //单击托盘图标
            if(this->isHidden()) {
                this->show();
            }
            break;
        case QSystemTrayIcon::DoubleClick:
            //双击托盘图标
            if(this->isHidden()) {
                this->show();
            }
            break;
        default:
            break;
        }
    });

    //建立托盘操作的菜单
    createActions();
    createMenu();
    //在系统托盘显示此对象
    m_sysTrayIcon->show();
}

//创建动作
void MainDialog::createActions()
{
    qActionLogin = new QAction("登录", this);
    connect(qActionLogin, &QAction::triggered ,this, &MainDialog::sloTryActLogin);
    qActionPerference = new QAction("首选项", this);
    connect(qActionPerference, &QAction::triggered ,this, &MainDialog::sloTryActPreference);
    qActionAboutSystem = new QAction("关于", this);
    connect(qActionAboutSystem,SIGNAL(triggered()),this,SLOT(sloTryActAboutSys()));
    qActionExit = new QAction("退出", this);
    connect(qActionExit,SIGNAL(triggered()),this,SLOT(sloTryActExit()));
}

//创建托盘菜单
void MainDialog::createMenu()
{
    qmTryMenu = new QMenu(this);
    if(mainWinPreference->bEnLoginSystem) {
        qmTryMenu->addAction(qActionLogin);        //新增菜单项---登录
    }
    qmTryMenu->addAction(qActionPerference);    //新增菜单项---首选项
    qmTryMenu->addAction(qActionAboutSystem);  //新增菜单项---关于系统
    qmTryMenu->addSeparator();                 //增加分隔符
    qmTryMenu->addAction(qActionExit);         //新增菜单项---退出程序
    m_sysTrayIcon->setContextMenu(qmTryMenu);  //把QMenu赋给QSystemTrayIcon对象
}

//托盘登录动作
void MainDialog::sloTryActLogin()
{
    emit widgetLogin->sigShowWindow();
}

//托盘首选项动作
void MainDialog::sloTryActPreference()
{
    emit mainWinPreference->sigShowWindow();
}

//托盘关于系统动作
void MainDialog::sloTryActAboutSys()
{
    QString qstrBuildTime = buildDateTime();   //获取最后编译时间
    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox qMesgShowMassage(this);
    qMesgShowMassage.setWindowTitle("关于 - 供件台状态显示");
    qMesgShowMassage.setText("嘉兴善索智能科技有限公司 copyright©2025"
                             "<br> version: " SOFTWARE_VERSION
                             "<br> build time: " + qstrBuildTime +
                             "<br> BUG Reporting & Source Code: <a href='https://github.com/XingYu-999/fdrDisplayStatus.git'>Github</a>"
                             "<br> E-mail: fubiju@gmail.com");
//    qMesgShowMassage.setStandardButtons(QMessageBox::Yes);
    qMesgShowMassage.addButton("确定", QMessageBox::YesRole);                 //自定义按钮
//    qMesgShowMassage.setButtonText(QMessageBox::Yes, QString("确 定"));

    //显示位置
    qMesgShowMassage.move(iDesktopWidth / 2 - 200, iDesktopHigh / 2 - 20);
    //添加LOGO
    qMesgShowMassage.setIconPixmap(QPixmap(":/image/SansoAiLogo.svg"));
    //显示
    qMesgShowMassage.exec();
}

//托盘退出动作
void MainDialog::sloTryActExit()
{
    qApp->exit();
}

//主窗口关闭事件重写
void MainDialog::closeEvent(QCloseEvent *event)
{
    //忽略窗口关闭事件
    QApplication::setQuitOnLastWindowClosed( true );
    //隐藏主窗口
    this->hide();
    event->ignore();
    //托盘图标显示信息
    m_sysTrayIcon->showMessage("退出提示","单机或双击右下角图标重新显示，右击退出",QSystemTrayIcon::Information,1000);
}

//检测本地是否有此Ip
bool MainDialog::checkIp(QString qStrCheckIp)
{
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface networkInterface, list) {
        QNetworkInterface::InterfaceFlags flags = networkInterface.flags();

        //网卡被使用且不是环回网卡
        if(flags.testFlags(QNetworkInterface::IsRunning) && !flags.testFlags(QNetworkInterface::IsLoopBack)) {
            //            qDebug() << "###################################";
            //            qDebug() << "设备名：" << networkInterface.name();
            //            qDebug() << "可读名称：" << networkInterface.humanReadableName();
            //            qDebug() << "硬件地址：" << networkInterface.hardwareAddress();
            //            qDebug() << "isValid：" << networkInterface.isValid();

            //获取ip列表
            QList ipList = networkInterface.allAddresses();
            foreach(QHostAddress ip, ipList) {
                if(ip.protocol() == QAbstractSocket::IPv4Protocol && (QHostAddress(qStrCheckIp) == ip)) {
                    return true;
                }
            }
            //            qDebug() << "###################################";
        }
    }
    return false;
}


//获取最后编译时间
QString MainDialog::buildDateTime()
{
    QString dateTime;
    dateTime += __DATE__;
    dateTime += __TIME__;
    dateTime.replace(" ", "");
    QDateTime qdBuildTime = QLocale(QLocale::English).toDateTime(dateTime, "MMMdyyyyhh:mm:ss");
//    return qdBuildTime.toString("yyyy-MM-dd:hh:mm:ss");
    return qdBuildTime.toString("yyyy-MM-dd");
}

void MainDialog::solCommunciationChange()
{
    usLastStep = usStep;
    usStep = 5;
}
