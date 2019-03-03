#include "capturewindow.h"
#include "ui_capturewindow.h"

#include <QPainter>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/videodev2.h>

#include <opencv2/opencv.hpp>

#include <sentech/stcam_dd.h>
#include <sentech/stcam_lib.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

CaptureWindow::CaptureWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CaptureWindow),
    devHandle(0),
    libHandle(NULL),
    isCameraALC(false),
    isCameraWB(false),
    isMultiROI(false),
    noiseReduction(0),
    transferBits(0),
    displayPixFormat(0),
    colorInterpolation(0),
    mirrorMode(0),
    rotationMode(0),
    timerId(0),
    imageWidth(640),
    imageHeight(480),
    pRawData(NULL),
    pBgrData(NULL),
    bufferNum(0),
    frameCount(0),
    captureStarted(false)
{
    ui->setupUi(this);
}

CaptureWindow::~CaptureWindow()
{
    if(pRawData != NULL) {
        free(pRawData);
        pRawData = NULL;
    }
    if(pBgrData != NULL) {
        free(pBgrData);
        pBgrData = NULL;
    }
    delete ui;
}

void CaptureWindow::Init(int fd, void* lib, bool cameraALC, bool cameraWB, bool multiROI)
{
    devHandle = fd;
    libHandle = lib;
    isCameraALC = cameraALC;
    isCameraWB = cameraWB;
    isMultiROI = multiROI;
	
	struct stcam_st_max_scan_size max_scan_size;
    int retVal = ioctl(devHandle, STCIOC_G_MAX_SCAN_SIZE, &max_scan_size);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_G_MAX_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}

	pRawData = (unsigned char*)malloc(max_scan_size.width * max_scan_size.height * BYTE_PER_PIXELS_MAX * PIXEL_COLOR_MAX);
	pBgrData = (unsigned char*)malloc(max_scan_size.width * max_scan_size.height * BYTE_PER_PIXELS_MAX * PIXEL_COLOR_MAX);
}

void CaptureWindow::startCapture()
{
    struct v4l2_requestbuffers req;
    CLEAR(req);
    req.count = BUFFER_REQUEST_MAX;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (-1 == ioctl(devHandle, VIDIOC_REQBUFS, &req)) {
        exit(EXIT_FAILURE);
    }
    if (req.count < BUFFER_REQUEST_MIN) {
        exit(EXIT_FAILURE);
    }
	bufferNum = req.count;

    struct v4l2_buffer buf;
    for (unsigned int i = 0; i < bufferNum; i++) {
        CLEAR(buf);
        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = i;

        if (-1 == ioctl(devHandle, VIDIOC_QUERYBUF, &buf))
            exit(EXIT_FAILURE);

        buffers[i].length = buf.length;
        buffers[i].start =
            mmap(NULL /* start anywhere */,
                buf.length,
                PROT_READ | PROT_WRITE /* required */,
                MAP_SHARED /* recommended */,
                devHandle, buf.m.offset);

        if (MAP_FAILED == buffers[i].start)
            exit(EXIT_FAILURE);

        if (-1 == ioctl(devHandle, VIDIOC_QBUF, &buf))
            exit(EXIT_FAILURE);
    }

    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == ioctl(devHandle, VIDIOC_STREAMON, &type))
        exit(EXIT_FAILURE);

    timerId = startTimer(1);
	
    captureStarted = true;
    show();
}

void CaptureWindow::stopCapture()
{
    if(!captureStarted)
		return;
	
    enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == ioctl(devHandle, VIDIOC_STREAMOFF, &type))
        exit(EXIT_FAILURE);

    for (unsigned int i = 0; i < bufferNum; ++i)
        munmap(buffers[i].start, buffers[i].length);

    captureStarted = false;
}

void CaptureWindow::setNoiseReduction(int value)
{
    noiseReduction = value;
}

void CaptureWindow::setTransferBits(int value)
{
    transferBits = value;
}

void CaptureWindow::setDisplayPixFormat(int value)
{
    displayPixFormat = value;
}

void CaptureWindow::setColorInterpolation(int value)
{
    colorInterpolation = value;
}

void CaptureWindow::setMirrorMode(int value)
{
    mirrorMode = value;
}

void CaptureWindow::setRotationMode(int value)
{
    rotationMode = value;
}

void CaptureWindow::closeEvent(QCloseEvent *)
{
	stopCapture();
}

void CaptureWindow::timerEvent(QTimerEvent *e)
{
    if(e->timerId() == timerId)
    {
        update();
    }
}

void CaptureWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    struct v4l2_control control;
    control.id = STC_CID_COLOR_ARRAY;
    if (0 != ioctl(devHandle, VIDIOC_G_CTRL, &control)) {
        return;
    }
    unsigned short colorArray = control.value;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(devHandle, &fds);

    /* Timeout. */
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    if (0 != select(devHandle + 1, &fds, NULL, NULL, &tv)) {
        unsigned long imageLinePitch;
        unsigned long imageSize;
        unsigned int bitPerPixel = 8;

        struct v4l2_format fmt;
        CLEAR(fmt);
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == ioctl(devHandle, VIDIOC_G_FMT, &fmt))
            return;
        imageWidth = fmt.fmt.pix.width;
        imageHeight = fmt.fmt.pix.height;
        imageLinePitch = fmt.fmt.pix.bytesperline;
        imageSize = fmt.fmt.pix.sizeimage;

        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (-1 == ioctl(devHandle, VIDIOC_DQBUF, &buf)) {
            return;
        }
        assert(buf.index < bufferNum);

        if(isMultiROI) {
            unsigned long multiWidth = 0;
            unsigned long multiHeight = 0;
            unsigned long multiLinePitch = 0;

            unsigned char* tempRowData = (unsigned char*)malloc(imageSize);
            memcpy(tempRowData, (unsigned char*)buffers[buf.index].start, imageSize);
            unsigned char* pbyteDecodedRaw = NULL;

            StCam_DecodingCombinedMultiROI(libHandle, DECODING_COMBINED_MULTI_ROI_EXCEPT_BLANK_ROW_AND_COL, tempRowData, &pbyteDecodedRaw, &multiWidth, &multiHeight, &multiLinePitch);
            imageWidth = multiWidth;
            imageHeight = multiHeight;
            imageLinePitch = multiLinePitch;
            imageSize = imageLinePitch * imageHeight;
            memcpy(pRawData, pbyteDecodedRaw, imageSize);
            free(tempRowData);
        } else {
            memcpy(pRawData, (unsigned char*)buffers[buf.index].start, imageSize);
        }

        if(transferBits == STC_MENU_TRANSFER_BITS_PER_PIXEL_BGR_08) {
            StCam_ConvYUVOrBGRToBGRImage(libHandle, imageWidth, imageHeight, transferBits, pRawData, displayPixFormat, pBgrData);
        } else {
            switch (transferBits) {
            case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12:
            case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_12P:
            case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12:
            case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_12P:
            case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10:
            case STC_MENU_TRANSFER_BITS_PER_PIXEL_RAW_10P:
            case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10:
            case STC_MENU_TRANSFER_BITS_PER_PIXEL_MONO_10P:
            {
                unsigned short* rowDataTemp = NULL;
                rowDataTemp = (unsigned short*)malloc(imageSize);
                memcpy(rowDataTemp, pRawData, imageSize);
                unsigned long tempImageLinePitch = imageLinePitch;
                imageLinePitch = 0;
                StCam_ConvTo8BitsImage(libHandle, imageWidth, imageHeight, tempImageLinePitch, transferBits, rowDataTemp, &imageLinePitch, pRawData);
                free(rowDataTemp);
            }
                break;
            default:
                break;
            }

            if(!isCameraALC) {
                if(frameCount >= 5) {
                    float brightness[4];
                    unsigned long alcStatus;
                    StCam_GetAveragePixelValue(libHandle, imageWidth, imageHeight, imageLinePitch, colorArray, transferBits, pRawData, 0, 0, imageWidth, imageHeight, brightness);
                    if(colorArray != STC_MENU_COLOR_ARRAY_MONO) {
                        StCam_ALC(libHandle, (unsigned short)brightness[0], &alcStatus);
                    } else {
                        unsigned short average = (unsigned short)((brightness[1] + brightness[2])/2);
                         StCam_ALC(libHandle, average, &alcStatus);
                    }
                } else {
                    frameCount++;
                }
            }
            StCam_NoiseReduction(libHandle, noiseReduction, imageWidth, imageHeight, imageLinePitch, colorArray, pRawData, bitPerPixel);
            StCam_ShadingCorrection(libHandle, imageWidth, imageHeight, imageLinePitch, pRawData, bitPerPixel);
            if(colorArray != STC_MENU_COLOR_ARRAY_MONO) {
                if(!isCameraWB){
                    StCam_WhiteBalance(libHandle, imageWidth, imageHeight, imageLinePitch, colorArray, pRawData, bitPerPixel);
                }
                StCam_RawColorGamma(libHandle, imageWidth, imageHeight, colorArray, pRawData, bitPerPixel);
            }
            StCam_MirrorRotation(libHandle, mirrorMode, rotationMode, &imageWidth, &imageHeight, &colorArray, pRawData);

            StCam_ColorInterpolation(libHandle, imageWidth, imageHeight, colorArray, pRawData, pBgrData, colorInterpolation, displayPixFormat, bitPerPixel);
            StCam_BGRGamma(libHandle, imageWidth, imageHeight, displayPixFormat, pBgrData, bitPerPixel);
            if(colorArray != STC_MENU_COLOR_ARRAY_MONO) {
                StCam_HueSaturationColorMatrix(libHandle, imageWidth, imageHeight, displayPixFormat, pBgrData, bitPerPixel);
            }
            StCam_Sharpness(libHandle, imageWidth, imageHeight, displayPixFormat, pBgrData, bitPerPixel);
        }
        if (-1 == ioctl(devHandle, VIDIOC_QBUF, &buf))
            return;
    }

    if((imageWidth != 0) && (imageHeight != 0)) {
        if(colorArray == STC_MENU_COLOR_ARRAY_MONO) {
            cv::Mat src(imageHeight, imageWidth, CV_8UC1);
            cv::Mat dst;
            memcpy(src.data, pBgrData, src.step * src.rows);

            cv::cvtColor(src, dst, CV_GRAY2BGR);
            QImage img(dst.data, imageWidth, imageHeight, QImage::Format_RGB888);
            img = img.rgbSwapped();
            painter.drawImage(0, 0, img);
        } else {
            if(displayPixFormat == STCAM_PIXEL_FORMAT_32_BGR) {
                QImage img(pBgrData, imageWidth, imageHeight, QImage::Format_RGB32);
                painter.drawImage(0, 0, img);
            } else {
                QImage img(pBgrData, imageWidth, imageHeight, QImage::Format_RGB888);
                img = img.rgbSwapped();
                painter.drawImage(0, 0, img);
            }
        }
    }
}

