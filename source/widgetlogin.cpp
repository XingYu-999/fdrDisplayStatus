#include "widgetlogin.h"
#include "ui_widgetlogin.h"

WidgetLogin::WidgetLogin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetLogin)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | windowFlags());      //去掉窗口边框

//    this->setAttribute(Qt::WA_TranslucentBackground);                   //背景透明

    connect(this, &WidgetLogin::sigShowWindow, [=] () {
        this->show();
    });
}

WidgetLogin::~WidgetLogin()
{
    delete ui;
}

void WidgetLogin::on_toolBtnSmallest_clicked()
{
    this->showMinimized();
}

void WidgetLogin::on_toolBtnClose_clicked()
{
    this->close();
}

void WidgetLogin::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        //求坐标差值
        //当前点击坐标-窗口左上角坐标
        p = e->globalPos() - this->frameGeometry().topLeft();
    }
}

void WidgetLogin::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        //移到左上角
        move(e->globalPos() - p);
    }
}

