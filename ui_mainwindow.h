/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.1
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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>
#include "devicelist.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    DeviceList *devicelist;
    QTextEdit *textEdit_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *add_device;
    QLabel *label_2;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(769, 469);
        QFont font;
        font.setPointSize(10);
        MainWindow->setFont(font);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        devicelist = new DeviceList(centralWidget);
        if (devicelist->columnCount() < 3)
            devicelist->setColumnCount(3);
        devicelist->setObjectName(QStringLiteral("devicelist"));
        QFont font1;
        font1.setPointSize(8);
        devicelist->setFont(font1);
        devicelist->setMouseTracking(true);
        devicelist->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        devicelist->setRowCount(0);
        devicelist->setColumnCount(3);
        devicelist->horizontalHeader()->setCascadingSectionResizes(false);
        devicelist->horizontalHeader()->setDefaultSectionSize(75);
        devicelist->horizontalHeader()->setHighlightSections(false);
        devicelist->horizontalHeader()->setMinimumSectionSize(37);
        devicelist->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        devicelist->horizontalHeader()->setStretchLastSection(false);
        devicelist->verticalHeader()->setVisible(false);

        gridLayout->addWidget(devicelist, 1, 0, 1, 2);

        textEdit_2 = new QTextEdit(centralWidget);
        textEdit_2->setObjectName(QStringLiteral("textEdit_2"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(textEdit_2->sizePolicy().hasHeightForWidth());
        textEdit_2->setSizePolicy(sizePolicy);
        textEdit_2->setMaximumSize(QSize(16777215, 100));

        gridLayout->addWidget(textEdit_2, 3, 0, 1, 2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        add_device = new QPushButton(centralWidget);
        add_device->setObjectName(QStringLiteral("add_device"));
        add_device->setToolTipDuration(1);

        horizontalLayout->addWidget(add_device);


        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        QFont font2;
        font2.setPointSize(14);
        label_2->setFont(font2);

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QString());
#ifndef QT_NO_TOOLTIP
        add_device->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Click to Add New Device</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
        add_device->setText(QApplication::translate("MainWindow", "Add Device", 0));
        label_2->setText(QApplication::translate("MainWindow", "Activity", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
