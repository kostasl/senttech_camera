#include "settingfile.h"
#include "ui_settingfile.h"

#include <stdio.h>
#include <sentech/stcam_lib.h>
#include "QDir"

settingfile::settingfile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::settingfile),
    libHandle(NULL)
{
    ui->setupUi(this);
}

settingfile::~settingfile()
{
    delete ui;
}

void settingfile::Init(void* lib)
{
    libHandle = lib;
    ui->edit_filepath->setText(QDir::currentPath());
    show();
}

void settingfile::on_read_clicked()
{
    int retVal = 0;
    char *filePath;
    filePath = ui->edit_filepath->text().toUtf8().data();
    retVal = StCam_ReadSettingFile(libHandle, filePath);
    if (0 > retVal) {
        fprintf(stderr, "StCam_ReadSettingFile ERROR: %d\n", retVal);
        return;
    }
}

void settingfile::on_write_clicked()
{
    int retVal = 0;
    char *filePath;
    filePath = ui->edit_filepath->text().toUtf8().data();
    retVal = StCam_WriteSettingFile(libHandle, filePath);
    if (0 > retVal) {
        fprintf(stderr, "StCam_WriteSettingFile ERROR: %d\n", retVal);
        return;
    }
}
