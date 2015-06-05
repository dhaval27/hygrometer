#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAbstractSocket>
#include <iostream>



//QString file_name="C:\\Users\\dhaval\\Desktop\\data4.xml";

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

    socket = NULL;

    maxDevice=10;

    time_interval=15000;
    ui->devicelist->show();

//    deviceList = new DeviceList(ui->devicelist);

//    for(int i=0; i < deviceList->deviceCount; i++)
//    {
//        AddDevice(deviceList->device + i);
//    }
    updateTable();
}

#if 0
void DeviceList::xmlDomUpdateStatus(QString deviceTag,QVector<Device>::iterator iterator)
{
    QDomDocument document;
    QFile file(FILE_NAME);
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
#if 0
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
#endif
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

    QFile file1(FILE_NAME);
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

    qDebug()<<IpAddress;
    qDebug()<<deviceIndex;
    bool devicePresent= false;
 #if 0
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
#endif
}
#endif
void MainWindow::updateTable()
{
    qDebug()<<"in updateTable";
#if 0
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
        ui->devicelist->setItem(i,0,new QdevicelistItem(deviceList->device[i].dev_id));
        ui->devicelist->setItem(i,1,new QdevicelistItem(deviceList->device[i].dev_ip));
        ui->devicelist->setItem(i,2,new QdevicelistItem(deviceList->device[i].dev_location));
        i++;
        (deviceList->iterator1)++;
    }
#endif
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::saveStatus()
{
#if 0
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
#endif
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


void MainWindow::accept1()
{
    int row_count;
    qDebug()<<"row_count in accept1()="<<(row_count=ui->devicelist->rowCount());
    row_count=row_count+1;
#if 0
    ui->devicelist->deviceTag=QString("Device%1").arg(row_count);

    ui->devicelist->iterator1= deviceList->device.begin();
    qDebug()<<"no. of devices"<<deviceList->device.size();

    while((ui->devicelist->iterator1) != (ui->devicelist->device.end()))
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

    ui->devicelist->setItem(((ui->devicelist->rowCount())-1),0,new QdevicelistItem(deviceList->deviceTag));
    ui->devicelist->setItem(((ui->devicelist->rowCount())-1),1,new QdevicelistItem(deviceList->registerDialog->ip_Edit->text()));
    ui->devicelist->setItem(((ui->devicelist->rowCount())-1),2,new QdevicelistItem(deviceList->registerDialog->location_Edit->text()));

    //deviceList->xmlDomReader();
    deviceList->xmlDomUpdate(deviceList->deviceTag);
    deviceList->deviceListUpdate(deviceList->deviceTag);
    deviceList->showDeviceList();
#endif
}

#if 0
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
#endif



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
    if(ui->devicelist->rowCount()<maxDevice)
    {
#if 1
        ui->devicelist->registerDialog= new RegisterDialog(ui->devicelist);
        ui->devicelist->registerDialog->exec();
#endif
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

