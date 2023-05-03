#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    bool run_flag;
protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::Widget *ui;
    QUdpSocket *socket;
    QTimer *timer;


private slots:
    void initForm();
    void initConfig();
    void saveConfig();
    void initTimer();
    void append(int type, const QString &data, bool clear = false);

private slots:
    void error();
    void readData();
    void sendData(const QString &ip, int port, const QString &data);
    void hex_sendData(const QString &ip, int port, const QString &data);
private slots:
    void on_btnSave_clicked();
    void on_btnClear_clicked();
    void on_btnSend_clicked();
    void on_btn_xVel_clicked();
    void on_btn_yVel_clicked();
    void on_btn_zVel_clicked();
    void on_btn_gaitPeriod_clicked();
    void on_btn_gaitMode_clicked();
    void on_btn_press_clicked();
    void on_btn_runOrstop_clicked();
};
#endif // WIDGET_H
