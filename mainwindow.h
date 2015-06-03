#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>
#include <QNetworkAddressEntry>
#include <QProcess>
#include <QDialog>
#include <QDebug>
#include <QInputDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QRegExpValidator>
#include <QFile>
#include <QDate>
#include <QTime>
#include <QTableWidget>
#include <QPushButton>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QXmlStreamWriter>
#include <QXmlAttributes>
#include <QXmlStreamReader>
#include <QXmlDefaultHandler>
#include <iostream>
#include <QErrorMessage>
#include <QDomElement>
#include <vector>
#include <QThread>


namespace Ui {
class MainWindow;
}

class RegisterDialogue: public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialogue(QWidget *parent);
    void registerInFile();
    void qXmlReader();
    void xmlDomWritter();
    //void xmlDomReader();
    //void retriveElements(QDomElement,QString);
    //void xmlDomUpdate();

    int totalDevice;

    ~RegisterDialogue();

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



class Device
{
public:
    Device()
    {}
    //~DeviceWidget();

    Device(QString a)
    {
        dev_id=a;
    }
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

    QString dev_id;
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
};

class DeviceList
{


public:
    DeviceList();

    Device *d1;
    QVector<Device> device;
    QVector<Device>::iterator iterator1;
    QString deviceTag;
    RegisterDialogue *registerDialog;
    void xmlDomDelete(QString);
    void xmlDomUpdate(QString);
    int xmlDomReader();
    void retriveElements(QDomElement,QString);
    void deviceListUpdate(QString);
    void deviceListDelete(QString);
    void showDeviceList();
    void xmlDomUpdateStatus(QString,QVector<Device>::iterator);

    void updateDevice(QString);
    //QVector<QString> v;
    //QVector<QString>::iterator iterator;

    QString newIpAddress;
    QString newSubnet;
    QString max_temp;
    QString min_temp;
    QString max_humidity;
    QString min_humidity;

};

class MyThread : public QThread
{
    Q_OBJECT
private:
    QTcpSocket *socket;
    QString IpAddress;
    QByteArray Data;
    QString temp;
    QString humidity;
    QString state;
    QString status;
    QString earlierState;
    DeviceList device_list;
   // qintptr socketDescriptor;

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

public:
    explicit MyThread(QString ip, QObject *parent = 0);
    void getStatus(QString);
    void run();
};


class MainWindow : public QMainWindow
{
    Q_OBJECT
    //friend class UnregisterDialogue;
    //friend class myButton;
public:
    explicit MainWindow(QWidget *parent = 0);
    bool doConnect(QString ipAddress);
    void getStatus();
    void saveStatus();
    void addRow();
    void addButtons(int );
    void updateTable();

    //void xmlDomDelete(QString);
    //void xmlDomUpdate(QString);
    //int xmlDomReader();
    //void retriveElements(QDomElement,QString);
    //void showDeviceList();
    //void deviceListUpdate();
    //void deviceListDelete(QString);
    //void registerInFile();
    Ui::MainWindow *ui;
    //friend class registerDialogue;
    //friend class MyXmlContentHandler;
    //QVector<DeviceWidget> device;
    //QVector<DeviceWidget>::iterator iterator;
    ~MainWindow();

private:

    QTcpSocket *socket;
    QTcpSocket *socket1;
    QTcpServer *server;
    QNetworkAddressEntry *netadd;
    QProcess *process;
    QTimer *timer;
    QByteArray data;
    QString temp;
    QString humidity;
    QString state;
    QString status;
    QString earlierState;
    QString activity_text;
    QWidget* pWidget;
    QPushButton *btn_getStatus;
    QPushButton *btn_settings;
    QPushButton *btn_unRegister;
    QPushButton *btn_configure;
    QPushButton *btn_register;
    QHBoxLayout* pLayout;
    QString ipAddress;
    MyThread *thread;
    //QDialog *regDialog;
    //RegisterDialogue *registerDialog;
    //DeviceWidget *device;
    DeviceList *deviceList;

    int maxDevice;
    int rowCount;
    int row_status;
    int row_unRegister;
    int row_register;
    int time_interval;
    int colNum_state;
    int colNum_temp;
    int colNum_humidity;
    int colNum_getStatus;
    int colNum_settings;
    int colNum_configure;
    int colNum_unRegister;
    char *p;
    struct st_status{
        char st_ip[16];
        char st_temp[8];
        char st_humidity[8];
        char st_state[8];
        //struct st_status *p;
    };


public slots:
    void newConnection();
    void readAllStdOut();
    void starts();
    void geterror();
    void readData();
    void closeConnection();
    void writeData(qint64);
    void doIt();
    void accept1();
    //void readyRead();
    //void RegisterDialogue::accept();

private slots:
    void on_btn_getStatus_clicked();
    void on_btn_settings_clicked();
    void on_btn_unRegister_clicked();
    void on_btn_configure_clicked();
    void on_add_device_clicked();
};

class IPLineEdit :public QLineEdit
{
    Q_OBJECT
public:
    explicit IPLineEdit(QWidget *parent);
    ~IPLineEdit();
private:
    QRegExpValidator *ipValidator;

};

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

class MyXmlContentHandler :public QXmlDefaultHandler
{
  public:
    MyXmlContentHandler():QXmlDefaultHandler()
    {

    };
    ~MyXmlContentHandler()
    {

    };

    bool startElement(const QString & namespaceURI, const QString & localName,
                    const QString & qName, const QXmlAttributes & atts )
    {
        std::cout << "Read Start Tag : " << localName.toStdString()<< std::endl;
        std::cout << "Tag Attributes: " << std::endl;
        qDebug() << "count=" <<atts.count();
        qDebug() << "qName=" <<qName;
        qDebug() << "localName=" <<localName;
        qDebug() << "namespaceURI="<<namespaceURI;

        for(int index = 0 ; index < atts.length();index++)
        {
          std::cout << atts.localName(index).toStdString()<< "="
          << atts.value(index).toStdString()<< std::endl;
          //if(atts.localName(index).toStdString()=="location")
              //;atts.value(index).toStdString();
        }

        std::cout << "------------------------" << std::endl;
        return true;
    };
};




#endif // MAINWINDOW_H
