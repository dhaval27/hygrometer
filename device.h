#ifndef DEVICE_H
#define DEVICE_H

#include <qstring.h>
#include <qobject.h>
#include <qtcpsocket.h>
#include <qpushbutton.h>
#include <QLineEdit>
#include <QRegExpValidator>
#include <QWidget>
#include <QLabel>
#include <QDialog>
#include <QDialogButtonBox>
#include <QBoxLayout>

#define MAX_DEVICE 5

class IPLineEdit :public QLineEdit
{
    Q_OBJECT
public:
    explicit IPLineEdit(QWidget *parent);
    ~IPLineEdit();
private:
    QRegExpValidator *ipValidator;

};

/***************************************************
 "IPSubnetDialogue" is a class of a Dialogbox that
   will open while "Settings" button clicked
***************************************************/

class IPSubnetDialogue :public QDialog
{
    Q_OBJECT
public:
     explicit IPSubnetDialogue(QWidget *parent);
    ~IPSubnetDialogue();
    QString get_ipaddress();
    QString get_subnet();

private slots:
    void accept();
    void reject();
private:
    QVBoxLayout *vbox;
    QHBoxLayout *ip_hbox;
    QHBoxLayout *subnet_hbox;

    QLabel* ipLabel;
    QLabel* subnetLabel;

    QLineEdit *ipEdit;
    QLineEdit *subnetEdit;

    QDialogButtonBox *buttonBox;
};

/***************************************************
 "ConfigureDialog" is a class of a Dialogbox that
   will open while "Configure" button clicked
***************************************************/

class ConfigureDialog :public QDialog
{
    Q_OBJECT

public:
     explicit ConfigureDialog(QWidget *parent);
    ~ConfigureDialog();
    QString get_max_temp();
    QString get_min_temp();
    QString get_max_humidity();
    QString get_min_humidity();

private slots:
    void accept();
    void reject();
private:
    QVBoxLayout *vbox;
    QHBoxLayout *max_temp_hbox;
    QHBoxLayout *min_temp_hbox;
    QHBoxLayout *max_humidity_hbox;
    QHBoxLayout *min_humidity_hbox;

    QLabel* max_temp_Label;
    QLabel* min_temp_Label;
    QLabel* max_humidity_Label;
    QLabel* min_humidity_Label;

    QLineEdit *max_temp_Edit;
    QLineEdit *min_temp_Edit;
    QLineEdit *max_humidity_Edit;
    QLineEdit *min_humidity_Edit;

    QDialogButtonBox *buttonBox;
};

/***************************************************
 "Device" is a class, which is having device related
        buttons & all the information of device
***************************************************/

class Device:public QObject
{
    Q_OBJECT
public:
    ~Device();
    void initDevice(QObject *parent, int device_id = -1);
    void setIP(QString a)
    {
        dev_ip=a;
    }
    void setSubnet(QString a)
    {
        dev_subnet=a;
    }
    void setLocation(QString a)
    {
        dev_location=a;
    }
    void setTemp(QString a)
    {
        dev_temp=a;
    }
    void setHumidity(QString a)
    {
        dev_humidity=a;
    }
    void setState(QString a)
    {
        dev_state=a;
    }
    void setTempMax(QString a)
    {
        dev_temp_max=a;
    }
    void setTempMin(QString a)
    {
        dev_temp_min=a;
    }
    void setHumidityMax(QString a)
    {
        dev_humidity_max=a;
    }
    void setHumidityMin(QString a)
    {
        dev_humidity_min=a;
    }

    int dev_id;
    QString dev_ip;
    QString dev_subnet;
    QString dev_location;
    QString dev_temp;
    QString dev_humidity;
    QString dev_state;
    QString dev_temp_min;
    QString dev_temp_max;
    QString dev_humidity_min;
    QString dev_humidity_max;
    QString dev_earlierState;
    QString newIpAddress;
    QString newSubnet;
    QPushButton *btn_getStatus;
    QPushButton *btn_settings;
    QPushButton *btn_unRegister;
    QPushButton *btn_configure;
    QTcpSocket *socket;
    bool doConnect(QString ipAddress);

private:
    QObject *parent;
    QByteArray data;
    IPSubnetDialogue *netInfo;
    ConfigureDialog *config;

private slots:
    void on_btn_remove_clicked();
    void on_btn_getStatus_clicked();
    void on_btn_settings_clicked();
    void on_btn_configure_clicked();
    void readData();
    void closeConnection();

signals:
    void delete_me(int dev_id);
    void update_me(int dev_id);
    void data_available(int dev_id,QByteArray data);

};
#endif // DEVICE_H
