/********************************************************************************
** Form generated from reading UI file 'capturewindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAPTUREWINDOW_H
#define UI_CAPTUREWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CaptureWindow
{
public:

    void setupUi(QWidget *CaptureWindow)
    {
        if (CaptureWindow->objectName().isEmpty())
            CaptureWindow->setObjectName(QStringLiteral("CaptureWindow"));
        CaptureWindow->resize(640, 480);

        retranslateUi(CaptureWindow);

        QMetaObject::connectSlotsByName(CaptureWindow);
    } // setupUi

    void retranslateUi(QWidget *CaptureWindow)
    {
        CaptureWindow->setWindowTitle(QApplication::translate("CaptureWindow", "Capture", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class CaptureWindow: public Ui_CaptureWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAPTUREWINDOW_H
