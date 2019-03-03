/********************************************************************************
** Form generated from reading UI file 'settingfile.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGFILE_H
#define UI_SETTINGFILE_H

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

class Ui_settingfile
{
public:
    QLineEdit *edit_filepath;
    QLabel *label_filepath;
    QPushButton *read;
    QPushButton *write;

    void setupUi(QWidget *settingfile)
    {
        if (settingfile->objectName().isEmpty())
            settingfile->setObjectName(QStringLiteral("settingfile"));
        settingfile->resize(394, 113);
        edit_filepath = new QLineEdit(settingfile);
        edit_filepath->setObjectName(QStringLiteral("edit_filepath"));
        edit_filepath->setGeometry(QRect(80, 20, 291, 27));
        label_filepath = new QLabel(settingfile);
        label_filepath->setObjectName(QStringLiteral("label_filepath"));
        label_filepath->setGeometry(QRect(10, 20, 67, 17));
        read = new QPushButton(settingfile);
        read->setObjectName(QStringLiteral("read"));
        read->setGeometry(QRect(160, 60, 99, 27));
        write = new QPushButton(settingfile);
        write->setObjectName(QStringLiteral("write"));
        write->setGeometry(QRect(270, 60, 99, 27));

        retranslateUi(settingfile);

        QMetaObject::connectSlotsByName(settingfile);
    } // setupUi

    void retranslateUi(QWidget *settingfile)
    {
        settingfile->setWindowTitle(QApplication::translate("settingfile", "Form", Q_NULLPTR));
        label_filepath->setText(QApplication::translate("settingfile", "file path", Q_NULLPTR));
        read->setText(QApplication::translate("settingfile", "read", Q_NULLPTR));
        write->setText(QApplication::translate("settingfile", "write", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class settingfile: public Ui_settingfile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGFILE_H
