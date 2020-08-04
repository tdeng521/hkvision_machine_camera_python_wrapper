#ifndef IMAGE_CAPTURE_H_
#define IMAGE_CAPTURE_H_

#define IMAGECAPTURE_API __declspec(dllexport)

/*
��ʼ������ͷ
return��1�ɹ��� ����ֵΪ������
*/
extern "C"  IMAGECAPTURE_API int initCamera(const char* cam);

/*
�ɼ�ͼ�񣬱��浽ָ���ļ�
param:
     imagePath:ͼ���ļ�·��
return: 
     1�ɹ�������ֵΪ������
*/
extern "C"  IMAGECAPTURE_API int captureImageToFile(const char* cam,const char* imagePath);

/*
�ɼ�ͼ�񣬽�ͼ�񿽱���ָ���ڴ�
param:
	 pdata: ͼ���ڴ�����ָ��
	 height:ͼ���
	 width��ͼ���
	 nchannels:ͼ��ͨ����

return:
	 1�ɹ�������ֵΪ������
*/
extern "C"  IMAGECAPTURE_API int captureImage(const char* cam,unsigned char* pdata,int height,int width,int nchannels);

/*
�ر�����ͷ
return:1�ɹ�������ֵΪ������
*/
extern "C"  IMAGECAPTURE_API int closeCamera(const char* cam);


#endif // !IMAGE_CAPTURE_H_
