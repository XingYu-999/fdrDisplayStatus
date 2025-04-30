#include "mainwinpreference.h"
#include "ui_mainwinpreference.h"

MainWinPreference::MainWinPreference(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWinPreference)
{
    ui->setupUi(this);
    connect(this, &MainWinPreference::sigShowWindow, [=] () {
        this->show();
    });

    if(!configXmlRead()) {
        configXmlCreat();
    }
    uiInit();
}

MainWinPreference::~MainWinPreference()
{
    delete ui;
}

void MainWinPreference::uiInit()
{
    this->ui->cbStartOnSystem->setChecked(bStartOnSystem);
    this->ui->cbEnLoginSystem->setChecked(bEnLoginSystem);

    this->ui->sBoxLevelNo->setMinimum(1);
    this->ui->sBoxLevelNo->setMaximum(3);
    this->ui->sBoxLevelNo->setSingleStep(1);
    this->ui->sBoxLevelNo->setValue(iLevelNum);

    this->ui->sBoxFdrId->setMinimum(1);
    this->ui->sBoxFdrId->setMaximum(12);
    this->ui->sBoxFdrId->setSingleStep(1);
    this->ui->sBoxFdrId->setValue(iFdrNum);

    this->ui->sBoxtimeOut->setMinimum(1000);
    this->ui->sBoxtimeOut->setMaximum(10000);
    this->ui->sBoxtimeOut->setSingleStep(1000);
    this->ui->sBoxtimeOut->setValue(iTimeOutMs);

    this->ui->sBoxHttpIntervalTime->setMinimum(500);
    this->ui->sBoxHttpIntervalTime->setMaximum(5000);
    this->ui->sBoxHttpIntervalTime->setSingleStep(100);
    this->ui->sBoxHttpIntervalTime->setValue(iGetStatusIntervalMs);

    this->ui->lEditGetFdrStatusUrl->setText(qstrUrl);

    this->setWindowTitle("供件台程序 - 首选项");
    // this->setWindowIcon(QPixmap(":/image/SansoAiLogo.svg");

}

bool MainWinPreference::openXmlFile(QString qstrFilePath)
{
    QFile file(qstrFilePath);
    if(!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << QObject::tr("error::ParserXML->OpenXmlFile->file.open->%s\n") << qstrFilePath;
        return false;
    }
    if(!qdSystemConfigXml.setContent(&file)) {
        qDebug() << QObject::tr("error::ParserXML->OpenXmlFile->doc.setContent\n") << qstrFilePath;
        file.close();
        return false;
    }
    file.close();
    return true;
}

//改变配置文件
bool MainWinPreference::changeXmlFile(QString qstrRootName, QString qstrNodeName, QString qstrElementName, QString qstrNewValue)
{
    //配置写入文件
    if(!openXmlFile(CONFIG_FILE_PATH)) {
        return false;
    }

    // 获得根节点
    QDomElement root = qdSystemConfigXml.documentElement();
    //判断根节点名称是否符合
    if(root.tagName().compare(qstrRootName) != 0) {
        return false;
    }
    // 读取子节点
    QDomNode node = root.firstChild();
    while(!node.isNull()) {
        //获取第一层子节点元素
        QDomElement nodeElement = node.toElement();
        if(!nodeElement.isNull()) {
            //判断第一层子节点名字
            if(nodeElement.nodeName().compare(qstrNodeName) == 0) {
                QDomNodeList list = nodeElement.childNodes();
                for(int i = 0; i < list.count(); i++) {
                    //获取元素名称
                    QDomNode node = list.at(i);
                    if(node.isElement()) {
                        if(node.nodeName().compare(qstrElementName) == 0) {
                            //修改值
                            QDomNode oldnode = node.firstChild();
                            node.firstChild().setNodeValue(qstrNewValue);
                            QDomNode newnode = node.firstChild();
                            node.replaceChild(newnode,oldnode);
                        }
                    }
                }
            }
        }
        //第一层子节点下一节点
        node = node.nextSibling();
    }

    //重写入文件
    QFile qfSystemConfig(CONFIG_FILE_PATH);
    if(!qfSystemConfig.open(QFile::WriteOnly | QFile::Truncate)) {
        return false;
    }

    QTextStream ts(&qfSystemConfig);
    ts.reset();
    //    ts.setCodec("utf-8");
    qdSystemConfigXml.save(ts, 4, QDomNode::EncodingFromTextStream);
    qfSystemConfig.close();

    return true;
}

//系统配置文件读取
bool MainWinPreference::configXmlRead()
{
    QDomDocument doc;
    QFile file(CONFIG_FILE_PATH);

    if(!file.open(QFileDevice::ReadOnly)) {
        QMessageBox::information(NULL, "提示", "系统配置文件不存在!"
                                                 "<br> 将新建默认配置文件");
        return false;
    }

    //关联文件
    if (!doc.setContent(&file)) {
        QMessageBox::information(NULL, "提示", "操作的文件不是XML文件或者XML文件有错误！"
                                                 "<br> 将删除原配置文件并新建默认配置文件");
        file.close();
        file.deleteLater();
        return false;
    }

    // 获得根节点
    QDomElement root = doc.documentElement();
    // 读取第一个子节点
    QDomNode node = root.firstChild();

    while(!node.isNull())   {
        QDomNodeList sonList = node.childNodes();       //读取子结点集合
        QString rootName = node.toElement().tagName();  //读取父节点名字

        //软件配置
        if(rootName.compare("software") == 0) {
            for(int sonNode = 0;sonNode < sonList.size();sonNode++) {
                QDomElement sonElement = sonList.at(sonNode).toElement();                           //获取子结点
                if(sonElement.toElement().tagName().compare("softwareVersion") == 0)                //软件版本
                    qStrSoftwareVersion = sonElement.text();
                else if(sonElement.toElement().tagName().compare("xmlVersion") == 0)                //xml版本
                {
                    qStrXmlVersion = sonElement.text();
                    if(qStrXmlVersion != XML_VERSION) {
                        QMessageBox::information(NULL, "提示", "XML版本错误！"
                                                               "<br> 将删除原配置文件并新建默认配置文件");
                        file.close();
                        file.deleteLater();
                        return false;
                    }
                }
            }
        }
        //系统配置
        else if(rootName.compare("system") == 0) {
            for(int sonNode = 0;sonNode < sonList.size();sonNode++) {
                QDomElement sonElement = sonList.at(sonNode).toElement();                           //获取子结点
                if(sonElement.toElement().tagName().compare("startOnSystem") == 0)                  //开机自启
                    bStartOnSystem = sonElement.text().toInt();
                else if(sonElement.toElement().tagName().compare("displayLogo") == 0)               //显示LOGO
                    bDisplayLogo = sonElement.text().toInt();
                else if(sonElement.toElement().tagName().compare("loginSystem") == 0)               //登录系统
                    bEnLoginSystem = sonElement.text().toInt();
            }
        }
        //通讯配置
        else if(rootName.compare("communication") == 0) {
            for(int sonNode = 0;sonNode < sonList.size();sonNode++) {
                QDomElement sonElement = sonList.at(sonNode).toElement();                           //获取子结点
                if(sonElement.toElement().tagName().compare("address") == 0)                        //ip地址
                    qstrAddress = sonElement.text();
                else if(sonElement.toElement().tagName().compare("port") == 0)                      //端口号
                    iPort = sonElement.text().toInt();
                else if(sonElement.toElement().tagName().compare("url") == 0)                       //Url
                    qstrUrl = sonElement.text();
                else if(sonElement.toElement().tagName().compare("timeOutMs") == 0)                 //超时时间
                    iTimeOutMs = sonElement.text().toInt();
                else if(sonElement.toElement().tagName().compare("getStatusIntervalMs") == 0)       //轮询间隔
                    iGetStatusIntervalMs = sonElement.text().toInt();
            }
        }
        //供件台参数
        else if(rootName.compare("fdrParameter") == 0) {
            for(int sonNode = 0;sonNode < sonList.size();sonNode++) {
                QDomElement sonElement = sonList.at(sonNode).toElement();                           //获取子结点
                if(sonElement.toElement().tagName().compare("levelNum") == 0)                       //分拣机层号
                    iLevelNum = sonElement.text().toInt();
                else if(sonElement.toElement().tagName().compare("fdrNum") == 0)                    //供件台编号
                    iFdrNum = sonElement.text().toInt();
                else if(sonElement.toElement().tagName().compare("statusCodeCount") == 0)           //供件台编号
                    iStatusCodeCount = sonElement.text().toInt();
            }
        }
        //显示格式
        else if(rootName.compare("display") == 0) {
            for(int sonNode = 0;sonNode < sonList.size();sonNode++) {
                QDomElement sonElement = sonList.at(sonNode).toElement();                           //获取子结点
                if(sonElement.toElement().tagName().compare("displayTopSize") == 0)                 //顶部显示字号
                    iDisplayTopSize = sonElement.text().toInt();
                else if(sonElement.toElement().tagName().compare("displayCenterSize") == 0)         //中心显示字号
                    iDisplayCenterSize = sonElement.text().toInt();
            }
        }

        node = node.nextSibling();                      //读取下一父节点
    }

    if(qStrXmlVersion != XML_VERSION) {
        QMessageBox::information(NULL, "提示", "XML版本错误！"
                                               "<br> 将删除原配置文件并新建默认配置文件");
        file.close();
        file.deleteLater();
        return false;
    }

    file.close();
    return true;
}

//创建配置文件
void MainWinPreference::configXmlCreat()
{
    QDomDocument doc;
    QFile file(CONFIG_FILE_PATH);
    file.open(QFileDevice::ReadWrite | QFileDevice::Truncate);  // 文件存在则创建，否则创建一个文件
    QDomProcessingInstruction instruction;  // 创建XML处理类，通常用于处理第一行描述信息
    // 创建XML头部格式
    instruction = doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    doc.appendChild(instruction);   // 添加到XML文件中

    // 创建根节点Basic
    QDomElement Basic = doc.createElement("Basic");
    doc.appendChild(Basic);
        //添加子节点software
        QDomElement software = doc.createElement("software");
        Basic.appendChild(software);
            //软件版本
            addElement(software, doc, "softwareVersion", SOFTWARE_VERSION);
            //xml版本
            addElement(software, doc, "xmlVersion", XML_VERSION);
        Basic.appendChild(software);

        //添加子节点system
        QDomElement system = doc.createElement("system");
        Basic.appendChild(system);
            //开机自启
            addElement(system, doc, "startOnSystem", "1");
            //显示logo
            addElement(system, doc, "displayLogo", "1");
            //登录系统
            addElement(system, doc, "loginSystem", "0");
        Basic.appendChild(system);

        //通讯配置
        QDomElement communication = doc.createElement("communication");
        Basic.appendChild(communication);
            //IP地址
            addElement(communication, doc, "address", "127.0.0.1");
            //端口
            addElement(communication, doc, "port", "10020");
            //链接接口
            addElement(communication, doc, "url", "http://192.168.5.7:38900/jtsorterservice/getfeederstatus");
            //超时时间
            addElement(communication, doc, "timeOutMs", "10000");
            //获取状态间隔
            addElement(communication, doc, "getStatusIntervalMs", "500");
        Basic.appendChild(communication);

        //供件台参数
        QDomElement fdrParameter = doc.createElement("fdrParameter");
        Basic.appendChild(fdrParameter);
            //层号
            addElement(fdrParameter, doc, "levelNum", "1");
            //供件台编号
            addElement(fdrParameter, doc, "fdrNum", "1");
            //错误代码总数
            addElement(fdrParameter, doc, "statusCodeCount", "14");
        Basic.appendChild(fdrParameter);

        //供件台参数
        QDomElement display = doc.createElement("display");
        Basic.appendChild(display);
            //在顶部显示文字大小
            addElement(display, doc, "displayTopSize", "25");
            //在中间显示文字大小
            addElement(display, doc, "displayCenterSize", "40");
            //报警后中间显示
            addElement(display, doc, "enCenterDis", "1");
        Basic.appendChild(display);

        //报警代码表
        QDomElement fdrStatusCode = doc.createElement("fdrStatusCode");
        Basic.appendChild(fdrStatusCode);
            addElement(fdrStatusCode, doc, "errCode1", "未知");
            addElement(fdrStatusCode, doc, "errCode2", "运行");
            addElement(fdrStatusCode, doc, "errCode3", "停止");
            addElement(fdrStatusCode, doc, "errCode4", "光幕异常");
            addElement(fdrStatusCode, doc, "errCode5", "邮件间距异常");
            addElement(fdrStatusCode, doc, "errCode6", "邮件超高");
            addElement(fdrStatusCode, doc, "errCode7", "伺服驱动器报警");
            addElement(fdrStatusCode, doc, "errCode8", "违规停机");
            addElement(fdrStatusCode, doc, "errCode9", "邮件纵向超标");
            addElement(fdrStatusCode, doc, "errCode10", "邮件横向超标");
            addElement(fdrStatusCode, doc, "errCode11", "通讯异常");
            addElement(fdrStatusCode, doc, "errCode12", "扫码异常");
            addElement(fdrStatusCode, doc, "errCode13", "相机异常");
            addElement(fdrStatusCode, doc, "errCode14", "邮件超重");
        Basic.appendChild(fdrStatusCode);

    QTextStream stream(&file);
    doc.save(stream, 4);		// 缩进四格
    file.close();
}

//取消按钮按下
void MainWinPreference::on_btnCancel_clicked()
{
    this->close();
}

//应用按钮按下
void MainWinPreference::on_btnApply_clicked()
{
    bool bChangeComplete = true;
    bool bCommunicationChange = false;

    //开机自启配置
    if(bStartOnSystem != this->ui->cbStartOnSystem->isChecked()) {
        bStartOnSystem = this->ui->cbStartOnSystem->isChecked();
        /*  通过添加注册方式开机自启，因需要管理员权限，且经常发生配置文件不读取的情况，所以不适用
        QString qstrApplicationName = QApplication::applicationName();                          //获取应用名称
        QSettings *settings = new QSettings(AUTO_RUN_KEY, QSettings::NativeFormat);             //创建QSetting, 需要添加QSetting头文件
        if(bOnProgram) {
            QString qstrApplicationPath = QApplication::applicationFilePath();                  //找到应用的目录
            settings->setValue(qstrApplicationName, qstrApplicationPath.replace("/", "\\"));    //写入注册表
        }
        else {
            settings->remove(qstrApplicationName);                                              //从注册表中删除
        }
        */
        if(!changeXmlFile("Basic", "system", "startOnSystem", QString::number(bStartOnSystem))) bChangeComplete = false;
    }

    //程序启动快捷方式放入用户启动目录
    if(bStartOnSystem) {    //创建开机启动快捷方式
        //创建桌面快捷方式
        //        QString strDesktopLink = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/";
        //        strDesktopLink += QCoreApplication::applicationName() + ".lnk";
        //        QFileInfo  dir0(strDesktopLink);
        //        if (!dir0.isFile())
        //        {
        //            fApp.link(strDesktopLink);
        //        }

        //程序启动快捷方式放入用户启动目录
        QString strAppPath = QCoreApplication::applicationFilePath();//要创建快捷方式的应用程序绝对路径
        QFile fApp(strAppPath);

        QString qstrStartMenu = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/";
        qstrStartMenu += "Startup/";

        QString qstrStartMenuLink = qstrStartMenu;
        QDir dirPath;
        dirPath.mkpath(qstrStartMenuLink);
        qstrStartMenuLink +=  QCoreApplication::applicationName() + ".lnk";
        QFileInfo  qfDir(qstrStartMenuLink);

        //判断是否已存在
        if (!qfDir.isFile())
        {
            fApp.link(qstrStartMenuLink);
            qDebug() << "creat on program path.";
        }
    }
    else {      //删除开机自启快捷方式
        //判断是否已存在
        //程序启动快捷方式放入用户启动目录
        QString strAppPath = QCoreApplication::applicationFilePath();//要创建快捷方式的应用程序绝对路径
        QFile fApp(strAppPath);

        QString qstrStartMenu = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/";
        qstrStartMenu += "Startup/";

        QString qstrStartMenuLink = qstrStartMenu;
        QDir dirPath;
        dirPath.mkpath(qstrStartMenuLink);
        qstrStartMenuLink +=  QCoreApplication::applicationName() + ".lnk";
        QFileInfo  qfDir(qstrStartMenuLink);
        if (qfDir.isFile())
        {
            if(QFile::remove(qstrStartMenuLink)) {
                qDebug() << "delete on program path.";
            }
        }
    }

    //登录系统
    if(bEnLoginSystem != this->ui->cbEnLoginSystem->isChecked()) {
        bEnLoginSystem = this->ui->cbEnLoginSystem->isChecked();
        if(!changeXmlFile("Basic", "system", "loginSystem", QString::number(bEnLoginSystem))) bChangeComplete = false;
    }

    //分拣机层号
    if(iLevelNum != this->ui->sBoxLevelNo->value()) {
        iLevelNum = this->ui->sBoxLevelNo->value();
        bCommunicationChange = true;
        if(!changeXmlFile("Basic", "fdrParameter", "levelNum", QString::number(iLevelNum))) bChangeComplete = false;
    }

    //供件台编号
    if(iFdrNum != this->ui->sBoxFdrId->value()) {
        iFdrNum = this->ui->sBoxFdrId->value();
        bCommunicationChange = true;
        if(!changeXmlFile("Basic", "fdrParameter", "fdrNum", QString::number(iFdrNum))) bChangeComplete = false;
    }

    //通讯接口
    if(qstrUrl != this->ui->lEditGetFdrStatusUrl->text()) {
        qstrUrl = this->ui->lEditGetFdrStatusUrl->text();
        bCommunicationChange = true;
        if(!changeXmlFile("Basic", "communication", "url", this->ui->lEditGetFdrStatusUrl->text())) bChangeComplete = false;
    }

    //超时时间
    if(iTimeOutMs != this->ui->sBoxtimeOut->value()) {
        iTimeOutMs = this->ui->sBoxtimeOut->value();
        bCommunicationChange = true;
        if(!changeXmlFile("Basic", "communication", "timeOutMs", QString::number(iTimeOutMs))) bChangeComplete = false;
    }

    //HTTP发送时间间隔
    if(iGetStatusIntervalMs != this->ui->sBoxHttpIntervalTime->value()) {
        iGetStatusIntervalMs = this->ui->sBoxHttpIntervalTime->value();
        bCommunicationChange = true;
        if(!changeXmlFile("Basic", "communication", "getStatusIntervalMs", QString::number(iGetStatusIntervalMs))) bChangeComplete = false;
    }

    if(bCommunicationChange) {
        emit sigCommunicationSChange();
    }

    if(bChangeComplete) {
        MessageTips *mMessageTips = new MessageTips("保存成功",this);
        mMessageTips->show();
    }
    else {
        MessageTips *mMessageTips = new MessageTips("部分失败",this);
        mMessageTips->show();
    }
}

//确认按钮按下
void MainWinPreference::on_btnYes_clicked()
{
    on_btnApply_clicked();
    this->close();
}

//添加XML元素
void MainWinPreference::addElement(QDomNode father, QDomDocument qdDoc, QString elementName, QString elementText)
{
    QDomElement newElement = qdDoc.createElement(elementName);
    QDomText newElementText = qdDoc.createTextNode(elementText);
    newElement.appendChild(newElementText);
    father.appendChild(newElement);
}
