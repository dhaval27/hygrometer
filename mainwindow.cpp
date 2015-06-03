#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAbstractSocket>
#include <iostream>

#define Hygrometer

//QString file_name="C:\\Users\\dhaval\\Desktop\\data4.xml";
#define file_name "/home/dhaval27/dhaval/project/hygrometer/data4.xml"
//bool flag=true;
bool flag_update=true;
int deviceIndex=0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);

    //QMessageBox::StandardButton msgBox= QMessageBox::question(this,"Configure","Do You want to Display Temperature & Humidity ?",QMessageBox::Yes|QMessageBox::No);
    //if(msgBox==QMessageBox::Yes)
#ifdef Hygrometer
        colNum_state=3;
        colNum_temp=4;
        colNum_humidity=5;
        colNum_getStatus=6;
        colNum_settings=7;
        colNum_configure=8;
        colNum_unRegister=9;
        ui->tableWidget->setColumnCount(10);
        ui->tableWidget->setHorizontalHeaderLabels(QString("DEVICE;IP ADDRESS;LOCATION;STATE;TEMP;HUMIDITY;;;;").split(";"));

        //btn_status= new QPushButton("Status",ui->tableWidget);
        //ui->tableWidget->setHorizontalHeaderItem(colNum_getStatus,new QTableWidgetItem(colNum_getStatus));
#else
        colNum_state=3;
        colNum_getStatus=4;
        colNum_settings=5;
        colNum_unRegister=6;
        ui->tableWidget->setColumnCount(7);

        ui->tableWidget->setHorizontalHeaderLabels(QString("DEVICE;IP ADDRESS;LOCATION;STATE; ; ;").split(";"));
#endif
    //ui->tableWidget->resizeColumnsToContents();

    QHeaderView *header= ui->tableWidget->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    socket = NULL;

    maxDevice=10;

    time_interval=15000;
    rowCount=ui->tableWidget->rowCount();
    qDebug()<<"rowCount earlier="<<rowCount;

    deviceList= new DeviceList();

    int devicesPresent=deviceList->xmlDomReader();
    qDebug()<<"devicesPresent"<<devicesPresent;
    for(int i=0;i<devicesPresent;i++)
    {
        addRow();
    }
    updateTable();
}

MyThread::MyThread(QString ipAddress, QObject *parent) :
    QThread(parent)
{
    this->IpAddress = ipAddress;
}

void MyThread::run()
{
    // thread starts here
    qDebug() << " Thread started";

    socket = new QTcpSocket();
    socket->connectToHost(IpAddress, 5099);

    socket->waitForConnected(5000);
    socket->write("#TELL STATUS\n");

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    qDebug() << IpAddress << " Client connected";

    // make this thread a loop,
    // thread will stay alive so that signal/slot to function properly
    // not dropped out in the middle when thread dies

    exec();
}

void MyThread::readyRead()
{
    // get the information
    Data = socket->readAll();

    // will write on server side window
    qDebug() << IpAddress << " Data in: " << Data;

    if(Data.contains("###STATUS"))
    {
        deviceIndex=0;
        device_list.xmlDomReader();
        this->getStatus(IpAddress);

        //deviceIndex=0;
        //device_list.xmlDomReader();
        //device_list.showDeviceList();
    }

    //MainWindow::ui->tableWidget->setItem(0,4,new QTableWidgetItem("hello"));
    //socket->write(Data);
}

void MyThread::disconnected()
{
    qDebug() << IpAddress << " Disconnected";
    socket->deleteLater();

    exit(0);
}

void MyThread::getStatus(QString IpAddress)
{
    qDebug()<<IpAddress <<" data in getStatus="<<Data;
    status= Data;
    qDebug()<<"status="<<status;
    status= status.remove('\n');
    qDebug()<<"status after remove \n="<<status;

    earlierState= state;
    state= status.mid(status.lastIndexOf("#")+1);
    qDebug()<<"state="<<state;

    /*if(state!=earlierState)
    {
        activity_text=ui->textEdit_2->toPlainText();
        qDebug()<<"activity_text="<<activity_text;
        activity_text.append("Device 1 changed its status.\n");
        activity_text.append(QTime::currentTime().toString("HH:mm:ss"));
        activity_text.append(", ");
        activity_text.append(QDate::currentDate().toString("dd MMM"));
        activity_text.append("\n");
        ui->textEdit_2->setText(activity_text);
    }*/

    status= status.remove(status.indexOf("#STATE"),status.size());
    qDebug()<<"status after removal of #STATE="<<status;

    humidity= status.mid(status.lastIndexOf("#")+1);
    qDebug()<<"humdity="<<humidity;

    status= status.remove(status.indexOf("#HUMIDITY"),status.size());
    qDebug()<<"status after removal of #HUMIDITY="<<status;

    temp= status.mid(status.lastIndexOf("#")+1);
    qDebug()<<"temp="<<temp;

    qDebug()<<"device size="<<device_list.device.size();
    qDebug()<<"deviceIndex="<<deviceIndex;
    bool devicePresent= false;
    device_list.iterator1= device_list.device.begin();

    QString deviceID;
    while((device_list.iterator1) != (device_list.device.end()))
    {
        if((device_list.iterator1->dev_ip)==IpAddress)
        {
            //qDebug()<<this->iterator1->dev_id;
            devicePresent= true;
            deviceID= device_list.iterator1->dev_id;
            break;
        }
        (device_list.iterator1)++;
    }

    if(!devicePresent)
    {
        qDebug()<<"device Not Present";
    }
    else
    {
        qDebug()<<"device  Present";
        device_list.iterator1->setTemp(temp);
        device_list.iterator1->setHumidity(humidity);
        device_list.iterator1->setState(state);
    }
    device_list.xmlDomUpdateStatus(deviceID,device_list.iterator1);
    device_list.deviceListUpdate(deviceID);

    //device_list.updateDevice(IpAddress);
    //status= status.remove(status.indexOf("#TEMP"),status.size());
    //qDebug()<<"status after removal of #STATE="<<status;
}

void DeviceList::xmlDomUpdateStatus(QString deviceTag,QVector<Device>::iterator iterator)
{
    QDomDocument document;
    QFile file(file_name);
    bool exist= file.exists();
    qDebug()<<"exist while writting="<<exist;
    int size= file.size();
    qDebug()<<"size="<<size;

    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        QMessageBox::warning(0,"Error!","Error In opening File");
        return;
    }
    else
    {
        if(exist)
        {
            if(!document.setContent(&file))
            {
                qDebug()<<"failed to load the file";
                return;
            }
            file.close();
        }
    }

    QDomElement root;
    if(exist)
    {
        //if("Devices"==document.documentElement().tagName())
        {
            qDebug()<<"file exist n "<<document.documentElement().tagName();
            root = document.firstChildElement();
            qDebug()<<"root="<<root.tagName();

        }
    }
    else
    {
        qDebug()<<"file doesn't exist ";
        return;
    }

    //get the list;
    //traverse to particular node or child
    //action(add/delete/insertion)
    QDomNode node = root.firstChild();
    QDomNode parentNode = node.parentNode();
    qDebug()<<"N NODE=    "<<node.nodeName()<<"   P NODE="<<parentNode.nodeName();

    QDomElement newElement= document.createElement(deviceTag);

    //root.appendChild(newElement);

    /*****************if only Parent node is present*************/
    if(node.isNull())
    {
        qDebug()<<"root is null";
        newElement.setAttribute("IP",registerDialog->ip_Edit->text());
        newElement.setAttribute("Subnet",registerDialog->subnet_Edit->text());
        newElement.setAttribute("Location",registerDialog->location_Edit->text());

        root.appendChild(newElement);
        qDebug()<<"N NODE=    "<<node.nodeName()<<"   P NODE="<<parentNode.nodeName();

        qDebug() << "UPDATE= " <<document.toString();

        file.close();

        #if 1
        QFile file1(file_name);
        if(!file1.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(0,"Error!","Error In opening File4");
        }
        else
        {
            QTextStream stream(&file1);
            stream<<document.toString();
            file1.close();
            qDebug()<<"Writing is done";
        }
        return;
        #endif
    }

    bool devicePresent=false;
#if 1
    QDomElement ele,ele_temp;
    while(!node.isNull())
    {
        if (node.isElement())
        {
            ele = node.toElement();
            qDebug() << "Element name: " << ele.tagName();
            if(newElement.tagName()==ele.tagName())
            {
                qDebug()<<"device already present";
                devicePresent=true;
                ele_temp=ele;
            }
            qDebug() << "nodeName="<<node.nodeName()<<"  nodeValue="<<node.nodeValue();
        }
        node= node.nextSibling();
    }
#endif

    qDebug()<<"N NODE=    "<<node.nodeName()<<"   P NODE="<<parentNode.nodeName();

    if(!devicePresent)
    {
        qDebug()<<"device not present";
        return;
    }
    else
    {
        qDebug()<<"in else";
        ele_temp.setAttribute("Temp",iterator->dev_temp);
        ele_temp.setAttribute("Humidity",iterator->dev_humidity);
        ele_temp.setAttribute("State",iterator->dev_state);
        /*QMessageBox::StandardButton msgBox= QMessageBox::question(this,"Warning!","Device already present.\n Do you really want to replace it?",QMessageBox::Yes|QMessageBox::No);
        if(msgBox==QMessageBox::Yes)
        {
            parentNode.replaceChild(newElement,ele_temp);
            qDebug()<<"newElement replaced";
        }
        else
        {
            qDebug()<<"newElement NOT replaced";
            return;
        }*/
    }

    qDebug()<<"N NODE=    "<<node.nodeName()<<"   P NODE="<<parentNode.nodeName();

    qDebug() << "UPDATE= " <<document.toString();

    file.close();

#if 1

    QFile file1(file_name);
    if(!file1.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(0,"Error!","Error In opening File4");
    }
    else
    {
        QTextStream stream(&file1);
        stream<<document.toString();
        file1.close();
        qDebug()<<"Writing is done";
    }
#endif
}

void DeviceList::updateDevice(QString IpAddress)
{
    qDebug()<<"in updateDevice";
    qDebug()<<this->device.size();
    qDebug()<<IpAddress;
    qDebug()<<deviceIndex;
    bool devicePresent= false;
    this->iterator1= this->device.begin();

    while((this->iterator1) != (this->device.end()))
    {
        if((this->iterator1->dev_ip)==IpAddress)
        {
            //qDebug()<<this->iterator1->dev_id;
            devicePresent= true;
            break;
        }
        (this->iterator1)++;
    }

    if(!devicePresent)
    {
        qDebug()<<"device Not Present";
    }
    else
    {
        qDebug()<<"device  Present";
        this->iterator1->setTemp(this->max_temp);
        this->iterator1->setHumidity(this->max_temp);
        this->iterator1->setState(this->min_temp);
    }
    showDeviceList();
}

void MainWindow::updateTable()
{
    qDebug()<<"in updateTable";
    deviceList->iterator1= deviceList->device.begin();
    int i=0;
    qDebug()<<"no. of devices"<<deviceList->device.size();

    while((deviceList->iterator1) != (deviceList->device.end()))
    {
#if 0
        qDebug()<<"device="<<(this->device[i].dev_id);
        qDebug()<<"device_ip="<<(this->device[i].dev_ip);
        qDebug()<<"device_subnet="<<(this->device[i].dev_subnet);
        qDebug()<<"device_location="<<(this->device[i].dev_location);
        qDebug()<<"********";
#endif
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(deviceList->device[i].dev_id));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(deviceList->device[i].dev_ip));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(deviceList->device[i].dev_location));
        i++;
        (deviceList->iterator1)++;
    }
}


void DeviceList::showDeviceList()
{
    qDebug()<<"in showDeviceList";
    this->iterator1= this->device.begin();
    int i=0;
    qDebug()<<"no. of devices"<<this->device.size();

    while((this->iterator1) != (this->device.end()))
    {
        qDebug()<<"device           ="<<(this->device[i].dev_id);
        qDebug()<<"device_ip        ="<<(this->device[i].dev_ip);
        qDebug()<<"device_subnet    ="<<(this->device[i].dev_subnet);
        qDebug()<<"device_location  ="<<(this->device[i].dev_location);
        qDebug()<<"device_temp      ="<<(this->device[i].dev_temp);
        qDebug()<<"device_humidity  ="<<(this->device[i].dev_humidity);
        qDebug()<<"device_state     ="<<(this->device[i].dev_state);
        qDebug()<<"dev_temp_max     ="<<(this->device[i].dev_temp_max);
        qDebug()<<"dev_temp_min     ="<<(this->device[i].dev_temp_min);
        qDebug()<<"dev_humidity_max ="<<(this->device[i].dev_humidity_max);
        qDebug()<<"dev_humidity_max ="<<(this->device[i].dev_humidity_max);

        qDebug()<<"********";
        /*
        if(flag_update==true)
        {
            ui->tableWidget->setItem(i,1,new QTableWidgetItem(deviceList->device[i].dev_id));
            ui->tableWidget->setItem(i,2,new QTableWidgetItem(deviceList->device[i].dev_ip));
        }
        */
        (this->iterator1)++;
        i++;
    }
    //flag_update=false;
}

DeviceList::DeviceList()
{}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getStatus(QString host_address)
{
    qDebug()<<"data in getStatus="<<data;
    status= data;
    qDebug()<<"status="<<status;
    status= status.remove('\n');
    qDebug()<<"status after remove \n="<<status;

    earlierState= state;
    state= status.mid(status.lastIndexOf("#")+1);
    qDebug()<<"state="<<state;

    //qDebug()<<host_address;

    qDebug()<<"old_host="<<old_host;
    QString deviceId= ui->tableWidget->item(row_status,0)->text();
    //if(state!=earlierState)
    if((old_host!=host_address)||(state!=earlierState))
    {
        activity_text=ui->textEdit_2->toPlainText();
        qDebug()<<"activity_text="<<activity_text;
        activity_text.append(deviceId+=" changed its status.\n");
        activity_text.append(QTime::currentTime().toString("HH:mm:ss"));
        activity_text.append(", ");
        activity_text.append(QDate::currentDate().toString("dd MMM"));
        activity_text.append("\n");
        ui->textEdit_2->setText(activity_text);
        old_host=host_address;
    }

    status= status.remove(status.indexOf("#STATE"),status.size());
    qDebug()<<"status after removal of #STATE="<<status;

    humidity= status.mid(status.lastIndexOf("#")+1);
    qDebug()<<"humdity="<<humidity;

    status= status.remove(status.indexOf("#HUMIDITY"),status.size());
    qDebug()<<"status after removal of #HUMIDITY="<<status;

    temp= status.mid(status.lastIndexOf("#")+1);
    qDebug()<<"temp="<<temp;

    //status= status.remove(status.indexOf("#TEMP"),status.size());
    //qDebug()<<"status after removal of #STATE="<<status;

    //p= malloc(sizeof(st_status));
    saveStatus();

    ui->tableWidget->setItem(row_status,colNum_state,new QTableWidgetItem(state));

#ifdef Hygrometer
        ui->tableWidget->setItem(row_status,colNum_temp,new QTableWidgetItem(temp));
        ui->tableWidget->setItem(row_status,colNum_humidity,new QTableWidgetItem(humidity));
#endif
}

void MainWindow::saveStatus()
{
    st_status s1[8];

    memset(s1,0,sizeof(s1));

    //p= (char *)malloc(sizeof(st_status));
    strcpy(s1[0].st_temp,temp.toStdString().c_str());
    strcpy(s1[0].st_humidity,humidity.toStdString().c_str());
    strcpy(s1[0].st_state,state.toStdString().c_str());

    qDebug()<<"st_ip="<<s1[0].st_ip;
    qDebug()<<"st_temp="<<s1[0].st_temp;
    qDebug()<<"st_ip="<<s1[0].st_humidity;
    qDebug()<<"st_state="<<s1[0].st_state;
}

void MainWindow::readData()
{
    data= socket->readAll();
    QString host_address=socket->peerAddress().toString();
    qDebug()<<host_address;

    ui->textEdit->append(data);
    if(data.contains("###STATUS#"))
    {
        getStatus(host_address);
    }
}

void  MainWindow::closeConnection()
{
    socket->close();
    socket = NULL;
}

void MainWindow::writeData(qint64 data)
{
    qDebug()<<"data="<<data;
}

void MainWindow::newConnection()
{
    /*socket= server->nextPendingConnection();
    qDebug()<<"socket started";

    //connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(writeData(qint64)));
    connect(socket,SIGNAL(readyRead()),this,SLOT(readData()));
    socket->write("Hello Client\n");
    socket->flush();*/
}

void MainWindow::starts()
{
    //QByteArray data= process->readAllStandardOutput();
    qDebug()<<"started";
}

void MainWindow::geterror()
{
    //QByteArray data= process->readAllStandardOutput();
    QProcess::ProcessError *Error= new QProcess::ProcessError;
    qDebug()<<"Error="<<process->error();
    qDebug()<<"State="<<process->state();
}

void MainWindow::readAllStdOut()
{
    QByteArray data= process->readAllStandardOutput();
    qDebug()<<"data="<<data;
}


bool MainWindow::doConnect(QString ipAddress)
{
    bool ret = false;
    qDebug()<<"in doConnect...";
    if(socket) {
        if(socket->state() == QAbstractSocket::UnconnectedState) {
            socket->close();
            socket = NULL;
        } else if(socket->state() == QAbstractSocket::HostLookupState) {
            ui->textEdit->setPlainText("socket state HostLookupState\n");
        } else if(socket->state() == QAbstractSocket::ConnectingState ) {
            ui->textEdit->setPlainText("socket state Connecting\n");
        } else if(socket->state() == QAbstractSocket::ConnectedState ) {
            if(ipAddress==(socket->peerAddress().toString()))
                return true;
            else
            {
                socket->close();
                socket = NULL;
                //return true;
            }
        } else if(socket->state() == QAbstractSocket::BoundState  ) {
            ui->textEdit->setPlainText("socket state BoundState \n");
        } else if(socket->state() == QAbstractSocket::ClosingState  ) {
            ui->textEdit->setPlainText("socket state ClosingState \n");
        }
    }

    socket = new QTcpSocket(this);
    socket->connectToHost(ipAddress, 5099);

    ret = socket->waitForConnected(5000);
    if(ret == false)
      return false;
    //socket->write("Hello server\r\n\r\n");
    connect(socket,SIGNAL(readyRead()),this,SLOT(readData()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(closeConnection()) );
    return true;

}

RegisterDialogue::RegisterDialogue(QWidget *parent): QDialog(parent)
{
    vbox            = new QVBoxLayout(this);
    ip_hbox         = new QHBoxLayout();
    subnet_hbox     = new QHBoxLayout();
    location_hbox   = new QHBoxLayout();
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(accepted()), this->parent(), SLOT(accept1()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    ip_Label = new QLabel();
    subnet_Label = new QLabel();
    location_Label= new QLabel();

    ip_Label->setText("IpAddress:");
    subnet_Label->setText("Subnet Mask:");
    location_Label->setText("Location:");

    ip_Edit = new IPLineEdit(this);
    subnet_Edit = new IPLineEdit(this);
    location_Edit = new QLineEdit();

    ip_Edit->setFixedWidth(150);
    subnet_Edit->setFixedWidth(150);
    location_Edit->setFixedWidth(150);

    ip_hbox->addWidget(ip_Label);
    ip_hbox->addWidget(ip_Edit);
    subnet_hbox->addWidget(subnet_Label);
    subnet_hbox->addWidget(subnet_Edit);
    location_hbox->addWidget(location_Label);
    location_hbox->addWidget(location_Edit);

    vbox->addLayout(ip_hbox);
    vbox->addLayout(subnet_hbox);
    vbox->addLayout(location_hbox);

    vbox->addWidget(buttonBox);

    this->setWindowTitle("Register Device");
}

void MainWindow::accept1()
{
    int row_count;
    qDebug()<<"row_count in accept1()="<<(row_count=ui->tableWidget->rowCount());
    row_count=row_count+1;

    deviceList->deviceTag=QString("Device%1").arg(row_count);

    deviceList->iterator1= deviceList->device.begin();
    qDebug()<<"no. of devices"<<deviceList->device.size();

    while((deviceList->iterator1) != (deviceList->device.end()))
    {
        if((deviceList->iterator1->dev_id)==deviceList->deviceTag)
        {
            row_count++;
            deviceList->deviceTag=QString("Device%1").arg(row_count);
            qDebug()<<"deviceTag="<<deviceList->deviceTag;
        }
        (deviceList->iterator1)++;
    }
    addRow();

    ui->tableWidget->setItem(((ui->tableWidget->rowCount())-1),0,new QTableWidgetItem(deviceList->deviceTag));
    ui->tableWidget->setItem(((ui->tableWidget->rowCount())-1),1,new QTableWidgetItem(deviceList->registerDialog->ip_Edit->text()));
    ui->tableWidget->setItem(((ui->tableWidget->rowCount())-1),2,new QTableWidgetItem(deviceList->registerDialog->location_Edit->text()));

    //deviceList->xmlDomReader();
    deviceList->xmlDomUpdate(deviceList->deviceTag);
    deviceList->deviceListUpdate(deviceList->deviceTag);
    deviceList->showDeviceList();
}

void DeviceList::deviceListUpdate(QString deviceTag)
{
    qDebug()<<"in deviceListUpdate";
    showDeviceList();
    qDebug()<<this->device.size();
    qDebug()<<deviceTag;
    qDebug()<<(deviceIndex=this->device.size());
    bool devicePresent= false;
    this->iterator1= this->device.begin();

    while((this->iterator1) != (this->device.end()))
    {
        if((this->iterator1->dev_id)==deviceTag)
        {
            //qDebug()<<this->iterator1->dev_id;
            devicePresent= true;
            break;
        }
        (this->iterator1)++;
    }

    if(!devicePresent)
    {
        qDebug()<<"device Not Present";
        this->device.push_back(Device(deviceTag));
        this->device[deviceIndex].setIP(this->registerDialog->ip_Edit->text());
        this->device[deviceIndex].setSubnet(this->registerDialog->subnet_Edit->text());
        this->device[deviceIndex].setLocation(this->registerDialog->location_Edit->text());
        deviceIndex++;
    }
    else
    {
        qDebug()<<"device already Present";
        this->iterator1->setIP(this->newIpAddress);
        this->iterator1->setSubnet(this->newSubnet);
        this->iterator1->setTempMax(this->max_temp);
        this->iterator1->setTempMin(this->min_temp);
        this->iterator1->setHumidityMax(this->max_humidity);
        this->iterator1->setHumidityMin(this->min_humidity);
        deviceIndex++;
    }
    showDeviceList();
}

void DeviceList::deviceListDelete(QString deviceTag)
{
    qDebug()<<"in deviceListDelete";
    qDebug()<<this->device.size();
    qDebug()<<deviceTag;
    qDebug()<<deviceIndex;

    this->iterator1= this->device.begin();
    while((this->iterator1)!=(this->device.end()))
    {
        if((this->iterator1->dev_id)==deviceTag)
        {
            qDebug()<<"in if ";
            this->device.erase(this->iterator1);
            deviceIndex--;
            //showDeviceList();
            return;
        }
        (this->iterator1)++;
    }
}

void RegisterDialogue::registerInFile()
{
    QXmlStreamWriter *xmlWriter= new QXmlStreamWriter();
    QFile file("C:\\Users\\dhaval\\Desktop\\data1.xml");
    bool exist= file.exists();
    qDebug()<<"exist while writting="<<exist;
    int size= file.size();
    qDebug()<<"size="<<size;
    totalDevice=10;

    if(!file.open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(0,"Error!","Error In opening File");
    }
    else
    {
        qDebug()<<"file open";

        xmlWriter->setDevice(&file);

        QString string="<\\DEVICES>p";
        if(exist==true)
        {
            //file.seek(file.size()-string.size());
        }

        /* Writes a document start with the XML version number. */
        if(exist==false)
        {
            xmlWriter->writeStartDocument();
            xmlWriter->writeCharacters("\r\n");
            xmlWriter->writeStartElement("Devices");
            xmlWriter->writeCharacters("\r\n");
        }

#if 1
        static int deviceNum = 0;
        //int updateDeviceNum = 2;
        while(deviceNum<totalDevice) {
            //if(deviceNum == updateDeviceNum) {
                /*add character data to tag device_ */
                QString deviceName =  QString("device%1").arg(deviceNum);
                //qDebug()<< "deviceName";
                xmlWriter->writeStartElement(deviceName);
                /*add one attribute and its value*/
                xmlWriter->writeAttribute("IP",ip_Edit->text());
                /*add one attribute and its value*/
                xmlWriter->writeAttribute("subnet",subnet_Edit->text());
                /*add one attribute and its value*/
                xmlWriter->writeAttribute("location",location_Edit->text());
                //xmlWriter->writeCharacters ("DeviceID");
                /*close tag device_  */
                xmlWriter->writeEndElement();
                xmlWriter->writeCharacters("\r\n");
                deviceNum++;
            //}
        }
#endif
        if(exist==false)
        {
            /*end tag Devices*/
            xmlWriter->writeEndElement();
            //xmlWriter->writeCharacters("\r\n");
            /*end document */
            xmlWriter->writeEndDocument();
        }
    }
    file.close();

}

void RegisterDialogue::xmlDomWritter()
{
    QFile file("C:\\Users\\dhaval\\Desktop\\data3.xml");
    bool exist= file.exists();
    qDebug()<<"exist while writting="<<exist;
    int size= file.size();
    qDebug()<<"size="<<size;

    QDomDocument document;
    //if(docXML)
    //    document = docXML;
    QDomElement root= document.createElement("Devices");
    document.appendChild(root);

    //static int i=0;
    QDomElement device = document.createElement("Device");
    device.setAttribute("IP",this->ip_Edit->text());
    device.setAttribute("Subnet",this->subnet_Edit->text());
    device.setAttribute("Location",this->location_Edit->text());
    root.appendChild(device);

    /*QDomNode n = root.firstChild();
    while (!n.isNull())
    {
        if (n.isElement())
        {
            QDomElement e = n.toElement();
            qDebug() << "Element name: " << e.tagName();

            break;
        }
        n = n.nextSibling();
    }*/

    //QDomNode n= document.firstChild();
    //n.appendChild(device);

    if(!file.open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(0,"Error!","Error In opening File");
    }
    else
    {
        /*if(exist)
        {
            file.seek(file.size());
        }*/
        QTextStream stream(&file);
        stream<<document.toString();
        file.close();
        qDebug()<<"Writing is done";
    }

}

void RegisterDialogue::accept()
{
    //this->setResult(QDialog::Accepted);

    //registerInFile();
    //xmlDomWritter();
    QFile file("C:\\Users\\dhaval\\Desktop\\data.txt");
    bool exist= file.exists();
    qDebug()<<"exist="<<exist;

    if(file.open((QIODevice::ReadWrite)|(QIODevice::Text)))
    {
        if(exist)
        {
            /*QByteArray read= file.readAll();

            if(read.contains(ip_Edit->text()));
            {
                QErrorMessage *errorMessage= new QErrorMessage;
                errorMessage->setWindowTitle("Error!");
                errorMessage->showMessage("Device IP already registered");
                return;
            }*/
            file.seek(file.size());
        }
        QTextStream out(&file);
        out<<ip_Edit->text()<<";";
        out<<subnet_Edit->text()<<";";
        out<<location_Edit->text()<<"\r\n";
    }

    //xmlDomUpdate();
    //xmlDomReader();
    //qXmlReader();

    this->done(QDialog::Accepted);
    this->close();
    this->hide();
}

int DeviceList::xmlDomReader()
{
    QDomDocument document;
    QFile file(file_name);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::warning(0,"Error!","Error In opening File");
        return 0;
    }
    else
    {
        if(!document.setContent(&file))
        {
            qDebug()<<"failed to load the file";
            return 0;
        }
        file.close();
    }
    int child;
    qDebug()<<"parent node="<<document.documentElement().tagName();
    qDebug()<<"no. of child="<<(child=document.documentElement().childNodes().count());
    if(child==0)
    {
        return child;
    }
    QDomElement root = document.firstChildElement();
    QDomNode n = root.firstChild();

    while(!n.isNull())
    {
        if(n.isElement())
        {
            QDomElement e = n.toElement();
            qDebug() << "Element name: " << e.tagName();

            retriveElements(root, e.tagName());
        }
        n = n.nextSibling();
    }
    return child;
}

void DeviceList::retriveElements(QDomElement root, QString tag)
{
    qDebug()<<"in retrive element";
    QDomNodeList nodes = root.elementsByTagName(tag);
    //qDebug()<<"root="<<root.tagName();
    qDebug()<<"#nodes="<<nodes.count();

    //for(int i=0; i<nodes.count(); i++)
    {
        QDomNode elm = nodes.at(0);
        int atts_count=0;
        if(elm.isElement())
        {
            QDomNamedNodeMap atts= elm.attributes();
            qDebug()<<"atts_count="<<(atts_count=atts.count());
            QDomElement e = elm.toElement();
            //deviceList->device.push_back(DeviceWidget());


            this->device.push_back(Device(tag));

            /*if(flag==true)
            {
                qDebug()<<"flag="<<flag;
                deviceList->iterator1=deviceList->device.begin();
                flag=false;
            }*/

            //deviceList->iterator1->dev_id= tag;
            qDebug()<<"device tag="<<this->device[deviceIndex].dev_id;

            for(int i=0;i<atts_count;i++)
            {
                QString attribute= atts.item(i).toAttr().name();
                qDebug()<<attribute<<"="<<e.attribute(attribute);
                //ui->tableWidget->setItem(,1,new QTableWidgetItem(tag));

                if(attribute == "IP")
                {
                    qDebug()<<"in IP";
                    //if((deviceList->iterator1->dev_id)==tag)
                    //    deviceList->iterator1->setIP(e.attribute(attribute));
                    this->device[deviceIndex].setIP(e.attribute(attribute));
                    qDebug()<<"ip in retreive="<<this->device[deviceIndex].dev_ip;
                    //ui->tableWidget->setItem(,2,new QTableWidgetItem(e.attribute(attribute)));
                }
                if(attribute == "Subnet")
                {
                    qDebug()<<"in Subnet";
                    //if((deviceList->iterator1->dev_id)==tag)
                    //    deviceList->iterator1->setSubnet(e.attribute(attribute));
                    this->device[deviceIndex].setSubnet(e.attribute(attribute));
                    qDebug()<<"subnet in retreive="<<this->device[deviceIndex].dev_subnet;
                    //ui->tableWidget->setItem(,2,new QTableWidgetItem(e.attribute(attribute)));
                }
                if(attribute == "Location")
                {
                    qDebug()<<"in Location";
                    //if((deviceList->iterator1->dev_id)==tag)
                    //    deviceList->iterator1->setLocation(e.attribute(attribute));
                    this->device[deviceIndex].setLocation(e.attribute(attribute));
                    qDebug()<<"location in retreive="<<this->device[deviceIndex].dev_location;
                    //ui->tableWidget->setItem(,2,new QTableWidgetItem(e.attribute(attribute)));
                }
                if(attribute == "Max_Temp")
                {
                    //if((deviceList->iterator1->dev_id)==tag)
                    //    deviceList->iterator1->setLocation(e.attribute(attribute));
                    this->device[deviceIndex].setTempMax(e.attribute(attribute));
                    qDebug()<<"max_temp in retreive="<<this->device[deviceIndex].dev_temp_max;
                    //ui->tableWidget->setItem(,2,new QTableWidgetItem(e.attribute(attribute)));
                }
                if(attribute == "Min_Temp")
                {
                    //if((deviceList->iterator1->dev_id)==tag)
                    //    deviceList->iterator1->setLocation(e.attribute(attribute));
                    this->device[deviceIndex].setTempMin(e.attribute(attribute));
                    qDebug()<<"min_temp in retreive="<<this->device[deviceIndex].dev_temp_min;
                    //ui->tableWidget->setItem(,2,new QTableWidgetItem(e.attribute(attribute)));
                }
                if(attribute == "Max_Humidity")
                {
                    this->device[deviceIndex].setHumidityMax(e.attribute(attribute));
                    qDebug()<<"max_humidity in retreive="<<this->device[deviceIndex].dev_humidity_max;
                    //ui->tableWidget->setItem(,2,new QTableWidgetItem(e.attribute(attribute)));
                }
                if(attribute == "Min_Humidity")
                {
                    //if((deviceList->iterator1->dev_id)==tag)
                    //    deviceList->iterator1->setLocation(e.attribute(attribute));
                    this->device[deviceIndex].setHumidityMin(e.attribute(attribute));
                    qDebug()<<"mai_Humidity in retreive="<<this->device[deviceIndex].dev_humidity_min;
                    //ui->tableWidget->setItem(,2,new QTableWidgetItem(e.attribute(attribute)));
                }
                /*if(attribute == "Temp")
                {
                    qDebug()<<"in Temp";
                    //if((deviceList->iterator1->dev_id)==tag)
                    //    deviceList->iterator1->setLocation(e.attribute(attribute));
                    this->device[deviceIndex].setTemp(e.attribute(attribute));
                    qDebug()<<"temp in retreive="<<this->device[deviceIndex].dev_temp;
                    //ui->tableWidget->setItem(,2,new QTableWidgetItem(e.attribute(attribute)));
                }
                if(attribute == "Humidity")
                {
                    qDebug()<<"in Humidity";
                    //if((deviceList->iterator1->dev_id)==tag)
                    //    deviceList->iterator1->setLocation(e.attribute(attribute));
                    this->device[deviceIndex].setHumidity(e.attribute(attribute));
                    qDebug()<<"humidity in retreive="<<this->device[deviceIndex].dev_humidity;
                    //ui->tableWidget->setItem(,2,new QTableWidgetItem(e.attribute(attribute)));
                }
                if(attribute == "State")
                {
                    qDebug()<<"in State";
                    //if((deviceList->iterator1->dev_id)==tag)
                    //    deviceList->iterator1->setLocation(e.attribute(attribute));
                    this->device[deviceIndex].setState(e.attribute(attribute));
                    qDebug()<<"state in retreive="<<this->device[deviceIndex].dev_state;
                    //ui->tableWidget->setItem(,2,new QTableWidgetItem(e.attribute(attribute)));
                }*/
                //(deviceList->iterator1)++;

                //qDebug()<<"subnet="<<e.attribute(subnet);
                //qDebug()<<"location="<<e.attribute(location);
            }
        }
        deviceIndex++;
        qDebug()<<"deviceIndex="<<deviceIndex;
    }
}

void DeviceList::xmlDomUpdate(QString deviceTag)
{
    QDomDocument document;
    QFile file(file_name);
    bool exist= file.exists();
    qDebug()<<"exist while writting="<<exist;
    int size= file.size();
    qDebug()<<"size="<<size;

    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        QMessageBox::warning(0,"Error!","Error In opening File");
        return;
    }
    else
    {
        if(exist)
        {
            if(!document.setContent(&file))
            {
                qDebug()<<"failed to load the file";
                return;
            }
            file.close();
        }
    }

    QDomElement root;
    if(exist)
    {
        //if("Devices"==document.documentElement().tagName())
        {
            qDebug()<<"file exist n "<<document.documentElement().tagName();
            root = document.firstChildElement();
            qDebug()<<"root="<<root.tagName();

        }
    }
    else
    {
        qDebug()<<"file doesn't exist ";
        root = document.createElement("Devices");
        document.appendChild(root);
        QDomElement newElement= document.createElement(deviceTag);
        newElement.setAttribute("IP",registerDialog->ip_Edit->text());
        newElement.setAttribute("Subnet",registerDialog->subnet_Edit->text());
        newElement.setAttribute("Location",registerDialog->location_Edit->text());
        newElement.setAttribute("Temp","0");
        newElement.setAttribute("Humidity","0");
        newElement.setAttribute("State","0");
        root.appendChild(newElement);

            QTextStream stream(&file);
            stream<<document.toString();
            file.close();
            qDebug()<<"Writing is done";

        return;
    }

    //get the list;
    //traverse to particular node or child
    //action(add/delete/insertion)
    QDomNode node = root.firstChild();
    QDomNode parentNode = node.parentNode();
    qDebug()<<"N NODE=    "<<node.nodeName()<<"   P NODE="<<parentNode.nodeName();

    QDomElement newElement= document.createElement(deviceTag);

    //root.appendChild(newElement);

    /*****************if only Parent node is present*************/
    if(node.isNull())
    {
        qDebug()<<"root is null";
        newElement.setAttribute("IP",registerDialog->ip_Edit->text());
        newElement.setAttribute("Subnet",registerDialog->subnet_Edit->text());
        newElement.setAttribute("Location",registerDialog->location_Edit->text());
        newElement.setAttribute("Temp","0");
        newElement.setAttribute("Humidity","0");
        newElement.setAttribute("State","0");

        root.appendChild(newElement);
        qDebug()<<"N NODE=    "<<node.nodeName()<<"   P NODE="<<parentNode.nodeName();

        qDebug() << "UPDATE= " <<document.toString();

        file.close();

        #if 1
        QFile file1(file_name);
        if(!file1.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(0,"Error!","Error In opening File4");
        }
        else
        {
            QTextStream stream(&file1);
            stream<<document.toString();
            file1.close();
            qDebug()<<"Writing is done";
        }
        return;
        #endif
    }

    bool devicePresent=false;
#if 1
    QDomElement ele,ele_temp;
    while(!node.isNull())
    {
        if (node.isElement())
        {
            ele = node.toElement();
            qDebug() << "Element name: " << ele.tagName();
            if(newElement.tagName()==ele.tagName())
            {
                qDebug()<<"device already present";
                devicePresent=true;
                ele_temp=ele;
            }
            qDebug() << "nodeName="<<node.nodeName()<<"  nodeValue="<<node.nodeValue();
        }
        node= node.nextSibling();
    }
#endif

    qDebug()<<"N NODE=    "<<node.nodeName()<<"   P NODE="<<parentNode.nodeName();

    if(!devicePresent)
    {
        newElement.setAttribute("IP",registerDialog->ip_Edit->text());
        newElement.setAttribute("Subnet",registerDialog->subnet_Edit->text());
        newElement.setAttribute("Location",registerDialog->location_Edit->text());
        newElement.setAttribute("Temp","0");
        newElement.setAttribute("Humidity","0");
        newElement.setAttribute("State","0");

        parentNode.insertAfter(newElement,parentNode.lastChildElement());
        qDebug()<<"newElement inserted";
    }
    else
    {
        qDebug()<<"in else";
        ele_temp.setAttribute("IP",this->newIpAddress);
        ele_temp.setAttribute("Subnet",this->newSubnet);
        ele_temp.setAttribute("Max_Temp",this->max_temp);
        ele_temp.setAttribute("Min_Temp",this->min_temp);
        ele_temp.setAttribute("Max_Humidity",this->max_humidity);
        ele_temp.setAttribute("Min_Humidity",this->min_humidity);
        /*QMessageBox::StandardButton msgBox= QMessageBox::question(this,"Warning!","Device already present.\n Do you really want to replace it?",QMessageBox::Yes|QMessageBox::No);
        if(msgBox==QMessageBox::Yes)
        {
            parentNode.replaceChild(newElement,ele_temp);
            qDebug()<<"newElement replaced";
        }
        else
        {
            qDebug()<<"newElement NOT replaced";
            return;
        }*/
    }

    qDebug()<<"N NODE=    "<<node.nodeName()<<"   P NODE="<<parentNode.nodeName();

    qDebug() << "UPDATE= " <<document.toString();

    file.close();

#if 1

    QFile file1(file_name);
    if(!file1.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(0,"Error!","Error In opening File4");
    }
    else
    {
        QTextStream stream(&file1);
        stream<<document.toString();
        file1.close();
        qDebug()<<"Writing is done";
    }
#endif
}

void DeviceList::xmlDomDelete(QString deviceTag)
{
    QDomDocument document;
    QFile file(file_name);
    if(!file.open(QIODevice::ReadWrite|QIODevice::Text))
    {
        QMessageBox::warning(0,"Error!","Error In opening File");
        return;
    }
    else
    {
        if(!document.setContent(&file))
        {
            qDebug()<<"failed to load the file";
            return;
        }
        file.close();
    }

    QDomElement root = document.firstChildElement();
    //get the list;
    //traverse to particular node or child
    //action(add/delete/insertion)
    QDomNode node = root.firstChild();
    QDomNode parentNode = node.parentNode();

    QDomElement ele;
    while(!node.isNull()) {
        if (node.isElement()) {
            ele = node.toElement();
            qDebug() << "Element name: " << ele.tagName();
            qDebug() << "nodeName="<<node.nodeName()<<"  nodeValue="<<node.nodeValue();
            if(ele.tagName()==deviceTag)
            {
                 parentNode.removeChild(ele);
                 break;
            }
        }
        node = node.nextSibling();
    }

    qDebug()<<"N NODE=    "<<node.nodeName()<<"   P NODE="<<parentNode.nodeName()<<" "<< parentNode.lastChild().nodeName();

    qDebug() << "UPDATE" <<document.toString();

    file.close();

#if 1
    QFile file1(file_name);
    bool exist= file1.exists();
    qDebug()<<"exist while writting="<<exist;
    int size= file1.size();
    qDebug()<<"size="<<size;

    if(!file1.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(0,"Error!","Error In opening File4");
    }
    else
    {
        QTextStream stream(&file1);
        stream<<document.toString();
        file1.close();
        qDebug()<<"Writing is done";
    }
#endif
}

void RegisterDialogue::qXmlReader()
{
    QXmlSimpleReader* parser 		= new QXmlSimpleReader();
    MyXmlContentHandler* handler 	= new MyXmlContentHandler();

       parser->setContentHandler(handler);

       std::cout<<	"Starting parsing"<< std::endl;

       if(parser->parse(new QXmlInputSource(new QFile("C:\\Users\\dhaval\\Desktop\\data1.xml"))))
       {
       std::cout<<"Parsed Successfully!"<< std::endl;
       }
       else
       {
       std::cout<<"Parsing Failed..."<< std::endl;
       }
}

void RegisterDialogue::reject()
{
    //this->setResult(QDialog::Rejected);
    this->done(QDialog::Rejected);
    this->close();
    this->hide();
}

RegisterDialogue::~RegisterDialogue()
{
    delete vbox;
    delete ip_hbox;
    delete subnet_hbox;
    delete location_hbox;
}

IPSubnetDialogue::IPSubnetDialogue(QWidget *parent): QDialog(parent)
{

    //local variable
    vbox        = new QVBoxLayout(this);
    ip_hbox     = new QHBoxLayout();
    subnet_hbox = new QHBoxLayout();
    ipLabel = new QLabel;
    subnetLabel = new QLabel;
    ipEdit = new IPLineEdit(this);
    subnetEdit = new IPLineEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

    ipLabel->setText("IpAddress");
    subnetLabel->setText("Subnet Mask");

    ip_hbox->addWidget(ipLabel);
    ip_hbox->addWidget(ipEdit);

    subnet_hbox->addWidget(subnetLabel);
    subnet_hbox->addWidget(subnetEdit);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    vbox->addLayout(ip_hbox);
    vbox->addLayout(subnet_hbox);

    vbox->addWidget(buttonBox);

    this->setWindowTitle("Ip/Subnet");
}

ConfigureDialog::ConfigureDialog(QWidget *parent): QDialog(parent)
{
    vbox        = new QVBoxLayout(this);

    max_temp_hbox     = new QHBoxLayout();
    min_temp_hbox = new QHBoxLayout();
    max_temp_Label = new QLabel;
    min_temp_Label = new QLabel;
    max_temp_Edit = new IPLineEdit(this);
    min_temp_Edit = new IPLineEdit(this);

    max_humidity_hbox     = new QHBoxLayout();
    min_humidity_hbox = new QHBoxLayout();
    max_humidity_Label = new QLabel;
    min_humidity_Label = new QLabel;
    max_humidity_Edit = new IPLineEdit(this);
    min_humidity_Edit = new IPLineEdit(this);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

    max_temp_Label->setText("Max.Temp");
    min_temp_Label->setText("Min.Temp");
    max_humidity_Label->setText("IpAddress");
    min_humidity_Label->setText("Subnet Mask");


    max_temp_hbox->addWidget(max_temp_Label);
    max_temp_hbox->addWidget(max_temp_Edit);

    min_temp_hbox->addWidget(min_temp_Label);
    min_temp_hbox->addWidget(min_temp_Edit);

    max_humidity_hbox->addWidget(max_humidity_Label);
    max_humidity_hbox->addWidget(max_humidity_Edit);

    min_humidity_hbox->addWidget(min_humidity_Label);
    min_humidity_hbox->addWidget(min_humidity_Edit);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    vbox->addLayout(max_temp_hbox);
    vbox->addLayout(min_temp_hbox);
    vbox->addLayout(max_humidity_hbox);
    vbox->addLayout(min_humidity_hbox);

    vbox->addWidget(buttonBox);

    this->setWindowTitle("Configure Temp & Humidity");
}

IPSubnetDialogue::~IPSubnetDialogue()
{
    delete vbox;
    delete ip_hbox;
    delete subnet_hbox;
}

void IPSubnetDialogue::accept()
{
    //this->setResult(QDialog::Accepted);
    this->done(QDialog::Accepted);
    this->close();
    this->hide();
}

void IPSubnetDialogue::reject()
{
    //this->setResult(QDialog::Rejected);
    this->done(QDialog::Rejected);
    this->close();
    this->hide();
}

ConfigureDialog::~ConfigureDialog()
{
    delete vbox;
    delete max_temp_hbox;
    delete min_temp_hbox;
    delete max_humidity_hbox;
    delete min_humidity_hbox;
}

void ConfigureDialog::accept()
{
    //this->setResult(QDialog::Accepted);
    this->done(QDialog::Accepted);
    this->close();
    this->hide();
}

void ConfigureDialog::reject()
{
    //this->setResult(QDialog::Rejected);
    this->done(QDialog::Rejected);
    this->close();
    this->hide();
}

IPLineEdit::IPLineEdit(QWidget *parent): QLineEdit(parent)
{
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    // You may want to use QRegularExpression for new code with Qt 5, however.
    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
   ipValidator = new QRegExpValidator(ipRegex, this);
   this->setFixedWidth(150);
   this->setValidator(ipValidator);
}

IPLineEdit::~IPLineEdit()
{
    delete ipValidator;
}

QString IPSubnetDialogue::get_ipaddress()
{
    return this->ipEdit->text();
}

QString IPSubnetDialogue::get_subnet()
{
    return this->subnetEdit->text();
}
QString ConfigureDialog::get_max_temp()
{
    return this->max_temp_Edit->text();
}
QString ConfigureDialog::get_min_temp()
{
    return this->min_temp_Edit->text();
}
QString ConfigureDialog::get_max_humidity()
{
    return this->max_humidity_Edit->text();
}
QString ConfigureDialog::get_min_humidity()
{
    return this->min_humidity_Edit->text();
}

void MainWindow::addRow()
{
    rowCount= ui->tableWidget->rowCount();
    qDebug()<<"rowCount="<<rowCount;

    ui->tableWidget->insertRow(rowCount);

    //static int i=rowCount+1;
    //ui->tableWidget->setItem(rowCount,1,new QTableWidgetItem(QString("Device%1").arg(i++)));

    //ui->tableWidget->setItem(rowCount,2,new QTableWidgetItem(""));

    addButtons(rowCount);
}

void MainWindow::addButtons(int row)
{
    qDebug()<<"i in addButtons="<<row;
    /*pWidget = new QWidget();
    pLayout = new QHBoxLayout(pWidget);
    btn_getStatus = new myButton(pWidget);
    btn_getStatus->setText("Get Status");
    pLayout->addWidget(btn_getStatus);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pWidget->setLayout(pLayout);
    ui->tableWidget->setCellWidget(i, 5, pWidget);

    QWidget* qWidget = new QWidget();
    QPushButton* btn_settings = new QPushButton();
    btn_settings->setText("Settings");
    QHBoxLayout* qLayout = new QHBoxLayout(qWidget);
    qLayout->addWidget(btn_settings);
    qLayout->setAlignment(Qt::AlignCenter);
    qLayout->setContentsMargins(0, 0, 0, 0);
    qWidget->setLayout(qLayout);
    ui->tableWidget->setCellWidget(i, 6, qWidget);
    */

    //btn_register= new QPushButton("Register",ui->tableWidget);
    //ui->tableWidget->setCellWidget(i,0,btn_register);

    btn_getStatus= new QPushButton("Get Status",ui->tableWidget);
    ui->tableWidget->setCellWidget(row,colNum_getStatus,btn_getStatus);

    btn_settings= new QPushButton("Settings",ui->tableWidget);
    ui->tableWidget->setCellWidget(row,colNum_settings,btn_settings);

    if((ui->tableWidget->columnCount())==10)
    {
        btn_configure= new QPushButton("Configure",ui->tableWidget);
        ui->tableWidget->setCellWidget(row,colNum_configure,btn_configure);
        connect(btn_configure,SIGNAL(clicked()),this,SLOT(on_btn_configure_clicked()));
    }
    btn_unRegister= new QPushButton("Remove",ui->tableWidget);
    ui->tableWidget->setCellWidget(row,colNum_unRegister,btn_unRegister);

    //connect(btn_register,SIGNAL(clicked()),this,SLOT(on_btn_register_clicked()));
    connect(btn_getStatus,SIGNAL(clicked()),this,SLOT(on_btn_getStatus_clicked()));
    connect(btn_settings,SIGNAL(clicked()),this,SLOT(on_btn_settings_clicked()));

    connect(btn_unRegister,SIGNAL(clicked()),this,SLOT(on_btn_unRegister_clicked()));
    //connect(btn_getStatus,SIGNAL(clicked(int i)),this,SLOT(on_myButton_clicked(int i)));

}

void MainWindow::on_btn_unRegister_clicked()
{
    qDebug()<<"btn_unRegister_clicked";

    int row_count= ui->tableWidget->rowCount();
    qDebug()<<" in unRegister row_count="<<row_count;
    qDebug()<<"cellWidget="<<ui->tableWidget->cellWidget(0,colNum_unRegister);

    for(row_unRegister=0; row_unRegister<row_count; row_unRegister++)
    {
          //if(sender() == but1(row,col)){
        qDebug()<<"row="<<row_unRegister;
        qDebug()<<ui->tableWidget->cellWidget(row_unRegister,colNum_unRegister);
        if(sender() == ui->tableWidget->cellWidget(row_unRegister,colNum_unRegister))
        {
            qDebug() << row_unRegister << sender();
            break;
        }
    }

    QMessageBox::StandardButton msgBox= QMessageBox::question(this,"Warning !","Do you really want to delete this device?",QMessageBox::Yes|QMessageBox::No);
    if(msgBox==QMessageBox::Yes)
    {
        qDebug()<<"sender="<<sender();
        QString deviceTag= ui->tableWidget->item(row_unRegister,0)->text();
        deviceList->deviceListDelete(deviceTag);
        deviceList->showDeviceList();
        deviceList->xmlDomDelete(deviceTag);
        //qDebug()<<"position="<<btn_getStatus->pos();
        ui->tableWidget->removeRow(row_unRegister);
        rowCount--;
    }
    else
    {
        qDebug()<<"Did not deleted";
    }
}

void MainWindow::on_btn_getStatus_clicked()
{
    qDebug()<<"btn_getStatus_clicked";

    int row_count= ui->tableWidget->rowCount();
    qDebug()<<"row_count="<<row_count;
    qDebug()<<"cellWidget="<<ui->tableWidget->cellWidget(0,colNum_getStatus);

    for(row_status=0; row_status<row_count; row_status++)
    {
          //if(sender() == but1(row,col)){
        qDebug()<<"row="<<row_status;
            if(sender() == ui->tableWidget->cellWidget(row_status,colNum_getStatus))
            {
                qDebug() << row_status << sender();
                break;
            }
    }

    qDebug()<<"sender="<<sender();
    //qDebug()<<"position="<<btn_getStatus->pos();

    ipAddress  = ui->tableWidget->item(row_status,1)->text();
    qDebug()<<"ipAddress="<<ipAddress;
    if(ipAddress.count('.')!=3)
    {
        qDebug()<<"count of '.'="<<ipAddress.count('.');
        QErrorMessage *errorMessage= new QErrorMessage;
        errorMessage->setWindowTitle("Error!");
        errorMessage->showMessage("Please enter valid IPAddress first");
        return;
    }

#if 0
    thread= new MyThread(ipAddress,this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
#endif

#if 1
    bool ret = false;
    qint64 wrote;
    ret = doConnect(ipAddress);

    if (ret == false)
    {
        ui->textEdit->setPlainText("Device not found in network.\"Please check your network connection\" ");
        return;
    }
    else
    {
        ui->textEdit->setPlainText("Device is present in Network");
        timer= new QTimer(this);
        timer->setInterval(time_interval);
        timer->start();
        connect(timer,SIGNAL(timeout()),this,SLOT(doIt()));
    }
    wrote = socket->write("#TELL STATUS\n");
    if (wrote == -1)
    {
        ui->textEdit->setPlainText("Connection Lost from device");
        ui->textEdit->append(socket->errorString().toStdString().c_str());
    }
#endif

}


void MainWindow::on_btn_configure_clicked()
{
    qDebug()<<"on_btn_configure_clicked";
    ConfigureDialog *config= new ConfigureDialog(this);

    bool ret= false;
    QString ipAddress;
    int row_count= ui->tableWidget->rowCount();
    qDebug()<<"row_count="<<row_count;
    qDebug()<<"cellWidget="<<ui->tableWidget->cellWidget(0,colNum_configure);

    int row;
    for(row=0; row<row_count; row++)
    {
        qDebug()<<"row="<<row;
        if(sender() == ui->tableWidget->cellWidget(row,colNum_configure))
        {
            qDebug() << row << sender();
            break;
        }
    }
    qDebug()<<"sender="<<sender();

    QString dev_id = ui->tableWidget->item(row,0)->text();
    ipAddress= ui->tableWidget->item(row,1)->text();
    qDebug()<<"ipAddress in settings="<<ipAddress;

    ret =  config->exec();
    if(ret == QDialog::Accepted){
        qDebug()<<"dialog accepted";
        deviceList->max_temp= config->get_max_temp();
        deviceList->min_temp= config->get_min_temp();
        deviceList->max_humidity= config->get_max_humidity();
        deviceList->min_humidity= config->get_min_humidity();
        deviceList->xmlDomUpdate(dev_id);
        deviceList->deviceListUpdate(dev_id);
    }
    else
       return;

    if(socket!=NULL)
    {
        socket->write("#MAX_TEMP#");
        socket->write(config->get_max_temp().toStdString().c_str());
        socket->write("\n");
        socket->write("#MIN_TEMP#");
        socket->write(config->get_min_temp().toStdString().c_str());
        socket->write("\n");
        socket->write("#MAX_HUMIDITY#");
        socket->write(config->get_max_humidity().toStdString().c_str());
        socket->write("\n");
        socket->write("#MIN_HUMIDITY#");
        socket->write(config->get_min_humidity().toStdString().c_str());
        socket->write("\n");
    }
}

void MainWindow::on_btn_settings_clicked()
{
    qDebug()<<"on_btn_settings_clicked";
    bool ret = false;
    QString ipAddress;
    //QString newIpAddress = "";
    //QString newSubnet = "";
    IPSubnetDialogue *netInfo = new IPSubnetDialogue(this);

    int row_count= ui->tableWidget->rowCount();
    qDebug()<<"row_count="<<row_count;
    qDebug()<<"cellWidget="<<ui->tableWidget->cellWidget(0,colNum_settings);

    int row;
    for(row=0; row<row_count; row++)
    {
          //if(sender() == but1(row,col)){
        qDebug()<<"row="<<row;
            if(sender() == ui->tableWidget->cellWidget(row,colNum_settings))
            {
                qDebug() << row << sender();
                break;
            }
    }

    qDebug()<<"sender="<<sender();
    //qDebug()<<"position="<<btn_getStatus->pos();

    ipAddress  = ui->tableWidget->item(row,1)->text();
    qDebug()<<"ipAddress in settings="<<ipAddress;
    QString dev_id = ui->tableWidget->item(row,0)->text();

    if(ipAddress == "") {
        ui->textEdit->setPlainText("Please fill IP address of device. \"look at lcd of device\" ");
        return;
    }
    ret =  netInfo->exec();
    if(ret == QDialog::Accepted) {
        deviceList->newIpAddress = netInfo->get_ipaddress();
        deviceList->newSubnet = netInfo->get_subnet();
        //deviceList->xmlDomReader();

        /********************************
        *********    TO DO    **********/
#if 1
        deviceList->xmlDomUpdate(dev_id);
        deviceList->deviceListUpdate(dev_id);
        updateTable();
#endif
    }
    else
       return;

    if(deviceList->newIpAddress == "") {
        ui->textEdit->setPlainText("Please fill IP address of device. \"look at lcd of device\" ");
        return;
    }
    ret = doConnect(ipAddress);
    if (ret == false) {
        ui->textEdit->setPlainText("Device not found in network.\"Please check your network connection\" ");
        return;
    }
    else
    {
        ui->textEdit->setPlainText("Device is present in Network");
    }

    if(socket!=NULL)
    {
        socket->write("#IP#");
        socket->write(deviceList->newIpAddress.toStdString().c_str());
        socket->write("\n");
        socket->write("#SUBNET#");
        socket->write(netInfo->get_subnet().toStdString().c_str());
        socket->write("\n");
    }
}

void MainWindow::doIt()
{
    /*qint64 wrote= socket->write("#TELL STATUS\n");
    if (wrote == -1)
    {
        ui->textEdit->setPlainText("Connection Lost from device");
        ui->textEdit->append(socket->errorString().toStdString().c_str());
    }*/
}



void MainWindow::on_add_device_clicked()
{
    if(ui->tableWidget->rowCount()<maxDevice)
    {
        deviceList->registerDialog= new RegisterDialogue(this);
        deviceList->registerDialog->exec();

    }
    else
    {
        QErrorMessage *errorMessage= new QErrorMessage(this);
        {
            errorMessage->setWindowTitle("ERROR!");
            errorMessage->showMessage("SORRY! YOU CANNOT ADD MORE THAN 10 DEVICE ");
        }
    }
}

