#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <sys/ioctl.h>

#include <sentech/stcam_dd.h>
#include <sentech/stcam_lib.h>

static void usage(FILE *fp, int, char **argv)
{
	fprintf(fp,
		"Usage: %s [options]\n\n"
		"Options:\n"
		"-d | --device name	   Video device name\n"
		"-h | --help		   Print this message\n"
		"",
		argv[0]);
}

static const char short_options[] = "d:h";

static const struct option
long_options[] = {
	{ "device",			required_argument,	NULL, 'd' },
	{ "help",			no_argument,		NULL, 'h' },
	{ 0, 0, 0, 0 }
};


//**********************************
// public function
//**********************************

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	capWindow(new CaptureWindow),
	cameraIdWindow(new cameraid),
	settingFileWindow(new settingfile),
	devHandle(0),
	libHandle(NULL),
	isCameraALC(false),
	isCameraWB(false),
	isMultiROI(false),
	isCameraFlip(false),
	isIo0ReadOnly(false),
	isIo1ReadOnly(false),
	isIo2ReadOnly(false),
	isIo3ReadOnly(false),
	hasSw(false),
	hasTemperature(false),
	isAutoExposureEnd(false),
	autoExposureEndTimerId(0),
    autoExposureEndTime(500),
	isEnableSettingChange(false),
	isEnableDisplayChange(false)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete settingFileWindow;
	delete cameraIdWindow;
	delete capWindow;
	delete ui;
}

void MainWindow::Init(int argc, char *argv[])
{
	int retVal = 0;
	char *dev_name = (char*)("/dev/video0");
	for (;;) {
		int idx;
		int c;

		c = getopt_long(argc, argv, short_options, long_options, &idx);

		if (-1 == c)
			break;

		switch (c) {
		case 0: /* getopt_long() flag */
			break;

		case 'd':
			dev_name = optarg;
			break;

		case 'h':
			usage(stdout, argc, argv);
			exit(EXIT_SUCCESS);

		default:
			usage(stderr, argc, argv);
			exit(EXIT_FAILURE);
		}
	}

	devHandle = open(dev_name, O_RDWR | O_NONBLOCK, 0);
	if (-1 == devHandle) {
		fprintf(stderr, "Cannot open device'%s': %d\n", dev_name, errno);
		return;
	}

	libHandle = StCam_Open(devHandle);
	if (NULL == libHandle) {
		fprintf(stderr, "Cannot open library\n");
		return;
	}

	// Init Main Display
	struct stcam_st_cam_version camVersion;
	retVal = ioctl(devHandle, STCIOC_G_CAM_VERSION, &camVersion);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_G_CAM_VERSION ERROR: %d\n", retVal);
		return;
	}
	ui->mainCamera->setText(QString((const char*)camVersion.product_name));
	ui->mainFpga->setText(QString::number(camVersion.fpga_version, 16));
	ui->mainFirm->setText(QString::number(camVersion.firm_version, 16));

	struct v4l2_capability capability;
	retVal = ioctl(devHandle, VIDIOC_QUERYCAP, &capability);
	if (0 > retVal) {
		fprintf(stderr, "VIDIOC_QUERYCAP ERROR: %d\n", retVal);
		return;
	}
	ui->mainDriver->setText(QString::number(capability.version, 16));

	unsigned long libVersion;
	retVal = StCam_GetlibVersion(libHandle, &libVersion);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetlibVersion ERROR: %d\n", retVal);
		return;
	}
	ui->mainLibrary->setText(QString::number(libVersion, 16));

	bool isMono = false;
	int colorArray = 0;
	if(getDriverCtrlValue(STC_CID_COLOR_ARRAY, colorArray) < 0) {
		return;
	}
	if(STC_MENU_COLOR_ARRAY_MONO == colorArray) {
		isMono = true;
	}
	
	// Init Tab
	InitShutterGain();
	InitWhiteBalance(isMono);
	InitY();
	InitColorGamma(isMono);
	InitTriggerMode();
	InitTriggerTiming();
	InitIO();
	InitHDR();
	InitEeprom();
	InitOther(isMono);
	
	capWindow->Init(devHandle, libHandle, isCameraALC, isCameraWB, isMultiROI);
	isEnableDisplayChange = true;
	update();
}


//**********************************
// event
//**********************************

void MainWindow::closeEvent(QCloseEvent *)
{
	settingFileWindow->close();
	cameraIdWindow->close();
	capWindow->close();
	StCam_Close(libHandle);
	::close(devHandle);
}

void MainWindow::paintEvent(QPaintEvent *)
{
    if(!isEnableDisplayChange)return;
    isEnableDisplayChange = false;
	isEnableSettingChange = false;
	switch(ui->tabWidget->currentIndex()) {
	case 0:
		UpdateShutterGain();
		break;
	case 1:
		UpdateWhiteBalance();
		break;
	case 2:
		UpdateY();
		break;
	case 3:
		UpdateColorGamma();
		break;
	case 4:
		UpdateTriggerMode();
		break;
	case 5:
		UpdateTriggerTiming();
		break;
	case 6:
		UpdateIO();
		break;
	case 7:
		UpdateHDR();
		break;
	case 8:
		UpdateEeprom();
		break;
	case 9:
		UpdateOther();
		break;
	default:
		break;
	}
	isEnableSettingChange = true;
}

void MainWindow::timerEvent(QTimerEvent *e)
{
	if(e->timerId() == autoExposureEndTimerId)
	{
		killTimer(autoExposureEndTimerId);
		if(setDriverCtrlValue(STC_CID_TRIGGER_SOFTWARE, STC_MENU_TRIGGER_SOFTWARE_EXPOSURE_END) < 0) {
			return;
		}
	}
}


//**********************************
// main window operation
//**********************************

void MainWindow::on_mainCapture_clicked()
{
	capWindow->startCapture();
}

void MainWindow::on_mainCameraId_clicked()
{
	cameraIdWindow->Init(devHandle);
}

void MainWindow::on_mainSettingFile_clicked()
{
	settingFileWindow->Init(libHandle);
}


//**********************************
// Initialize
//**********************************

// ShutterGain
void MainWindow::InitShutterGain()
{
	int retVal = 0;
	struct v4l2_queryctrl queryctrl;
	struct v4l2_querymenu querymenu;

	// Exposure
	int maxShortExposure = 0;
	if(getDriverCtrlValue(STC_CID_MAX_SHORT_EXPOSURE, maxShortExposure) < 0) {
		return;
	}
	queryctrl.id = V4L2_CID_EXPOSURE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "V4L2_CID_EXPOSURE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->sgExposureSlider->setDisabled(true);
		ui->sgExposureEdit->setDisabled(true);
	} else {
		ui->sgExposureSlider->setMinimum(queryctrl.minimum);
		ui->sgExposureSlider->setMaximum(maxShortExposure);
	}

	// Analog Gain
	queryctrl.id = V4L2_CID_GAIN;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "V4L2_CID_GAIN QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->sgAnalogGainSlider->setDisabled(true);
		ui->sgAnalogGainEdit->setDisabled(true);
	} else {
		ui->sgAnalogGainSlider->setMinimum(queryctrl.minimum);
		ui->sgAnalogGainSlider->setMaximum(queryctrl.maximum);
	}

	// Digital Gain
	queryctrl.id = STC_CID_DIGITAL_GAIN;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_DIGITAL_GAIN QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->sgDigitalGainSlider->setDisabled(true);
		ui->sgDigitalGainEdit->setDisabled(true);
	} else {
		ui->sgDigitalGainSlider->setMinimum(queryctrl.minimum);
		ui->sgDigitalGainSlider->setMaximum(queryctrl.maximum);
	}

	// ALC Mode
	queryctrl.id = STC_CID_ALC_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_ALC_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		isCameraALC = false;
		ui->sgAlcMode->addItem(QString("Off"));
		alcModeList << STCAM_ALCMODE_OFF;
		ui->sgAlcMode->addItem(QString("AE/AGC On(PC)"));
		alcModeList << STCAM_ALCMODE_PC_AE_AGC_ON;
		ui->sgAlcMode->addItem(QString("AE On(PC)"));
		alcModeList << STCAM_ALCMODE_PC_AE_ON;
		ui->sgAlcMode->addItem(QString("AGC On(PC)"));
		alcModeList << STCAM_ALCMODE_PC_AGC_ON;
		ui->sgAlcMode->addItem(QString("AE/AGC OneShot(PC)"));
		alcModeList << STCAM_ALCMODE_PC_AE_AGC_ONESHOT;
		ui->sgAlcMode->addItem(QString("AE OneShot(PC)"));
		alcModeList << STCAM_ALCMODE_PC_AE_ONESHOT;
		ui->sgAlcMode->addItem(QString("AGC OneShot(PC)"));
		alcModeList << STCAM_ALCMODE_PC_AGC_ONESHOT;
	} else {
		isCameraALC = true;
		querymenu.id = STC_CID_ALC_MODE;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->sgAlcMode->addItem(QString((const char*)querymenu.name));
				alcModeList << querymenu.index;
			}
		}
	}

	// ALC Target
	queryctrl.id = STC_CID_ALC_TARGET_LEVEL;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_ALC_TARGET_LEVEL QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->sgAlcTargetSlider->setMinimum(0);
		ui->sgAlcTargetSlider->setMaximum(255);
	} else {
		ui->sgAlcTargetSlider->setMinimum(queryctrl.minimum);
		ui->sgAlcTargetSlider->setMaximum(queryctrl.maximum);
	}

	// AE Min Exposure
	queryctrl.id = STC_CID_AE_MIN_EXPOSURE_CLOCK;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_AE_MIN_EXPOSURE_CLOCK QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->sgMinExposureSlider->setMinimum(0);
        ui->sgMinExposureSlider->setMaximum(maxShortExposure);
	} else {
		ui->sgMinExposureSlider->setMinimum(queryctrl.minimum);
        ui->sgMinExposureSlider->setMaximum(maxShortExposure);
	}

	// AE Max Exposure
	queryctrl.id = STC_CID_AE_MAX_EXPOSURE_CLOCK;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_AE_MAX_EXPOSURE_CLOCK QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->sgMaxExposureSlider->setMinimum(0);
        ui->sgMaxExposureSlider->setMaximum(maxShortExposure);
	} else {
		ui->sgMaxExposureSlider->setMinimum(queryctrl.minimum);
        ui->sgMaxExposureSlider->setMaximum(maxShortExposure);
	}

	// AGC Min Gain
	queryctrl.id = STC_CID_AGC_MIN_GAIN;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_AGC_MIN_GAIN QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->sgMinGainSlider->setMinimum(0);
		ui->sgMinGainSlider->setMaximum(255);
	} else {
		ui->sgMinGainSlider->setMinimum(queryctrl.minimum);
		ui->sgMinGainSlider->setMaximum(queryctrl.maximum);
	}

	// AGC Max Gain
	queryctrl.id = STC_CID_AGC_MAX_GAIN;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_AGC_MAX_GAIN QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->sgMaxGainSlider->setMinimum(0);
		ui->sgMaxGainSlider->setMaximum(255);
	} else {
		ui->sgMaxGainSlider->setMinimum(queryctrl.minimum);
		ui->sgMaxGainSlider->setMaximum(queryctrl.maximum);
	}

	// Adjustment Digital Gain
	queryctrl.id = STC_CID_ADJUSTMENT_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_ADJUSTMENT_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->sgAdjustGain->setDisabled(true);
	} else {
		ui->sgAdjustGain->addItem(QString("Off"));
		adjustmentModeList << 0;
		ui->sgAdjustGain->addItem(QString("On"));
		adjustmentModeList << 1;
	}

}

// WhiteBalance
void MainWindow::InitWhiteBalance(bool isMono)
{
	if(isMono) {
		ui->WhiteBalance->setDisabled(true);
		return;
	}
	int retVal = 0;
	struct v4l2_queryctrl queryctrl;
	struct v4l2_querymenu querymenu;
	unsigned short wbMaxGainR, wbMaxGainGr, wbMaxGainGb, wbMaxGainB;

	// WB Mode
	queryctrl.id = STC_CID_WHITE_BALANCE_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_WHITE_BALANCE_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		isCameraWB = false;
		ui->wbMode->addItem(QString("Off"));
		wbModeList << STCAM_WB_OFF;
		ui->wbMode->addItem(QString("Manual(PC)"));
		wbModeList << STCAM_WB_MANUAL;
		ui->wbMode->addItem(QString("Auto(PC)"));
		wbModeList << STCAM_WB_FULLAUTO;
		ui->wbMode->addItem(QString("OneShot(PC)"));
		wbModeList << STCAM_WB_ONESHOT;
		retVal = StCam_GetWhiteBalanceMaxGain(libHandle, &wbMaxGainR, &wbMaxGainGr, &wbMaxGainGb, &wbMaxGainB);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetWhiteBalanceMaxGain ERROR: %d\n", retVal);
			return;
		}
	} else {
		isCameraWB = true;
		querymenu.id = STC_CID_WHITE_BALANCE_MODE;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->wbMode->addItem(QString((const char*)querymenu.name));
				wbModeList << querymenu.index;
			}
		}
	}

	// R Gain
	queryctrl.id = STC_CID_WHITE_BALANCE_GAIN_R;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_WHITE_BALANCE_GAIN_R QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->wbRGainSlider->setMinimum(0);
		ui->wbRGainSlider->setMaximum(wbMaxGainR);
	} else {
		ui->wbRGainSlider->setMinimum(queryctrl.minimum);
		ui->wbRGainSlider->setMaximum(queryctrl.maximum);
	}

	// Gr Gain
	queryctrl.id = STC_CID_WHITE_BALANCE_GAIN_GR;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_WHITE_BALANCE_GAIN_GR QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->wbGrGainSlider->setMinimum(0);
		ui->wbGrGainSlider->setMaximum(wbMaxGainGr);
	} else {
		ui->wbGrGainSlider->setMinimum(queryctrl.minimum);
		ui->wbGrGainSlider->setMaximum(queryctrl.maximum);
	}

	// Gb Gain
	queryctrl.id = STC_CID_WHITE_BALANCE_GAIN_GB;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_WHITE_BALANCE_GAIN_GB QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->wbGbGainSlider->setMinimum(0);
		ui->wbGbGainSlider->setMaximum(wbMaxGainGb);
	} else {
		ui->wbGbGainSlider->setMinimum(queryctrl.minimum);
		ui->wbGbGainSlider->setMaximum(queryctrl.maximum);
	}

	// B Gain
	queryctrl.id = STC_CID_WHITE_BALANCE_GAIN_B;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_WHITE_BALANCE_GAIN_B QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->wbBGainSlider->setMinimum(0);
		ui->wbBGainSlider->setMaximum(wbMaxGainB);
	} else {
		ui->wbBGainSlider->setMinimum(queryctrl.minimum);
		ui->wbBGainSlider->setMaximum(queryctrl.maximum);
	}

	// Hue/Saturation Mode
	ui->wbHSMode->addItem(QString("Off"));
	hueSaturationList << 0;
	ui->wbHSMode->addItem(QString("On"));
	hueSaturationList << 1;

	// Hue
	ui->wbHueSlider->setMinimum(STCAM_HUE_MIN_VALUE);
	ui->wbHueSlider->setMaximum(STCAM_HUE_MAX_VALUE);

	// Saturation
	ui->wbSaturationSlider->setMinimum(STCAM_SATURATION_MIN_VALUE);
	ui->wbSaturationSlider->setMaximum(STCAM_SATURATION_MAX_VALUE);

	// High Chroma Suppression Start Level
	ui->wbHcsStartSlider->setMinimum(STCAM_CHROMA_SUPPRESSION_MIN_VALUE);
	ui->wbHcsStartSlider->setMaximum(STCAM_CHROMA_SUPPRESSION_MAX_VALUE);

	// High Chroma Suppression Suppression Level
	ui->wbHcsSuppressionSlider->setMinimum(STCAM_CHROMA_SUPPRESSION_MIN_VALUE);
	ui->wbHcsSuppressionSlider->setMaximum(STCAM_CHROMA_SUPPRESSION_MAX_VALUE);

	// Low Chroma Suppression Start Level
	ui->wbLcsStartSlider->setMinimum(STCAM_CHROMA_SUPPRESSION_MIN_VALUE);
	ui->wbLcsStartSlider->setMaximum(STCAM_CHROMA_SUPPRESSION_MAX_VALUE);

	// Low Chroma Suppression Suppression Level
	ui->wbLcsSuppressionSlider->setMinimum(STCAM_CHROMA_SUPPRESSION_MIN_VALUE);
	ui->wbLcsSuppressionSlider->setMaximum(STCAM_CHROMA_SUPPRESSION_MAX_VALUE);
}

// Y
void MainWindow::InitY()
{
	int retVal = 0;
	struct v4l2_queryctrl queryctrl;

	// Y Gamma Mode
	ui->yGammaMode->addItem(QString("Off"));
	yGammaList << STCAM_GAMMA_OFF;
	ui->yGammaMode->addItem(QString("On"));
	yGammaList << STCAM_GAMMA_ON;
	ui->yGammaMode->addItem(QString("Reverse"));
	yGammaList << STCAM_GAMMA_REVERSE;

	// Y Gamma Value
	ui->yGammaValueSlider->setMinimum(1);
	ui->yGammaValueSlider->setMaximum(500);

	// Camera Gamma
	queryctrl.id = STC_CID_CAMERA_GAMMA;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_CAMERA_GAMMA QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->yCameraGammaSlider->setDisabled(true);
	} else {
		ui->yCameraGammaSlider->setMinimum(queryctrl.minimum);
		ui->yCameraGammaSlider->setMaximum(queryctrl.maximum);
	}

	// Sharpness Mode
	ui->ySharpnessMode->addItem(QString("Off"));
	sharpnessModeList << STCAM_SHARPNESS_OFF;
	ui->ySharpnessMode->addItem(QString("On"));
	sharpnessModeList << STCAM_SHARPNESS_ON;

	// Sharpness Gain
	ui->ySharpnessgainSlider->setMinimum(0);
	ui->ySharpnessgainSlider->setMaximum(500);

	// Sharpness Coring
	ui->ySharpnessCoringSlider->setMinimum(0);
	ui->ySharpnessCoringSlider->setMaximum(255);

	// Digital Clamp
	queryctrl.id = STC_CID_DIGITAL_CLAMP;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_DIGITAL_CLAMP QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->yDigitalClampSlider->setDisabled(true);
	} else {
		ui->yDigitalClampSlider->setMinimum(queryctrl.minimum);
		ui->yDigitalClampSlider->setMaximum(queryctrl.maximum);
	}

	// Analog Black Level
	queryctrl.id = STC_CID_ANALOG_BLACK_LEVEL;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_ANALOG_BLACK_LEVEL QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->yAnalogBlackLevelSlider->setDisabled(true);
	} else {
		ui->yAnalogBlackLevelSlider->setMinimum(queryctrl.minimum);
		ui->yAnalogBlackLevelSlider->setMaximum(queryctrl.maximum);
	}

	// Shading Correction Target Level
	ui->yShadingTargetSlider->setMinimum(0);
	ui->yShadingTargetSlider->setMaximum(255);

	// Shading Correction Mode
	ui->yShadingMode->addItem(QString("Off"));
	shadingModeList << 0;
	ui->yShadingMode->addItem(QString("Calibration(Multiplication)"));
	shadingModeList << 1;
	ui->yShadingMode->addItem(QString("On(Multiplication)"));
	shadingModeList << 2;
	ui->yShadingMode->addItem(QString("Calibration(Addition)"));
	shadingModeList << 3;
	ui->yShadingMode->addItem(QString("On(Addition)"));
	shadingModeList << 4;
}

// ColorGamma
void MainWindow::InitColorGamma(bool isMono)
{
	if(isMono) {
		ui->ColorGamma->setDisabled(true);
		return;
	}

	// R Gamma Mode
	ui->cgRGammaMode->addItem(QString("Off"));
	rGammaList << STCAM_GAMMA_OFF;
	ui->cgRGammaMode->addItem(QString("On"));
	rGammaList << STCAM_GAMMA_ON;
	ui->cgRGammaMode->addItem(QString("Reverse"));
	rGammaList << STCAM_GAMMA_REVERSE;

	// R Gamma Value
	ui->cgRGammaValueSlider->setMinimum(1);
	ui->cgRGammaValueSlider->setMaximum(500);

	// Gr Gamma Mode
	ui->cgGrGammaMode->addItem(QString("Off"));
	grGammaList << STCAM_GAMMA_OFF;
	ui->cgGrGammaMode->addItem(QString("On"));
	grGammaList << STCAM_GAMMA_ON;
	ui->cgGrGammaMode->addItem(QString("Reverse"));
	grGammaList << STCAM_GAMMA_REVERSE;

	// Gr Gamma Value
	ui->cgGrGammaValueSlider->setMinimum(1);
	ui->cgGrGammaValueSlider->setMaximum(500);

	// Gb Gamma Mode
	ui->cgGbGammaMode->addItem(QString("Off"));
	gbGammaList << STCAM_GAMMA_OFF;
	ui->cgGbGammaMode->addItem(QString("On"));
	gbGammaList << STCAM_GAMMA_ON;
	ui->cgGbGammaMode->addItem(QString("Reverse"));
	gbGammaList << STCAM_GAMMA_REVERSE;

	// Gb Gamma Value
	ui->cgGbGammaValueSlider->setMinimum(1);
	ui->cgGbGammaValueSlider->setMaximum(500);

	// B Gamma Mode
	ui->cgBGammaMode->addItem(QString("Off"));
	bGammaList << STCAM_GAMMA_OFF;
	ui->cgBGammaMode->addItem(QString("On"));
	bGammaList << STCAM_GAMMA_ON;
	ui->cgBGammaMode->addItem(QString("Reverse"));
	bGammaList << STCAM_GAMMA_REVERSE;

	// B Gamma Value
	ui->cgBGammaValueSlider->setMinimum(1);
	ui->cgBGammaValueSlider->setMaximum(500);
}

// Trigger Mode
void MainWindow::InitTriggerMode()
{
	int retVal = 0;
	struct v4l2_queryctrl queryctrl;
	struct v4l2_querymenu querymenu;

	// Exposure Mode
	queryctrl.id = STC_CID_EXPOSURE_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_EXPOSURE_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->tmExposureMode->setDisabled(true);
	} else {
		querymenu.id = STC_CID_EXPOSURE_MODE;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->tmExposureMode->addItem(QString((const char*)querymenu.name));
				exposureModeList << querymenu.index;
			}
		}
	}

	// Trigger Selector
	queryctrl.id = STC_CID_FRAME_START_TRIGGER_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_FRAME_START_TRIGGER_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags != V4L2_CTRL_FLAG_DISABLED) {
		ui->tmTriggerSelect->addItem(QString("FrameStart"));
		triggerSelectorList << 0;
	}
	queryctrl.id = STC_CID_FRAME_BURST_START_TRIGGER_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_FRAME_BURST_START_TRIGGER_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags != V4L2_CTRL_FLAG_DISABLED) {
		ui->tmTriggerSelect->addItem(QString("FrameBurstStart"));
		triggerSelectorList << 1;
	}
	queryctrl.id = STC_CID_EXPOSURE_START_TRIGGER_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_EXPOSURE_START_TRIGGER_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags != V4L2_CTRL_FLAG_DISABLED) {
		ui->tmTriggerSelect->addItem(QString("ExposureStart"));
		triggerSelectorList << 2;
	}
	queryctrl.id = STC_CID_EXPOSURE_END_TRIGGER_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_EXPOSURE_END_TRIGGER_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags != V4L2_CTRL_FLAG_DISABLED) {
		ui->tmTriggerSelect->addItem(QString("ExposureEnd"));
		triggerSelectorList << 3;
	}
	queryctrl.id = STC_CID_READ_OUT_START_TRIGGER_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_READ_OUT_START_TRIGGER_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags != V4L2_CTRL_FLAG_DISABLED) {
		ui->tmTriggerSelect->addItem(QString("ReadOutStart"));
		triggerSelectorList << 4;
	}
	ui->tmTriggerSelect->setCurrentIndex(0);

	// Trigger Mode
	queryctrl.id = STC_CID_FRAME_START_TRIGGER_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_FRAME_START_TRIGGER_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->tmTriggerMode->setDisabled(true);
	} else {
		ui->tmTriggerMode->addItem(QString("Off"));
		triggerModeList << 0;
		ui->tmTriggerMode->addItem(QString("On"));
		triggerModeList << 1;
	}

	// Trigger Source
	queryctrl.id = STC_CID_FRAME_START_TRIGGER_SOURCE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_FRAME_START_TRIGGER_SOURCE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->tmTriggerSource->setDisabled(true);
	} else {
		querymenu.id = STC_CID_FRAME_START_TRIGGER_SOURCE;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->tmTriggerSource->addItem(QString((const char*)querymenu.name));
				triggerSourceList << querymenu.index;
			}
		}
	}

	// Trigger Delay
	queryctrl.id = STC_CID_FRAME_START_TRIGGER_DELAY;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_FRAME_START_TRIGGER_DELAY QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->tmTriggerDelaySlider->setDisabled(true);
		ui->tmTriggerDelayEdit->setDisabled(true);
	} else {
		ui->tmTriggerDelaySlider->setMinimum(queryctrl.minimum);
		ui->tmTriggerDelaySlider->setMaximum(queryctrl.maximum);
	}

	// Trigger Overlap
	queryctrl.id = STC_CID_FRAME_START_TRIGGER_OVERLAP;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_FRAME_START_TRIGGER_OVERLAP QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->tmTriggerOverlap->setDisabled(true);
	} else {
		querymenu.id = STC_CID_FRAME_START_TRIGGER_OVERLAP;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->tmTriggerOverlap->addItem(QString((const char*)querymenu.name));
				triggerOverlapList << querymenu.index;
			}
		}
	}

	// Auto ExposureEnd Trigger
	ui->tmAutoExposureEndSlider->setMinimum(500);
	ui->tmAutoExposureEndSlider->setMaximum(600000);
	ui->tmAutoExposureEndSlider->setDisabled(true);
	ui->tmAutoExposureEndEdit->setDisabled(true);

	// Noise Reduction
	ui->tmNoiseReduction->addItem(QString("Off"));
	noiseReductionList << STCAM_NR_OFF;
	ui->tmNoiseReduction->addItem(QString("Easy"));
	noiseReductionList << STCAM_NR_EASY;
	ui->tmNoiseReduction->addItem(QString("Calibration"));
	noiseReductionList << STCAM_NR_DARK_CL;
	ui->tmNoiseReduction->addItem(QString("Complex"));
	noiseReductionList << STCAM_NR_COMPLEX;
	ui->tmNoiseReduction->setCurrentIndex(0);

	// Wait HD
	queryctrl.id = STC_CID_TRIGGER_WAIT_HD;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_TRIGGER_WAIT_HD QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->tmWaitHd->setDisabled(true);
	} else {
		ui->tmWaitHd->addItem(QString("Off"));
		waitHdList << 0;
		ui->tmWaitHd->addItem(QString("On"));
		waitHdList << 1;
	}

	// Wait Read Out
	queryctrl.id = STC_CID_TRIGGER_WAIT_READOUT;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_TRIGGER_WAIT_READOUT QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->tmWaitReadOut->setDisabled(true);
	} else {
		ui->tmWaitReadOut->addItem(QString("Off"));
		waitReadOutList << 0;
		ui->tmWaitReadOut->addItem(QString("On"));
		waitReadOutList << 1;
	}

	// Sensor Shutter Mode
	queryctrl.id = STC_CID_SENSOR_SHUTTER_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_SENSOR_SHUTTER_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->tmShutterMode->setDisabled(true);
	} else {
		querymenu.id = STC_CID_SENSOR_SHUTTER_MODE;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->tmShutterMode->addItem(QString((const char*)querymenu.name));
				sensorShutterModeList << querymenu.index;
			}
		}
	}
}

// Trigger Timing
void MainWindow::InitTriggerTiming()
{
	int retVal = 0;
	struct v4l2_queryctrl queryctrl;

	// Strobe Start Delay
	queryctrl.id = STC_CID_STROBE_START_DELAY;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_STROBE_START_DELAY QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->ttStrobeStartSlider->setDisabled(true);
		ui->ttStrobeStartEdit->setDisabled(true);
	} else {
		ui->ttStrobeStartSlider->setMinimum(queryctrl.minimum);
		ui->ttStrobeStartSlider->setMaximum(queryctrl.maximum);
	}

	// Strobe End Delay
	queryctrl.id = STC_CID_STROBE_END_DELAY;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_STROBE_END_DELAY QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->ttStrobeEndSlider->setDisabled(true);
		ui->ttStrobeEndEdit->setDisabled(true);
	} else {
		ui->ttStrobeEndSlider->setMinimum(queryctrl.minimum);
		ui->ttStrobeEndSlider->setMaximum(queryctrl.maximum);
	}

	// Output Pulse Delay
	queryctrl.id = STC_CID_OUTPUT_PULSE_DELAY;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_OUTPUT_PULSE_DELAY QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->ttPulseDelaySlider->setDisabled(true);
		ui->ttPulseDelayEdit->setDisabled(true);
	} else {
		ui->ttPulseDelaySlider->setMinimum(queryctrl.minimum);
		ui->ttPulseDelaySlider->setMaximum(queryctrl.maximum);
	}

	// Output Pulse Duration
	queryctrl.id = STC_CID_OUTPUT_PULSE_DURATION;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_OUTPUT_PULSE_DURATION QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->ttPulseDurationSlider->setDisabled(true);
		ui->ttPulseDurationEdit->setDisabled(true);
	} else {
		ui->ttPulseDurationSlider->setMinimum(queryctrl.minimum);
		ui->ttPulseDurationSlider->setMaximum(queryctrl.maximum);
	}

	// Read Out Delay
	queryctrl.id = STC_CID_READOUT_DELAY;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_READOUT_DELAY QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->ttReadOutSlider->setDisabled(true);
		ui->ttReadOutEdit->setDisabled(true);
	} else {
		ui->ttReadOutSlider->setMinimum(queryctrl.minimum);
		ui->ttReadOutSlider->setMaximum(queryctrl.maximum);
	}

	// Line Debounce Time
	queryctrl.id = STC_CID_LINE_DEBOUNCE_TIME;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_LINE_DEBOUNCE_TIME QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->ttLineDebounceSlider->setDisabled(true);
		ui->ttLineDebounceEdit->setDisabled(true);
	} else {
		ui->ttLineDebounceSlider->setMinimum(queryctrl.minimum);
		ui->ttLineDebounceSlider->setMaximum(queryctrl.maximum);
	}
}

// IO
void MainWindow::InitIO()
{
	int retVal = 0;
	struct v4l2_queryctrl queryctrl;
	struct v4l2_querymenu querymenu;

	// IO0 IN/OUT
	queryctrl.id = STC_CID_IO0_DIRECTION;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO0_DIRECTION QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io0InOut->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO0_DIRECTION;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io0InOut->addItem(QString((const char*)querymenu.name));
				io0InOutList << querymenu.index;
			}
		}
		if(queryctrl.flags == V4L2_CTRL_FLAG_READ_ONLY) {
			isIo0ReadOnly = true;
			ui->io0InOut->setDisabled(true);
		}
	}

	// IO1 IN/OUT
	queryctrl.id = STC_CID_IO1_DIRECTION;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO1_DIRECTION QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io1InOut->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO1_DIRECTION;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io1InOut->addItem(QString((const char*)querymenu.name));
				io1InOutList << querymenu.index;
			}
		}
		if(queryctrl.flags == V4L2_CTRL_FLAG_READ_ONLY) {
			isIo1ReadOnly = true;
			ui->io1InOut->setDisabled(true);
		}
	}

	// IO2 IN/OUT
	queryctrl.id = STC_CID_IO2_DIRECTION;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO2_DIRECTION QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io2InOut->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO2_DIRECTION;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io2InOut->addItem(QString((const char*)querymenu.name));
				io2InOutList << querymenu.index;
			}
		}
		if(queryctrl.flags == V4L2_CTRL_FLAG_READ_ONLY) {
			isIo2ReadOnly = true;
			ui->io2InOut->setDisabled(true);
		}
	}

	// IO3 IN/OUT
	queryctrl.id = STC_CID_IO3_DIRECTION;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO3_DIRECTION QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io3InOut->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO3_DIRECTION;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io3InOut->addItem(QString((const char*)querymenu.name));
				io3InOutList << querymenu.index;
			}
		}
		if(queryctrl.flags == V4L2_CTRL_FLAG_READ_ONLY) {
			isIo3ReadOnly = true;
			ui->io3InOut->setDisabled(true);
		}
	}

	// IO0 MODE
	queryctrl.id = STC_CID_IO0_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO0_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io0Mode->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO0_MODE;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io0Mode->addItem(QString((const char*)querymenu.name));
				io0ModeList << querymenu.index;
			}
		}
	}

	// IO1 MODE
	queryctrl.id = STC_CID_IO1_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO1_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io1Mode->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO1_MODE;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io1Mode->addItem(QString((const char*)querymenu.name));
				io1ModeList << querymenu.index;
			}
		}
	}

	// IO2 MODE
	queryctrl.id = STC_CID_IO2_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO2_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io2Mode->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO2_MODE;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io2Mode->addItem(QString((const char*)querymenu.name));
				io2ModeList << querymenu.index;
			}
		}
	}

	// IO3 MODE
	queryctrl.id = STC_CID_IO3_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO3_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io3Mode->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO3_MODE;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io3Mode->addItem(QString((const char*)querymenu.name));
				io3ModeList << querymenu.index;
			}
		}
	}

	// IO0 NEGA/POSI
	queryctrl.id = STC_CID_IO0_POLARITY;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO0_POLARITY QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io0NegaPosi->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO0_POLARITY;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io0NegaPosi->addItem(QString((const char*)querymenu.name));
				io0NegaPosiList << querymenu.index;
			}
		}
	}

	// IO1 NEGA/POSI
	queryctrl.id = STC_CID_IO1_POLARITY;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO1_POLARITY QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io1NegaPosi->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO1_POLARITY;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io1NegaPosi->addItem(QString((const char*)querymenu.name));
				io1NegaPosiList << querymenu.index;
			}
		}
	}

	// IO2 NEGA/POSI
	queryctrl.id = STC_CID_IO2_POLARITY;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO2_POLARITY QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io2NegaPosi->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO2_POLARITY;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io2NegaPosi->addItem(QString((const char*)querymenu.name));
				io2NegaPosiList << querymenu.index;
			}
		}
	}

	// IO3 NEGA/POSI
	queryctrl.id = STC_CID_IO3_POLARITY;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO3_POLARITY QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io3NegaPosi->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO3_POLARITY;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io3NegaPosi->addItem(QString((const char*)querymenu.name));
				io3NegaPosiList << querymenu.index;
			}
		}
	}

	// IO0 STATUS
	queryctrl.id = STC_CID_IO0_STATUS;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO0_STATUS QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io0Status->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO0_STATUS;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io0Status->addItem(QString((const char*)querymenu.name));
				io0StatusList << querymenu.index;
			}
		}
	}

	// IO1 STATUS
	queryctrl.id = STC_CID_IO1_STATUS;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO1_STATUS QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io1Status->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO1_STATUS;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io1Status->addItem(QString((const char*)querymenu.name));
				io1StatusList << querymenu.index;
			}
		}
	}

	// IO2 STATUS
	queryctrl.id = STC_CID_IO2_STATUS;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO2_STATUS QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io2Status->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO2_STATUS;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io2Status->addItem(QString((const char*)querymenu.name));
				io2StatusList << querymenu.index;
			}
		}
	}

	// IO3 STATUS
	queryctrl.id = STC_CID_IO3_STATUS;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_IO3_STATUS QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->io3Status->setDisabled(true);
	} else {
		querymenu.id = STC_CID_IO3_STATUS;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->io3Status->addItem(QString((const char*)querymenu.name));
				io3StatusList << querymenu.index;
			}
		}
	}

	// Reset Switch
	queryctrl.id = STC_CID_RESET_SWITCH;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_RESET_SWITCH QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->ioResetSwitch->setDisabled(true);
	} else {
		ui->ioResetSwitch->addItem(QString("Off"));
		resetSwitchList << 0;
		ui->ioResetSwitch->addItem(QString("On"));
		resetSwitchList << 1;
	}

	// SW
	queryctrl.id = STC_CID_SW_STATUS;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_SW_STATUS QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		hasSw = false;
	} else {
		hasSw = true;
	}
	
	// LED
	queryctrl.id = STC_CID_LED_STATUS;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_LED_STATUS QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->ioLed->setDisabled(true);
	} else {
		querymenu.id = STC_CID_LED_STATUS;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->ioLed->addItem(QString((const char*)querymenu.name));
				ledList << querymenu.index;
			}
		}
	}
	
	// Temperature
	queryctrl.id = STC_CID_TEMPERATURE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_TEMPERATURE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		hasTemperature = false;
	} else {
		hasTemperature = true;
	}
}

// HDR
void MainWindow::InitHDR()
{
	int retVal = 0;
	
	int hdrType = 0;
	if(getDriverCtrlValue(STC_CID_HDR_TYPE, hdrType) < 0) {
		return;
	}
	if(hdrType == STC_MENU_HDR_TYPE_NONE) {
		ui->HDR->setDisabled(true);
		return;
	}

	struct stcam_st_hdr hdr;
	retVal = ioctl(devHandle, STCIOC_G_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_G_HDR ERROR: %d\n", retVal);
		return;
	}

	// HDR Mode
	ui->hdrMode->addItem(QString("Off"));
	hdrModeList << 0;
	ui->hdrMode->addItem(QString("On"));
	hdrModeList << 1;

	// HDR Slope Num
	ui->hdrSlope->addItem(QString("1"));
	hdrSlopeNumList << 1;
	ui->hdrSlope->addItem(QString("2"));
	hdrSlopeNumList << 2;
	ui->hdrSlope->addItem(QString("3"));
	hdrSlopeNumList << 3;

	// Knee1
	ui->hdrKnee1Slider->setMinimum(0);
	ui->hdrKnee1Slider->setMaximum(255);

	// Vlow2
	ui->hdrVlow2Slider->setMinimum(0);
	ui->hdrVlow2Slider->setMaximum(63);

	// Knee2
	ui->hdrKnee2Slider->setMinimum(0);
	ui->hdrKnee2Slider->setMaximum(255);

	// Vlow3
	ui->hdrVlow3Slider->setMinimum(0);
	ui->hdrVlow3Slider->setMaximum(63);
}

// EEPROM
void MainWindow::InitEeprom()
{
	int retVal = 0;
	struct v4l2_queryctrl queryctrl;

	queryctrl.id = STC_CID_CAMERA_SETTING;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_CAMERA_SETTING QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->EEPROM->setDisabled(true);
	}
}

// Other
void MainWindow::InitOther(bool isMono)
{
	int retVal = 0;
	struct v4l2_queryctrl queryctrl;
	struct v4l2_querymenu querymenu;

	// Scan Mode
	queryctrl.id = STC_CID_SCAN_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_SCAN_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->otScanMode->setDisabled(true);
	} else {
		querymenu.id = STC_CID_SCAN_MODE;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->otScanMode->addItem(QString((const char*)querymenu.name));
				scanModeList << querymenu.index;
			}
		}
	}

	// H Decimation Binning
	queryctrl.id = STC_CID_H_DECIMATION_BINNING;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_H_DECIMATION_BINNING QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->otHDeciBin->setDisabled(true);
	} else {
		querymenu.id = STC_CID_H_DECIMATION_BINNING;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->otHDeciBin->addItem(QString((const char*)querymenu.name));
				hBinDeciList << querymenu.index;
			}
		}
	}

	// V Decimation Binning
	queryctrl.id = STC_CID_V_DECIMATION_BINNING;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_V_DECIMATION_BINNING QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->otVDeciBin->setDisabled(true);
	} else {
		querymenu.id = STC_CID_V_DECIMATION_BINNING;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->otVDeciBin->addItem(QString((const char*)querymenu.name));
				vBinDeciList << querymenu.index;
			}
		}
	}

	// H/V Binning Sum
	queryctrl.id = STC_CID_BINNINGSUM_MODE;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_BINNINGSUM_MODE QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->otBinSum->setDisabled(true);
	} else {
		querymenu.id = STC_CID_BINNINGSUM_MODE;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->otBinSum->addItem(QString((const char*)querymenu.name));
				binSumList << querymenu.index;
			}
		}
	}

	// Region - Region Mode
	int maxRoiCount = 0;
	if(getDriverCtrlValue(STC_CID_MAX_ROI_COUNT, maxRoiCount) < 0) {
		return;
	}
	if(maxRoiCount == 0) {
		ui->otRegion->addItem(QString("0"));
		regionList << 0;
		ui->otRegion->setDisabled(true);
		ui->otRegionMode->addItem(QString("On"));
		regionModeList << 1;
		ui->otRegionMode->setDisabled(true);
	} else {
        for(int i=0; i<maxRoiCount; i++) {
			ui->otRegion->addItem(QString::number(i));
			regionList << i;
		}
		ui->otRegionMode->addItem(QString("On"));
		regionModeList << 1;
		ui->otRegionMode->addItem(QString("Off"));
		regionModeList << 0;
		isMultiROI = true;

		ui->otRegion->setCurrentIndex(0);
		ui->otRegionMode->setCurrentIndex(0);
	}

	struct stcam_st_max_scan_size maxScanSize;
	retVal = ioctl(devHandle, STCIOC_G_MAX_SCAN_SIZE, &maxScanSize);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_G_MAX_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}

	// Offset X
	ui->otOffsetXSlider->setMinimum(0);
	ui->otOffsetXSlider->setMaximum(maxScanSize.width);

	// Offset Y
	ui->otOffsetYSlider->setMinimum(0);
	ui->otOffsetYSlider->setMaximum(maxScanSize.height);

	// Width
	ui->otWidthSlider->setMinimum(0);
	ui->otWidthSlider->setMaximum(maxScanSize.width);

	// Height
	ui->otHeightSlider->setMinimum(0);
	ui->otHeightSlider->setMaximum(maxScanSize.height);

	// V Blank for FPS
	queryctrl.id = STC_CID_V_BLANK;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_V_BLANK QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->otVBlankSlider->setDisabled(true);
		ui->otVBlankEdit->setDisabled(true);
	} else {
		ui->otVBlankSlider->setMinimum(queryctrl.minimum);
		ui->otVBlankSlider->setMaximum(queryctrl.maximum);
	}

	// Clock
	queryctrl.id = STC_CID_CLOCK;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_CLOCK QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->otClock->setDisabled(true);
	} else {
		querymenu.id = STC_CID_CLOCK;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->otClock->addItem(QString((const char*)querymenu.name));
				clockList << querymenu.index;
			}
		}
	}

	// Transfer bits
	queryctrl.id = STC_CID_TRANSFER_BITS_PER_PIXEL;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "STC_CID_TRANSFER_BITS_PER_PIXEL QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		ui->otTransferBits->setDisabled(true);
	} else {
		querymenu.id = STC_CID_TRANSFER_BITS_PER_PIXEL;
		for(int i=0; i<=queryctrl.maximum; i++) {
			querymenu.index = i;
			if (0 == ioctl(devHandle, VIDIOC_QUERYMENU, &querymenu)) {
				ui->otTransferBits->addItem(QString((const char*)querymenu.name));
				tranceferBitList << querymenu.index;
			}
		}
		int transferBits = 0;
		if(getDriverCtrlValue(STC_CID_TRANSFER_BITS_PER_PIXEL, transferBits, tranceferBitList, ui->otTransferBits) < 0) {
			return;
		}
        capWindow->setTransferBits(tranceferBitList[ui->otTransferBits->currentIndex()]);
	}

	// Display Pixel Format
	if(isMono) {
		ui->otDisplayPixFormat->setDisabled(true);
		capWindow->setDisplayPixFormat(STCAM_PIXEL_FORMAT_08_MONO_OR_RAW);
	} else {
		ui->otDisplayPixFormat->addItem(QString("BGR24"));
		displayPixFormatList << STCAM_PIXEL_FORMAT_24_BGR;
		ui->otDisplayPixFormat->addItem(QString("BGR32"));
		displayPixFormatList << STCAM_PIXEL_FORMAT_32_BGR;
		ui->otDisplayPixFormat->setCurrentIndex(0);
		capWindow->setDisplayPixFormat(STCAM_PIXEL_FORMAT_24_BGR);
	}

	// Color Interpolation
	if(isMono) {
		ui->otColorInterpolation->setDisabled(true);
		capWindow->setColorInterpolation(STCAM_COLOR_INTERPOLATION_NONE_MONO);
	} else {
		ui->otColorInterpolation->addItem(QString("None(Mono)"));
		colorInterpolationList << STCAM_COLOR_INTERPOLATION_NONE_MONO;
		ui->otColorInterpolation->addItem(QString("None(Color)"));
		colorInterpolationList << STCAM_COLOR_INTERPOLATION_NONE_COLOR;
		ui->otColorInterpolation->addItem(QString("Nearest Neighbor"));
		colorInterpolationList << STCAM_COLOR_INTERPOLATION_NEAREST_NEIGHBOR;
		ui->otColorInterpolation->addItem(QString("Bilinear"));
		colorInterpolationList << STCAM_COLOR_INTERPOLATION_BILINEAR;
		ui->otColorInterpolation->addItem(QString("Bilinear(False color reduction)"));
		colorInterpolationList << STCAM_COLOR_INTERPOLATION_BILINEAR_FALSE_COLOR_REDUCTION;
		ui->otColorInterpolation->addItem(QString("Bicubic"));
		colorInterpolationList << STCAM_COLOR_INTERPOLATION_BICUBIC;
		ui->otColorInterpolation->setCurrentIndex(3);
		capWindow->setColorInterpolation(STCAM_COLOR_INTERPOLATION_BILINEAR);
	}

	// Mirror Mode
	bool isHFlip=true;
	bool isVFlip=true;
	queryctrl.id = V4L2_CID_HFLIP;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "V4L2_CID_HFLIP QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		isHFlip = false;
	}
	queryctrl.id = V4L2_CID_VFLIP;
	retVal = ioctl(devHandle, VIDIOC_QUERYCTRL, &queryctrl);
	if (0 > retVal) {
		fprintf(stderr, "V4L2_CID_VFLIP QUERYCTRL ERROR: %d\n", retVal);
		return;
	}
	if(queryctrl.flags == V4L2_CTRL_FLAG_DISABLED) {
		isVFlip = false;
	}
	if(isHFlip && isVFlip) {
		isCameraFlip = true;
	}
	if(isCameraFlip) {
		ui->otMirrorMode->addItem(QString("Off"));
		mirrorModeList << 0;
		ui->otMirrorMode->addItem(QString("Horizontal"));
		mirrorModeList << 1;
		ui->otMirrorMode->addItem(QString("Vertical"));
		mirrorModeList << 2;
		ui->otMirrorMode->addItem(QString("Horizontal/Vertical"));
		mirrorModeList << 3;
	} else {
		ui->otMirrorMode->addItem(QString("Off"));
		mirrorModeList << 0;
		ui->otMirrorMode->addItem(QString("Horizontal(PC)"));
		mirrorModeList << 1;
		ui->otMirrorMode->addItem(QString("Vertical(PC)"));
		mirrorModeList << 2;
		ui->otMirrorMode->addItem(QString("Horizontal/Vertical(PC)"));
		mirrorModeList << 3;
	}
	ui->otMirrorMode->setCurrentIndex(0);

	// Rotation Mode
	ui->otRotationMode->addItem(QString("Off"));
	rotationModeList << STCAM_ROTATION_OFF;
	ui->otRotationMode->addItem(QString("Clockwise 90"));
	rotationModeList << STCAM_ROTATION_CLOCKWISE_90;
	ui->otRotationMode->addItem(QString("Counterclockwise 90"));
	rotationModeList << STCAM_ROTATION_COUNTERCLOCKWISE_90;
	ui->otRotationMode->setCurrentIndex(0);
}

//**********************************
// Update
//**********************************

void MainWindow::UpdateShutterGain()
{
	int retVal = 0;
	int i = 0;
	float fValue = 0;
	
	// Exposure
	if(ui->sgExposureSlider->isEnabled()) {
		int exposure = 0;
        if(getDriverCtrlValue(V4L2_CID_EXPOSURE, exposure, ui->sgExposureSlider, ui->sgExposureEdit) < 0) {
			return;
        }
		retVal = StCam_GetExposureTimeFromClock(libHandle, exposure, &fValue);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
			return;
		}
		ui->sgExposure->setText(getTimeString(fValue));
	}
	
	// Analog Gain
	if(ui->sgAnalogGainSlider->isEnabled()) {
		int analogGain = 0;
		if(getDriverCtrlValue(V4L2_CID_GAIN, analogGain, ui->sgAnalogGainSlider, ui->sgAnalogGainEdit) < 0) {
			return;
		}
		retVal = StCam_GetGainDBFromSettingValue(libHandle, analogGain, &fValue);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetGainDBFromSettingValue ERROR: %d\n", retVal);
			return;
		}
		ui->sgAnalogGain->setText(QString::number(fValue, 'g', 4));
	}

	// Digital Gain
	if(ui->sgDigitalGainSlider->isEnabled()) {
		int digitalGain = 0;
		if(getDriverCtrlValue(STC_CID_DIGITAL_GAIN, digitalGain, ui->sgDigitalGainSlider, ui->sgDigitalGainEdit) < 0) {
			return;
		}
		retVal = StCam_GetDigitalGainTimesFromSettingValue(libHandle, digitalGain, &fValue);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetDigitalGainTimesFromSettingValue ERROR: %d\n", retVal);
			return;
		}
        ui->sgDigitalGain->setText(QString::number(fValue, 'g', 4));
	}

	// ALC Mode
	if(isCameraALC) {
		int alcMode = 0;
		if(getDriverCtrlValue(STC_CID_ALC_MODE, alcMode, alcModeList, ui->sgAlcMode) < 0) {
			return;
		}
	} else {
		unsigned char alcMode;
		retVal = StCam_GetALCMode(libHandle, &alcMode);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetALCMode ERROR: %d\n", retVal);
			return;
		}
		for(i=0; i<alcModeList.length();i++) {
			if(alcModeList[i]==alcMode) {
				ui->sgAlcMode->setCurrentIndex(i);
				break;
			}
		}
	}
	
	// ALC Target
	if(isCameraALC) {
		int alcTargetLevel = 0;
		if(getDriverCtrlValue(STC_CID_ALC_TARGET_LEVEL, alcTargetLevel, ui->sgAlcTargetSlider, ui->sgAlcTargetEdit) < 0) {
			return;
		}
	} else {
		unsigned short targetLevel;
		retVal = StCam_GetALCTargetLevel(libHandle, &targetLevel);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetALCTargetLevel ERROR: %d\n", retVal);
			return;
		}
		ui->sgAlcTargetSlider->setSliderPosition(targetLevel);
		ui->sgAlcTargetEdit->setText(QString::number(targetLevel));
	}

	// AE Min Exposure
	if(isCameraALC) {
		int aeMinExposure = 0;
		if(getDriverCtrlValue(STC_CID_AE_MIN_EXPOSURE_CLOCK, aeMinExposure, ui->sgMinExposureSlider, ui->sgMinExposureEdit) < 0) {
			return;
		}
		retVal = StCam_GetExposureTimeFromClock(libHandle, aeMinExposure, &fValue);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
			return;
		}
		ui->sgMinExposure->setText(getTimeString(fValue));
	} else {
		unsigned long exposure;
		retVal = StCam_GetAEMinExposureClock(libHandle, &exposure);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetAEMinExposureClock ERROR: %d\n", retVal);
			return;
		}
		ui->sgMinExposureSlider->setSliderPosition(exposure);
		ui->sgMinExposureEdit->setText(QString::number(exposure));
		retVal = StCam_GetExposureTimeFromClock(libHandle, exposure, &fValue);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
			return;
		}
		ui->sgMinExposure->setText(getTimeString(fValue));
	}

	// AE Max Exposure
	if(isCameraALC) {
		int aeMaxExposure = 0;
		if(getDriverCtrlValue(STC_CID_AE_MAX_EXPOSURE_CLOCK, aeMaxExposure, ui->sgMaxExposureSlider, ui->sgMaxExposureEdit) < 0) {
			return;
		}
		retVal = StCam_GetExposureTimeFromClock(libHandle, aeMaxExposure, &fValue);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
			return;
		}
		ui->sgMaxExposure->setText(getTimeString(fValue));
	} else {
		unsigned long exposure;
		retVal = StCam_GetAEMaxExposureClock(libHandle, &exposure);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetAEMaxExposureClock ERROR: %d\n", retVal);
			return;
		}
		ui->sgMaxExposureSlider->setSliderPosition(exposure);
		ui->sgMaxExposureEdit->setText(QString::number(exposure));
		retVal = StCam_GetExposureTimeFromClock(libHandle, exposure, &fValue);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
			return;
		}
		ui->sgMaxExposure->setText(getTimeString(fValue));
	}

	// AGC Min Gain
	if(isCameraALC) {
		int agcMinGain = 0;
		if(getDriverCtrlValue(STC_CID_AGC_MIN_GAIN, agcMinGain, ui->sgMinGainSlider, ui->sgMinGainEdit) < 0) {
			return;
		}
		if(ui->sgAnalogGainSlider->isEnabled()) {
			retVal = StCam_GetGainDBFromSettingValue(libHandle, agcMinGain, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetGainDBFromSettingValue ERROR: %d\n", retVal);
				return;
			}
		} else {
			retVal = StCam_GetDigitalGainTimesFromSettingValue(libHandle, agcMinGain, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetDigitalGainTimesFromSettingValue ERROR: %d\n", retVal);
				return;
			}
		}
		ui->sgMinGain->setText(QString::number(fValue, 'g', 4));
	} else {
		unsigned short gain;
		retVal = StCam_GetAGCMinGain(libHandle, &gain);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetAGCMinGain ERROR: %d\n", retVal);
			return;
		}
		ui->sgMinGainSlider->setSliderPosition(gain);
		ui->sgMinGainEdit->setText(QString::number(gain));
		if(ui->sgAnalogGainSlider->isEnabled()) {
            retVal = StCam_GetGainDBFromSettingValue(libHandle, gain, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetGainDBFromSettingValue ERROR: %d\n", retVal);
				return;
			}
		} else {
            retVal = StCam_GetDigitalGainTimesFromSettingValue(libHandle, gain, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetDigitalGainTimesFromSettingValue ERROR: %d\n", retVal);
				return;
			}
		}
		ui->sgMinGain->setText(QString::number(fValue, 'g', 4));
	}
	
	// AGC Max Gain
	if(isCameraALC) {
		int agcMaxGain = 0;
		if(getDriverCtrlValue(STC_CID_AGC_MAX_GAIN, agcMaxGain, ui->sgMaxGainSlider, ui->sgMaxGainEdit) < 0) {
			return;
		}
		if(ui->sgAnalogGainSlider->isEnabled()) {
			retVal = StCam_GetGainDBFromSettingValue(libHandle, agcMaxGain, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetGainDBFromSettingValue ERROR: %d\n", retVal);
				return;
			}
		} else {
			retVal = StCam_GetDigitalGainTimesFromSettingValue(libHandle, agcMaxGain, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetDigitalGainTimesFromSettingValue ERROR: %d\n", retVal);
				return;
			}
		}
		ui->sgMaxGain->setText(QString::number(fValue, 'g', 4));
	} else {
		unsigned short gain;
		retVal = StCam_GetAGCMaxGain(libHandle, &gain);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetAGCMaxGain ERROR: %d\n", retVal);
			return;
		}
		ui->sgMaxGainSlider->setSliderPosition(gain);
		ui->sgMaxGainEdit->setText(QString::number(gain));
		if(ui->sgAnalogGainSlider->isEnabled()) {
            retVal = StCam_GetGainDBFromSettingValue(libHandle, gain, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetGainDBFromSettingValue ERROR: %d\n", retVal);
				return;
			}
		} else {
            retVal = StCam_GetDigitalGainTimesFromSettingValue(libHandle, gain, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetDigitalGainTimesFromSettingValue ERROR: %d\n", retVal);
				return;
			}
		}
		ui->sgMaxGain->setText(QString::number(fValue, 'g', 4));
	}
	
	// Adjustment Digital Gain
	if(ui->sgAdjustGain->isEnabled()) {
		int adjustmentMode = 0;
		if(getDriverCtrlValue(STC_CID_ADJUSTMENT_MODE, adjustmentMode, adjustmentModeList, ui->sgAdjustGain) < 0) {
			return;
		}
	}
}

void MainWindow::UpdateWhiteBalance()
{
	if(!ui->WhiteBalance->isEnabled()) {
		return;
	}
	
	int retVal = 0;
	int i = 0;
	float fValue = 0;
	
	// WB Mode
	if(isCameraWB) {
		int whiteBalanceMode = 0;
		if(getDriverCtrlValue(STC_CID_WHITE_BALANCE_MODE, whiteBalanceMode, wbModeList, ui->wbMode) < 0) {
			return;
		}
	} else {
		unsigned char wbMode;
		retVal = StCam_GetWhiteBalanceMode(libHandle, &wbMode);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetWhiteBalanceMode ERROR: %d\n", retVal);
			return;
		}
		for(i=0; i<wbModeList.length();i++) {
			if(wbModeList[i]==wbMode) {
				ui->wbMode->setCurrentIndex(i);
				break;
			}
		}
	}
	
	unsigned short wbGainR, wbGainGr, wbGainGb, wbGainB;
	if(!isCameraWB) {
		retVal = StCam_GetWhiteBalanceGain(libHandle, &wbGainR, &wbGainGr, &wbGainGb, &wbGainB);
		if (0 > retVal) {
			fprintf(stderr, "StCam_GetWhiteBalanceGain ERROR: %d\n", retVal);
			return;
		}
	}
	
	// R Gain
	if(isCameraWB) {
		int whiteBalanceGainR = 0;
		if(getDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_R, whiteBalanceGainR, ui->wbRGainSlider, ui->wbRGainEdit) < 0) {
			return;
		}
        fValue = (float)whiteBalanceGainR / 128;
		ui->wbRGain->setText(QString::number(fValue, 'g', 4));
	} else {
		ui->wbRGainSlider->setSliderPosition(wbGainR);
		ui->wbRGainEdit->setText(QString::number(wbGainR));
		fValue = (float)wbGainR / 128;
		ui->wbRGain->setText(QString::number(fValue, 'g', 4));
	}
	
	// Gr Gain
	if(isCameraWB) {
		int whiteBalanceGainGr = 0;
		if(getDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_GR, whiteBalanceGainGr, ui->wbGrGainSlider, ui->wbGrGainEdit) < 0) {
			return;
		}
        fValue = (float)whiteBalanceGainGr / 128;
		ui->wbGrGain->setText(QString::number(fValue, 'g', 4));
	} else {
		ui->wbGrGainSlider->setSliderPosition(wbGainGr);
		ui->wbGrGainEdit->setText(QString::number(wbGainGr));
		fValue = (float)wbGainGr / 128;
		ui->wbGrGain->setText(QString::number(fValue, 'g', 4));
	}
	
	// Gb Gain
	if(isCameraWB) {
		int whiteBalanceGainGb = 0;
		if(getDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_GB, whiteBalanceGainGb, ui->wbGbGainSlider, ui->wbGbGainEdit) < 0) {
			return;
		}
        fValue = (float)whiteBalanceGainGb / 128;
		ui->wbGbGain->setText(QString::number(fValue, 'g', 4));
	} else {
		ui->wbGbGainSlider->setSliderPosition(wbGainGb);
		ui->wbGbGainEdit->setText(QString::number(wbGainGb));
		fValue = (float)wbGainGb / 128;
		ui->wbGbGain->setText(QString::number(fValue, 'g', 4));
	}
	
	// B Gain
	if(isCameraWB) {
		int whiteBalanceGainB = 0;
		if(getDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_B, whiteBalanceGainB, ui->wbBGainSlider, ui->wbBGainEdit) < 0) {
			return;
		}
        fValue = (float)whiteBalanceGainB / 128;
		ui->wbBGain->setText(QString::number(fValue, 'g', 4));
	} else {
		ui->wbBGainSlider->setSliderPosition(wbGainB);
		ui->wbBGainEdit->setText(QString::number(wbGainB));
		fValue = (float)wbGainB / 128;
		ui->wbBGain->setText(QString::number(fValue, 'g', 4));
	}
	
	unsigned char hueSaturationMode;
	unsigned short saturation;
	short hue;
	retVal = StCam_GetHueSaturationMode(libHandle, &hueSaturationMode, &hue, &saturation);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetHueSaturationMode ERROR: %d\n", retVal);
		return;
	}
	// Hue/Saturation Mode
	for(i=0; i<hueSaturationList.length();i++) {
		if(hueSaturationList[i]==hueSaturationMode) {
			ui->wbHSMode->setCurrentIndex(i);
			break;
		}
	}
	
	// Hue
	ui->wbHueEdit->setText(QString::number(hue));
	ui->wbHueSlider->setSliderPosition(hue);
	fValue = (float)hue / 10;
	ui->wbHue->setText(QString::number(fValue, 'g', 4));
	
	// Saturation
	ui->wbSaturationSlider->setSliderPosition(saturation);
	ui->wbSaturationEdit->setText(QString::number(saturation));
	fValue = (float)saturation / 10;
	ui->wbSaturation->setText(QString::number(fValue, 'g', 4));
	
	unsigned short highStartLevel, highSuppression, lowStartLevel, lowSuppression;
	retVal = StCam_GetHighChromaSuppression(libHandle, &highStartLevel, &highSuppression);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetHighChromaSuppression ERROR: %d\n", retVal);
		return;
	}
	retVal = StCam_GetLowChromaSuppression(libHandle, &lowStartLevel, &lowSuppression);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetLowChromaSuppression ERROR: %d\n", retVal);
		return;
	}
	
	// High Chroma Suppression Start Level
	ui->wbHcsStartSlider->setSliderPosition(highStartLevel);
	ui->wbHcsStartEdit->setText(QString::number(highStartLevel));
	
	// High Chroma Suppression Suppression Level
	ui->wbHcsSuppressionSlider->setSliderPosition(highSuppression);
	ui->wbHcsSuppressionEdit->setText(QString::number(highSuppression));
	
	// Low Chroma Suppression Start Level
	ui->wbLcsStartSlider->setSliderPosition(lowStartLevel);
	ui->wbLcsStartEdit->setText(QString::number(lowStartLevel));
	
	// Low Chroma Suppression Suppression Level
	ui->wbLcsSuppressionSlider->setSliderPosition(lowSuppression);
	ui->wbLcsSuppressionEdit->setText(QString::number(lowSuppression));
}

void MainWindow::UpdateY()
{
	int retVal = 0;
	int i = 0;
	
	unsigned char gammaMode;
	unsigned short gamma;
	retVal = StCam_GetGammaMode(libHandle, STCAM_GAMMA_TARGET_Y, &gammaMode, &gamma, NULL, NULL, NULL, NULL);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetGammaMode ERROR: %d\n", retVal);
		return;
	}
	
	// Y Gamma Mode
	for(i=0; i<yGammaList.length();i++) {
		if(yGammaList[i]==gammaMode) {
			ui->yGammaMode->setCurrentIndex(i);
			break;
		}
	}
	
	// Y Gamma Value
	ui->yGammaValueSlider->setSliderPosition(gamma);
	ui->yGammaValue->setText(QString::number(gamma));
	
	// Camera Gamma
	if(ui->yCameraGammaSlider->isEnabled()) {
		int cameraGamma = 0;
		if(getDriverCtrlValue(STC_CID_CAMERA_GAMMA, cameraGamma, ui->yCameraGammaSlider, ui->yCameraGamma) < 0) {
			return;
		}
	}
	
	unsigned char sharpnessMode, sharpnessCoring;
	unsigned short sharpnessGain;
	retVal = StCam_GetSharpnessMode(libHandle, &sharpnessMode, &sharpnessGain, &sharpnessCoring);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetSharpnessMode ERROR: %d\n", retVal);
		return;
	}
	
	// Sharpness Mode
	for(i=0; i<sharpnessModeList.length();i++) {
		if(sharpnessModeList[i]==sharpnessMode) {
			ui->ySharpnessMode->setCurrentIndex(i);
			break;
		}
	}
	
	// Sharpness Gain
	ui->ySharpnessgainSlider->setSliderPosition(sharpnessGain);
	ui->ySharpnessgain->setText(QString::number(sharpnessGain));

	// Sharpness Coring
	ui->ySharpnessCoringSlider->setSliderPosition(sharpnessCoring);
	ui->ySharpnessCoring->setText(QString::number(sharpnessCoring));
	
	// Digital Clamp
	if(ui->yDigitalClampSlider->isEnabled()) {
		int digitalClamp = 0;
		if(getDriverCtrlValue(STC_CID_DIGITAL_CLAMP, digitalClamp, ui->yDigitalClampSlider, ui->yDigitalClamp) < 0) {
			return;
		}
	}
	
	// Analog Black Level
	if(ui->yAnalogBlackLevelSlider->isEnabled()) {
		int analogBlackLevel = 0;
		if(getDriverCtrlValue(STC_CID_ANALOG_BLACK_LEVEL, analogBlackLevel, ui->yAnalogBlackLevelSlider, ui->yAnalogBlackLevel) < 0) {
			return;
		}
	}
	
	// Shading Correction Target Level
	unsigned short shadingTarget;
	retVal = StCam_GetShadingCorrectionTarget(libHandle, &shadingTarget);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetShadingCorrectionTarget ERROR: %d\n", retVal);
		return;
	}
	
	ui->yShadingTargetSlider->setSliderPosition(shadingTarget);
	ui->yShadingTarget->setText(QString::number(shadingTarget));
	
	// Shading Correction Mode
	unsigned long shadingMode;
	retVal = StCam_GetShadingCorrectionMode(libHandle, &shadingMode);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetShadingCorrectionMode ERROR: %d\n", retVal);
		return;
	}
	
	for(i=0; i<shadingModeList.length();i++) {
		if(shadingModeList[i]==(int)shadingMode) {
			ui->yShadingMode->setCurrentIndex(i);
			break;
		}
	}
}

void MainWindow::UpdateColorGamma()
{
	if(!ui->ColorGamma->isEnabled()) {
		return;
	}
	
	int retVal = 0;
	int i = 0;

	unsigned char gammaMode;
	unsigned short gamma;
	
	retVal = StCam_GetGammaMode(libHandle, STCAM_GAMMA_TARGET_R, &gammaMode, &gamma, NULL, NULL, NULL, NULL);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetGammaMode ERROR: %d\n", retVal);
		return;
	}
	
	// R Gamma Mode
	for(i=0; i<rGammaList.length();i++) {
		if(rGammaList[i]==gammaMode) {
			ui->cgRGammaMode->setCurrentIndex(i);
			break;
		}
	}
	
	// R Gamma Value
	ui->cgRGammaValueSlider->setSliderPosition(gamma);
	ui->cgRGammaValue->setText(QString::number(gamma));
	
	retVal = StCam_GetGammaMode(libHandle, STCAM_GAMMA_TARGET_GR, &gammaMode, &gamma, NULL, NULL, NULL, NULL);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetGammaMode ERROR: %d\n", retVal);
		return;
	}
	
	// Gr Gamma Mode
	for(i=0; i<grGammaList.length();i++) {
		if(grGammaList[i]==gammaMode) {
			ui->cgGrGammaMode->setCurrentIndex(i);
			break;
		}
	}
	
	// Gr Gamma Value
	ui->cgGrGammaValueSlider->setSliderPosition(gamma);
	ui->cgGrGammaValue->setText(QString::number(gamma));
	
	retVal = StCam_GetGammaMode(libHandle, STCAM_GAMMA_TARGET_GB, &gammaMode, &gamma, NULL, NULL, NULL, NULL);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetGammaMode ERROR: %d\n", retVal);
		return;
	}
	
	// Gb Gamma Mode
	for(i=0; i<gbGammaList.length();i++) {
		if(gbGammaList[i]==gammaMode) {
			ui->cgGbGammaMode->setCurrentIndex(i);
			break;
		}
	}
	
	// Gb Gamma Value
	ui->cgGbGammaValueSlider->setSliderPosition(gamma);
	ui->cgGbGammaValue->setText(QString::number(gamma));
	
	retVal = StCam_GetGammaMode(libHandle, STCAM_GAMMA_TARGET_B, &gammaMode, &gamma, NULL, NULL, NULL, NULL);
	if (0 > retVal) {
		fprintf(stderr, "StCam_GetGammaMode ERROR: %d\n", retVal);
		return;
	}
	
	// B Gamma Mode
	for(i=0; i<bGammaList.length();i++) {
		if(bGammaList[i]==gammaMode) {
			ui->cgBGammaMode->setCurrentIndex(i);
			break;
		}
	}
	
	// B Gamma Value
	ui->cgBGammaValueSlider->setSliderPosition(gamma);
	ui->cgBGammaValue->setText(QString::number(gamma));
	
}

void MainWindow::UpdateTriggerMode()
{
	int retVal = 0;
	float fValue = 0;
	
	// Exposure Mode
	if(ui->tmExposureMode->isEnabled()) {
		int exposureMode = 0;
		if(getDriverCtrlValue(STC_CID_EXPOSURE_MODE, exposureMode, exposureModeList, ui->tmExposureMode) < 0) {
			return;
		}
	}
	
	// Trigger Selector
	int triggerSelect = triggerSelectorList[ui->tmTriggerSelect->currentIndex()];
	struct st_trigger_command {
		unsigned int triggerMode;
		unsigned int triggerSource;
		unsigned int triggerDeray;
		unsigned int triggerOverlap;
	};
	struct st_trigger_command triggerCommand[] = {
		{ STC_CID_FRAME_START_TRIGGER_MODE, STC_CID_FRAME_START_TRIGGER_SOURCE, STC_CID_FRAME_START_TRIGGER_DELAY, STC_CID_FRAME_START_TRIGGER_OVERLAP},
		{ STC_CID_FRAME_BURST_START_TRIGGER_MODE, STC_CID_FRAME_BURST_START_TRIGGER_SOURCE, STC_CID_FRAME_BURST_START_TRIGGER_DELAY, STC_CID_FRAME_BURST_START_TRIGGER_OVERLAP },
		{ STC_CID_EXPOSURE_START_TRIGGER_MODE, STC_CID_EXPOSURE_START_TRIGGER_SOURCE, STC_CID_EXPOSURE_START_TRIGGER_DELAY, STC_CID_EXPOSURE_START_TRIGGER_OVERLAP },
		{ STC_CID_EXPOSURE_END_TRIGGER_MODE, STC_CID_EXPOSURE_END_TRIGGER_SOURCE, STC_CID_EXPOSURE_END_TRIGGER_DELAY, STC_CID_EXPOSURE_END_TRIGGER_OVERLAP },
		{ STC_CID_READ_OUT_START_TRIGGER_MODE, STC_CID_READ_OUT_START_TRIGGER_SOURCE, STC_CID_READ_OUT_START_TRIGGER_DELAY, STC_CID_READ_OUT_START_TRIGGER_OVERLAP }
	};
    if(triggerSelect > (int)(sizeof(triggerCommand)/sizeof(struct st_trigger_command))) {
		return;
	}
	
	// Trigger Mode
	if(ui->tmTriggerMode->isEnabled()) {
		int triggerMode = 0;
		if(getDriverCtrlValue(triggerCommand[triggerSelect].triggerMode, triggerMode, triggerModeList, ui->tmTriggerMode) < 0) {
			return;
		}
	}

	// Trigger Source
	if(ui->tmTriggerSource->isEnabled()) {
		int triggerSource = 0;
		if(getDriverCtrlValue(triggerCommand[triggerSelect].triggerSource, triggerSource, triggerSourceList, ui->tmTriggerSource) < 0) {
			return;
		}
	}

	// Trigger Delay
	if(ui->tmTriggerDelaySlider->isEnabled()) {
		int triggerDeray = 0;
		if(getDriverCtrlValue(triggerCommand[triggerSelect].triggerDeray, triggerDeray, ui->tmTriggerDelaySlider, ui->tmTriggerDelayEdit) < 0) {
			return;
		}
		if(triggerDeray != 0) {
			retVal = StCam_GetExposureTimeFromClock(libHandle, triggerDeray, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
				return;
			}
		} else {
			fValue = 0;
		}
		ui->tmTriggerDelay->setText(getTimeString(fValue));
	}

	// Trigger Overlap
	if(ui->tmTriggerOverlap->isEnabled()) {
		int triggerOverlap = 0;
		if(getDriverCtrlValue(triggerCommand[triggerSelect].triggerOverlap, triggerOverlap, triggerOverlapList, ui->tmTriggerOverlap) < 0) {
			return;
		}
	}
	
	// Auto ExposureEnd Trigger
	ui->tmAutoExposureEndEdit->setText(QString::number(autoExposureEndTime));
	ui->tmAutoExposureEndSlider->setSliderPosition(autoExposureEndTime);
	
	// Noise Reduction
	
	// Wait HD
	if(ui->tmWaitHd->isEnabled()) {
		int waitHd = 0;
		if(getDriverCtrlValue(STC_CID_TRIGGER_WAIT_HD, waitHd, waitHdList, ui->tmWaitHd) < 0) {
			return;
		}
	}
	
	// Wait Read Out
	if(ui->tmWaitReadOut->isEnabled()) {
		int waitReadout = 0;
		if(getDriverCtrlValue(STC_CID_TRIGGER_WAIT_READOUT, waitReadout, waitReadOutList, ui->tmWaitReadOut) < 0) {
			return;
		}
	}
	
	// Sensor Shutter Mode
	if(ui->tmShutterMode->isEnabled()) {
		int shutterMode = 0;
		if(getDriverCtrlValue(STC_CID_SENSOR_SHUTTER_MODE, shutterMode, sensorShutterModeList, ui->tmShutterMode) < 0) {
			return;
		}
	}
}

void MainWindow::UpdateTriggerTiming()
{
	int retVal = 0;
	float fValue = 0;
	
	// Strobe Start Delay
	if(ui->ttStrobeStartSlider->isEnabled()) {
		int strobeStart = 0;
		if(getDriverCtrlValue(STC_CID_STROBE_START_DELAY, strobeStart, ui->ttStrobeStartSlider, ui->ttStrobeStartEdit) < 0) {
			return;
		}
		if(strobeStart != 0) {
			retVal = StCam_GetExposureTimeFromClock(libHandle, strobeStart, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
				return;
			}
		} else {
			fValue = 0;
		}
		ui->ttStrobeStart->setText(getTimeString(fValue));
	}
	
	// Strobe End Delay
	if(ui->ttStrobeEndSlider->isEnabled()) {
		int strobeEnd = 0;
		if(getDriverCtrlValue(STC_CID_STROBE_END_DELAY, strobeEnd, ui->ttStrobeEndSlider, ui->ttStrobeEndEdit) < 0) {
			return;
		}
		if(strobeEnd != 0) {
			retVal = StCam_GetExposureTimeFromClock(libHandle, strobeEnd, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
				return;
			}
		} else {
			fValue = 0;
		}
		ui->ttStrobeEnd->setText(getTimeString(fValue));
	}
	
	// Output Pulse Delay
	if(ui->ttPulseDelaySlider->isEnabled()) {
		int pulseDelay = 0;
		if(getDriverCtrlValue(STC_CID_OUTPUT_PULSE_DELAY, pulseDelay, ui->ttPulseDelaySlider, ui->ttPulseDelayEdit) < 0) {
			return;
		}
		if(pulseDelay != 0) {
			retVal = StCam_GetExposureTimeFromClock(libHandle, pulseDelay, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
				return;
			}
		} else {
			fValue = 0;
		}
		ui->ttPulseDelay->setText(getTimeString(fValue));
	}
	
	// Output Pulse Duration
	if(ui->ttPulseDurationSlider->isEnabled()) {
		int pulseDuration = 0;
		if(getDriverCtrlValue(STC_CID_OUTPUT_PULSE_DURATION, pulseDuration, ui->ttPulseDurationSlider, ui->ttPulseDurationEdit) < 0) {
			return;
		}
		if(pulseDuration != 0) {
			retVal = StCam_GetExposureTimeFromClock(libHandle, pulseDuration, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
				return;
			}
		} else {
			fValue = 0;
		}
		ui->ttPulseDuration->setText(getTimeString(fValue));
	}

	// Read Out Delay
	if(ui->ttReadOutSlider->isEnabled()) {
		int readoutDelay = 0;
		if(getDriverCtrlValue(STC_CID_READOUT_DELAY, readoutDelay, ui->ttReadOutSlider, ui->ttReadOutEdit) < 0) {
			return;
		}
		if(readoutDelay != 0) {
			retVal = StCam_GetExposureTimeFromClock(libHandle, readoutDelay, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
				return;
			}
		} else {
			fValue = 0;
		}
		ui->ttReadOut->setText(getTimeString(fValue));
	}

	// Line Debounce Time
	if(ui->ttLineDebounceSlider->isEnabled()) {
		int lineDebounce = 0;
		if(getDriverCtrlValue(STC_CID_LINE_DEBOUNCE_TIME, lineDebounce, ui->ttLineDebounceSlider, ui->ttLineDebounceEdit) < 0) {
			return;
		}
		if(lineDebounce != 0) {
			retVal = StCam_GetExposureTimeFromClock(libHandle, lineDebounce, &fValue);
			if (0 > retVal) {
				fprintf(stderr, "StCam_GetExposureTimeFromClock ERROR: %d\n", retVal);
				return;
			}
		} else {
			fValue = 0;
		}
		ui->ttLineDebounce->setText(getTimeString(fValue));
	}
}

void MainWindow::UpdateIO()
{
	// IO0 IN/OUT
	if(ui->io0InOut->isEnabled() || isIo0ReadOnly) {
		int io0InOut = 0;
		if(getDriverCtrlValue(STC_CID_IO0_DIRECTION, io0InOut, io0InOutList, ui->io0InOut) < 0) {
			return;
		}
	}
	
	// IO1 IN/OUT
	if(ui->io1InOut->isEnabled() || isIo1ReadOnly) {
		int io1InOut = 0;
		if(getDriverCtrlValue(STC_CID_IO1_DIRECTION, io1InOut, io1InOutList, ui->io1InOut) < 0) {
			return;
		}
	}
	
	// IO2 IN/OUT
	if(ui->io2InOut->isEnabled() || isIo2ReadOnly) {
		int io2InOut = 0;
		if(getDriverCtrlValue(STC_CID_IO2_DIRECTION, io2InOut, io2InOutList, ui->io2InOut) < 0) {
			return;
		}
	}
	
	// IO3 IN/OUT
	if(ui->io3InOut->isEnabled() || isIo3ReadOnly) {
		int io3InOut = 0;
		if(getDriverCtrlValue(STC_CID_IO3_DIRECTION, io3InOut, io3InOutList, ui->io3InOut) < 0) {
			return;
		}
	}
	
	// IO0 MODE
	if(ui->io0Mode->isEnabled()) {
		int io0Mode = 0;
		if(getDriverCtrlValue(STC_CID_IO0_MODE, io0Mode, io0ModeList, ui->io0Mode) < 0) {
			return;
		}
	}
	
	// IO1 MODE
	if(ui->io1Mode->isEnabled()) {
		int io1Mode = 0;
		if(getDriverCtrlValue(STC_CID_IO1_MODE, io1Mode, io1ModeList, ui->io1Mode) < 0) {
			return;
		}
	}
	
	// IO2 MODE
	if(ui->io2Mode->isEnabled()) {
		int io2Mode = 0;
		if(getDriverCtrlValue(STC_CID_IO2_MODE, io2Mode, io2ModeList, ui->io2Mode) < 0) {
			return;
		}
	}
	
	// IO3 MODE
	if(ui->io3Mode->isEnabled()) {
		int io3Mode = 0;
		if(getDriverCtrlValue(STC_CID_IO3_MODE, io3Mode, io3ModeList, ui->io3Mode) < 0) {
			return;
		}
	}
	
	// IO0 NEGA/POSI
	if(ui->io0NegaPosi->isEnabled()) {
		int io0NegaPosi = 0;
		if(getDriverCtrlValue(STC_CID_IO0_POLARITY, io0NegaPosi, io0NegaPosiList, ui->io0NegaPosi) < 0) {
			return;
		}
	}
	
	// IO1 NEGA/POSI
	if(ui->io1NegaPosi->isEnabled()) {
		int io1NegaPosi = 0;
		if(getDriverCtrlValue(STC_CID_IO1_POLARITY, io1NegaPosi, io1NegaPosiList, ui->io1NegaPosi) < 0) {
			return;
		}
	}
	
	// IO2 NEGA/POSI
	if(ui->io2NegaPosi->isEnabled()) {
		int io2NegaPosi = 0;
		if(getDriverCtrlValue(STC_CID_IO2_POLARITY, io2NegaPosi, io2NegaPosiList, ui->io2NegaPosi) < 0) {
			return;
		}
	}
	
	// IO3 NEGA/POSI
	if(ui->io3NegaPosi->isEnabled()) {
		int io3NegaPosi = 0;
		if(getDriverCtrlValue(STC_CID_IO3_POLARITY, io3NegaPosi, io3NegaPosiList, ui->io3NegaPosi) < 0) {
			return;
		}
	}
	
	// IO0 STATUS
	if(ui->io0Status->isEnabled()) {
		int io0Status = 0;
		if(getDriverCtrlValue(STC_CID_IO0_STATUS, io0Status, io0StatusList, ui->io0Status) < 0) {
			return;
		}
	}
	
	// IO1 STATUS
	if(ui->io1Status->isEnabled()) {
		int io1Status = 0;
		if(getDriverCtrlValue(STC_CID_IO1_STATUS, io1Status, io1StatusList, ui->io1Status) < 0) {
			return;
		}
	}
	
	// IO2 STATUS
	if(ui->io2Status->isEnabled()) {
		int io2Status = 0;
		if(getDriverCtrlValue(STC_CID_IO2_STATUS, io2Status, io2StatusList, ui->io2Status) < 0) {
			return;
		}
	}
	
	// IO3 STATUS
	if(ui->io3Status->isEnabled()) {
		int io3Status = 0;
		if(getDriverCtrlValue(STC_CID_IO3_STATUS, io3Status, io3StatusList, ui->io3Status) < 0) {
			return;
		}
	}
	
	// Reset Switch
	if(ui->ioResetSwitch->isEnabled()) {
		int resetSwitch = 0;
		if(getDriverCtrlValue(STC_CID_RESET_SWITCH, resetSwitch, resetSwitchList, ui->ioResetSwitch) < 0) {
			return;
		}
	}
	
	// SW
	if(hasSw) {
		int swStatus = 0;
		if(getDriverCtrlValue(STC_CID_SW_STATUS, swStatus) < 0) {
			return;
		}
		ui->ioSw0->setText(QString::number(swStatus & 0x0001));
		ui->ioSw1->setText(QString::number((swStatus >> 1) & 0x0001));
		ui->ioSw2->setText(QString::number((swStatus >> 2) & 0x0001));
		ui->ioSw3->setText(QString::number((swStatus >> 3) & 0x0001));
	}
	
	// LED
	if(ui->ioLed->isEnabled()) {
		int led = 0;
		if(getDriverCtrlValue(STC_CID_LED_STATUS, led, ledList, ui->ioLed) < 0) {
			return;
		}
	}
	
	// Temperature
	if(hasTemperature) {
		int temperature = 0;
		if(getDriverCtrlValue(STC_CID_TEMPERATURE, temperature) < 0) {
			return;
		}
		ui->ioTemperature->setText(QString::number(temperature));
	}
}

void MainWindow::UpdateHDR()
{
	if(!ui->HDR->isEnabled()) {
		return;
	}
	
	int retVal = 0;
	int i = 0;

	struct stcam_st_hdr hdr;
	retVal = ioctl(devHandle, STCIOC_G_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_G_HDR ERROR: %d\n", retVal);
		return;
	}
	
	// HDR Mode
	for(i=0; i<hdrModeList.length();i++) {
		if(hdrModeList[i]==hdr.cmosis_4m.mode) {
			ui->hdrMode->setCurrentIndex(i);
			break;
		}
	}
	
	// HDR Slope Num
	for(i=0; i<hdrSlopeNumList.length();i++) {
		if(hdrSlopeNumList[i]==hdr.cmosis_4m.slope) {
			ui->hdrSlope->setCurrentIndex(i);
			break;
		}
	}
	
	// Knee1
	ui->hdrKnee1Edit->setText(QString::number(hdr.cmosis_4m.knee1));
	ui->hdrKnee1Slider->setSliderPosition(hdr.cmosis_4m.knee1);

	// Vlow2
	ui->hdrVlow2Edit->setText(QString::number(hdr.cmosis_4m.vlow2));
	ui->hdrVlow2Slider->setSliderPosition(hdr.cmosis_4m.vlow2);

	// Knee2
	ui->hdrKnee2Edit->setText(QString::number(hdr.cmosis_4m.knee2));
	ui->hdrKnee2Slider->setSliderPosition(hdr.cmosis_4m.knee2);

	// Vlow3
	ui->hdrVlow3Edit->setText(QString::number(hdr.cmosis_4m.vlow3));
	ui->hdrVlow3Slider->setSliderPosition(hdr.cmosis_4m.vlow3);
}

void MainWindow::UpdateEeprom()
{
	// No Need Update
}

void MainWindow::UpdateOther()
{
	int retVal = 0;
	int i = 0;
	
	// Scan Mode
	if(ui->otScanMode->isEnabled()) {
		int scanMode = 0;
		if(getDriverCtrlValue(STC_CID_SCAN_MODE, scanMode, scanModeList, ui->otScanMode) < 0) {
			return;
		}
	}
	
	// H Decimation Binning
	if(ui->otHDeciBin->isEnabled()) {
		int hDeciBin = 0;
		if(getDriverCtrlValue(STC_CID_H_DECIMATION_BINNING, hDeciBin, hBinDeciList, ui->otHDeciBin) < 0) {
			return;
		}
	}
	
	// V Decimation Binning
	if(ui->otVDeciBin->isEnabled()) {
		int vDeciBin = 0;
		if(getDriverCtrlValue(STC_CID_V_DECIMATION_BINNING, vDeciBin, vBinDeciList, ui->otVDeciBin) < 0) {
			return;
		}
	}
	
	// H/V Binning Sum
	if(ui->otBinSum->isEnabled()) {
		int binSum = 0;
		if(getDriverCtrlValue(STC_CID_BINNINGSUM_MODE, binSum, binSumList, ui->otBinSum) < 0) {
			return;
		}
	}

    struct stcam_st_scan_size scanSize;
    scanSize.roi.index = regionList[ui->otRegion->currentIndex()];
    scanSize.roi.region_mode = 1;
    scanSize.roi.offset_x = 100;
    scanSize.roi.offset_y = 200;
    scanSize.roi.width = 300;
    scanSize.roi.height = 400;
	retVal = ioctl(devHandle, STCIOC_G_SCAN_SIZE, &scanSize);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_G_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}
	
	// Region Mode
	for(i=0; i<regionModeList.length();i++) {
		if(regionModeList[i]==scanSize.roi.region_mode) {
			ui->otRegionMode->setCurrentIndex(i);
			break;
		}
	}
	
	// Offset X
	ui->otOffsetXEdit->setText(QString::number(scanSize.roi.offset_x));
	ui->otOffsetXSlider->setSliderPosition(scanSize.roi.offset_x);

	// Offset Y
	ui->otOffsetYEdit->setText(QString::number(scanSize.roi.offset_y));
	ui->otOffsetYSlider->setSliderPosition(scanSize.roi.offset_y);

	// Width
	ui->otWidthEdit->setText(QString::number(scanSize.roi.width));
	ui->otWidthSlider->setSliderPosition(scanSize.roi.width);

	// Height
	ui->otHeightEdit->setText(QString::number(scanSize.roi.height));
	ui->otHeightSlider->setSliderPosition(scanSize.roi.height);

	// V Blank for FPS
	if(ui->otVBlankSlider->isEnabled()) {
		int vBlank = 0;
		if(getDriverCtrlValue(STC_CID_V_BLANK, vBlank, ui->otVBlankSlider, ui->otVBlankEdit) < 0) {
			return;
		}
	}
	
	// Clock
	if(ui->otClock->isEnabled()) {
		int clock = 0;
		if(getDriverCtrlValue(STC_CID_CLOCK, clock, clockList, ui->otClock) < 0) {
			return;
		}
	}
	
	// FPS
	int fps = 0;
	if(getDriverCtrlValue(STC_CID_OUTPUT_FPS, fps) < 0) {
		return;
	}
	ui->otFps->setText(QString::number(fps));
	
	// Transfer bits
	if(ui->otTransferBits->isEnabled()) {
		int transferBits = 0;
		if(getDriverCtrlValue(STC_CID_TRANSFER_BITS_PER_PIXEL, transferBits, tranceferBitList, ui->otTransferBits) < 0) {
			return;
		}
	}
	
	// Display Pixel Format
	
	// Color Interpolation
	
	// Mirror Mode
	
	// Rotation Mode
}

//**********************************
// slots
//**********************************

void MainWindow::on_sgExposureSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(V4L2_CID_EXPOSURE, position);
}

void MainWindow::on_sgExposureEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(V4L2_CID_EXPOSURE, ui->sgExposureEdit->text().toInt());
}

void MainWindow::on_sgAnalogGainSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(V4L2_CID_GAIN, position);
}

void MainWindow::on_sgAnalogGainEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(V4L2_CID_GAIN, ui->sgAnalogGainEdit->text().toInt());
}

void MainWindow::on_sgDigitalGainSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_DIGITAL_GAIN, position);
}

void MainWindow::on_sgDigitalGainEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_DIGITAL_GAIN, ui->sgDigitalGainEdit->text().toInt());
}

void MainWindow::on_sgAlcMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	if(isCameraALC) {
		setDriverCtrlValue(STC_CID_ALC_MODE, alcModeList[index]);
	} else {
		int retVal = StCam_SetALCMode(libHandle, alcModeList[index]);
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetALCMode ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_sgAlcTargetSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	if(isCameraALC) {
		setDriverCtrlValue(STC_CID_ALC_TARGET_LEVEL, position);
	} else {
		int retVal = StCam_SetALCTargetLevel(libHandle, position);
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetALCTargetLevel ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_sgAlcTargetEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	if(isCameraALC) {
		setDriverCtrlValue(STC_CID_ALC_TARGET_LEVEL, ui->sgAlcTargetEdit->text().toInt());
	} else {
		int retVal = StCam_SetALCTargetLevel(libHandle, ui->sgAlcTargetEdit->text().toInt());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetALCTargetLevel ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_sgMinExposureSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	if(isCameraALC) {
		setDriverCtrlValue(STC_CID_AE_MIN_EXPOSURE_CLOCK, position);
	} else {
		int retVal = StCam_SetAEMinExposureClock(libHandle, position);
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetAEMinExposureClock ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_sgMinExposureEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	if(isCameraALC) {
		setDriverCtrlValue(STC_CID_AE_MIN_EXPOSURE_CLOCK, ui->sgMinExposureEdit->text().toInt());
	} else {
		int retVal = StCam_SetAEMinExposureClock(libHandle, ui->sgMinExposureEdit->text().toInt());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetAEMinExposureClock ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_sgMaxExposureSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	if(isCameraALC) {
        setDriverCtrlValue(STC_CID_AE_MAX_EXPOSURE_CLOCK, position);
	} else {
		int retVal = StCam_SetAEMaxExposureClock(libHandle, position);
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetAEMaxExposureClock ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_sgMaxExposureEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	if(isCameraALC) {
        setDriverCtrlValue(STC_CID_AE_MAX_EXPOSURE_CLOCK, ui->sgMaxExposureEdit->text().toInt());
	} else {
		int retVal = StCam_SetAEMaxExposureClock(libHandle, ui->sgMaxExposureEdit->text().toInt());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetAEMaxExposureClock ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_sgMinGainSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	if(isCameraALC) {
        setDriverCtrlValue(STC_CID_AGC_MIN_GAIN, position);
	} else {
		int retVal = StCam_SetAGCMinGain(libHandle, position);
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetAGCMinGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_sgMinGainEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	if(isCameraALC) {
        setDriverCtrlValue(STC_CID_AGC_MIN_GAIN, ui->sgMinGainEdit->text().toInt());
	} else {
		int retVal = StCam_SetAGCMinGain(libHandle, ui->sgMinGainEdit->text().toInt());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetAGCMinGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_sgMaxGainSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	if(isCameraALC) {
        setDriverCtrlValue(STC_CID_AGC_MAX_GAIN, position);
	} else {
		int retVal = StCam_SetAGCMaxGain(libHandle, position);
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetAGCMaxGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_sgMaxGainEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	if(isCameraALC) {
        setDriverCtrlValue(STC_CID_AGC_MAX_GAIN, ui->sgMaxGainEdit->text().toInt());
	} else {
		int retVal = StCam_SetAGCMaxGain(libHandle, ui->sgMaxGainEdit->text().toInt());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetAGCMaxGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_sgAdjustGain_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_ADJUSTMENT_MODE, adjustmentModeList[index]);
}

void MainWindow::on_wbMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	if(isCameraWB) {
        setDriverCtrlValue(STC_CID_WHITE_BALANCE_MODE, wbModeList[index]);
	} else {
		int retVal = StCam_SetWhiteBalanceMode(libHandle, wbModeList[index]);
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetWhiteBalanceMode ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_wbRGainSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	if(isCameraWB) {
        setDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_R, position);
	} else {
		int retVal = StCam_SetWhiteBalanceGain(libHandle, position, ui->wbGrGainSlider->value(), ui->wbGbGainSlider->value(), ui->wbBGainSlider->value());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetWhiteBalanceGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_wbRGainEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	if(isCameraWB) {
		setDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_R, ui->wbRGainEdit->text().toInt());
	} else {
		int retVal = StCam_SetWhiteBalanceGain(libHandle, ui->wbRGainEdit->text().toInt(), ui->wbGrGainSlider->value(), ui->wbGbGainSlider->value(), ui->wbBGainSlider->value());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetWhiteBalanceGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_wbGrGainSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	if(isCameraWB) {
		setDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_GR, position);
	} else {
		int retVal = StCam_SetWhiteBalanceGain(libHandle, ui->wbRGainSlider->value(), position, ui->wbGbGainSlider->value(), ui->wbBGainSlider->value());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetWhiteBalanceGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_wbGrGainEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	if(isCameraWB) {
		setDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_GR, ui->wbGrGainEdit->text().toInt());
	} else {
		int retVal = StCam_SetWhiteBalanceGain(libHandle, ui->wbRGainSlider->value(), ui->wbGrGainEdit->text().toInt(), ui->wbGbGainSlider->value(), ui->wbBGainSlider->value());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetWhiteBalanceGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_wbGbGainSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	if(isCameraWB) {
		setDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_GB, position);
	} else {
		int retVal = StCam_SetWhiteBalanceGain(libHandle, ui->wbRGainSlider->value(), ui->wbGrGainSlider->value(), position, ui->wbBGainSlider->value());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetWhiteBalanceGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_wbGbGainEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	if(isCameraWB) {
		setDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_GB, ui->wbGbGainEdit->text().toInt());
	} else {
		int retVal = StCam_SetWhiteBalanceGain(libHandle, ui->wbRGainSlider->value(), ui->wbGrGainSlider->value(), ui->wbGbGainEdit->text().toInt(), ui->wbBGainSlider->value());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetWhiteBalanceGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_wbBGainSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	if(isCameraWB) {
		setDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_B, position);
	} else {
		int retVal = StCam_SetWhiteBalanceGain(libHandle, ui->wbRGainSlider->value(), ui->wbGrGainSlider->value(), ui->wbGbGainSlider->value(), position);
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetWhiteBalanceGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_wbBGainEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	if(isCameraWB) {
		setDriverCtrlValue(STC_CID_WHITE_BALANCE_GAIN_B, ui->wbBGainEdit->text().toInt());
	} else {
		int retVal = StCam_SetWhiteBalanceGain(libHandle, ui->wbRGainSlider->value(), ui->wbGrGainSlider->value(), ui->wbGbGainSlider->value(), ui->wbBGainEdit->text().toInt());
		if (0 > retVal) {
			fprintf(stderr, "StCam_SetWhiteBalanceGain ERROR: %d\n", retVal);
			return;
		}
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_wbHSMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetHueSaturationMode(libHandle, hueSaturationList[index], ui->wbHueSlider->value(), ui->wbSaturationSlider->value());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetHueSaturationMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbHueSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetHueSaturationMode(libHandle, hueSaturationList[ui->wbHSMode->currentIndex()], position, ui->wbSaturationSlider->value());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetHueSaturationMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbHueEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetHueSaturationMode(libHandle, hueSaturationList[ui->wbHSMode->currentIndex()], ui->wbHueEdit->text().toInt(), ui->wbSaturationSlider->value());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetHueSaturationMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbSaturationSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetHueSaturationMode(libHandle, hueSaturationList[ui->wbHSMode->currentIndex()], ui->wbHueSlider->value(), position);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetHueSaturationMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbSaturationEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetHueSaturationMode(libHandle, hueSaturationList[ui->wbHSMode->currentIndex()], ui->wbHueSlider->value(), ui->wbSaturationEdit->text().toInt());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetHueSaturationMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbHcsStartSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetHighChromaSuppression(libHandle, position, ui->wbHcsSuppressionSlider->value());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetHighChromaSuppression ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbHcsStartEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetHighChromaSuppression(libHandle, ui->wbHcsStartEdit->text().toInt(), ui->wbHcsSuppressionSlider->value());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetHighChromaSuppression ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbHcsSuppressionSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetHighChromaSuppression(libHandle, ui->wbHcsStartSlider->value(), position);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetHighChromaSuppression ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbHcsSuppressionEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetHighChromaSuppression(libHandle, ui->wbHcsStartSlider->value(), ui->wbHcsSuppressionEdit->text().toInt());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetHighChromaSuppression ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbLcsStartSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetLowChromaSuppression(libHandle, position, ui->wbLcsSuppressionSlider->value());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetLowChromaSuppression ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbLcsStartEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetLowChromaSuppression(libHandle, ui->wbLcsStartEdit->text().toInt(), ui->wbLcsSuppressionSlider->value());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetLowChromaSuppression ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbLcsSuppressionSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetLowChromaSuppression(libHandle, ui->wbLcsStartSlider->value(), position);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetLowChromaSuppression ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_wbLcsSuppressionEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetLowChromaSuppression(libHandle, ui->wbLcsStartSlider->value(), ui->wbLcsSuppressionEdit->text().toInt());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetLowChromaSuppression ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_yGammaMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetGammaMode(libHandle, STCAM_GAMMA_TARGET_Y, yGammaList[index], ui->yGammaValueSlider->value(), 0, 0, NULL, 8);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetGammaMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_yGammaValueSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetGammaMode(libHandle, STCAM_GAMMA_TARGET_Y, yGammaList[ui->yGammaMode->currentIndex()], position, 0, 0, NULL, 8);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetGammaMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_yCameraGammaSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_CAMERA_GAMMA, position);
}

void MainWindow::on_ySharpnessMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetSharpnessMode(libHandle, sharpnessModeList[index], ui->ySharpnessgainSlider->value(), ui->ySharpnessCoringSlider->value());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetSharpnessMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_ySharpnessgainSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetSharpnessMode(libHandle, sharpnessModeList[ui->ySharpnessMode->currentIndex()], position, ui->ySharpnessCoringSlider->value());
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetSharpnessMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_ySharpnessCoringSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetSharpnessMode(libHandle, sharpnessModeList[ui->ySharpnessMode->currentIndex()], ui->ySharpnessgainSlider->value(), position);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetSharpnessMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_yDigitalClampSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_DIGITAL_CLAMP, position);
}

void MainWindow::on_yAnalogBlackLevelSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_ANALOG_BLACK_LEVEL, position);
}

void MainWindow::on_yShadingTargetSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetShadingCorrectionTarget(libHandle, position);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetShadingCorrectionTarget ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_yShadingMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetShadingCorrectionMode(libHandle,shadingModeList[index]);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetShadingCorrectionMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_cgRGammaMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetGammaMode(libHandle, STCAM_GAMMA_TARGET_R, rGammaList[index], ui->cgRGammaValueSlider->value(), 0, 0, NULL, 8);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetGammaMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_cgRGammaValueSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetGammaMode(libHandle, STCAM_GAMMA_TARGET_R, rGammaList[ui->cgRGammaMode->currentIndex()], position, 0, 0, NULL, 8);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetGammaMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_cgGrGammaMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetGammaMode(libHandle, STCAM_GAMMA_TARGET_GR, grGammaList[index], ui->cgGrGammaValueSlider->value(), 0, 0, NULL, 8);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetGammaMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_cgGrGammaValueSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetGammaMode(libHandle, STCAM_GAMMA_TARGET_GR, grGammaList[ui->cgGrGammaMode->currentIndex()], position, 0, 0, NULL, 8);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetGammaMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_cgGbGammaMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetGammaMode(libHandle, STCAM_GAMMA_TARGET_GB, gbGammaList[index], ui->cgGbGammaValueSlider->value(), 0, 0, NULL, 8);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetGammaMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_cgGbGammaValueSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetGammaMode(libHandle, STCAM_GAMMA_TARGET_GB, gbGammaList[ui->cgGbGammaMode->currentIndex()], position, 0, 0, NULL, 8);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetGammaMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_cgBGammaMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetGammaMode(libHandle, STCAM_GAMMA_TARGET_B, gbGammaList[index], ui->cgBGammaValueSlider->value(), 0, 0, NULL, 8);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetGammaMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_cgBGammaValueSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	int retVal = StCam_SetGammaMode(libHandle, STCAM_GAMMA_TARGET_B, gbGammaList[ui->cgBGammaMode->currentIndex()], position, 0, 0, NULL, 8);
	if (0 > retVal) {
		fprintf(stderr, "StCam_SetGammaMode ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tmExposureMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_EXPOSURE_MODE, exposureModeList[index]);
}

void MainWindow::on_tmTriggerSelect_currentIndexChanged(int)
{
	if(!isEnableSettingChange)return;
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tmTriggerMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	struct v4l2_control control;
	switch (triggerSelectorList[ui->tmTriggerSelect->currentIndex()]) {
	case 1:
		control.id = STC_CID_FRAME_BURST_START_TRIGGER_MODE;
		break;
	case 2:
		control.id = STC_CID_EXPOSURE_START_TRIGGER_MODE;
		break;
	case 3:
		control.id = STC_CID_EXPOSURE_END_TRIGGER_MODE;
		break;
	case 4:
		control.id = STC_CID_READ_OUT_START_TRIGGER_MODE;
		break;
	case 0:
	default:
		control.id = STC_CID_FRAME_START_TRIGGER_MODE;
		break;
	}
	control.value = triggerModeList[index];
	int retVal = ioctl(devHandle, VIDIOC_S_CTRL, &control);
	if (0 > retVal) {
		fprintf(stderr, "TRIGGER_MODE SET ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tmTriggerSource_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	struct v4l2_control control;
	switch (triggerSelectorList[ui->tmTriggerSelect->currentIndex()]) {
	case 1:
		control.id = STC_CID_FRAME_BURST_START_TRIGGER_SOURCE;
		break;
	case 2:
		control.id = STC_CID_EXPOSURE_START_TRIGGER_SOURCE;
		break;
	case 3:
		control.id = STC_CID_EXPOSURE_END_TRIGGER_SOURCE;
		break;
	case 4:
		control.id = STC_CID_READ_OUT_START_TRIGGER_SOURCE;
		break;
	case 0:
	default:
		control.id = STC_CID_FRAME_START_TRIGGER_SOURCE;
		break;
	}
	control.value = triggerSourceList[index];
	int retVal = ioctl(devHandle, VIDIOC_S_CTRL, &control);
	if (0 > retVal) {
		fprintf(stderr, "TRIGGER_SOURCE SET ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tmTriggerDelaySlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	struct v4l2_control control;
	switch (triggerSelectorList[ui->tmTriggerSelect->currentIndex()]) {
	case 1:
		control.id = STC_CID_FRAME_BURST_START_TRIGGER_DELAY;
		break;
	case 2:
		control.id = STC_CID_EXPOSURE_START_TRIGGER_DELAY;
		break;
	case 3:
		control.id = STC_CID_EXPOSURE_END_TRIGGER_DELAY;
		break;
	case 4:
		control.id = STC_CID_READ_OUT_START_TRIGGER_DELAY;
		break;
	case 0:
	default:
		control.id = STC_CID_FRAME_START_TRIGGER_DELAY;
		break;
	}
	control.value = position;
	int retVal = ioctl(devHandle, VIDIOC_S_CTRL, &control);
	if (0 > retVal) {
		fprintf(stderr, "TRIGGER_DELAY SET ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tmTriggerDelayEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	struct v4l2_control control;
	switch (triggerSelectorList[ui->tmTriggerSelect->currentIndex()]) {
	case 1:
		control.id = STC_CID_FRAME_BURST_START_TRIGGER_DELAY;
		break;
	case 2:
		control.id = STC_CID_EXPOSURE_START_TRIGGER_DELAY;
		break;
	case 3:
		control.id = STC_CID_EXPOSURE_END_TRIGGER_DELAY;
		break;
	case 4:
		control.id = STC_CID_READ_OUT_START_TRIGGER_DELAY;
		break;
	case 0:
	default:
		control.id = STC_CID_FRAME_START_TRIGGER_DELAY;
		break;
	}
	control.value = ui->tmTriggerDelayEdit->text().toInt();
	int retVal = ioctl(devHandle, VIDIOC_S_CTRL, &control);
	if (0 > retVal) {
		fprintf(stderr, "TRIGGER_DELAY SET ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tmTriggerOverlap_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	struct v4l2_control control;
	switch (triggerSelectorList[ui->tmTriggerSelect->currentIndex()]) {
	case 1:
		control.id = STC_CID_FRAME_BURST_START_TRIGGER_OVERLAP;
		break;
	case 2:
		control.id = STC_CID_EXPOSURE_START_TRIGGER_OVERLAP;
		break;
	case 3:
		control.id = STC_CID_EXPOSURE_END_TRIGGER_OVERLAP;
		break;
	case 4:
		control.id = STC_CID_READ_OUT_START_TRIGGER_OVERLAP;
		break;
	case 0:
	default:
		control.id = STC_CID_FRAME_START_TRIGGER_OVERLAP;
		break;
	}
	control.value = triggerOverlapList[index];
	int retVal = ioctl(devHandle, VIDIOC_S_CTRL, &control);
	if (0 > retVal) {
		fprintf(stderr, "TRIGGER_OVERLAP SET ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tmFrameStart_clicked()
{
	if(!isEnableSettingChange)return;
	struct v4l2_control control;
	control.id = STC_CID_TRIGGER_SOFTWARE;
	if(triggerSelectorList[ui->tmTriggerSelect->currentIndex()] == 1){
		control.value = STC_MENU_TRIGGER_SOFTWARE_FRAME_BURST_START;
	} else {
		control.value = STC_MENU_TRIGGER_SOFTWARE_FRAME_START;
	}
	int retVal = ioctl(devHandle, VIDIOC_S_CTRL, &control);
	if (0 > retVal) {
		fprintf(stderr, "STC_MENU_TRIGGER_SOFTWARE_FRAME_START SET ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tmExposureStart_clicked()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_TRIGGER_SOFTWARE, STC_MENU_TRIGGER_SOFTWARE_EXPOSURE_START);
	if(isAutoExposureEnd) {
		autoExposureEndTimerId = startTimer(autoExposureEndTime);
	}
}

void MainWindow::on_tmExposureEnd_clicked()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_TRIGGER_SOFTWARE, STC_MENU_TRIGGER_SOFTWARE_EXPOSURE_END);
}

void MainWindow::on_tmReadOut_clicked()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_TRIGGER_SOFTWARE, STC_MENU_TRIGGER_SOFTWARE_READ_OUT_START);
}

void MainWindow::on_tmAutoExposureEndCheck_clicked(bool checked)
{
	if(!isEnableSettingChange)return;
	if(checked) {
		ui->tmAutoExposureEndSlider->setDisabled(false);
		ui->tmAutoExposureEndEdit->setDisabled(false);
		isAutoExposureEnd = true;
	} else {
		ui->tmAutoExposureEndSlider->setDisabled(true);
		ui->tmAutoExposureEndEdit->setDisabled(true);
		isAutoExposureEnd = false;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tmAutoExposureEndSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	autoExposureEndTime = position;
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tmAutoExposureEndEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	autoExposureEndTime = ui->tmAutoExposureEndEdit->text().toInt();
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tmNoiseReduction_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	capWindow->setNoiseReduction(noiseReductionList[index]);
}

void MainWindow::on_tmWaitHd_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_TRIGGER_WAIT_HD, waitHdList[index]);
}

void MainWindow::on_tmWaitReadOut_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_TRIGGER_WAIT_READOUT, waitReadOutList[index]);
}

void MainWindow::on_tmShutterMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_SENSOR_SHUTTER_MODE, sensorShutterModeList[index]);
}

void MainWindow::on_ttStrobeStartSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_STROBE_START_DELAY, position);
}

void MainWindow::on_ttStrobeStartEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_STROBE_START_DELAY, ui->ttStrobeStartEdit->text().toInt());
}

void MainWindow::on_ttStrobeEndSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_STROBE_END_DELAY, position);
}

void MainWindow::on_ttStrobeEndEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_STROBE_END_DELAY, ui->ttStrobeEndEdit->text().toInt());
}

void MainWindow::on_ttPulseDelaySlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_OUTPUT_PULSE_DELAY, position);
}

void MainWindow::on_ttPulseDelayEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_OUTPUT_PULSE_DELAY, ui->ttPulseDelayEdit->text().toInt());
}

void MainWindow::on_ttPulseDurationSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_OUTPUT_PULSE_DURATION, position);
}

void MainWindow::on_ttPulseDurationEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_OUTPUT_PULSE_DURATION, ui->ttPulseDurationEdit->text().toInt());
}

void MainWindow::on_ttReadOutSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_READOUT_DELAY, position);
}

void MainWindow::on_ttReadOutEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_READOUT_DELAY, ui->ttReadOutEdit->text().toInt());
}

void MainWindow::on_ttLineDebounceSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_LINE_DEBOUNCE_TIME, position);
}

void MainWindow::on_ttLineDebounceEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_LINE_DEBOUNCE_TIME, ui->ttLineDebounceEdit->text().toInt());
}

void MainWindow::on_io0InOut_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO0_DIRECTION, io0InOutList[index]);
}

void MainWindow::on_io1InOut_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO1_DIRECTION, io1InOutList[index]);
}

void MainWindow::on_io2InOut_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO2_DIRECTION, io2InOutList[index]);
}

void MainWindow::on_io3InOut_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO3_DIRECTION, io3InOutList[index]);
}

void MainWindow::on_io0Mode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO0_MODE, io0ModeList[index]);
}

void MainWindow::on_io1Mode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO1_MODE, io1ModeList[index]);
}

void MainWindow::on_io2Mode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO2_MODE, io2ModeList[index]);
}

void MainWindow::on_io3Mode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO3_MODE, io3ModeList[index]);
}

void MainWindow::on_io0NegaPosi_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO0_POLARITY, io0NegaPosiList[index]);
}

void MainWindow::on_io1NegaPosi_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO1_POLARITY, io1NegaPosiList[index]);
}

void MainWindow::on_io2NegaPosi_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO2_POLARITY, io2NegaPosiList[index]);
}

void MainWindow::on_io3NegaPosi_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO3_POLARITY, io3NegaPosiList[index]);
}

void MainWindow::on_io0Status_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO0_STATUS, io0StatusList[index]);
}

void MainWindow::on_io1Status_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO1_STATUS, io1StatusList[index]);
}

void MainWindow::on_io2Status_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO2_STATUS, io2StatusList[index]);
}

void MainWindow::on_io3Status_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_IO3_STATUS, io3StatusList[index]);
}

void MainWindow::on_ioResetSwitch_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_RESET_SWITCH, resetSwitchList[index]);
}

void MainWindow::on_ioLed_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_LED_STATUS, ledList[index]);
}

void MainWindow::on_ioResetCounter_clicked()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_RESET_COUNTER, 0);
}

void MainWindow::on_ioClearBuffer_clicked()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_CLEAR_BUFFER, 0);
}

void MainWindow::on_hdrMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	struct stcam_st_hdr hdr;
	hdr.cmosis_4m.mode = hdrModeList[index];
	hdr.cmosis_4m.slope = hdrSlopeNumList[ui->hdrSlope->currentIndex()];
	hdr.cmosis_4m.knee1 = ui->hdrKnee1Slider->value();
	hdr.cmosis_4m.vlow2 = ui->hdrVlow2Slider->value();
	hdr.cmosis_4m.knee2 = ui->hdrKnee2Slider->value();
	hdr.cmosis_4m.vlow3 = ui->hdrVlow3Slider->value();
	int retVal = ioctl(devHandle, STCIOC_S_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_HDR ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_hdrSlope_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	struct stcam_st_hdr hdr;
	hdr.cmosis_4m.mode = hdrModeList[ui->hdrMode->currentIndex()];
	hdr.cmosis_4m.slope = hdrSlopeNumList[index];
	hdr.cmosis_4m.knee1 = ui->hdrKnee1Slider->value();
	hdr.cmosis_4m.vlow2 = ui->hdrVlow2Slider->value();
	hdr.cmosis_4m.knee2 = ui->hdrKnee2Slider->value();
	hdr.cmosis_4m.vlow3 = ui->hdrVlow3Slider->value();
	int retVal = ioctl(devHandle, STCIOC_S_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_HDR ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_hdrKnee1Slider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	struct stcam_st_hdr hdr;
	hdr.cmosis_4m.mode = hdrModeList[ui->hdrMode->currentIndex()];
	hdr.cmosis_4m.slope = hdrSlopeNumList[ui->hdrSlope->currentIndex()];
	hdr.cmosis_4m.knee1 = position;
	hdr.cmosis_4m.vlow2 = ui->hdrVlow2Slider->value();
	hdr.cmosis_4m.knee2 = ui->hdrKnee2Slider->value();
	hdr.cmosis_4m.vlow3 = ui->hdrVlow3Slider->value();
	int retVal = ioctl(devHandle, STCIOC_S_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_HDR ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_hdrKnee1Edit_editingFinished()
{
	if(!isEnableSettingChange)return;
	struct stcam_st_hdr hdr;
	hdr.cmosis_4m.mode = hdrModeList[ui->hdrMode->currentIndex()];
	hdr.cmosis_4m.slope = hdrSlopeNumList[ui->hdrSlope->currentIndex()];
	hdr.cmosis_4m.knee1 = ui->hdrKnee1Edit->text().toInt();
	hdr.cmosis_4m.vlow2 = ui->hdrVlow2Slider->value();
	hdr.cmosis_4m.knee2 = ui->hdrKnee2Slider->value();
	hdr.cmosis_4m.vlow3 = ui->hdrVlow3Slider->value();
	int retVal = ioctl(devHandle, STCIOC_S_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_HDR ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_hdrVlow2Slider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	struct stcam_st_hdr hdr;
	hdr.cmosis_4m.mode = hdrModeList[ui->hdrMode->currentIndex()];
	hdr.cmosis_4m.slope = hdrSlopeNumList[ui->hdrSlope->currentIndex()];
	hdr.cmosis_4m.knee1 = ui->hdrKnee1Slider->value();
	hdr.cmosis_4m.vlow2 = position;
	hdr.cmosis_4m.knee2 = ui->hdrKnee2Slider->value();
	hdr.cmosis_4m.vlow3 = ui->hdrVlow3Slider->value();
	int retVal = ioctl(devHandle, STCIOC_S_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_HDR ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_hdrVlow2Edit_editingFinished()
{
	if(!isEnableSettingChange)return;
	struct stcam_st_hdr hdr;
	hdr.cmosis_4m.mode = hdrModeList[ui->hdrMode->currentIndex()];
	hdr.cmosis_4m.slope = hdrSlopeNumList[ui->hdrSlope->currentIndex()];
	hdr.cmosis_4m.knee1 = ui->hdrKnee1Slider->value();
	hdr.cmosis_4m.vlow2 = ui->hdrVlow2Edit->text().toInt();
	hdr.cmosis_4m.knee2 = ui->hdrKnee2Slider->value();
	hdr.cmosis_4m.vlow3 = ui->hdrVlow3Slider->value();
	int retVal = ioctl(devHandle, STCIOC_S_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_HDR ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_hdrKnee2Slider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	struct stcam_st_hdr hdr;
	hdr.cmosis_4m.mode = hdrModeList[ui->hdrMode->currentIndex()];
	hdr.cmosis_4m.slope = hdrSlopeNumList[ui->hdrSlope->currentIndex()];
	hdr.cmosis_4m.knee1 = ui->hdrKnee1Slider->value();
	hdr.cmosis_4m.vlow2 = ui->hdrVlow2Slider->value();
	hdr.cmosis_4m.knee2 = position;
	hdr.cmosis_4m.vlow3 = ui->hdrVlow3Slider->value();
	int retVal = ioctl(devHandle, STCIOC_S_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_HDR ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_hdrKnee2Edit_editingFinished()
{
	if(!isEnableSettingChange)return;
	struct stcam_st_hdr hdr;
	hdr.cmosis_4m.mode = hdrModeList[ui->hdrMode->currentIndex()];
	hdr.cmosis_4m.slope = hdrSlopeNumList[ui->hdrSlope->currentIndex()];
	hdr.cmosis_4m.knee1 = ui->hdrKnee1Slider->value();
	hdr.cmosis_4m.vlow2 = ui->hdrVlow2Slider->value();
	hdr.cmosis_4m.knee2 = ui->hdrKnee2Edit->text().toInt();
	hdr.cmosis_4m.vlow3 = ui->hdrVlow3Slider->value();
	int retVal = ioctl(devHandle, STCIOC_S_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_HDR ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_hdrVlow3Slider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	struct stcam_st_hdr hdr;
	hdr.cmosis_4m.mode = hdrModeList[ui->hdrMode->currentIndex()];
	hdr.cmosis_4m.slope = hdrSlopeNumList[ui->hdrSlope->currentIndex()];
	hdr.cmosis_4m.knee1 = ui->hdrKnee1Slider->value();
	hdr.cmosis_4m.vlow2 = ui->hdrVlow2Slider->value();
	hdr.cmosis_4m.knee2 = ui->hdrKnee2Slider->value();
	hdr.cmosis_4m.vlow3 = position;
	int retVal = ioctl(devHandle, STCIOC_S_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_HDR ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_hdrVlow3Edit_editingFinished()
{
	if(!isEnableSettingChange)return;
	struct stcam_st_hdr hdr;
	hdr.cmosis_4m.mode = hdrModeList[ui->hdrMode->currentIndex()];
	hdr.cmosis_4m.slope = hdrSlopeNumList[ui->hdrSlope->currentIndex()];
	hdr.cmosis_4m.knee1 = ui->hdrKnee1Slider->value();
	hdr.cmosis_4m.vlow2 = ui->hdrVlow2Slider->value();
	hdr.cmosis_4m.knee2 = ui->hdrKnee2Slider->value();
	hdr.cmosis_4m.vlow3 = ui->hdrVlow3Edit->text().toInt();
	int retVal = ioctl(devHandle, STCIOC_S_HDR, &hdr);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_HDR ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_eepLoadStandard_clicked()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_CAMERA_SETTING, STC_MENU_CAMERA_SETTING_STANDARD_READ);
}

void MainWindow::on_eepSaveStandard_clicked()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_CAMERA_SETTING, STC_MENU_CAMERA_SETTING_STANDARD_WRITE);
}

void MainWindow::on_eepReset_clicked()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_CAMERA_SETTING, STC_MENU_CAMERA_SETTING_INITIALIZE);
}

void MainWindow::on_eepLoadDpp_clicked()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_CAMERA_SETTING, STC_MENU_CAMERA_SETTING_DEF_PIX_POS_READ);
}

void MainWindow::on_eepSaveDpp_clicked()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_CAMERA_SETTING, STC_MENU_CAMERA_SETTING_DEF_PIX_POS_WRITE);
}

void MainWindow::on_otScanMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_SCAN_MODE, scanModeList[index]);
}

void MainWindow::on_otHDeciBin_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_H_DECIMATION_BINNING, hBinDeciList[index]);
}

void MainWindow::on_otVDeciBin_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_V_DECIMATION_BINNING, vBinDeciList[index]);
}

void MainWindow::on_otBinSum_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_BINNINGSUM_MODE, binSumList[index]);
}

void MainWindow::on_otRegion_currentIndexChanged(int)
{
	if(!isEnableSettingChange)return;
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otRegionMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	struct stcam_st_scan_size scanSize;
	scanSize.roi.index = regionList[ui->otRegion->currentIndex()];
	scanSize.roi.region_mode = regionModeList[index];
	scanSize.roi.offset_x = ui->otOffsetXSlider->value();
	scanSize.roi.offset_y = ui->otOffsetYSlider->value();
	scanSize.roi.width = ui->otWidthSlider->value();
	scanSize.roi.height = ui->otHeightSlider->value();
	int retVal = ioctl(devHandle, STCIOC_S_SCAN_SIZE, &scanSize);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otOffsetXSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	struct stcam_st_scan_size scanSize;
	scanSize.roi.index = regionList[ui->otRegion->currentIndex()];
	scanSize.roi.region_mode = regionModeList[ui->otRegionMode->currentIndex()];
	scanSize.roi.offset_x = position;
	scanSize.roi.offset_y = ui->otOffsetYSlider->value();
	scanSize.roi.width = ui->otWidthSlider->value();
	scanSize.roi.height = ui->otHeightSlider->value();
	int retVal = ioctl(devHandle, STCIOC_S_SCAN_SIZE, &scanSize);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otOffsetXEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	struct stcam_st_scan_size scanSize;
	scanSize.roi.index = regionList[ui->otRegion->currentIndex()];
	scanSize.roi.region_mode = regionModeList[ui->otRegionMode->currentIndex()];
	scanSize.roi.offset_x = ui->otOffsetXEdit->text().toInt();
	scanSize.roi.offset_y = ui->otOffsetYSlider->value();
	scanSize.roi.width = ui->otWidthSlider->value();
	scanSize.roi.height = ui->otHeightSlider->value();
	int retVal = ioctl(devHandle, STCIOC_S_SCAN_SIZE, &scanSize);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otOffsetYSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	struct stcam_st_scan_size scanSize;
	scanSize.roi.index = regionList[ui->otRegion->currentIndex()];
	scanSize.roi.region_mode = regionModeList[ui->otRegionMode->currentIndex()];
	scanSize.roi.offset_x = ui->otOffsetXSlider->value();
	scanSize.roi.offset_y = position;
	scanSize.roi.width = ui->otWidthSlider->value();
	scanSize.roi.height = ui->otHeightSlider->value();
	int retVal = ioctl(devHandle, STCIOC_S_SCAN_SIZE, &scanSize);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otOffsetYEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	struct stcam_st_scan_size scanSize;
	scanSize.roi.index = regionList[ui->otRegion->currentIndex()];
	scanSize.roi.region_mode = regionModeList[ui->otRegionMode->currentIndex()];
	scanSize.roi.offset_x = ui->otOffsetXSlider->value();
	scanSize.roi.offset_y = ui->otOffsetYEdit->text().toInt();
	scanSize.roi.width = ui->otWidthSlider->value();
	scanSize.roi.height = ui->otHeightSlider->value();
	int retVal = ioctl(devHandle, STCIOC_S_SCAN_SIZE, &scanSize);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otWidthSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	struct stcam_st_scan_size scanSize;
	scanSize.roi.index = regionList[ui->otRegion->currentIndex()];
	scanSize.roi.region_mode = regionModeList[ui->otRegionMode->currentIndex()];
	scanSize.roi.offset_x = ui->otOffsetXSlider->value();
	scanSize.roi.offset_y = ui->otOffsetYSlider->value();
	scanSize.roi.width = position;
	scanSize.roi.height = ui->otHeightSlider->value();
	int retVal = ioctl(devHandle, STCIOC_S_SCAN_SIZE, &scanSize);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otWidthEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	struct stcam_st_scan_size scanSize;
	scanSize.roi.index = regionList[ui->otRegion->currentIndex()];
	scanSize.roi.region_mode = regionModeList[ui->otRegionMode->currentIndex()];
	scanSize.roi.offset_x = ui->otOffsetXSlider->value();
	scanSize.roi.offset_y = ui->otOffsetYSlider->value();
	scanSize.roi.width = ui->otWidthEdit->text().toInt();
	scanSize.roi.height = ui->otHeightSlider->value();
	int retVal = ioctl(devHandle, STCIOC_S_SCAN_SIZE, &scanSize);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otHeightSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	struct stcam_st_scan_size scanSize;
	scanSize.roi.index = regionList[ui->otRegion->currentIndex()];
	scanSize.roi.region_mode = regionModeList[ui->otRegionMode->currentIndex()];
	scanSize.roi.offset_x = ui->otOffsetXSlider->value();
	scanSize.roi.offset_y = ui->otOffsetYSlider->value();
	scanSize.roi.width = ui->otWidthSlider->value();
	scanSize.roi.height = position;
	int retVal = ioctl(devHandle, STCIOC_S_SCAN_SIZE, &scanSize);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otHeightEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	struct stcam_st_scan_size scanSize;
	scanSize.roi.index = regionList[ui->otRegion->currentIndex()];
	scanSize.roi.region_mode = regionModeList[ui->otRegionMode->currentIndex()];
	scanSize.roi.offset_x = ui->otOffsetXSlider->value();
	scanSize.roi.offset_y = ui->otOffsetYSlider->value();
	scanSize.roi.width = ui->otWidthSlider->value();
	scanSize.roi.height = ui->otHeightEdit->text().toInt();
	int retVal = ioctl(devHandle, STCIOC_S_SCAN_SIZE, &scanSize);
	if (0 > retVal) {
		fprintf(stderr, "STCIOC_S_SCAN_SIZE ERROR: %d\n", retVal);
		return;
	}
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otVBlankSlider_sliderMoved(int position)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_V_BLANK, position);
}

void MainWindow::on_otVBlankEdit_editingFinished()
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_V_BLANK, ui->otVBlankEdit->text().toInt());
}

void MainWindow::on_otClock_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_CLOCK, clockList[index]);
}

void MainWindow::on_otTransferBits_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	setDriverCtrlValue(STC_CID_TRANSFER_BITS_PER_PIXEL, tranceferBitList[index]);
	capWindow->setTransferBits(tranceferBitList[index]);
}

void MainWindow::on_otDisplayPixFormat_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	capWindow->setDisplayPixFormat(displayPixFormatList[index]);
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otColorInterpolation_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	capWindow->setColorInterpolation(colorInterpolationList[index]);
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_otMirrorMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	if(isCameraFlip) {
		switch (mirrorModeList[index]) {
		case 1:
			setDriverCtrlValue(V4L2_CID_HFLIP, 1);
			setDriverCtrlValue(V4L2_CID_VFLIP, 0);
			break;
		case 2:
			setDriverCtrlValue(V4L2_CID_HFLIP, 0);
			setDriverCtrlValue(V4L2_CID_VFLIP, 1);
			break;
		case 3:
			setDriverCtrlValue(V4L2_CID_HFLIP, 1);
			setDriverCtrlValue(V4L2_CID_VFLIP, 1);
			break;
		case 0:
		default:
			setDriverCtrlValue(V4L2_CID_HFLIP, 0);
			setDriverCtrlValue(V4L2_CID_VFLIP, 0);
			break;
		}
	} else {
		capWindow->setMirrorMode(mirrorModeList[index]);
		isEnableDisplayChange = true;
		update();
	}
}

void MainWindow::on_otRotationMode_currentIndexChanged(int index)
{
	if(!isEnableSettingChange)return;
	capWindow->setRotationMode(rotationModeList[index]);
	isEnableDisplayChange = true;
	update();
}

void MainWindow::on_tabWidget_currentChanged(int /*index*/)
{
    isEnableDisplayChange = true;
    update();
}

//**********************************
// local utility
//**********************************

QString MainWindow::getTimeString(float time)
{
	if((time < 1) && (time != 0)) {
		time *= 1000;
		if(time < 1) {
			time *= 1000;
			if(time < 1) {
				time *= 1000;
				return QString(QString::number(time, 'g', 4) + "[ns]");
			}
			return QString(QString::number(time, 'g', 4) + "[us]");
		}
		return QString(QString::number(time, 'g', 4) + "[ms]");
	}
	return QString(QString::number(time, 'g', 4) + "[s]");
}

int MainWindow::getDriverCtrlValue(unsigned int id, int &value)
{
    struct v4l2_control control;
    control.id = id;
    value = 0;
    int ret = ioctl(devHandle, VIDIOC_G_CTRL, &control);
    if (0 > ret) {
        fprintf(stderr, "VIDIOC_G_CTRL GET ERROR: id[%d] error[%d]\n", id, ret);
    }
    value = control.value;
    return ret;
}

int MainWindow::getDriverCtrlValue(unsigned int id, int &value, QSlider *slider, QLineEdit *lineEdit)
{
    int ret = getDriverCtrlValue(id, value);
    if(ret < 0) {
		return ret;
	}
    slider->setSliderPosition(value);
    lineEdit->setText(QString::number(value));
	return 0;
}

int MainWindow::getDriverCtrlValue(unsigned int id, int &value, QList<int> list, QComboBox *comboBox)
{
    int ret = getDriverCtrlValue(id, value);
    if(ret < 0) {
		return ret;
	}
	for(int i=0; i<list.length();i++) {
		if(list[i]==value) {
			comboBox->setCurrentIndex(i);
			break;
		}
	}
	return 0;
}

int MainWindow::setDriverCtrlValue(unsigned int id, int value)
{
	struct v4l2_control control;
	control.id = id;
	control.value = value;
	int ret = ioctl(devHandle, VIDIOC_S_CTRL, &control);
    if (0 > ret) {
		fprintf(stderr, "VIDIOC_S_CTRL GET ERROR: id[%d] error[%d]\n", id, ret);
		return ret;
	}
	isEnableDisplayChange = true;
	update();
	return 0;
}


