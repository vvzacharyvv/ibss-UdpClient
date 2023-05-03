#include "appconfig.h"
#include "quihelper.h"

QString AppConfig::ConfigFile = "config.ini";
int AppConfig::CurrentIndex = 0;


//UdpClient
bool AppConfig::HexSendUdpClient = false;
bool AppConfig::HexReceiveUdpClient = false;
bool AppConfig::AsciiUdpClient = false;
bool AppConfig::DebugUdpClient = false;
bool AppConfig::AutoSendUdpClient = false;
int AppConfig::IntervalUdpClient = 1000;
QString AppConfig::UdpBindIP = "127.0.0.1";
int AppConfig::UdpBindPort = 6001;
QString AppConfig::UdpServerIP = "127.0.0.1";
int AppConfig::UdpServerPort = 6000;


void AppConfig::readConfig()
{
    QSettings set(AppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig");
    AppConfig::CurrentIndex = set.value("CurrentIndex").toInt();
    set.endGroup();


    set.beginGroup("UdpClientConfig");
    AppConfig::HexSendUdpClient = set.value("HexSendUdpClient", AppConfig::HexSendUdpClient).toBool();
    AppConfig::HexReceiveUdpClient = set.value("HexReceiveUdpClient", AppConfig::HexReceiveUdpClient).toBool();
    AppConfig::AsciiUdpClient = set.value("AsciiUdpClient", AppConfig::AsciiUdpClient).toBool();
    AppConfig::DebugUdpClient = set.value("DebugUdpClient", AppConfig::DebugUdpClient).toBool();
    AppConfig::AutoSendUdpClient = set.value("AutoSendUdpClient", AppConfig::AutoSendUdpClient).toBool();
    AppConfig::IntervalUdpClient = set.value("IntervalUdpClient", AppConfig::IntervalUdpClient).toInt();
    AppConfig::UdpBindIP = set.value("UdpBindIP", AppConfig::UdpBindIP).toString();
    AppConfig::UdpBindPort = set.value("UdpBindPort", AppConfig::UdpBindPort).toInt();
    AppConfig::UdpServerIP = set.value("UdpServerIP", AppConfig::UdpServerIP).toString();
    AppConfig::UdpServerPort = set.value("UdpServerPort", AppConfig::UdpServerPort).toInt();
    set.endGroup();

    //配置文件不存在或者不全则重新生成
    if (!QUIHelper::checkIniFile(AppConfig::ConfigFile)) {
        writeConfig();
        return;
    }
}


void AppConfig::writeConfig()
{
    QSettings set(AppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("AppConfig");
    set.setValue("CurrentIndex", AppConfig::CurrentIndex);
    set.endGroup();

    set.beginGroup("UdpClientConfig");
    set.setValue("HexSendUdpClient", AppConfig::HexSendUdpClient);
    set.setValue("HexReceiveUdpClient", AppConfig::HexReceiveUdpClient);
    set.setValue("DebugUdpClient", AppConfig::DebugUdpClient);
    set.setValue("AutoSendUdpClient", AppConfig::AutoSendUdpClient);
    set.setValue("IntervalUdpClient", AppConfig::IntervalUdpClient);
    set.setValue("UdpBindIP", AppConfig::UdpBindIP);
    set.setValue("UdpBindPort", AppConfig::UdpBindPort);
    set.setValue("UdpServerIP", AppConfig::UdpServerIP);
    set.setValue("UdpServerPort", AppConfig::UdpServerPort);
    set.endGroup();
}
