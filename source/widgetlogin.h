#ifndef WIDGETLOGIN_H
#define WIDGETLOGIN_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class WidgetLogin;
}

class WidgetLogin : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetLogin(QWidget *parent = nullptr);
    ~WidgetLogin();

    QPoint p;
    void mouseMoveEvent(QMouseEvent *e);//鼠标移动
    void mousePressEvent(QMouseEvent *e);//鼠标按下移动

private slots:

    void on_toolBtnSmallest_clicked();

    void on_toolBtnClose_clicked();

signals:
    void sigShowWindow();

private:
    Ui::WidgetLogin *ui;
};

#endif // WIDGETLOGIN_H
