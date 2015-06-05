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

#include "devicelist.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    //friend class UnregisterDialogue;
    //friend class myButton;
public:
    explicit MainWindow(QWidget *parent = 0);
    //bool doConnect(QString ipAddress);
    void getStatus(QString);
    void saveStatus();
    void AddDevice(Device *dev);
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


    QWidget* pWidget;
    QPushButton *btn_configure;
    QPushButton *btn_register;
    QPushButton *btn_status;
    QHBoxLayout* pLayout;
    QString ipAddress;
    QString old_host;
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

    void writeData(qint64);
    void doIt();
    void accept1();
    //void on_btn_unregister_clicked();
    //void readyRead();
    //void RegisterDialogue::accept();

private slots:
    void on_add_device_clicked();

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
