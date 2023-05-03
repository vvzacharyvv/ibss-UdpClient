#include "widget.h"
#include "ui_widget.h"
#include "quihelper.h"
#include "quihelperdata.h"
#include<QDebug>
QString floatToHex(QString Mode,QString str_text)
{
    QString str,str_set;
    str="23020";
    str+=Mode;
    str_set=str_text;
   float  m_fl=str_set.toFloat();
   uint f_uint = *(uint*)&m_fl;
   QString f_hex = QString("%1").arg(f_uint, 4, 16, QLatin1Char('0'));
   QString temp;
   temp.resize(8);
   temp[0]=f_hex[6];
   temp[1]=f_hex[7];
   temp[2]=f_hex[4];
   temp[3]=f_hex[5];
   temp[4]=f_hex[2];
   temp[5]=f_hex[3];
   temp[6]=f_hex[0];
   temp[7]=f_hex[1];
   str+=temp;
   str+="AA";
   qDebug()<<"str="<<str;
   return str;
}
QString intTohex(int data)
{   QString str;
    int decade=data/16;
    int bit=data%16;
    if(decade<10) str=QString("%1").arg(decade);
    if(decade==10) str="A";
    if(decade==11) str="B";
    if(decade==12) str="C";
    if(decade==13) str="D";
    if(decade==14) str="E";
    if(decade==15) str="F";
    if(bit<10) str+=QString("%1").arg(bit);
    if(bit==10) str+="A";
    if(bit==11) str+="B";
    if(bit==12) str+="C";
    if(bit==13) str+="D";
    if(bit==14) str+="E";
    if(bit==15) str+="F";
    return str;
}
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
    run_flag=0;
}

Widget::~Widget()
{
    delete ui;
}
bool Widget::eventFilter(QObject *watched, QEvent *event)
 {
    if (watched == ui->txtMain->viewport()) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            on_btnClear_clicked();
        }
    }

    return QWidget::eventFilter(watched, event);
 }
void Widget::initForm()
{
    QFont font;
    font.setPixelSize(16);
    ui->txtMain->setFont(font);
    ui->txtMain->viewport()->installEventFilter(this);

    //实例化对象并绑定信号槽
    socket = new QUdpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(error()));
#else
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
#endif

    //定时器发送数据
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(on_btnSend_clicked()));

    //填充数据到下拉框
    ui->cboxInterval->addItems(AppData::Intervals);
    ui->cboxData->addItems(AppData::Datas);
    AppData::loadIP(ui->cboxBindIP);
}
void Widget::initConfig()
{
    ui->ckHexSend->setChecked(AppConfig::HexSendUdpClient);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceiveUdpClient);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAscii->setChecked(AppConfig::AsciiUdpClient);
    connect(ui->ckAscii, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::DebugUdpClient);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSendUdpClient);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->cboxInterval->setCurrentIndex(ui->cboxInterval->findText(QString::number(AppConfig::IntervalUdpClient)));
    connect(ui->cboxInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->cboxBindIP->setCurrentIndex(ui->cboxBindIP->findText(AppConfig::UdpBindIP));
    connect(ui->cboxBindIP, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtBindPort->setText(QString::number(AppConfig::UdpBindPort));
    connect(ui->txtBindPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(AppConfig::UdpServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(AppConfig::UdpServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    this->initTimer();
}
void Widget::saveConfig()
{
    AppConfig::HexSendUdpClient = ui->ckHexSend->isChecked();
    AppConfig::HexReceiveUdpClient = ui->ckHexReceive->isChecked();
    AppConfig::AsciiUdpClient = ui->ckAscii->isChecked();
    AppConfig::DebugUdpClient = ui->ckDebug->isChecked();
    AppConfig::AutoSendUdpClient = ui->ckAutoSend->isChecked();
    AppConfig::IntervalUdpClient = ui->cboxInterval->currentText().toInt();
    AppConfig::UdpBindIP = ui->cboxBindIP->currentText();
    AppConfig::UdpBindPort = ui->txtBindPort->text().trimmed().toInt();
    AppConfig::UdpServerIP = ui->txtServerIP->text().trimmed();
    AppConfig::UdpServerPort = ui->txtServerPort->text().trimmed().toInt();
    AppConfig::writeConfig();

    this->initTimer();
}
void Widget::initTimer()
{
    if (timer->interval() != AppConfig::IntervalUdpClient) {
        timer->setInterval(AppConfig::IntervalUdpClient);
    }

    if (AppConfig::AutoSendUdpClient) {
        if (!timer->isActive()) {
            timer->start();
        }
    } else {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}
void Widget::append(int type, const QString &data, bool clear)
{
    static int currentCount = 0;
    static int maxCount = 100;

    if (clear) {
        ui->txtMain->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }

    if (ui->ckShow->isChecked()) {
        return;
    }

    //过滤回车换行符
    QString strData = data;
    strData = strData.replace("\r", "");
    strData = strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        strType = "发送";
        ui->txtMain->setTextColor(QColor("#22A3A9"));
    } else if (type == 1) {
        strType = "接收";
        ui->txtMain->setTextColor(QColor("#753775"));
    } else {
        strType = "错误";
        ui->txtMain->setTextColor(QColor("#D64D54"));
    }

    strData = QString("时间[%1] %2: %3").arg(TIMEMS).arg(strType).arg(strData);
    ui->txtMain->append(strData);
    currentCount++;
}
void Widget::error()
{
      append(2, socket->errorString());
}
void Widget::readData()
{
    QHostAddress host;
    quint16 port;
    QByteArray data;
    QString buffer;

    while (socket->hasPendingDatagrams()) {
        data.resize(socket->pendingDatagramSize());
        socket->readDatagram(data.data(), data.size(), &host, &port);

        if (AppConfig::HexReceiveUdpClient) {
            buffer = QUIHelperData::byteArrayToHexStr(data);
        } else if (AppConfig::AsciiUdpClient) {
            buffer = QUIHelperData::byteArrayToAsciiStr(data);
        } else {
            buffer = QString(data);
        }

        QString ip = host.toString();
        ip = ip.replace("::ffff:", "");
        if (ip.isEmpty()) {
            continue;
        }

        QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(buffer);
        append(1, str);

        if (AppConfig::DebugUdpClient) {
            int count = AppData::Keys.count();
            for (int i = 0; i < count; i++) {
                if (AppData::Keys.at(i) == buffer) {
                    sendData(ip, port, AppData::Values.at(i));
                    break;
                }
            }
        }
    }
}
void Widget::sendData(const QString &ip, int port, const QString &data)
{
    QByteArray buffer;
    if (AppConfig::HexSendUdpClient) {
        buffer = QUIHelperData::hexStrToByteArray(data);
    } else if (AppConfig::AsciiUdpClient) {
        buffer = QUIHelperData::asciiStrToByteArray(data);
    } else {
        buffer = data.toUtf8();
    }

    //绑定网卡和端口,没有绑定过才需要绑定
    //采用端口是否一样来判断是为了方便可以直接动态绑定切换端口
    if (socket->localPort() != AppConfig::UdpBindPort) {
        socket->abort();
        socket->bind(QHostAddress(AppConfig::UdpBindIP), AppConfig::UdpBindPort);
    }

    //指定地址和端口发送数据
    socket->writeDatagram(buffer, QHostAddress(ip), port);

    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    append(0, str);
}
 void  Widget::hex_sendData(const QString &ip, int port, const QString &data)
{
    QByteArray buffer;
    buffer = QUIHelperData::hexStrToByteArray(data);

    //绑定网卡和端口,没有绑定过才需要绑定
    //采用端口是否一样来判断是为了方便可以直接动态绑定切换端口
    if (socket->localPort() != AppConfig::UdpBindPort) {
        socket->abort();
        socket->bind(QHostAddress(AppConfig::UdpBindIP), AppConfig::UdpBindPort);
    }

    //指定地址和端口发送数据
    socket->writeDatagram(buffer, QHostAddress(ip), port);

    QString str = QString("[%1:%2] %3").arg(ip).arg(port).arg(data);
    append(0, str);
}
void Widget::on_btnSave_clicked()
{
    QString data = ui->txtMain->toPlainText();
    AppData::saveData(data);
    on_btnClear_clicked();
}
void Widget::on_btnClear_clicked()
{
     append(0, "", true);
}
void Widget::on_btnSend_clicked()
{
    QString data = ui->cboxData->currentText();
    if (data.length() <= 0) {
        return;
    }

    sendData(AppConfig::UdpServerIP, AppConfig::UdpServerPort, data);
}

void Widget::on_btn_xVel_clicked()
{

   QString data=floatToHex("1",ui->edit_x->text());
   if (data.length() <= 0) {
       return;
   }

   hex_sendData(AppConfig::UdpServerIP, AppConfig::UdpServerPort, data);
}

void Widget::on_btn_yVel_clicked()
{
    QString data=floatToHex("2",ui->edit_y->text());
    if (data.length() <= 0) {
        return;
    }

    hex_sendData(AppConfig::UdpServerIP, AppConfig::UdpServerPort, data);
}

void Widget::on_btn_zVel_clicked()
{
    QString data=floatToHex("3",ui->edit_z->text());
    if (data.length() <= 0) {
        return;
    }

    hex_sendData(AppConfig::UdpServerIP, AppConfig::UdpServerPort, data);
}

void Widget::on_btn_gaitPeriod_clicked()
{
    QString data=floatToHex("4",ui->spb_GaitPeriod->text());
    if (data.length() <= 0) {
        return;
    }

    hex_sendData(AppConfig::UdpServerIP, AppConfig::UdpServerPort, data);
}

void Widget::on_btn_gaitMode_clicked()
{
    QString str_set;
    switch (ui->cmb_GaitMode->currentIndex()) {
        case 0://90 trot
            str_set="00";
                    break;
        case 1://90 amb
            str_set="01";
            break;
        case 2://180 trot
            str_set="02";
                    break;
        case 3://180 amb
            str_set="03";
                    break;
        case 4://90 out amb
            str_set="05";
                    break;
        case 5://90 in amb
            str_set="07";
                    break;
    default:
        break;
    }
    if(ui->cmb_GaitMode->currentIndex()==0||ui->cmb_GaitMode->currentIndex()==2) ui->spb_GaitPeriod->setValue(4);
    if(ui->cmb_GaitMode->currentIndex()==1||ui->cmb_GaitMode->currentIndex()==3||ui->cmb_GaitMode->currentIndex()==4||ui->cmb_GaitMode->currentIndex()==5) ui->spb_GaitPeriod->setValue(8);
     QString data="230206000000"+str_set+"AA";
     if (data.length() <= 0) {
         return;
     }

     hex_sendData(AppConfig::UdpServerIP, AppConfig::UdpServerPort, data);
}

void Widget::on_btn_press_clicked()
{
    QString str_set;
    str_set=intTohex(ui->edit_p1->text().toInt())+intTohex(ui->edit_p2->text().toInt())+intTohex(ui->edit_p3->text().toInt())+intTohex(ui->edit_p4->text().toInt());

    QString data="230205"+str_set+"AA";
    if (data.length() <= 0) {
        return;
    }
    hex_sendData(AppConfig::UdpServerIP, AppConfig::UdpServerPort, data);
}

void Widget::on_btn_runOrstop_clicked()
{
    QString data;
    if(run_flag==0)
    {
        data="23020000000001AA";
        ui->btn_runOrstop->setText("停止");
        run_flag=1;
    }
    else if(run_flag==1)
    {
        data="23020000000000AA";
        ui->btn_runOrstop->setText("运行");
        run_flag=0;
    }
    if (data.length() <= 0) {
        return;
    }
    hex_sendData(AppConfig::UdpServerIP, AppConfig::UdpServerPort, data);
}
