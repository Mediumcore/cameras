//#include <iostream>

//#include <stdio.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <sys/ioctl.h>
//#include <linux/videodev.h>

//using namespace std;

//int main(){
//    int fd;
//    struct video_capability video_cap;
//    struct video_window     video_win;
//    struct video_picture   video_pic;

//    if((fd = open("/dev/video0", O_RDONLY)) == -1){
//        perror("cam_info: Can't open device");
//        return 1;
//    }

//    if(ioctl(fd, VIDIOCGCAP, &video_cap) == -1)
//        perror("cam_info: Can't get capabilities");
//    else {
//        printf("Name:\t\t '%s'\n", video_cap.name);
//        printf("Minimum size:\t%d x %d\n", video_cap.minwidth, video_cap.minheight);
//        printf("Maximum size:\t%d x %d\n", video_cap.maxwidth, video_cap.maxheight);
//    }

//    if(ioctl(fd, VIDIOCGWIN, &video_win) == -1)
//        perror("cam_info: Can't get window information");
//    else
//        printf("Current size:\t%d x %d\n", video_win.width, video_win.height);

//    if(ioctl(fd, VIDIOCGPICT, &video_pic) == -1)
//        perror("cam_info: Can't get picture information");
//    else
//        printf("Current depth:\t%d\n", video_pic.depth);

//    close(fd);
//    return 0;
//}
/*****************************************************
 * 文件名：GetCameraInfo.c
 * 文件描述：linux下使用v4l2编程查询摄像头设备信息
 * 编写人：王廷云
 * 编写日期：2017-12-1
 * 修改日期：2018-1-1
*****************************************************/
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>                      // 下面四个头文件是linux系统编程特有的
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <linux/videodev2.h>                // 操作摄像头设备

int main(int argc, char **argv)
{
    int ret, i, j;
    int fd;

    /* 打开摄像头设备 */
    fd = open("/dev/webcam", O_RDWR);       // 注意查看摄像头设备名
    if (-1 == fd)
    {
        perror("open /dev/video0");
        return -1;
    }

    /* 查询打开的设备是否属于摄像头：设备video不一定是摄像头*/
    struct v4l2_capability cap;
    ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (-1 == ret)
    {
        perror("ioctl VIDIOC_QUERYCAP");
        close(fd);
    }
    if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
    {
        /* 如果为摄像头设备则打印摄像头驱动名字 */
        printf("Driver    Name: %s\n", cap.driver);
        printf("card    Name: %s\n", cap.card);
        printf("bus_info    Name: %s\n", cap.bus_info);
        printf("capabilities    Name: %d\n", cap.capabilities);
        printf("device_caps    Name: %d\n", cap.device_caps);
    }
    else
    {
        printf("open file is not video\n");
        close(fd);
        return -2;
    }

    /* 查询摄像头可捕捉的图片类型，VIDIOC_ENUM_FMT: 枚举摄像头帧格式 */
    struct v4l2_fmtdesc fmt;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // 指定需要枚举的类型
    for (i = 0; ; i++)                      // 有可能摄像头支持的图片格式不止一种
    {
        fmt.index = i;
        ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmt);
        if (-1 == ret)                      // 获取所有格式完成
        {
            break;
        }

        /* 打印摄像头图片格式 */
        printf("Picture Format: %s\n", fmt.description);

        /* 查询该图像格式所支持的分辨率 */
        struct v4l2_frmsizeenum frmsize;
        frmsize.pixel_format = fmt.pixelformat;
        for (j = 0; ; j++)                  //　该格式支持分辨率不止一种
        {
            frmsize.index = j;
            ret = ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize);
            if (-1 == ret)                  // 获取所有图片分辨率完成
            {
                break;
            }

            /* 打印图片分辨率 */
            printf("width: %d height: %d\n",
                    frmsize.discrete.width,frmsize.discrete.height);
        }

    }

    close(fd);                              // 不要忘记关闭打开的设备

    return 0;
}

