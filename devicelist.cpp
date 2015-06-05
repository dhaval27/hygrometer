#include <devicelist.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qheaderview.h>
#include <qerrormessage.h>
#include <qdatetime.h>

DeviceList::DeviceList(QWidget *parent)
{
    for( int i = 0; i < MAX_DEVICE; i++) {
        this->device[i].initDevice(this,-1);
        this->device[i].socket=NULL;
    }

}

void DeviceList::show(void)
{
#ifdef HYGROMETER

        colNum_state=3;
        colNum_temp=4;
        colNum_humidity=5;
        colNum_getStatus=6;
        colNum_settings=7;
        colNum_configure=8;
        colNum_unRegister=9;
        this->setColumnCount(10);
        this->setHorizontalHeaderLabels(QString("DEVICE;IP ADDRESS;LOCATION;STATE;TEMP;HUMIDITY;STATUS;;;").split(";"));

#else
        colNum_state=3;
        colNum_getStatus=4;
        colNum_settings=5;
        colNum_unRegister=6;
        this->setColumnCount(7);

        this->setHorizontalHeaderLabels(QString("DEVICE;IP ADDRESS;LOCATION;STATE; ; ;").split(";"));
#endif

    QFont *font = new QFont("Helvetica",10,8,false);
    font->setBold(true);
    QHeaderView *header= this->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    header->setFont(*font);
    header->setToolTip("Click to Get All Devices Status");
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(header,SIGNAL(sectionClicked(int)),this,SLOT(on_statusAll_clicked(int)));
    deviceCount = this->xmlDomReader();

    //this->showDeviceList();
}

void DeviceList::deleteDevice(int device_id)
{
    qDebug()<<"in deleteDevice"<<device_id;

    if( (device_id >= 0) && (device_id < MAX_DEVICE) )
    {
        this->device[device_id].dev_id = -1;

        QList<int>::iterator position = std::find(this->valid_device.begin(), this->valid_device.end(), device_id);
        this->removeRow(std::distance(this->valid_device.begin(),position));
        this->valid_device.erase(position);

        xmlDomDelete(device_id);
    }
    else
    {
        QErrorMessage *err_msg= new QErrorMessage(this);
        err_msg->setWindowTitle("ERROR!");
        err_msg->showMessage("SORRY! device id is not valid");
        return;
    }
}


void DeviceList::on_statusAll_clicked(int index)
{
    qDebug()<<"on_statusAll_clicked"<<index;
    if(index==6)
    {
        for(int i=0;i<MAX_DEVICE; i++)
        {
            bool ret = this->device[i].doConnect(this->device[i].dev_ip);
//            if(ret==false)
//            {
//                QErrorMessage *err_msg= new QErrorMessage();
//                err_msg->setWindowTitle("ERROR!");
//                err_msg->showMessage("SORRY! Device not found ");
//                return;
//            }
            if(this->device[i].socket!=NULL)
            {
                this->device[i].socket->write("#TELL STATUS\n");
            }
        }
    }

}

void DeviceList::updateDevice(int device_id)
{
    qDebug()<<"IN slot";
    this->device[device_id].dev_ip      = this->device[device_id].newIpAddress;
    this->device[device_id].dev_subnet  = this->device[device_id].newSubnet;
    xmlDomUpdate(device_id);
}


void DeviceList::data_available(int device_id,QByteArray data)
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
#if 0
    qDebug()<<"old_host="<<old_host;
    //QString deviceId= ui->devicelist->item(row_status,0)->text();
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
#endif

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

    int row_status= 0;
    int i;
    for(i=0; i< this->rowCount(); i++)
    {
        if( ( (this->item(i,0)->text() ) ) == QString::number(device_id) )
            row_status= i;
    }

    this->setItem(row_status,colNum_state,new QTableWidgetItem(state));
#ifdef HYGROMETER
    this->setItem(row_status,colNum_temp,new QTableWidgetItem(temp));
    this->setItem(row_status,colNum_humidity,new QTableWidgetItem(humidity));
#endif

}

int DeviceList::xmlDomReader()
{
    QDomDocument document;
    QFile file(FILE_NAME);
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
    if(child == 0)
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
            if(e.tagName() == "Device")
            retriveDeviceInfo(e);
        }
        n = n.nextSibling();
    }
    return child;
}

int DeviceList::retriveDeviceInfo(QDomElement device)
{
    int id = -1;
    int last_row;
    QString id_s = device.attribute("id");
    if(id_s == "")
    {
        return -1;
    }

    id = id_s.toInt();
    if(id >= MAX_DEVICE)
        return -1;

    qDebug()<<"id="<<id;

    connect(&this->device[id],SIGNAL(update_me(int)),this,SLOT(updateDevice(int)));
    connect(&this->device[id],SIGNAL(delete_me(int)),this,SLOT(deleteDevice(int)));
    connect(&this->device[id],SIGNAL(data_available(int,QByteArray)),this,SLOT(data_available(int,QByteArray)));


    last_row = this->valid_device.size();
    this->valid_device.push_back(id);
    this->device[id].dev_id = id;
    this->device[id].setTemp(device.attribute("Temp"));
    this->device[id].setHumidity(device.attribute("Humidity"));
    this->device[id].setIP(device.attribute("IP"));
    this->device[id].setState(device.attribute("State"));
    this->device[id].setSubnet(device.attribute("Subnet"));
    this->device[id].setHumidityMax(device.attribute("Max_Humidity"));
    this->device[id].setHumidityMin(device.attribute("Min_humidity"));
    this->device[id].setTempMax(device.attribute("Max_Temp"));
    this->device[id].setTempMin(device.attribute("Min_Temp"));
    AddDevice(this->device+id,last_row);

}

void DeviceList::xmlDomUpdate(int device_id)
{
    QDomDocument document;
    QFile file(FILE_NAME);
    bool exist= file.exists();
    qDebug()<<"exist while writting="<<exist;

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
        }
    }
    else
    {
        qDebug()<<"file doesn't exist ";
        root = document.createElement("Devices");
        document.appendChild(root);

        QDomElement newElement= document.createElement("Device");
#if 1
        newElement.setAttribute("id",device_id);
        newElement.setAttribute("IP",registerDialog->ip_Edit->text());
        newElement.setAttribute("Subnet",registerDialog->subnet_Edit->text());
        newElement.setAttribute("Location",registerDialog->location_Edit->text());
        newElement.setAttribute("Temp","0");
        newElement.setAttribute("Humidity","0");
        newElement.setAttribute("State","0");
#endif
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
    //qDebug()<<"N NODE=    "<<node.nodeName()<<"   P NODE="<<parentNode.nodeName();

    QDomElement newElement= document.createElement("Device");

    //root.appendChild(newElement);

    /*****************if only Parent node is present*************/
    if(node.isNull())
    {
#if 1
        qDebug()<<"root is null";
        newElement.setAttribute("id",device_id);
        newElement.setAttribute("IP",registerDialog->ip_Edit->text());
        newElement.setAttribute("Subnet",registerDialog->subnet_Edit->text());
        newElement.setAttribute("Location",registerDialog->location_Edit->text());
        newElement.setAttribute("Temp","0");
        newElement.setAttribute("Humidity","0");
        newElement.setAttribute("State","0");
#endif
        root.appendChild(newElement);
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
        return;
        #endif
    }

    bool devicePresent=false;
    bool ok=true;
#if 1
    QDomElement ele,ele_temp;
    while(!node.isNull())
    {
        if (node.isElement())
        {
            ele = node.toElement();

            if( ( ele.attribute("id").toInt(&ok,10) ) == device_id)
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
#if 1
        newElement.setAttribute("id",device_id);
        newElement.setAttribute("IP",registerDialog->ip_Edit->text());
        newElement.setAttribute("Subnet",registerDialog->subnet_Edit->text());
        newElement.setAttribute("Location",registerDialog->location_Edit->text());
        newElement.setAttribute("Temp","0");
        newElement.setAttribute("Humidity","0");
        newElement.setAttribute("State","0");

        parentNode.insertAfter(newElement,parentNode.lastChildElement());
        qDebug()<<"newElement inserted";
#endif
    }
    else
    {
        qDebug()<<"in else";
        newElement.setAttribute("id",device_id);
        ele_temp.setAttribute("IP",this->device[device_id].newIpAddress);
        ele_temp.setAttribute("Subnet",this->device[device_id].newSubnet);
//        ele_temp.setAttribute("Max_Temp",this->max_temp);
//        ele_temp.setAttribute("Min_Temp",this->min_temp);
//        ele_temp.setAttribute("Max_Humidity",this->max_humidity);
//        ele_temp.setAttribute("Min_Humidity",this->min_humidity);
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

void DeviceList::xmlDomDelete(int device_id)
{
    QDomDocument document;
    QFile file(FILE_NAME);
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

            //qDebug()<<"id="<< QString::number( ele.attribute("id"));
            bool ok = true;
            if( ( ele.attribute("id").toInt(&ok,10) ) == device_id)
            {
                parentNode.removeChild(ele);
                break;
            }
        }
        node = node.nextSibling();
    }
    qDebug() << "UPDATE" <<document.toString();

    file.close();

#if 1
    QFile file1(FILE_NAME);
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


void DeviceList::showDeviceList()
{
    qDebug()<<"in showDeviceList";
#if 1
    qDebug()<<"no. of devices";
    int i=0;
    while(i < 5)
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
        i++;
    }
#endif
    //flag_update=false;
}



void DeviceList::AddDevice(Device *dev,int rowCount)
{
    qDebug()<<"rowCount="<<rowCount;

    this->insertRow(rowCount);
    //this->showDeviceList();

    this->setItem(rowCount,0,new QTableWidgetItem(QString::number(dev->dev_id)) );
    this->setItem(rowCount,1,new QTableWidgetItem(dev->dev_ip) );
    this->setItem(rowCount,2,new QTableWidgetItem(dev->dev_location) );
    this->setItem(rowCount,3,new QTableWidgetItem(dev->dev_state) );

    this->setCellWidget(rowCount,colNum_getStatus,dev->btn_getStatus);
    qDebug()<<dev->dev_id;

    this->setCellWidget(rowCount,colNum_settings,dev->btn_settings);

    this->setCellWidget(rowCount,colNum_unRegister,dev->btn_unRegister);

    this->setCellWidget(rowCount,colNum_configure,dev->btn_configure);
    qDebug()<<"End";
}

void DeviceList::on_addDevice_accept()
{
    qDebug()<<"on_addDevice_accept";
    int id= 0;

    for(int i=0; i< MAX_DEVICE ;i++)
    {
        if((this->device[i].dev_id)== i)
            id++;
        else
        {
            id=i;
            break;
        }
    }
    qDebug()<<"id in accept"<<id;

    if( id < MAX_DEVICE )
    {
        connect(&this->device[id],SIGNAL(delete_me(int)),this,SLOT(deleteDevice(int)));
        connect(&this->device[id],SIGNAL(data_available(int,QByteArray)),this,SLOT(data_available(int,QByteArray)));
        connect(&this->device[id],SIGNAL(update_me(int)),this,SLOT(updateDevice(int)));

        //QList<int>::iterator position = std::find(this->valid_device.begin(), this->valid_device.end(), id);

        this->valid_device.push_back(id);
        this->device[id].initDevice(this,id);
        this->device[id].dev_id= id;
        this->device[id].dev_ip         = registerDialog->ip_Edit->text();
        this->device[id].dev_subnet     = registerDialog->subnet_Edit->text();
        this->device[id].dev_location   = registerDialog->location_Edit->text();
        xmlDomUpdate(id);
        AddDevice(&this->device[id],(this->rowCount()));
        //xmlDomReader();
    }
    else
    {
        QErrorMessage *err_msg= new QErrorMessage(this);
        err_msg->setWindowTitle("ERROR!");
        err_msg->showMessage(QString("SORRY! YOU CANNOT ADD MORE THAN %1 DEVICE ").arg(MAX_DEVICE));
        return;
    }
}

RegisterDialog::RegisterDialog(QWidget *parent): QDialog(parent)
{
    vbox            = new QVBoxLayout(this);
    ip_hbox         = new QHBoxLayout();
    subnet_hbox     = new QHBoxLayout();
    location_hbox   = new QHBoxLayout();
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(accepted()), this->parent(), SLOT(on_addDevice_accept()) );
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

void RegisterDialog::accept()
{

    this->done(QDialog::Accepted);
    this->close();
    this->hide();
}

void RegisterDialog::reject()
{
    //this->setResult(QDialog::Rejected);
    this->done(QDialog::Rejected);
    this->close();
    this->hide();
}

RegisterDialog::~RegisterDialog()
{
    delete vbox;
    delete ip_hbox;
    delete subnet_hbox;
    delete location_hbox;
}

void DeviceList::deviceListUpdate(QString deviceTag)
{
#if 0
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
        this->iterator1->setTemp(this->max_temp);
        this->iterator1->setTempMin(this->min_temp);
        this->iterator1->setHumidityMax(this->max_humidity);
        this->iterator1->setHumidityMin(this->min_humidity);
        deviceIndex++;
    }
    showDeviceList();
#endif
}

void DeviceList::deviceListDelete(QString deviceTag)
{
#if 0
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
#endif
}

