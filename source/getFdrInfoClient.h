#ifndef GETFDRINFOCLIENT_H
#define GETFDRINFOCLIENT_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QString>

//以下是json数据传送所需头文件
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

//网络接口和HTTP客户端
#include <QNetworkInterface>
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class GetFdrInfoClient : public QObject
{
    Q_OBJECT
public:
    explicit GetFdrInfoClient();
    ~GetFdrInfoClient();
    int HttpConnectStatus = 0;  //HTTP请求状态 0:初始状态 1:正在请求中 2:完成请求

    QTimer* timerPostHttp;

    int iLevelNo = 2;                   //分拣机层号
    int iFeederId = 1;                  //分拣机编号
    QString qStrGetFdrStatusUrl = "http://127.0.0.1:38900/jtsorterservice/getfeederstatus";

    /*********************HTTP相关变量***************************/
    //构建一个manager对象
    QNetworkAccessManager* manager;
    QNetworkRequest request;
    QNetworkReply* reply;
    QJsonObject json;
    QJsonDocument jsonDoc;
    QByteArray dataArray;
    int iHttpTimeoutCounter = 0;     //超时计数
    int iPostTimeoutTime = 10000;       //超时时间配置ms

    bool checkIp(QString qStrCheckIp);


protected:

public slots:
    void sloRelpyFinished();
    void sloTimerTimeoutProcess();

signals:
    void sigGetFdrStatus(double dbFdrStatus);

};

#endif // GETFDRINFOCLIENT_H
