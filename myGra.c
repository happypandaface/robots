
// for memset
#include<string.h>
// for calloc
#include<stdlib.h>
// for printf
#include<stdio.h>
// for read
#include<unistd.h>
// for open/close:
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
// for ioctl (special file manipulation)
#include<sys/ioctl.h>
// for v4l2 structs
#include <linux/videodev2.h>

int exitErr(const char *err)
{
	printf("%s\n", err);
	return -1;
}

int main(int argc, char **argv)
{
	int fd = -1;
	int file_params;
	file_params |= O_RDWR;
	file_params |= O_NONBLOCK;
	fd = open("/dev/video0", file_params, 0);
	if (fd == -1)
	{
		printf("couldnt open video0\n");
    goto cleanup;
	}

  struct v4l2_capability cap;
	if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
	{
		printf("couldnt get capabilities\n");
		goto cleanup;
	}
	if (!(cap.capabilities & V4L2_CAP_READWRITE))
	{
		printf("cant straight io with video\n");
		goto cleanup;
	}
  if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
    printf("does not support streaming i/o\n");
    goto cleanup;
  }
	
	struct v4l2_format fmt;
	if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
	{
		printf("couldnt get format\n");
		goto cleanup;
	}
	
	int width = fmt.fmt.pix.width;
	int height = fmt.fmt.pix.height;
	int imgSize = fmt.fmt.pix.sizeimage;
	
	printf("video says: width: %i, height %i\n", width, height);

  struct v4l2_buffer buf;
  memset(&buf, 0, sizeof(buf));
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = 0;
	
  if (ioctl(fd, VIDIOC_QBUF, &buf) == -1)
  {
    printf("setup buf err\n");
    goto cleanup;
  }
  int type;
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == ioctl(fd, VIDIOC_STREAMON, &type))
  {
    printf("set stream type err\n");
    goto cleanup;
  }

  fd_set fds;
  struct timeval tv;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  /* Timeout. */
  tv.tv_sec = 2;
  tv.tv_usec = 0;
  if (-1 == select(fd + 1, &fds, NULL, NULL, &tv))
  {
    printf("couldn't select\n");
    goto cleanup;
  }



  
  cleanup:
  type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (fd == -1 || -1 == ioctl(fd, VIDIOC_STREAMOFF, &type))
  {
    printf("couldn't reset stream type\n");
  }
  close(fd);
}
