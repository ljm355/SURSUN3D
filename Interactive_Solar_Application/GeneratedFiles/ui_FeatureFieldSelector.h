/********************************************************************************
** Form generated from reading UI file 'FeatureFieldSelector.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FEATUREFIELDSELECTOR_H
#define UI_FEATUREFIELDSELECTOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FeatureFieldSelector
{
public:
    QWidget *horizontalLayoutWidget_23;
    QHBoxLayout *horizontalLayout_25;
    QLabel *label_22;
    QComboBox *comboBox_Field;
    QPushButton *pushButton_OK;
    QPushButton *pushButton_Cancel;

    void setupUi(QDialog *FeatureFieldSelector)
    {
        if (FeatureFieldSelector->objectName().isEmpty())
            FeatureFieldSelector->setObjectName(QString::fromUtf8("FeatureFieldSelector"));
        FeatureFieldSelector->resize(400, 75);
        FeatureFieldSelector->setMinimumSize(QSize(400, 75));
        FeatureFieldSelector->setMaximumSize(QSize(400, 75));
        horizontalLayoutWidget_23 = new QWidget(FeatureFieldSelector);
        horizontalLayoutWidget_23->setObjectName(QString::fromUtf8("horizontalLayoutWidget_23"));
        horizontalLayoutWidget_23->setGeometry(QRect(0, 10, 401, 25));
        horizontalLayout_25 = new QHBoxLayout(horizontalLayoutWidget_23);
        horizontalLayout_25->setSpacing(6);
        horizontalLayout_25->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_25->setObjectName(QString::fromUtf8("horizontalLayout_25"));
        horizontalLayout_25->setContentsMargins(0, 0, 0, 0);
        label_22 = new QLabel(horizontalLayoutWidget_23);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_22->sizePolicy().hasHeightForWidth());
        label_22->setSizePolicy(sizePolicy);

        horizontalLayout_25->addWidget(label_22);

        comboBox_Field = new QComboBox(horizontalLayoutWidget_23);
        comboBox_Field->setObjectName(QString::fromUtf8("comboBox_Field"));

        horizontalLayout_25->addWidget(comboBox_Field);

        pushButton_OK = new QPushButton(FeatureFieldSelector);
        pushButton_OK->setObjectName(QString::fromUtf8("pushButton_OK"));
        pushButton_OK->setGeometry(QRect(90, 40, 75, 23));
        pushButton_Cancel = new QPushButton(FeatureFieldSelector);
        pushButton_Cancel->setObjectName(QString::fromUtf8("pushButton_Cancel"));
        pushButton_Cancel->setGeometry(QRect(210, 40, 75, 23));

        retranslateUi(FeatureFieldSelector);
        QObject::connect(pushButton_OK, SIGNAL(clicked()), FeatureFieldSelector, SLOT(accept()));
        QObject::connect(pushButton_Cancel, SIGNAL(clicked()), FeatureFieldSelector, SLOT(reject()));

        QMetaObject::connectSlotsByName(FeatureFieldSelector);
    } // setupUi

    void retranslateUi(QDialog *FeatureFieldSelector)
    {
        FeatureFieldSelector->setWindowTitle(QApplication::translate("FeatureFieldSelector", "Select height field for extrusion", 0, QApplication::UnicodeUTF8));
        label_22->setText(QApplication::translate("FeatureFieldSelector", "Field", 0, QApplication::UnicodeUTF8));
        pushButton_OK->setText(QApplication::translate("FeatureFieldSelector", "Confirm", 0, QApplication::UnicodeUTF8));
        pushButton_Cancel->setText(QApplication::translate("FeatureFieldSelector", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FeatureFieldSelector: public Ui_FeatureFieldSelector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FEATUREFIELDSELECTOR_H
