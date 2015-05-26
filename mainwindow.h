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

class DeviceWidget
{
public:
    DeviceWidget(){}
    //~DeviceWidget();

    DeviceWidget(QString a)
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

    QString dev_id;
    QString dev_ip;
    QString dev_subnet;
    QString dev_location;
    QString dev_temp;
    QString dev_humidity;
    QString dev_state;
    QString dev_temp_min;
    QString dev_temp_max;
};

class DeviceList
{
public:
    DeviceList();

    DeviceWidget *d1;
    QVector<DeviceWidget> device;
    QVector<DeviceWidget>::iterator iterator1;
    QString deviceTag;
    RegisterDialogue *registerDialog;
    void xmlDomDelete(QString);
    void xmlDomUpdate(QString);
    int xmlDomReader();
    void retriveElements(QDomElement,QString);
    void deviceListUpdate();
    void deviceListDelete(QString);
    void showDeviceList();
    //QVector<QString> v;
    //QVector<QString>::iterator iterator;
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
    void openDilog();
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

protected:
    friend class UnregisterDialogue;

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
    QPushButton *btn_register;
    QHBoxLayout* pLayout;
    QString ipAddress;
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
    //void RegisterDialogue::accept();

//private slots:
    //void on_Setting_clicked();
    void on_getDeviceStatus_clicked();
    void on_btn_register_clicked();
    void on_btn_getStatus_clicked();
    void on_btn_settings_clicked();
    void on_btn_unRegister_clicked();
    //void on_pushButton_clicked();
    void on_Register_clicked();
    void on_Unregister_clicked();
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

class UnregisterDialogue: public QDialog
{
    Q_OBJECT
public:
    explicit UnregisterDialogue(QWidget *parent);

    ~UnregisterDialogue();

private slots:
    void accept();
    void reject();

private:
    QVBoxLayout *vbox;
    QHBoxLayout *ip_hbox;
    QLabel* ip_Label;
    QLineEdit *ip_Edit;
    QDialogButtonBox *buttonBox;
    int row_count;
    QString ip_del;
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
