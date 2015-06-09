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

    ui->devicelist->show();

    connect(ui->devicelist,SIGNAL(activity(QString)),this,SLOT(activity(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_add_device_clicked()
{
    if(ui->devicelist->rowCount()<MAX_DEVICE)
    {
        ui->devicelist->registerDialog= new RegisterDialog(ui->devicelist);
        ui->devicelist->registerDialog->exec();
    }
    else
    {
        QErrorMessage *errorMessage= new QErrorMessage(this);
        {
            errorMessage->setWindowTitle("ERROR!");
            errorMessage->showMessage(QString("SORRY! YOU CANNOT ADD MORE THAN %1 DEVICE ").arg(MAX_DEVICE));
        }
    }
}

void MainWindow::activity(QString activity_text)
{
    ui->textEdit_2->append(activity_text);
}
