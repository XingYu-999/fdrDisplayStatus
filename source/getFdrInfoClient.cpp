#include "getFdrInfoClient.h"

GetFdrInfoClient::GetFdrInfoClient()
{
    //定时器配置
    timerPostHttp = new QTimer;
    connect(timerPostHttp, &QTimer::timeout, this, &GetFdrInfoClient::sloTimerTimeoutProcess);
}

GetFdrInfoClient::~GetFdrInfoClient()
{
    if(HttpConnectStatus > 0) {
        manager->disconnect();
    }
    delete manager;
    delete reply;
}


//按定时时间处理的函数
void GetFdrInfoClient::sloTimerTimeoutProcess()
{
    //通讯参数被修改
    if(HttpConnectStatus == -1) {
        manager->disconnect();
        delete manager;
        HttpConnectStatus = 0;
    }

    if(HttpConnectStatus == 1) {
        iHttpTimeoutCounter += 1;

    }

    qDebug() << "Thread ID：" << QThread::currentThreadId() << " timeOut to get HttpMessage. counter : " << iHttpTimeoutCounter;

    if(HttpConnectStatus == 0) {    //使用子线程创建类
        iHttpTimeoutCounter = 0;
        //配置请求头
        manager = new QNetworkAccessManager;
        manager->setTransferTimeout(iPostTimeoutTime);  //设置超时时间
        connect(manager, &QNetworkAccessManager::finished, this, &GetFdrInfoClient::sloRelpyFinished);
        request.setUrl(QUrl(qStrGetFdrStatusUrl));
        request.setRawHeader("Content-Type", "application/json");
        request.setRawHeader("cache-control", "no-cache");

        //配置json数据格式
        json["levelno"] = QString::number(iLevelNo);
        json["feederid"] = QString::number(iFeederId);
        json["partid"] = "9";
        json["status"] = "0";
        jsonDoc.setObject(json);
        dataArray = jsonDoc.toJson(QJsonDocument::Compact);
    }


    if((HttpConnectStatus == 0) | (HttpConnectStatus == 2)) {
        reply = manager->post(request, dataArray);
        HttpConnectStatus = 1;
        iHttpTimeoutCounter = 0;
    }
}

void GetFdrInfoClient::sloRelpyFinished()
{
    iHttpTimeoutCounter = 0;
    qDebug() << "post finished, " << "Thread：" << QThread::currentThreadId();
    HttpConnectStatus = 2;
    if(reply->error()!=QNetworkReply::NoError){
        //处理中的错误信息
        qDebug()<<"reply error:"<<reply->errorString();
        //请求方式
        qDebug()<<"operation:"<<reply->operation();
        //状态码
        qDebug()<<"status code:"<<reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug()<<"url:"<<reply->url();
        qDebug()<<"raw header:"<<reply->rawHeaderList();
        //获取响应信息
        const QByteArray reply_data=reply->readAll();
        qDebug()<<"read all:"<<reply_data;
    }
    else {
        //请求方式
//        qDebug()<<"operation:"<<reply->operation();
        //状态码
//        qDebug()<<"status code:"<<reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//        qDebug()<<"url:"<<reply->url();
//        qDebug()<<"raw header:"<<reply->rawHeaderList();

        //获取响应信息
        const QByteArray reply_data=reply->readAll();
        qDebug()<<"read all:"<<reply_data;

        //解析json
        QJsonParseError json_error;
        QJsonDocument document=QJsonDocument::fromJson(reply_data, &json_error);
        if (!document.isNull() && (json_error.error == QJsonParseError::NoError)) {  // 解析未发生错误
            if(document.isObject()){
                const QJsonObject jsonObjectFirst=document.object();    //转化为对象
//                qDebug() << jsonObjectFirst;
                if(jsonObjectFirst.contains("data")){                 //第一级对象
                    QJsonValue value = jsonObjectFirst.value("data");  // 获取指定 key 对应的 value
                    if(value.isObject()) {                       //第二级对象
                        QJsonObject jsonObjectSecond = value.toObject();
                        if(jsonObjectSecond.contains("status")) {
                            QJsonValue valueSecond = jsonObjectSecond.value("status");
                            if (valueSecond.isString()) {  // 判断 value 是否为字符串
                                QString strName = value.toString();  // 将 value 转化为字符串
                                qDebug() << "status : " << strName;
                            }
                            else if(valueSecond.isDouble()) {
                                qDebug() << "status : " << valueSecond.toDouble();
                                emit sigGetFdrStatus(valueSecond.toDouble());
                            }
                        }
                    }
                }
            }
        }
        else{
            qDebug()<<"json error:"<<json_error.errorString();
        }
    }
    // reply->deleteLater();
}

//检测本地是否有此Ip
bool GetFdrInfoClient::checkIp(QString qStrCheckIp)
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

