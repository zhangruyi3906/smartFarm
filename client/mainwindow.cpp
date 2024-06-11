#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    cam_socket = new QTcpSocket;//new 对象
    ser_socket = new QTcpSocket;
    timer = new QTimer;
    ui->lineEdit_ip->setText("192.168.6.77");//设置界面上的ip一栏
    ui->lineEdit_port_cam->setText("6666");//设置界面上的摄像头端口一栏
    ui->lineEdit_port_ser->setText("7777");


    connect(ser_socket, SIGNAL(readyRead()), this, SLOT(Enc_Recv()));

    connect(cam_socket, SIGNAL(readyRead()), this, SLOT(Show_Viode()));

    connect(timer,SIGNAL(timeout()), this, SLOT(senddata()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Enc_Recv()//开始接收环境数据
{
    char buf[20] = {0};
    //1、接收信息
    ser_socket->read(buf, sizeof (buf));
    qDebug() << "data:" <<buf;

    int temp, hum, light;
    //2、解析收到的环境变量
    sscanf(buf, "%d:%d:%d", &temp, &hum, &light);
    qDebug() << "temp:" <<temp;
    qDebug() << "hum:" <<hum;
    qDebug() << "light:" <<light;
    ui->lineEdit_temp->setText(QString("%1.0").arg(temp));
    ui->lineEdit_hum->setText(QString("%1.0").arg(hum));
    ui->lineEdit_light->setText(QString("%1.0").arg(light));
}

void MainWindow::Show_Viode()//显示图片函数
{
    static int flag = 0;
    static int pix_size = 0;//定义的图片长度变量

    char size[10] = {0}; //接受长度的buf
    char pix_buf[640*480] = {0};// 接受内容的buf
    int ret;
    if(0 == flag)
    {
        ret = cam_socket->read(size,10);
        if(ret == -1)
        {
            qDebug() << "read error" << size;
            return;
        }
        qDebug() << "size" << size;
        pix_size = atoi(size);//将"4090\0" 转成->4090
        qDebug() << "pix_size" << pix_size;
        flag = 1;
        return;
    }
    else
    {
        if(cam_socket->bytesAvailable() >= pix_size)//还有数据则读取内容
        {
            ret = cam_socket->read(pix_buf, pix_size);//读取中
            qDebug()<<"pix_size" <<pix_size;

            QPixmap picture;
            picture.loadFromData((const uchar *)pix_buf,pix_size);//把内容放进picture里面

            ui->pix->setScaledContents(true);//启用缩放像素图以填充可用空间
            ui->pix->setPixmap(picture);//把图片显示在label中
            flag = 0;
        }
        else {
            return;
        }
    }

}


void MainWindow::senddata()//发送请求环境数据命令
{
    ser_socket->write("env", 3);//发送env命令到服务器，请求环境数据
}

void MainWindow::on_pushButton_connect_clicked()//连接服务器
{
    //3、连接摄像头模块，需要自己填写服务器的ip和端口号参数
    cam_socket->connectToHost(ui->lineEdit_ip->text(), ui->lineEdit_port_cam->text().toInt());
    cam_socket->write("ON",2);//这是发送命令告知服务器打开摄像头

    //4、连接控制模块，需要自己填写服务器的ip和端口号参数
    ser_socket->connectToHost(ui->lineEdit_ip->text(), ui->lineEdit_port_ser->text().toInt());
    timer->start(1000);//打开定时器，一秒接收一次环境数据
}

void MainWindow::on_pushButton_disconnect_clicked()//断开连接
{
    timer->stop();
    //5、关闭所有套接字的连接
    cam_socket->close();
    ser_socket->close();
}

void MainWindow::on_light_on_clicked()//开灯
{
    ser_socket->write("light_on",sizeof("light_on"));
}

void MainWindow::on_light_off_clicked()//关灯
{
    //6、发送关灯指令
    ser_socket->write("light_off",sizeof("light_off"));
}

void MainWindow::on_buzzer_on_clicked()//开蜂鸣器
{
    //7、发送报警指令
    ser_socket->write("buzzer_on",sizeof("buzzer_on"));
}

void MainWindow::on_buzzer_off_clicked()//关蜂鸣器
{
    //8、发送关闭报警指令
    ser_socket->write("buzzer_off",sizeof("buzzer_off"));
}

void MainWindow::on_feng_on_clicked()//开风扇
{
    //9、发送开风扇指令
    ser_socket->write("feng_on",sizeof("feng_on"));
}

void MainWindow::on_feng_off_clicked()//关风扇
{
    //10、发送关风扇指令
    ser_socket->write("feng_off",sizeof("feng_off"));
}
