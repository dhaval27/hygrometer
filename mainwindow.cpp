#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAbstractSocket>
#include <iostream>
#include "devicelist.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);

    ui->devicelist->show();
#ifdef HYGROMETER
    this->setWindowTitle("Humidity Temperature Sensor");
#else
    this->setWindowTitle("Device Status");
#endif

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
