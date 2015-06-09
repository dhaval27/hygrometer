#include "device.h"
#include <qdebug.h>
#include <qobject.h>
#include <qhostaddress.h>
#include <qmessagebox.h>
#include <qerrormessage.h>
#include "devicelist.h"

Device::~Device()
{}

void Device::initDevice(QObject *parent, int device_id)
{
    this->dev_id = device_id;
    this->parent = parent;
    btn_getStatus= new QPushButton("Get Status",NULL);
    btn_settings= new QPushButton("Settings",NULL);
    btn_unRegister= new QPushButton("Remove",NULL);

    connect(btn_getStatus,SIGNAL(clicked()),this,SLOT(on_btn_getStatus_clicked()));
    connect(btn_settings,SIGNAL(clicked()),this,SLOT(on_btn_settings_clicked()));
    connect(btn_unRegister,SIGNAL(clicked()),this,SLOT(on_btn_remove_clicked()));
#ifdef HYGROMETER
    btn_configure= new QPushButton("Configure",NULL);
    connect(btn_configure,SIGNAL(clicked()),this,SLOT(on_btn_configure_clicked()));
#endif
}

/***********************************************
 "doConnect" is a fun(), where socket is created
 & connection is made with perticular host device
 **********************************************/
bool Device::doConnect(QString ipAddress)
{
    bool ret = false;

    if(socket) {
        if(socket->state() == QAbstractSocket::UnconnectedState) {
            socket->close();
            socket = NULL;
        } else if(socket->state() == QAbstractSocket::HostLookupState) {
            //ui->textEdit->setPlainText("socket state HostLookupState\n");
        } else if(socket->state() == QAbstractSocket::ConnectingState ) {
            //ui->textEdit->setPlainText("socket state Connecting\n");
        } else if(socket->state() == QAbstractSocket::ConnectedState ) {
            return true;    ///< if already connected it will return from here only
        } else if(socket->state() == QAbstractSocket::BoundState  ) {
            //ui->textEdit->setPlainText("socket state BoundState \n");
        } else if(socket->state() == QAbstractSocket::ClosingState  ) {
            //ui->textEdit->setPlainText("socket state ClosingState \n");
        }
    }
    qDebug()<<"in device doConnect..."<<ipAddress;
    socket = new QTcpSocket();
    socket->connectToHost(ipAddress, 5099); ///< connecting with the host device with this "ipAddress"

    ret = socket->waitForConnected(5000);   ///< waiting for 5000 milliseconds for device to connect
    if(ret == false)
      return false;

    socket->write("#TELL STATUS\n");

    connect(socket,SIGNAL(readyRead()),this,SLOT(readData()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(closeConnection()) );
    return true;

}


/******************************************
 ***************   SLOTS   ****************
 *****************************************/
void Device::readData()
{
    data= socket->readAll();
    QString host_address=socket->peerAddress().toString();
    qDebug()<<host_address;

    qDebug()<<"data="<<data;
    if(data.contains("###STATUS"))
    {
        emit data_available(this->dev_id,data); ///< signal is emmitted when "###STATUS" is received
    }
    if(data.contains("#IPSUBNET CHANGED"))
    {
        emit update_me(this->dev_id);   ///< signal is emmitted when "#IPSUBNET CHANGED" is received
    }
}

void Device::closeConnection()
{
    socket->close();
    socket = NULL;
}

void Device::on_btn_getStatus_clicked()
{
    qDebug()<<"btn_getStatus_clicked";
    bool ret=false;

    //this->dev_ip= "127.0.0.1";
    qDebug()<<this->dev_ip;
    ret = doConnect(this->dev_ip);  ///< call doConnect and passing device's IP
    if(ret==false)  ///< check for device is present in network or not
    {
        QErrorMessage *err_msg= new QErrorMessage();
        err_msg->setWindowTitle("ERROR!");
        err_msg->showMessage("SORRY! Device not found ");
        return;
    }
}

void Device::on_btn_settings_clicked()
{
    qDebug()<<"on_btn_settings_clicked";
    bool ret = false;

    //emit update_me(this->dev_id);
    netInfo = new IPSubnetDialogue(NULL);
    ret =  netInfo->exec(); ///< show "IPSubnetDialogue" dialogbox to give inputs

    if(ret == QDialog::Accepted)
    {
        this->newIpAddress= netInfo->get_ipaddress();
        this->newSubnet   = netInfo->get_subnet();

        if(socket!=NULL)
        {
            socket->write("SET YOUR\n");
            socket->write("#IP#");
            socket->write(this->newIpAddress.toStdString().c_str());
            socket->write("\n");
            socket->write("#SUBNET#");
            socket->write(this->newSubnet.toStdString().c_str());
            socket->write("\n");
        }
    }
}

void Device::on_btn_configure_clicked()
{
    qDebug()<<"on_btn_configure_clicked";
    config= new ConfigureDialog(NULL);

    bool ret= false;
    ret =  config->exec();

    if(ret == QDialog::Accepted){
        this->dev_temp_max= config->get_max_temp();
        this->dev_temp_min= config->get_min_temp();
        this->dev_humidity_max= config->get_max_humidity();
        this->dev_humidity_min= config->get_min_humidity();
        //this->xmlDomUpdate(dev_id);

        if(socket!=NULL)
        {
            socket->write("#SET YOUR DEVICE TO\n");
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
}

void Device::on_btn_remove_clicked()
{

    qDebug()<<"btn_remove_clicked";
    qDebug()<<"id"<<this->dev_id;

    QMessageBox::StandardButton msgBox= QMessageBox::question(NULL,"Warning !","Do you really want to delete this device?",QMessageBox::Yes|QMessageBox::No);
    if(msgBox==QMessageBox::Yes)
    {
        emit delete_me(this->dev_id);
    }
    else
    {
        qDebug()<<"Did not deleted";
    }
}

/*************************************************
 ***************    DIALOG BOXES  ****************
 ************************************************/

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
    delete ipLabel;
    delete subnetLabel;
    delete ipEdit;
    delete subnetEdit;
    delete buttonBox;
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


ConfigureDialog::~ConfigureDialog()
{
    delete vbox;
    delete max_temp_hbox;
    delete min_temp_hbox;
    delete max_humidity_hbox;
    delete min_humidity_hbox;
    delete max_temp_Label;
    delete min_temp_Label;
    delete max_humidity_Label;
    delete min_humidity_Label;
    delete max_temp_Edit;
    delete min_temp_Edit;
    delete max_humidity_Edit;
    delete min_humidity_Edit;
    delete buttonBox;

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
