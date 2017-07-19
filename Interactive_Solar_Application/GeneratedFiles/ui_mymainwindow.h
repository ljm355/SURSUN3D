/********************************************************************************
** Form generated from reading UI file 'mymainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYMAINWINDOW_H
#define UI_MYMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MYMainWindow
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MYMainWindow)
    {
        if (MYMainWindow->objectName().isEmpty())
            MYMainWindow->setObjectName(QString::fromUtf8("MYMainWindow"));
        MYMainWindow->resize(400, 300);
        menuBar = new QMenuBar(MYMainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        MYMainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MYMainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MYMainWindow->addToolBar(mainToolBar);
        centralWidget = new QWidget(MYMainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        MYMainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MYMainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MYMainWindow->setStatusBar(statusBar);

        retranslateUi(MYMainWindow);

        QMetaObject::connectSlotsByName(MYMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MYMainWindow)
    {
        MYMainWindow->setWindowTitle(QApplication::translate("MYMainWindow", "MYMainWindow", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MYMainWindow: public Ui_MYMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYMAINWINDOW_H
