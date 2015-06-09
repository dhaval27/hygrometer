#include <devicelist.h>
#include <qfile.h>
#include <qmessagebox.h>
#include <qheaderview.h>
#include <qerrormessage.h>
#include <qdatetime.h>

DeviceList::DeviceList(QWidget *parent)
{
    for( int i = 0; i < MAX_DEVICE; i++) {
        this->device[i].initDevice(this,-1);    ///< initialising all the devices
        this->device[i].socket=NULL;
    }
}

DeviceList::~DeviceList()
{}

void DeviceList::show(void)
{
#ifdef HYGROMETER
        colNum_id       =0;
        colNum_ip       =1;
        colNum_location =2;
        colNum_state    =3;
        colNum_temp     =4;
        colNum_humidity =5;
        colNum_getStatus=6;
        colNum_settings =7;
        colNum_configure=8;
        colNum_unRegister=9;
        this->setColumnCount(10);
        this->setHorizontalHeaderLabels(QString("DEVICE;IP ADDRESS;LOCATION;STATE;TEMP;HUMIDITY;STATUS;;;REMOVE").split(";"));

#else
        colNum_id       =0;
        colNum_ip       =1;
        colNum_location =2;
        colNum_state    =3;
        colNum_getStatus=4;
        colNum_settings =5;
        colNum_unRegister=6;
        this->setColumnCount(7);

        this->setHorizontalHeaderLabels(QString("DEVICE;IP ADDRESS;LOCATION;STATE;STATUS; ;REMOVE").split(";"));
#endif

    QFont *font = new QFont("Helvetica",10,8,false);
    font->setBold(true);
    QHeaderView *header= this->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch); ///< resizes headers to the window size

    header->setFont(*font); ///< setting header font

    ///< setting tooltips for "STATUS" & "REMOVE" headers
    this->horizontalHeaderItem(colNum_getStatus)->setToolTip("Click to Get All Devices Status");
    this->horizontalHeaderItem(colNum_unRegister)->setToolTip("Click to Remove All Devices");

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);   ///< making all cell only readable

    connect(header,SIGNAL(sectionClicked(int)),this,SLOT(on_header_clicked(int)));
    deviceCount = this->xmlDomReader(); ///< reading the devices from the file which are already present
}

void DeviceList::AddDevice(Device *dev,int rowCount)
{
    qDebug()<<"rowCount="<<rowCount;

    this->insertRow(rowCount);
    //this->showDeviceList();

    this->setItem(rowCount,colNum_id,new QTableWidgetItem(QString::number(dev->dev_id)) );
    this->setItem(rowCount,colNum_ip,new QTableWidgetItem(dev->dev_ip) );
    this->setItem(rowCount,colNum_location,new QTableWidgetItem(dev->dev_location) );
    this->setItem(rowCount,colNum_state,new QTableWidgetItem(dev->dev_state) );

    this->setCellWidget(rowCount,colNum_getStatus,dev->btn_getStatus);
    this->setCellWidget(rowCount,colNum_settings,dev->btn_settings);
    this->setCellWidget(rowCount,colNum_unRegister,dev->btn_unRegister);

#ifdef HYGROMETER
    this->setItem(rowCount,colNum_temp,new QTableWidgetItem(dev->dev_temp) );
    this->setItem(rowCount,colNum_humidity,new QTableWidgetItem(dev->dev_humidity) );
    this->setCellWidget(rowCount,colNum_configure,dev->btn_configure);
#endif
    //qDebug()<<"End";
}



/********************************************
 ************   XMLDOM FUNS *****************
 *******************************************/

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

    ///< traversing in .xml document node by node
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

void DeviceList::retriveDeviceInfo(QDomElement device)
{
    int id = -1;
    int last_row;
    QString id_s = device.attribute("id");
    if(id_s == "")
    {
        return ;
    }

    id = id_s.toInt();
    if(id >= MAX_DEVICE)
        return ;

    qDebug()<<"id="<<id;

    connect(&this->device[id],SIGNAL(update_me(int)),this,SLOT(updateDevice(int)));
    connect(&this->device[id],SIGNAL(delete_me(int)),this,SLOT(deleteDevice(int)));
    connect(&this->device[id],SIGNAL(data_available(int,QByteArray)),this,SLOT(data_available(int,QByteArray)));

    last_row = this->valid_device.size();
    this->valid_device.push_back(id);   ///< adding device in List<>

    ///< defining the attributes of the device
    this->device[id].dev_id = id;
    this->device[id].setIP(device.attribute("IP"));
    this->device[id].setState(device.attribute("State"));
    this->device[id].setSubnet(device.attribute("Subnet"));
#ifdef HYGROMETER
    this->device[id].setTemp(device.attribute("Temp"));
    this->device[id].setHumidity(device.attribute("Humidity"));
    this->device[id].setHumidityMax(device.attribute("Max_Humidity"));
    this->device[id].setHumidityMin(device.attribute("Min_humidity"));
    this->device[id].setTempMax(device.attribute("Max_Temp"));
    this->device[id].setTempMin(device.attribute("Min_Temp"));
#endif
    AddDevice(this->device+id,last_row);    ///< adding the device in GUI
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
    if(exist)   ///< if file was already present
    {
        //if("Devices"==document.documentElement().tagName())
        {
            qDebug()<<"file exist n "<<document.documentElement().tagName();
            root = document.firstChildElement();
        }
    }
    else    ///< file does not exist previously
    {
        qDebug()<<"file doesn't exist ";
        root = document.createElement("Devices");   ///< adding root to the document
        document.appendChild(root);

        QDomElement newElement= document.createElement("Device");
        newElement.setAttribute("id",device_id);
        newElement.setAttribute("IP",registerDialog->ip_Edit->text());
        newElement.setAttribute("Subnet",registerDialog->subnet_Edit->text());
        newElement.setAttribute("Location",registerDialog->location_Edit->text());
        newElement.setAttribute("State","0");
#ifdef HYGROMETER
        newElement.setAttribute("Temp","0");
        newElement.setAttribute("Humidity","0");
#endif
        root.appendChild(newElement);   ///< creating the child element of root

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
    QDomElement newElement= document.createElement("Device");

    if(node.isNull())   ///< if only Parent node is present with no child
    {
        qDebug()<<"root is null";
        newElement.setAttribute("id",device_id);
        newElement.setAttribute("IP",registerDialog->ip_Edit->text());
        newElement.setAttribute("Subnet",registerDialog->subnet_Edit->text());
        newElement.setAttribute("Location",registerDialog->location_Edit->text());
        newElement.setAttribute("State","0");
#ifdef HYGROMETER
        newElement.setAttribute("Temp","0");
        newElement.setAttribute("Humidity","0");
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

            if( ( ele.attribute("id").toInt(&ok,10) ) == device_id) ///< checking that node is already present or not
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

    if(!devicePresent)  ///< if device not present already
    {
        newElement.setAttribute("id",device_id);
        newElement.setAttribute("IP",registerDialog->ip_Edit->text());
        newElement.setAttribute("Subnet",registerDialog->subnet_Edit->text());
        newElement.setAttribute("Location",registerDialog->location_Edit->text());
        newElement.setAttribute("State","0");
#ifdef HYGROMETER
        newElement.setAttribute("Temp","0");
        newElement.setAttribute("Humidity","0");
#endif
        parentNode.insertAfter(newElement,parentNode.lastChildElement());
        qDebug()<<"newElement inserted";
    }
    else    ///< if device is present already(when settings clicked & got the signal)
    {
        qDebug()<<"in else";
        newElement.setAttribute("id",device_id);
        ele_temp.setAttribute("IP",this->device[device_id].newIpAddress);
        ele_temp.setAttribute("Subnet",this->device[device_id].newSubnet);
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
    qDebug()<<"no. of devices="<<valid_device.size();
    int i=0;
    while(i < 5)
    {
        qDebug()<<"device           ="<<(this->device[i].dev_id);
        qDebug()<<"device_ip        ="<<(this->device[i].dev_ip);
        qDebug()<<"device_subnet    ="<<(this->device[i].dev_subnet);
        qDebug()<<"device_location  ="<<(this->device[i].dev_location);
        qDebug()<<"device_state     ="<<(this->device[i].dev_state);
#ifdef HYGROMETER
        qDebug()<<"device_temp      ="<<(this->device[i].dev_temp);
        qDebug()<<"device_humidity  ="<<(this->device[i].dev_humidity);
        qDebug()<<"dev_temp_max     ="<<(this->device[i].dev_temp_max);
        qDebug()<<"dev_temp_min     ="<<(this->device[i].dev_temp_min);
        qDebug()<<"dev_humidity_max ="<<(this->device[i].dev_humidity_max);
        qDebug()<<"dev_humidity_max ="<<(this->device[i].dev_humidity_max);
#endif
        qDebug()<<"********";
        i++;
    }
}

/****************************************
 ***********    SLOTS   *****************
 ***************************************/
void DeviceList::on_addDevice_accept()
{
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

        valid_device.push_back(id);
        qDebug()<<"valid size="<<valid_device.size();
        this->device[id].initDevice(this,id);
        this->device[id].dev_id= id;
        this->device[id].dev_ip         = registerDialog->ip_Edit->text();
        this->device[id].dev_subnet     = registerDialog->subnet_Edit->text();
        this->device[id].dev_location   = registerDialog->location_Edit->text();

        connect(&this->device[id],SIGNAL(delete_me(int)),this,SLOT(deleteDevice(int)));
        connect(&this->device[id],SIGNAL(data_available(int,QByteArray)),this,SLOT(data_available(int,QByteArray)));
        connect(&this->device[id],SIGNAL(update_me(int)),this,SLOT(updateDevice(int)));

        xmlDomUpdate(id);   ///< updating in xml file
        AddDevice(&this->device[id],(this->rowCount()));
        //xmlDomReader();
    }
}

void DeviceList::deleteDevice(int device_id)
{
    qDebug()<<"in deleteDevice"<<device_id;

    ///< disconnecting signals
    disconnect(&this->device[device_id],SIGNAL(delete_me(int)),this,SLOT(deleteDevice(int)));
    disconnect(&this->device[device_id],SIGNAL(data_available(int,QByteArray)),this,SLOT(data_available(int,QByteArray)));
    disconnect(&this->device[device_id],SIGNAL(update_me(int)),this,SLOT(updateDevice(int)));

    for(QList<int>::iterator p= valid_device.begin(); p!= valid_device.end(); p++)
    {
        qDebug()<<"valid device="<<*p;
    }

    if( (device_id >= 0) && (device_id < MAX_DEVICE) )
    {
        qDebug()<<"dev_id="<<this->device[device_id].dev_id;

        QList<int>::iterator position = std::find(this->valid_device.begin(), this->valid_device.end(), device_id); ///< finding device position in List

        qDebug()<<std::distance(this->valid_device.begin(),position);   ///< calculating distance in List
        this->removeRow(std::distance(this->valid_device.begin(),position));    ///< removing from GUI
        //this->device[device_id].dev_id = -1;
        this->device[device_id].dev_id= -1; ///< making device ID to -1.

        this->valid_device.erase(position); ///< removing device from the List
        qDebug()<<"size="<<valid_device.size();

        xmlDomDelete(device_id);    ///< removing device from .xml document
    }
    else
    {
        QErrorMessage *err_msg= new QErrorMessage(this);
        err_msg->setWindowTitle("ERROR!");
        err_msg->showMessage("SORRY! device id is not valid");
        return;
    }
}

void DeviceList::updateDevice(int device_id)
{
    ///< when msg is recevied from host device
    this->device[device_id].dev_ip      = this->device[device_id].newIpAddress;
    this->device[device_id].dev_subnet  = this->device[device_id].newSubnet;
    xmlDomUpdate(device_id);
}

///< when msg is recevied from host device
void DeviceList::data_available(int device_id,QByteArray data)
{
    qDebug()<<"data in getStatus="<<data;
    status= data;
    qDebug()<<"status="<<status;
    status= status.remove('\n');
    qDebug()<<"status after remove \n="<<status;

    this->device[device_id].dev_earlierState= state;

    state= status.mid(status.lastIndexOf("#")+1);
    qDebug()<<"state="<<state;

    //qDebug()<<host_address;
    if((state != (this->device[device_id].dev_earlierState)))   ///< checking if device's State is changed or not
    {
        activity_text= "";
        qDebug()<<"activity_text="<<activity_text;
        activity_text.append(QString("Device %1 changed its status.\n").arg(device_id));
        activity_text.append(QTime::currentTime().toString("HH:mm:ss"));
        activity_text.append(", ");
        activity_text.append(QDate::currentDate().toString("dd MMM"));
        activity_text.append("\n");
        emit activity(activity_text);   ///< signal emitted to update in GUI activity text
    }

#ifdef HYGROMETER
    status= status.remove(status.indexOf("#STATE"),status.size());
    qDebug()<<"status after removal of #STATE="<<status;

    humidity= status.mid(status.lastIndexOf("#")+1);
    qDebug()<<"humdity="<<humidity;

    status= status.remove(status.indexOf("#HUMIDITY"),status.size());
    qDebug()<<"status after removal of #HUMIDITY="<<status;

    temp= status.mid(status.lastIndexOf("#")+1);
    qDebug()<<"temp="<<temp;
#endif
    //status= status.remove(status.indexOf("#TEMP"),status.size());
    //qDebug()<<"status after removal of #STATE="<<status;

    int row_status= 0;
    int i;
    for(i=0; i< this->rowCount(); i++)
    {
        if( ( (this->item(i,colNum_id)->text() ) ) == QString::number(device_id) )
            row_status= i;
    }

    this->setItem(row_status,colNum_state,new QTableWidgetItem(state));
#ifdef HYGROMETER
    this->setItem(row_status,colNum_temp,new QTableWidgetItem(temp));
    this->setItem(row_status,colNum_humidity,new QTableWidgetItem(humidity));

    if(temp > (this->device[device_id].dev_temp_max))
        this->device[device_id].socket->write("#TEMP IS HIGH");
    else if(temp < (this->device[device_id].dev_temp_min))
        this->device[device_id].socket->write("#TEMP IS LOW");
    if(humidity > (this->device[device_id].dev_humidity_max))
        this->device[device_id].socket->write(" #HUMIDITY IS HIGH\n");
    else if(humidity > (this->device[device_id].dev_humidity_min))
        this->device[device_id].socket->write(" #HUMIDITY IS LOW\n");

#endif

}

void DeviceList::on_header_clicked(int index)
{
    qDebug()<<"on_header "<<index<<"clicked";
    if(index==colNum_getStatus) ///< checking if "STATUS" header is clicked or not
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
    if(index==colNum_unRegister)    ///< checking if "REMOVE" header is clicked or not
    {
        QMessageBox::StandardButton msgBox= QMessageBox::question(NULL,"Warning !","Do you really want to delete ALL devices?",QMessageBox::Yes|QMessageBox::No);
        if(msgBox==QMessageBox::Yes)
        {
            int rowCount= this->rowCount();

            for(int i=0,j=0; i < rowCount ; i++)
            {
                this->removeRow(j);
            }
        }
        else
        {
            qDebug()<<"Did not deleted";
        }
    }

}

/****************************************
 ********  REGISTER DIALOG  *************
 ***************************************/

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
    delete ip_Edit;
    delete subnet_Edit;
    delete location_Edit;
    delete ip_Label;
    delete subnet_Label;
    delete location_Label;
    delete buttonBox;
}
