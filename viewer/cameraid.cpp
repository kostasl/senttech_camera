#include "cameraid.h"
#include "ui_cameraid.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <sentech/stcam_dd.h>

cameraid::cameraid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cameraid),
    devHandle(0)
{
    ui->setupUi(this);
}

cameraid::~cameraid()
{
    delete ui;
}

void cameraid::Init(int fd)
{
    devHandle = fd;
    show();
}

void cameraid::on_read_clicked()
{
    int retVal = 0;
    struct stcam_st_cam_user_id cam_user_id;
    retVal = ioctl(devHandle, STCIOC_G_CAM_USER_ID, &cam_user_id);
    if (0 > retVal) {
        fprintf(stderr, "STCIOC_G_CAM_USER_ID ERROR: %d\n", retVal);
        return;
    }
    ui->edit_id->setText(QString(QString::number(cam_user_id.camera_id)));
    ui->edit_name->setText(QString(QString::fromUtf8((const char*)cam_user_id.camera_name, cam_user_id.length)));
}

void cameraid::on_write_clicked()
{
    int retVal = 0;
    struct stcam_st_cam_user_id cam_user_id;
    cam_user_id.camera_id = ui->edit_id->text().toInt();
    cam_user_id.length = ui->edit_name->text().length();
    std::string string = ui->edit_name->text().toStdString();
    memcpy(cam_user_id.camera_name, string.data(), cam_user_id.length);
    retVal = ioctl(devHandle, STCIOC_S_CAM_USER_ID, &cam_user_id);
    if (0 > retVal) {
        fprintf(stderr, "STCIOC_S_CAM_USER_ID ERROR: %d\n", retVal);
        return;
    }
}
