#ifndef CAPTUREWINDOW_H
#define CAPTUREWINDOW_H

#include <QWidget>
#include <QSocketNotifier>

#define UPDATE_STATUS_DISABLE   0x00
#define UPDATE_STATUS_ENABLE    0x01
#define UPDATE_STATUS_ONE_SHOT  0x02

#define BUFFER_REQUEST_MAX 4
#define BUFFER_REQUEST_MIN 2
#define BYTE_PER_PIXELS_MAX 2
#define PIXEL_COLOR_MAX 4

struct buffer {
    void	*start;
    size_t	length;
};

namespace Ui {
class CaptureWindow;
}

class CaptureWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CaptureWindow(QWidget *parent = 0);
    ~CaptureWindow();
    void Init(int fd, void* lib, bool cameraALC, bool cameraWB, bool multiROI);
    void startCapture();
    void stopCapture();

    void setNoiseReduction(int value);
    void setTransferBits(int value);
    void setDisplayPixFormat(int value);
    void setColorInterpolation(int value);
    void setMirrorMode(int value);
    void setRotationMode(int value);

protected:
    void closeEvent(QCloseEvent *);
    void timerEvent(QTimerEvent *);
    void paintEvent(QPaintEvent *);

private:

    Ui::CaptureWindow *ui;

    int devHandle;
    void* libHandle;
    bool isCameraALC;
    bool isCameraWB;
    bool isMultiROI;
	
    int noiseReduction;
    int transferBits;
    int displayPixFormat;
    int colorInterpolation;
    int mirrorMode;
    int rotationMode;
	
    int timerId;
    unsigned long imageWidth;
    unsigned long imageHeight;
    unsigned char *pRawData;
    unsigned char *pBgrData;
	
    buffer buffers[BUFFER_REQUEST_MAX];
    unsigned int bufferNum;
    int frameCount;
    bool captureStarted;
};

#endif // CAPTUREWINDOW_H
