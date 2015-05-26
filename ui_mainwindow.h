/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <mainwindow.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTextEdit *textEdit;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLineEdit *temprature;
    QLabel *label_7;
    QLabel *label_8;
    QLineEdit *humidity;
    QLineEdit *state;
    QLabel *label_9;
    QLabel *label_3;
    IPLineEdit *ip_address;
    QLabel *label_6;
    QLabel *label_4;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *getDeviceStatus;
    QPushButton *Setting;
    QPushButton *Register;
    QLabel *label_2;
    QTextEdit *textEdit_2;
    QTableWidget *tableWidget;
    QPushButton *Unregister;
    QPushButton *add_device;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(758, 465);
        QFont font;
        font.setPointSize(10);
        MainWindow->setFont(font);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        textEdit = new QTextEdit(centralWidget);
        textEdit->setObjectName(QStringLiteral("textEdit"));
        textEdit->setGeometry(QRect(10, 230, 691, 31));
        gridLayoutWidget = new QWidget(centralWidget);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 270, 521, 91));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        temprature = new QLineEdit(gridLayoutWidget);
        temprature->setObjectName(QStringLiteral("temprature"));

        gridLayout->addWidget(temprature, 1, 2, 1, 1);

        label_7 = new QLabel(gridLayoutWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy);
        label_7->setMaximumSize(QSize(16777215, 30));

        gridLayout->addWidget(label_7, 0, 2, 1, 1);

        label_8 = new QLabel(gridLayoutWidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        sizePolicy.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy);
        label_8->setMaximumSize(QSize(16777215, 30));

        gridLayout->addWidget(label_8, 0, 3, 1, 1);

        humidity = new QLineEdit(gridLayoutWidget);
        humidity->setObjectName(QStringLiteral("humidity"));

        gridLayout->addWidget(humidity, 1, 3, 1, 1);

        state = new QLineEdit(gridLayoutWidget);
        state->setObjectName(QStringLiteral("state"));

        gridLayout->addWidget(state, 1, 4, 1, 1);

        label_9 = new QLabel(gridLayoutWidget);
        label_9->setObjectName(QStringLiteral("label_9"));
        sizePolicy.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy);
        label_9->setMaximumSize(QSize(16777215, 30));

        gridLayout->addWidget(label_9, 0, 4, 1, 1);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 1, 0, 1, 1);

        ip_address = new IPLineEdit(gridLayoutWidget);
        ip_address->setObjectName(QStringLiteral("ip_address"));

        gridLayout->addWidget(ip_address, 1, 1, 1, 1);

        label_6 = new QLabel(gridLayoutWidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        sizePolicy.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy);
        label_6->setMaximumSize(QSize(16777215, 30));

        gridLayout->addWidget(label_6, 0, 1, 1, 1);

        label_4 = new QLabel(gridLayoutWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);
        label_4->setMaximumSize(QSize(16777215, 30));
        label_4->setFont(font);

        gridLayout->addWidget(label_4, 0, 0, 1, 1);

        pushButton = new QPushButton(gridLayoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout->addWidget(pushButton, 0, 5, 1, 1);

        pushButton_2 = new QPushButton(gridLayoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        gridLayout->addWidget(pushButton_2, 0, 6, 1, 1);

        getDeviceStatus = new QPushButton(gridLayoutWidget);
        getDeviceStatus->setObjectName(QStringLiteral("getDeviceStatus"));
        QFont font1;
        font1.setPointSize(8);
        getDeviceStatus->setFont(font1);

        gridLayout->addWidget(getDeviceStatus, 1, 5, 1, 1);

        Setting = new QPushButton(gridLayoutWidget);
        Setting->setObjectName(QStringLiteral("Setting"));
        Setting->setFont(font1);

        gridLayout->addWidget(Setting, 1, 6, 1, 1);

        Register = new QPushButton(centralWidget);
        Register->setObjectName(QStringLiteral("Register"));
        Register->setGeometry(QRect(406, 10, 71, 31));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(540, 270, 81, 31));
        QFont font2;
        font2.setPointSize(14);
        label_2->setFont(font2);
        textEdit_2 = new QTextEdit(centralWidget);
        textEdit_2->setObjectName(QStringLiteral("textEdit_2"));
        textEdit_2->setGeometry(QRect(540, 300, 211, 131));
        tableWidget = new QTableWidget(centralWidget);
        if (tableWidget->columnCount() < 9)
            tableWidget->setColumnCount(9);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setGeometry(QRect(10, 50, 721, 161));
        tableWidget->setFont(font1);
        tableWidget->setMouseTracking(true);
        tableWidget->setRowCount(0);
        tableWidget->setColumnCount(9);
        tableWidget->horizontalHeader()->setCascadingSectionResizes(false);
        tableWidget->horizontalHeader()->setDefaultSectionSize(75);
        tableWidget->horizontalHeader()->setMinimumSectionSize(37);
        Unregister = new QPushButton(centralWidget);
        Unregister->setObjectName(QStringLiteral("Unregister"));
        Unregister->setGeometry(QRect(480, 10, 75, 31));
        add_device = new QPushButton(centralWidget);
        add_device->setObjectName(QStringLiteral("add_device"));
        add_device->setGeometry(QRect(330, 10, 71, 31));
        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        textEdit->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Ubuntu'; font-size:11pt;\">Humidity Temprature Sensor</span></p></body></html>", 0));
        label_7->setText(QApplication::translate("MainWindow", "Temperature", 0));
        label_8->setText(QApplication::translate("MainWindow", "Humidity", 0));
        label_9->setText(QApplication::translate("MainWindow", "State(ON/OFF)", 0));
        label_3->setText(QApplication::translate("MainWindow", "Dev 1", 0));
        ip_address->setText(QString());
        label_6->setText(QApplication::translate("MainWindow", "IP Address ", 0));
        label_4->setText(QApplication::translate("MainWindow", "Device", 0));
        pushButton->setText(QApplication::translate("MainWindow", "PushButton", 0));
        pushButton_2->setText(QApplication::translate("MainWindow", "PushButton", 0));
        getDeviceStatus->setText(QApplication::translate("MainWindow", "Get Status", 0));
        Setting->setText(QApplication::translate("MainWindow", "Setting", 0));
        Register->setText(QApplication::translate("MainWindow", "Register", 0));
        label_2->setText(QApplication::translate("MainWindow", "Activity", 0));
        Unregister->setText(QApplication::translate("MainWindow", "Unregister", 0));
        add_device->setText(QApplication::translate("MainWindow", "Add Device", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
