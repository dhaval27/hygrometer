#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAbstractSocket>
#include <iostream>

QString file_name="C:\\Users\\dhaval\\Desktop\\data4.xml";
//bool flag=true;
bool flag_update=true;
static int deviceIndex=0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setHorizontalHeaderLabels(QString("REGISTER;DEVICE;IP ADDRESS;TEMP;HUMIDITY;STATE").split(";"));

    socket = NULL;

    maxDevice=10;

    time_interval=10000;
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
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(deviceList->device[i].dev_id));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(deviceList->device[i].dev_ip));
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
        qDebug()<<"device="<<(this->device[i].dev_id);
        qDebug()<<"device_ip="<<(this->device[i].dev_ip);
        qDebug()<<"device_subnet="<<(this->device[i].dev_subnet);
        qDebug()<<"device_location="<<(this->device[i].dev_location);
        qDebug()<<"********";
        /*
        if(flag_update==true)
        {
            ui->tableWidget->setItem(i,1,new QTableWidgetItem(deviceList->device[i].dev_id));
            ui->tableWidget->setItem(i,2,new QTableWidgetItem(deviceList->device[i].dev_ip));
        }
        i++;*/
        (this->iterator1)++;
    }
    //flag_update=false;
}

DeviceList::DeviceList()
{
    //d1= new DeviceWidget("hi");
    //iterator= device.begin();
    //device.push_back(DeviceWidget("hi !"));

    //iterator1 = device.begin();
    //device.push_back(DeviceWidget());
    //qDebug()<<"device[0]"<<device[0].dev_id;


    /*qDebug()<<"size="<<v.size();
    int i;
    iterator= v.begin();
    for(i=0;i<5;i++)
    {
        v.push_front(QString("v %1").arg(i));
        qDebug()<<"v[i]="<<v[i];
    }
    qDebug()<<"size="<<v.size();

    for(i=0;i<v.size();i++)
    {
        //v.push_front(QString("v %1").arg(i));
        qDebug()<<"v[i]="<<v[i];
    }*/
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::getStatus()
{
    qDebug()<<"data in getStatus="<<data;
    status= data;
    qDebug()<<"status="<<status;
    status= status.remove('\n');
    qDebug()<<"status after remove \n="<<status;

    earlierState= state;
    state= status.mid(status.lastIndexOf("#")+1);
    qDebug()<<"state="<<state;

    if(state!=earlierState)
    {
        activity_text=ui->textEdit_2->toPlainText();
        qDebug()<<"activity_text="<<activity_text;
        activity_text.append("Device 1 changed its status.\n");
        activity_text.append(QTime::currentTime().toString("HH:mm:ss"));
        activity_text.append(", ");
        activity_text.append(QDate::currentDate().toString("dd MMM"));
        activity_text.append("\n");
        ui->textEdit_2->setText(activity_text);
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

    ui->temprature->setText(temp);
    ui->humidity->setText(humidity);
    ui->state->setText(state);

    ui->tableWidget->setItem(row_status,3,new QTableWidgetItem(temp));
    ui->tableWidget->setItem(row_status,4,new QTableWidgetItem(humidity));
    ui->tableWidget->setItem(row_status,5,new QTableWidgetItem(state));
}

void MainWindow::saveStatus()
{
    st_status s1[8];

    memset(s1,0,sizeof(s1));

    //p= (char *)malloc(sizeof(st_status));
    strcpy(s1[0].st_ip,ui->ip_address->text().toStdString().c_str());
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

    ui->textEdit->append(data);
    if(data.contains("###STATUS#"))
    {
        getStatus();
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
    socket= server->nextPendingConnection();
    qDebug()<<"socket started";

    //connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(writeData(qint64)));
    connect(socket,SIGNAL(readyRead()),this,SLOT(readData()));
    socket->write("Hello Client\n");
    socket->flush();
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

    if(socket) {
        if(socket->state() == QAbstractSocket::UnconnectedState) {
            socket->close();
            socket = NULL;
        } else if(socket->state() == QAbstractSocket::HostLookupState) {
            ui->textEdit->setPlainText("socket state HostLookupState\n");
        } else if(socket->state() == QAbstractSocket::ConnectingState ) {
            ui->textEdit->setPlainText("socket state Connecting\n");
        } else if(socket->state() == QAbstractSocket::ConnectedState ) {
            return true;
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

void MainWindow::openDilog()
{
    QDialog *dialog= new QDialog;
    dialog->setWindowTitle("Register Device");

    //TODO add line edit add level add buttom ok
    dialog->exec();
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

    ip_Edit = new QLineEdit();
    subnet_Edit = new QLineEdit();
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

    ui->tableWidget->setItem(((ui->tableWidget->rowCount())-1),1,new QTableWidgetItem(deviceList->deviceTag));
    ui->tableWidget->setItem(((ui->tableWidget->rowCount())-1),2,new QTableWidgetItem(deviceList->registerDialog->ip_Edit->text()));

    deviceList->xmlDomUpdate(deviceList->deviceTag);
    deviceList->deviceListUpdate();
    deviceList->showDeviceList();
}

void DeviceList::deviceListUpdate()
{
    qDebug()<<"in deviceListUpdate";
    qDebug()<<this->device.size();
    qDebug()<<deviceTag;
    qDebug()<<deviceIndex;

    this->device.push_back(DeviceWidget(deviceTag));
    this->device[deviceIndex].setIP(this->registerDialog->ip_Edit->text());
    this->device[deviceIndex].setSubnet(this->registerDialog->subnet_Edit->text());
    this->device[deviceIndex].setLocation(this->registerDialog->location_Edit->text());
    deviceIndex++;
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

            this->device.push_back(DeviceWidget(tag));

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
    newElement.setAttribute("IP",registerDialog->ip_Edit->text());
    newElement.setAttribute("Subnet",registerDialog->subnet_Edit->text());
    newElement.setAttribute("Location",registerDialog->location_Edit->text());
    //root.appendChild(newElement);

    /*****************if only Parent node is present*************/
    if(node.isNull())
    {
        qDebug()<<"root is null";
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
        parentNode.insertAfter(newElement,parentNode.lastChildElement());
        qDebug()<<"newElement inserted";
    }
    else
    {
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

void MainWindow::on_getDeviceStatus_clicked()
{
    QString ipAddress  = ui->tableWidget->item(0,2)->text();
    bool ret = false;
    qint64 wrote;
    ret = doConnect(ipAddress);
    if (ret == false) {
        qDebug()<<"ret==false";
        ui->textEdit->setPlainText("Device not found in network.\"Please check your network connection\" ");
        return;
    }
    wrote = socket->write("#TELL STATUS\n");
    if (wrote == -1) {
        ui->textEdit->setPlainText("Connection Lost from device");
        ui->textEdit->append(socket->errorString().toStdString().c_str());
    }
}

void MainWindow::on_Register_clicked()
{
    //RegisterDialogue *reg= new RegisterDialogue(this);
    //reg->exec();

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

void MainWindow::addButtons(int i)
{
    qDebug()<<"i in addButtons="<<i;
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

    btn_register= new QPushButton("Register",ui->tableWidget);
    ui->tableWidget->setCellWidget(i,0,btn_register);

    btn_getStatus= new QPushButton("Get Status",ui->tableWidget);
    ui->tableWidget->setCellWidget(i,6,btn_getStatus);

    btn_settings= new QPushButton("Settings",ui->tableWidget);
    ui->tableWidget->setCellWidget(i,7,btn_settings);

    ui->tableWidget->setColumnWidth(8,30);
    btn_unRegister= new QPushButton("X",ui->tableWidget);
    ui->tableWidget->setCellWidget(i,8,btn_unRegister);

    connect(btn_register,SIGNAL(clicked()),this,SLOT(on_btn_register_clicked()));
    connect(btn_getStatus,SIGNAL(clicked()),this,SLOT(on_btn_getStatus_clicked()));
    connect(btn_settings,SIGNAL(clicked()),this,SLOT(on_btn_settings_clicked()));
    connect(btn_unRegister,SIGNAL(clicked()),this,SLOT(on_btn_unRegister_clicked()));
    //connect(btn_getStatus,SIGNAL(clicked(int i)),this,SLOT(on_myButton_clicked(int i)));

}

void MainWindow::on_btn_register_clicked()
{
    qDebug()<<"btn_register_clicked";

    /*int row_count= ui->tableWidget->rowCount();
    qDebug()<<" in register row_count="<<row_count;
    qDebug()<<"cellWidget="<<ui->tableWidget->cellWidget(0,0);

    for(row_register=0; row_register<row_count; row_register++)
    {
          //if(sender() == but1(row,col)){
        qDebug()<<"row="<<row_register;
        qDebug()<<ui->tableWidget->cellWidget(row_register,0);
        if(sender() == ui->tableWidget->cellWidget(row_register,0))
        {
            qDebug() << row_register << sender();
            break;
        }
    }

    QString deviceTag= ui->tableWidget->item(row_register,1)->text();
    qDebug()<<"deviceTag in register slot="<<deviceTag;
    qDebug()<<"sender="<<sender();

    registerDialog= new RegisterDialogue(this);
    registerDialog->exec();
    */
}

void MainWindow::on_btn_unRegister_clicked()
{
    qDebug()<<"btn_unRegister_clicked";

    int row_count= ui->tableWidget->rowCount();
    qDebug()<<" in unRegister row_count="<<row_count;
    qDebug()<<"cellWidget="<<ui->tableWidget->cellWidget(0,8);

    for(row_unRegister=0; row_unRegister<row_count; row_unRegister++)
    {
          //if(sender() == but1(row,col)){
        qDebug()<<"row="<<row_unRegister;
        qDebug()<<ui->tableWidget->cellWidget(row_unRegister,8);
        if(sender() == ui->tableWidget->cellWidget(row_unRegister,8))
        {
            qDebug() << row_unRegister << sender();
            break;
        }
    }

    QMessageBox::StandardButton msgBox= QMessageBox::question(this,"Warning !","Do you really want to delete this device?",QMessageBox::Yes|QMessageBox::No);
    if(msgBox==QMessageBox::Yes)
    {
        qDebug()<<"sender="<<sender();
        QString deviceTag= ui->tableWidget->item(row_unRegister,1)->text();
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
    qDebug()<<"cellWidget="<<ui->tableWidget->cellWidget(0,6);

    for(row_status=0; row_status<row_count; row_status++)
    {
          //if(sender() == but1(row,col)){
        qDebug()<<"row="<<row_status;
            if(sender() == ui->tableWidget->cellWidget(row_status,6))
            {
                qDebug() << row_status << sender();
                break;
            }
    }

    qDebug()<<"sender="<<sender();
    //qDebug()<<"position="<<btn_getStatus->pos();

    ipAddress  = ui->tableWidget->item(row_status,2)->text();
    qDebug()<<"ipAddress="<<ipAddress;
    if(ipAddress.count('.')!=3)
    {
        qDebug()<<"count of '.'="<<ipAddress.count('.');
        QErrorMessage *errorMessage= new QErrorMessage;
        errorMessage->setWindowTitle("Error!");
        errorMessage->showMessage("Please enter valid IPAddress first");
        return;
    }

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

}

void MainWindow::on_btn_settings_clicked()
{
    bool ret = false;
    QString ipAddress;
    QString newIpAddress = "";
    QString newSubnet = "";
    IPSubnetDialogue *netInfo = new IPSubnetDialogue(this);

    int row_count= ui->tableWidget->rowCount();
    qDebug()<<"row_count="<<row_count;
    qDebug()<<"cellWidget="<<ui->tableWidget->cellWidget(0,7);

    int row;
    for(row=0; row<row_count; row++)
    {
          //if(sender() == but1(row,col)){
        qDebug()<<"row="<<row;
            if(sender() == ui->tableWidget->cellWidget(row,7))
            {
                qDebug() << row << sender();
                break;
            }
    }

    qDebug()<<"sender="<<sender();
    //qDebug()<<"position="<<btn_getStatus->pos();

    ipAddress  = ui->tableWidget->item(row,2)->text();
    qDebug()<<"ipAddress in settings="<<ipAddress;
    if(ipAddress.count('.')!=3)
    {
        qDebug()<<"count of '.'="<<ipAddress.count('.');
        QErrorMessage *errorMessage= new QErrorMessage;
        errorMessage->setWindowTitle("Error!");
        errorMessage->showMessage("Please enter valid IPAddress first");
        return;
    }

    if(ipAddress == "") {
        ui->textEdit->setPlainText("Please fill IP address of device. \"look at lcd of device\" ");
        return;
    }
    ret =  netInfo->exec();
    if(ret == QDialog::Accepted) {
        newIpAddress = netInfo->get_ipaddress();
        newSubnet = netInfo->get_subnet();
    }
    else
       return;

    if(newIpAddress == "") {
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

    socket->write("#IP:");
    socket->write(newIpAddress.toStdString().c_str());
    socket->write("#SUBNET:");
    socket->write(newSubnet.toStdString().c_str());
}

void MainWindow::doIt()
{
    qint64 wrote= socket->write("#TELL STATUS\n");
    if (wrote == -1)
    {
        ui->textEdit->setPlainText("Connection Lost from device");
        ui->textEdit->append(socket->errorString().toStdString().c_str());
    }
}

/*void MainWindow::on_btn_settings_clicked()
{
    qDebug()<<"settings button slot";
}*/

void MainWindow::on_Unregister_clicked()
{
    qDebug()<<"unregister button slot";

    UnregisterDialogue *unRegister= new UnregisterDialogue(this);
    unRegister->exec();
}

UnregisterDialogue:: UnregisterDialogue(QWidget *parent):QDialog(parent)
{
    vbox= new QVBoxLayout(this);
    ip_hbox= new QHBoxLayout();
    ip_Label= new QLabel();
    ip_Edit= new QLineEdit();
    buttonBox= new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    ip_Label->setText("IP address:");

    ip_hbox->addWidget(ip_Label);
    ip_hbox->addWidget(ip_Edit);
    vbox->addLayout(ip_hbox);
    vbox->addWidget(buttonBox);

    this->setWindowTitle("Unregister Device");
}

void UnregisterDialogue::accept()
{
    //this->setResult(QDialog::Accepted);

    /*int rowNum;
    //row_count=2;
    row_count= ui->tableWidget->rowCount();
    //qDebug()<<"row_count="<<row_count;

    ip_del= ip_Edit->text();
    qDebug()<<"ip_del="<<ip_del;

    for(rowNum=0; rowNum<row_count; rowNum++)
    {
        if((ui->tableWidget->item(rowNum,1)->text())==ip_del)
        {
            ui->tableWidget->removeRow(rowNum);
        }
    }*/

    this->done(QDialog::Accepted);
    this->close();
    this->hide();
}

void UnregisterDialogue::reject()
{
    //this->setResult(QDialog::Rejected);
    this->done(QDialog::Rejected);
    this->close();
    this->hide();
}

UnregisterDialogue::~UnregisterDialogue()
{
    delete vbox;
    delete ip_hbox;
    delete ip_Edit;
    delete ip_Label;
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


