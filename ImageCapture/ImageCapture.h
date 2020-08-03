#ifndef IMAGE_CAPTURE_H_
#define IMAGE_CAPTURE_H_

#define IMAGECAPTURE_API __declspec(dllexport)

/*
初始化摄像头
return：1成功， 其他值为错误码
*/
extern "C"  IMAGECAPTURE_API int initCamera();

/*
采集图像，保存到指定文件
param:
     imagePath:图像文件路径
return: 
     1成功，其他值为错误码
*/
extern "C"  IMAGECAPTURE_API int captureImageToFile(const char* imagePath);

/*
采集图像，将图像拷贝到指定内存
param:
	 pdata: 图像内存数据指针
	 height:图像高
	 width：图像宽
	 nchannels:图像通道数

return:
	 1成功，其他值为错误码
*/
extern "C"  IMAGECAPTURE_API int captureImage(unsigned char* pdata,int height,int width,int nchannels);

/*
关闭摄像头
return:1成功，其他值为错误码
*/
extern "C"  IMAGECAPTURE_API int closeCamera();


#endif // !IMAGE_CAPTURE_H_
