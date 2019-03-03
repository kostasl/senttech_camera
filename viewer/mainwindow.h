#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QSlider>
#include <QLineEdit>
#include <QComboBox>
#include "capturewindow.h"
#include "cameraid.h"
#include "settingfile.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

	void Init(int argc, char *argv[]);

protected:
	void closeEvent(QCloseEvent *);
	void paintEvent(QPaintEvent *);
	void timerEvent(QTimerEvent *);

private slots:
	// slot for Main
	void on_mainCapture_clicked();
	void on_mainCameraId_clicked();
	void on_mainSettingFile_clicked();

	// slot for ShutterGain
	void on_sgExposureSlider_sliderMoved(int position);
    void on_sgExposureEdit_editingFinished();
	void on_sgAnalogGainSlider_sliderMoved(int position);
	void on_sgAnalogGainEdit_editingFinished();
	void on_sgDigitalGainSlider_sliderMoved(int position);
	void on_sgDigitalGainEdit_editingFinished();
	void on_sgAlcMode_currentIndexChanged(int index);
	void on_sgAlcTargetSlider_sliderMoved(int position);
	void on_sgAlcTargetEdit_editingFinished();
	void on_sgMinExposureSlider_sliderMoved(int position);
	void on_sgMinExposureEdit_editingFinished();
	void on_sgMaxExposureSlider_sliderMoved(int position);
	void on_sgMaxExposureEdit_editingFinished();
	void on_sgMinGainSlider_sliderMoved(int position);
	void on_sgMinGainEdit_editingFinished();
	void on_sgMaxGainSlider_sliderMoved(int position);
	void on_sgMaxGainEdit_editingFinished();
	void on_sgAdjustGain_currentIndexChanged(int index);

	// slot for WhiteBalance
	void on_wbMode_currentIndexChanged(int index);
	void on_wbRGainSlider_sliderMoved(int position);
	void on_wbRGainEdit_editingFinished();
	void on_wbGrGainSlider_sliderMoved(int position);
	void on_wbGrGainEdit_editingFinished();
	void on_wbGbGainSlider_sliderMoved(int position);
	void on_wbGbGainEdit_editingFinished();
	void on_wbBGainSlider_sliderMoved(int position);
	void on_wbBGainEdit_editingFinished();
	void on_wbHSMode_currentIndexChanged(int index);
	void on_wbHueSlider_sliderMoved(int position);
	void on_wbHueEdit_editingFinished();
	void on_wbSaturationSlider_sliderMoved(int position);
	void on_wbSaturationEdit_editingFinished();
	void on_wbHcsStartSlider_sliderMoved(int position);
	void on_wbHcsStartEdit_editingFinished();
	void on_wbHcsSuppressionSlider_sliderMoved(int position);
	void on_wbHcsSuppressionEdit_editingFinished();
	void on_wbLcsStartSlider_sliderMoved(int position);
	void on_wbLcsStartEdit_editingFinished();
	void on_wbLcsSuppressionSlider_sliderMoved(int position);
	void on_wbLcsSuppressionEdit_editingFinished();

	// slot for Y
	void on_yGammaMode_currentIndexChanged(int index);
	void on_yGammaValueSlider_sliderMoved(int position);
	void on_yCameraGammaSlider_sliderMoved(int position);
	void on_ySharpnessMode_currentIndexChanged(int index);
	void on_ySharpnessgainSlider_sliderMoved(int position);
	void on_ySharpnessCoringSlider_sliderMoved(int position);
	void on_yDigitalClampSlider_sliderMoved(int position);
	void on_yAnalogBlackLevelSlider_sliderMoved(int position);
	void on_yShadingTargetSlider_sliderMoved(int position);
	void on_yShadingMode_currentIndexChanged(int index);
	
	// slot for ColorGamma
	void on_cgRGammaMode_currentIndexChanged(int index);
	void on_cgRGammaValueSlider_sliderMoved(int position);
	void on_cgGrGammaMode_currentIndexChanged(int index);
	void on_cgGrGammaValueSlider_sliderMoved(int position);
	void on_cgGbGammaMode_currentIndexChanged(int index);
	void on_cgGbGammaValueSlider_sliderMoved(int position);
	void on_cgBGammaMode_currentIndexChanged(int index);
	void on_cgBGammaValueSlider_sliderMoved(int position);

	// slot for TriggerMode
	void on_tmExposureMode_currentIndexChanged(int index);
	void on_tmTriggerSelect_currentIndexChanged(int index);
	void on_tmTriggerMode_currentIndexChanged(int index);
	void on_tmTriggerSource_currentIndexChanged(int index);
	void on_tmTriggerDelaySlider_sliderMoved(int position);
	void on_tmTriggerDelayEdit_editingFinished();
	void on_tmTriggerOverlap_currentIndexChanged(int index);
	void on_tmFrameStart_clicked();
	void on_tmExposureStart_clicked();
	void on_tmExposureEnd_clicked();
	void on_tmReadOut_clicked();
	void on_tmAutoExposureEndSlider_sliderMoved(int position);
	void on_tmAutoExposureEndEdit_editingFinished();
	void on_tmNoiseReduction_currentIndexChanged(int index);
	void on_tmWaitHd_currentIndexChanged(int index);
	void on_tmWaitReadOut_currentIndexChanged(int index);
	void on_tmShutterMode_currentIndexChanged(int index);
	void on_tmAutoExposureEndCheck_clicked(bool checked);
	
	// slot for TriggerTiming
	void on_ttStrobeStartSlider_sliderMoved(int position);
	void on_ttStrobeStartEdit_editingFinished();
	void on_ttStrobeEndSlider_sliderMoved(int position);
	void on_ttStrobeEndEdit_editingFinished();
	void on_ttPulseDelaySlider_sliderMoved(int position);
	void on_ttPulseDelayEdit_editingFinished();
	void on_ttPulseDurationSlider_sliderMoved(int position);
	void on_ttPulseDurationEdit_editingFinished();
	void on_ttReadOutSlider_sliderMoved(int position);
	void on_ttReadOutEdit_editingFinished();
	void on_ttLineDebounceSlider_sliderMoved(int position);
	void on_ttLineDebounceEdit_editingFinished();

	// slot for IO
	void on_io0InOut_currentIndexChanged(int index);
	void on_io1InOut_currentIndexChanged(int index);
	void on_io2InOut_currentIndexChanged(int index);
	void on_io3InOut_currentIndexChanged(int index);
	void on_io0Mode_currentIndexChanged(int index);
	void on_io1Mode_currentIndexChanged(int index);
	void on_io2Mode_currentIndexChanged(int index);
	void on_io3Mode_currentIndexChanged(int index);
	void on_io0NegaPosi_currentIndexChanged(int index);
	void on_io1NegaPosi_currentIndexChanged(int index);
	void on_io2NegaPosi_currentIndexChanged(int index);
	void on_io3NegaPosi_currentIndexChanged(int index);
	void on_io0Status_currentIndexChanged(int index);
	void on_io1Status_currentIndexChanged(int index);
	void on_io2Status_currentIndexChanged(int index);
	void on_io3Status_currentIndexChanged(int index);
	void on_ioResetSwitch_currentIndexChanged(int index);
	void on_ioLed_currentIndexChanged(int index);
	void on_ioResetCounter_clicked();
	void on_ioClearBuffer_clicked();

	// slot for EEPROM
	void on_eepLoadStandard_clicked();
	void on_eepSaveStandard_clicked();
	void on_eepReset_clicked();
	void on_eepLoadDpp_clicked();
	void on_eepSaveDpp_clicked();

	// slot for HDR
	void on_hdrMode_currentIndexChanged(int index);
	void on_hdrSlope_currentIndexChanged(int index);
	void on_hdrKnee1Slider_sliderMoved(int position);
	void on_hdrKnee1Edit_editingFinished();
	void on_hdrVlow2Slider_sliderMoved(int position);
	void on_hdrVlow2Edit_editingFinished();
	void on_hdrKnee2Slider_sliderMoved(int position);
	void on_hdrKnee2Edit_editingFinished();
	void on_hdrVlow3Slider_sliderMoved(int position);
	void on_hdrVlow3Edit_editingFinished();

	// slot for other
	void on_otScanMode_currentIndexChanged(int index);
	void on_otHDeciBin_currentIndexChanged(int index);
	void on_otVDeciBin_currentIndexChanged(int index);
	void on_otBinSum_currentIndexChanged(int index);
	void on_otRegion_currentIndexChanged(int index);
	void on_otRegionMode_currentIndexChanged(int index);
	void on_otOffsetXSlider_sliderMoved(int position);
	void on_otOffsetXEdit_editingFinished();
	void on_otOffsetYSlider_sliderMoved(int position);
	void on_otOffsetYEdit_editingFinished();
	void on_otWidthSlider_sliderMoved(int position);
	void on_otWidthEdit_editingFinished();
	void on_otHeightSlider_sliderMoved(int position);
	void on_otHeightEdit_editingFinished();
	void on_otVBlankSlider_sliderMoved(int position);
	void on_otVBlankEdit_editingFinished();
	void on_otClock_currentIndexChanged(int index);
	void on_otTransferBits_currentIndexChanged(int index);
	void on_otDisplayPixFormat_currentIndexChanged(int index);
	void on_otColorInterpolation_currentIndexChanged(int index);
	void on_otMirrorMode_currentIndexChanged(int index);
	void on_otRotationMode_currentIndexChanged(int index);

    void on_tabWidget_currentChanged(int index);

private:
	// initialize
	void InitShutterGain();
	void InitWhiteBalance(bool isMono);
	void InitY();
	void InitColorGamma(bool isMono);
	void InitTriggerMode();
	void InitTriggerTiming();
	void InitIO();
	void InitHDR();
	void InitEeprom();
	void InitOther(bool isMono);
	
	// update
	void UpdateShutterGain();
	void UpdateWhiteBalance();
	void UpdateY();
	void UpdateColorGamma();
	void UpdateTriggerMode();
	void UpdateTriggerTiming();
	void UpdateIO();
	void UpdateHDR();
	void UpdateEeprom();
	void UpdateOther();
	
	// local utility
	QString getTimeString(float time);
	int getDriverCtrlValue(unsigned int id, int &value);
    int getDriverCtrlValue(unsigned int id, int &value, QSlider *slider, QLineEdit *lineEdit);
    int getDriverCtrlValue(unsigned int id, int &value, QList<int> list, QComboBox *comboBox);
	int setDriverCtrlValue(unsigned int id, int value);

	Ui::MainWindow *ui;
	CaptureWindow *capWindow;
	cameraid *cameraIdWindow;
	settingfile *settingFileWindow;
	
	int devHandle;
	void* libHandle;
	
	bool isCameraALC;
	bool isCameraWB;
	bool isMultiROI;
	bool isCameraFlip;
	bool isIo0ReadOnly;
	bool isIo1ReadOnly;
	bool isIo2ReadOnly;
	bool isIo3ReadOnly;
	bool hasSw;
	bool hasTemperature;
	bool isAutoExposureEnd;
	int autoExposureEndTimerId;
	int autoExposureEndTime;
	
	bool isEnableSettingChange;
	bool isEnableDisplayChange;

	// list
	QList<int> alcModeList;
	QList<int> adjustmentModeList;
	QList<int> wbModeList;
	QList<int> hueSaturationList;
	QList<int> yGammaList;
	QList<int> rGammaList;
	QList<int> grGammaList;
	QList<int> gbGammaList;
	QList<int> bGammaList;
	QList<int> sharpnessModeList;
	QList<int> shadingModeList;
	QList<int> exposureModeList;
	QList<int> triggerSelectorList;
	QList<int> triggerModeList;
	QList<int> triggerSourceList;
	QList<int> triggerOverlapList;
	QList<int> noiseReductionList;
	QList<int> waitHdList;
	QList<int> waitReadOutList;
	QList<int> sensorShutterModeList;
	QList<int> io0InOutList;
	QList<int> io1InOutList;
	QList<int> io2InOutList;
	QList<int> io3InOutList;
	QList<int> io0ModeList;
	QList<int> io1ModeList;
	QList<int> io2ModeList;
	QList<int> io3ModeList;
	QList<int> io0NegaPosiList;
	QList<int> io1NegaPosiList;
	QList<int> io2NegaPosiList;
	QList<int> io3NegaPosiList;
	QList<int> io0StatusList;
	QList<int> io1StatusList;
	QList<int> io2StatusList;
	QList<int> io3StatusList;
	QList<int> resetSwitchList;
	QList<int> ledList;
	QList<int> hdrModeList;
	QList<int> hdrSlopeNumList;
	QList<int> scanModeList;
	QList<int> hBinDeciList;
	QList<int> vBinDeciList;
	QList<int> binSumList;
	QList<int> regionList;
	QList<int> regionModeList;
	QList<int> clockList;
	QList<int> tranceferBitList;
	QList<int> displayPixFormatList;
	QList<int> colorInterpolationList;
	QList<int> mirrorModeList;
	QList<int> rotationModeList;
};

#endif // MAINWINDOW_H
