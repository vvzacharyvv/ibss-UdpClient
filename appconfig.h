#ifndef APPCONFIG_H
#define APPCONFIG_H
#include "head.h"
class AppConfig
{
public:
    static QString ConfigFile;          //配置文件路径
    static int CurrentIndex;            //当前索引

    //UDP客户端配置参数
    static bool HexSendUdpClient;       //16进制发送
    static bool HexReceiveUdpClient;    //16进制接收
    static bool AsciiUdpClient;         //ASCII模式
    static bool DebugUdpClient;         //启用数据调试
    static bool AutoSendUdpClient;      //自动发送数据
    static int IntervalUdpClient;       //发送数据间隔
    static QString UdpBindIP;           //绑定地址
    static int UdpBindPort;             //绑定端口
    static QString UdpServerIP;         //服务器地址
    static int UdpServerPort;           //服务器端口

    //读取配置参数
    static void readConfig();           //读取配置参数
    static void writeConfig();          //写入配置参数
};
#endif // APPCONFIG_H
