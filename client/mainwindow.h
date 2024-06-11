#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTcpSocket>
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots: 
    void on_pushButton_connect_clicked();

    void on_pushButton_disconnect_clicked();

    void Enc_Recv();

    void Show_Viode();

    void senddata();

    void on_light_on_clicked();

    void on_light_off_clicked();

    void on_buzzer_on_clicked();

    void on_buzzer_off_clicked();

    void on_feng_on_clicked();

    void on_feng_off_clicked();

private:
    Ui::MainWindow *ui;

     QTcpSocket *ser_socket ;      //定义串口控制的套接字对象
     QTcpSocket *cam_socket ;      //定义视频控制的套接字对象
     QTimer     *timer;            //定义刷新环境数据定时器对象
};

#endif // MAINWINDOW_H
