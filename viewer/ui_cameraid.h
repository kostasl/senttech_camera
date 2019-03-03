/********************************************************************************
** Form generated from reading UI file 'cameraid.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERAID_H
#define UI_CAMERAID_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_cameraid
{
public:
    QLineEdit *edit_id;
    QLineEdit *edit_name;
    QLabel *label_id;
    QLabel *label_name;
    QPushButton *read;
    QPushButton *write;

    void setupUi(QWidget *cameraid)
    {
        if (cameraid->objectName().isEmpty())
            cameraid->setObjectName(QStringLiteral("cameraid"));
        cameraid->resize(447, 165);
        edit_id = new QLineEdit(cameraid);
        edit_id->setObjectName(QStringLiteral("edit_id"));
        edit_id->setGeometry(QRect(120, 20, 113, 27));
        edit_name = new QLineEdit(cameraid);
        edit_name->setObjectName(QStringLiteral("edit_name"));
        edit_name->setGeometry(QRect(120, 70, 301, 27));
        label_id = new QLabel(cameraid);
        label_id->setObjectName(QStringLiteral("label_id"));
        label_id->setGeometry(QRect(10, 20, 67, 17));
        label_name = new QLabel(cameraid);
        label_name->setObjectName(QStringLiteral("label_name"));
        label_name->setGeometry(QRect(10, 70, 101, 17));
        read = new QPushButton(cameraid);
        read->setObjectName(QStringLiteral("read"));
        read->setGeometry(QRect(210, 120, 99, 27));
        write = new QPushButton(cameraid);
        write->setObjectName(QStringLiteral("write"));
        write->setGeometry(QRect(320, 120, 99, 27));

        retranslateUi(cameraid);

        QMetaObject::connectSlotsByName(cameraid);
    } // setupUi

    void retranslateUi(QWidget *cameraid)
    {
        cameraid->setWindowTitle(QApplication::translate("cameraid", "Camera ID", Q_NULLPTR));
        label_id->setText(QApplication::translate("cameraid", "camera id", Q_NULLPTR));
        label_name->setText(QApplication::translate("cameraid", "camera name", Q_NULLPTR));
        read->setText(QApplication::translate("cameraid", "read", Q_NULLPTR));
        write->setText(QApplication::translate("cameraid", "write", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class cameraid: public Ui_cameraid {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERAID_H
