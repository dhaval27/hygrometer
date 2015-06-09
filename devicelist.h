#ifndef DEVICELIST_H
#define DEVICELIST_H

#include "device.h"
#include <qdom.h>
#include <qtablewidget.h>
#include <qlist.h>
#define FILE_NAME "/home/dhaval27/dhaval/project/hygrometer/data4.xml"

#define HYGROMETER

/***************************************************
 "RegisterDialog" is a class of a Dialogbox that
   will open while "Add Device" button clicked
***************************************************/

class RegisterDialog: public QDialog
{
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget *parent);
    ~RegisterDialog();

private slots:
    void accept();
    void reject();

public:

    QVBoxLayout *vbox;
    QHBoxLayout *ip_hbox;
    QHBoxLayout *subnet_hbox;
    QHBoxLayout *location_hbox;

    QLabel* ip_Label;
    QLabel* subnet_Label;
    QLabel* location_Label;

    QLineEdit *ip_Edit;
    QLineEdit *subnet_Edit;
    QLineEdit *location_Edit;

    QDialogButtonBox *buttonBox;
};

/***************************************************
 "DeviceList" is a class of a QTableWidget, which is
  haiving all the devices present & where devices are
  to be displayed.("ui" is a parent for this class)
***************************************************/

class DeviceList: public QTableWidget
{
    Q_OBJECT
private:
    int colNum_id;
    int colNum_ip;
    int colNum_location;
    int colNum_state;
    int colNum_temp;
    int colNum_humidity;
    int colNum_getStatus;
    int colNum_settings;
    int colNum_configure;
    int colNum_unRegister;
    void AddDevice(Device *dev, int rowCount);
    QString temp;
    QString humidity;
    QString state;
    QString status;
    QString activity_text;

public:
    ~DeviceList();
    QList <int> valid_device;
    DeviceList(QWidget *parent);
    void show(void);
    Device device[MAX_DEVICE];
    int deviceCount;
    QString deviceTag;
    void xmlDomDelete(int device_id);
    void xmlDomUpdate(int device_id);
    int xmlDomReader();
    void retriveDeviceInfo(QDomElement device);
    void showDeviceList();
    void xmlDomUpdateStatus(QString,QVector<Device>::iterator);
    void updateDevice(QString);
    RegisterDialog *registerDialog;

    QObject *parent;
    QString newIpAddress;
    QString newSubnet;
    QString max_temp;
    QString min_temp;
    QString max_humidity;
    QString min_humidity;
    QString old_host;

private slots:
    void deleteDevice(int device_id);
    void updateDevice(int device_id);
    void on_addDevice_accept();
    void data_available(int device_id, QByteArray data);
    void on_header_clicked(int);

signals:
    void activity(QString );
};


#endif // DEVICELIST_H
