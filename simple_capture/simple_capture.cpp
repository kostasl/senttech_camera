/*
 *	V4L2 video capture example
 *
 *	This program can be used and distributed without restrictions.
 *
 *		This program is provided with the V4L2 API
 * see https://linuxtv.org/docs.php for more information
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include <getopt.h>	/* getopt_long() */

#include <fcntl.h>	/* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct buffer {
	void	*start;
	size_t	length;
};

static char				*dev_name;
static int				fd = -1;
struct buffer			*buffers;
static unsigned int		n_buffers;
static int				frameCount = 0;
static struct timeval	baseTime;

struct setting {
	bool	isSet;
	int		value;
};

static void errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

static int xioctl(int fh, int request, void *arg)
{
	int r;

	do {
		r = ioctl(fh, request, arg);
	} while (-1 == r && EINTR == errno);

	return r;
}

static int read_frame(void)
{
	struct v4l2_format fmt;
	struct v4l2_buffer buf;
	unsigned int fourcc;
	int code;
	struct timeval tv;

	/* 画像表示用にフレームサイズを取得 */
	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(fd, VIDIOC_G_FMT, &fmt))
		exit(EXIT_FAILURE);

	cv::Mat src(fmt.fmt.pix.height, fmt.fmt.pix.width, CV_8UC1);
 	cv::Mat dst;
	
	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	/* 画像格納領域をストリーミングキューから切断 */
	if (-1 == ioctl(fd, VIDIOC_DQBUF, &buf)) {
		switch (errno) {
		case EAGAIN:
			return 0;

		case EIO:
		default:
			exit(EXIT_FAILURE);
		}
	}

	assert(buf.index < n_buffers);

	/* バッファ内のデータを取り出し */
	memcpy(src.data, (unsigned char*)buffers[buf.index].start, src.step * src.rows);

	fourcc = fmt.fmt.pix.pixelformat;
	switch(fourcc) {
	case V4L2_PIX_FMT_Y10:
	case V4L2_PIX_FMT_Y12:
	case V4L2_PIX_FMT_GREY:
		code = CV_GRAY2RGB;
		break;
	case V4L2_PIX_FMT_SRGGB8:
	case V4L2_PIX_FMT_SRGGB10:
	case V4L2_PIX_FMT_SRGGB12:
		code = CV_BayerRG2RGB;
		break;
	case V4L2_PIX_FMT_SGRBG8:
	case V4L2_PIX_FMT_SGRBG10:
	case V4L2_PIX_FMT_SGRBG12:
		code = CV_BayerGR2RGB;
		break;
	case V4L2_PIX_FMT_SGBRG8:
	case V4L2_PIX_FMT_SGBRG10:
	case V4L2_PIX_FMT_SGBRG12:
		code = CV_BayerGB2RGB;
		break;
	case V4L2_PIX_FMT_SBGGR8:
	case V4L2_PIX_FMT_SBGGR10:
	case V4L2_PIX_FMT_SBGGR12:
		code = CV_BayerBG2RGB;
		break;
	case V4L2_PIX_FMT_BGR24:
		code = CV_BayerBG2RGB;
		break;
	default:
		code = CV_BayerGR2RGB;
		break;
	}
	cvtColor(src, dst, code);

	/* 画像出力 */
	cv::imshow("image", dst);

	frameCount++;
	gettimeofday(&tv, NULL);
	if(tv.tv_sec > baseTime.tv_sec)
	{
		fprintf(stdout, "fps = %d\n", frameCount);
		frameCount = 0;
		baseTime = tv;
	}

	/* 画像格納領域をストリーミングキューに接続 */
	if (-1 == ioctl(fd, VIDIOC_QBUF, &buf))
		exit(EXIT_FAILURE);

	return 1;
}

static void mainloop(void)
{
	while (1) {
		fd_set fds;
		struct timeval tv;
		int r;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		/* Timeout. */
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		/* バッファの読み出しが可能になるまで待つ */
		r = select(fd + 1, &fds, NULL, NULL, &tv);

		if (-1 == r) {
			if (EINTR == errno)
				continue;
			errno_exit("mainloop:select");
		}

		if (0 == r) {
			fprintf(stderr, "select timeout\n");
			return;
		}

		/* バッファ読み出し */
		read_frame();
		/* キー入力待ち */
		if(cv::waitKey(1) >= 0) {
			break;
		}
	}
}

static void stop_capturing(void)
{
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
		errno_exit("VIDIOC_STREAMOFF");
}

static void start_capturing(void)
{
	unsigned int i;
	enum v4l2_buf_type type;

	for (i = 0; i < n_buffers; ++i) {
		struct v4l2_buffer buf;

		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
		errno_exit("VIDIOC_STREAMON");
	gettimeofday(&baseTime, NULL);
}

static void uninit_device(void)
{
	unsigned int i;

	for (i = 0; i < n_buffers; ++i)
		if (-1 == munmap(buffers[i].start, buffers[i].length))
			errno_exit("munmap");

	free(buffers);
}

static void init_mmap(void)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
				 "memory mapping\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n",
			 dev_name);
		exit(EXIT_FAILURE);
	}

	buffers = (buffer*)calloc(req.count, sizeof(*buffers));

	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type		= V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory		= V4L2_MEMORY_MMAP;
		buf.index		= n_buffers;

		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start =
			mmap(NULL /* start anywhere */,
				buf.length,
				PROT_READ | PROT_WRITE /* required */,
				MAP_SHARED /* recommended */,
				fd, buf.m.offset);
		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

static void init_device(void)
{
	struct v4l2_capability cap;
	struct v4l2_crop crop;

	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n",
				 dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}
}

static void close_device(void)
{
	if (-1 == close(fd))
		errno_exit("close");

	fd = -1;
}

static void open_device(void)
{
	struct stat st;

	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n",
			 dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		exit(EXIT_FAILURE);
	}

	fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
			 dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static void usage(FILE *fp, int argc, char **argv)
{
	fprintf(fp,
		"Usage: %s [options]\n\n"
		"Version 0.1\n"
		"Options:\n"
		"-d | --device name    Video device name [%s]\n"
		"-h | --help           Print this message\n"
		"",
		argv[0], dev_name);
}

static const char short_options[] = "d:h";

static const struct option
long_options[] = {
	{ "device",			required_argument,	NULL, 'd' },
	{ "help",			no_argument,		NULL, 'h' },
	{ 0, 0, 0, 0 }
};

int main(int argc, char **argv)
{
	dev_name = (char*)("/dev/video0");

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

	open_device();
	init_device();
	init_mmap();
	start_capturing();
	mainloop();
	stop_capturing();
	uninit_device();
	close_device();
	fprintf(stderr, "\n");
	return 0;
}


