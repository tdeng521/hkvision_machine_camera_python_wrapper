#include "ImageCapture.h"
#include<string.h>
#include "MvCameraControl.h"
#include<iostream>
#include <stdio.h>
#include <thread>
#include <conio.h>
#include<string>

using namespace std;


bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo)
{
	if (NULL == pstMVDevInfo)
	{
		printf("The Pointer of pstMVDevInfo is NULL!\n");
		return false;
	}
	if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE)
	{
		int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
		int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
		int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
		int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

		// ch:��ӡ��ǰ���ip���û��Զ������� | en:print current ip and user defined name
		printf("CurrentIp: %d.%d.%d.%d\n", nIp1, nIp2, nIp3, nIp4);
		printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
	}
	else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE)
	{
		printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
		printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
		printf("Device Number: %d\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.nDeviceNumber);
	}
	else
	{
		printf("Not support.\n");
	}

	return true;
}


class Camera {
public:
	Camera() {};
	~Camera() {};

	int initCamera() {
		// ch:ö���豸 | en:Enum device
		MV_CC_DEVICE_INFO_LIST stDeviceList;
		memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
		int nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &stDeviceList);
		if (MV_OK != nRet)
		{
			cout << "Enum Devices fail! nRet: " << nRet << endl;
			return -1;
		}

		if (stDeviceList.nDeviceNum > 0)
		{
			for (unsigned int i = 0; i < stDeviceList.nDeviceNum; i++)
			{
				cout << "[device " << i << "]" << endl;
				MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
				if (NULL == pDeviceInfo)
				{
					return -2;
				}
				PrintDeviceInfo(pDeviceInfo);
			}
		}
		else
		{
			printf("Find No Devices!\n");
			return 0;
		}

		// ch:ѡ���豸��������� | en:Select device and create handle
		int nIndex = 0;
		nRet = MV_CC_CreateHandle(&m_handle, stDeviceList.pDeviceInfo[nIndex]);
		if (MV_OK != nRet)
		{
			printf("Create Handle fail! nRet [0x%x]\n", nRet);
			return -3;
		}

		// ch:���豸 | en:Open device
		nRet = MV_CC_OpenDevice(m_handle);
		if (MV_OK != nRet)
		{
			printf("Open Device fail! nRet [0x%x]\n", nRet);
			return -5;
		}

		// ch:̽��������Ѱ���С(ֻ��GigE�����Ч) | en:Detection network optimal package size(It only works for the GigE camera)
		if (stDeviceList.pDeviceInfo[nIndex]->nTLayerType == MV_GIGE_DEVICE)
		{
			int nPacketSize = MV_CC_GetOptimalPacketSize(m_handle);
			if (nPacketSize > 0)
			{
				nRet = MV_CC_SetIntValue(m_handle, "GevSCPSPacketSize", nPacketSize);
				if (nRet != MV_OK)
				{
					printf("Warning: Set Packet Size fail nRet [0x%x]!", nRet);
				}
			}
			else
			{
				printf("Warning: Get Packet Size fail nRet [0x%x]!", nPacketSize);
			}
		}

		// ch:���ô���ģʽΪoff | en:Set trigger mode as off
		nRet = MV_CC_SetEnumValue(m_handle, "TriggerMode", 0);
		if (MV_OK != nRet)
		{
			printf("Set Trigger Mode fail! nRet [0x%x]\n", nRet);
			return -6;
		}

		// ch:��ȡ���ݰ���С | en:Get payload size
		MVCC_INTVALUE stParam;
		memset(&stParam, 0, sizeof(MVCC_INTVALUE));
		nRet = MV_CC_GetIntValue(m_handle, "PayloadSize", &stParam);
		if (MV_OK != nRet)
		{
			printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);
			return -7;
		}
		m_nPayloadSize = stParam.nCurValue;

		// ch:��ʼȡ�� | en:Start grab image
		nRet = MV_CC_StartGrabbing(m_handle);
		if (MV_OK != nRet)
		{
			return 0;
			printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
		}


		return 1;
	};

	unsigned int __stdcall WorkThread(const char* imagepath)
	{
		cout << "do capture thread:" << std::string(imagepath) << endl;
		int nRet = MV_OK;

		MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
		memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
		unsigned char * pFrameBuf = (unsigned char *)malloc(sizeof(unsigned char) * (m_nPayloadSize));
		if (pFrameBuf == NULL)
		{
			return 0;
		}
		unsigned int nDataSize = m_nPayloadSize;

		nRet = MV_CC_GetOneFrameTimeout(m_handle, pFrameBuf, nDataSize, &stImageInfo, 1000);
		if (nRet == MV_OK)
		{
			printf("Get One Frame: Width[%d], Height[%d], nFrameNum[%d]\n",
				stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);
			//ͼƬ���������������            
			MV_SAVE_IMAGE_PARAM_EX stParam;
			// ch:BMPͼƬ��С��width * height * 3 + 2048(Ԥ��BMPͷ��С)
			// en:// BMP image size: width * height * 3 + 2048 (Reserved BMP header size)
			int nBufSize = stImageInfo.nWidth * stImageInfo.nHeight * 3 + 2048;

			//Դ����                 
			stParam.pData = pFrameBuf;                //ԭʼͼ������
			stParam.nDataLen = stImageInfo.nFrameLen;    //ԭʼͼ�����ݳ���
			stParam.enPixelType = stImageInfo.enPixelType;  //ԭʼͼ�����ݵ����ظ�ʽ
			stParam.nWidth = stImageInfo.nWidth;       //ͼ���
			stParam.nHeight = stImageInfo.nHeight;      //ͼ��� 
			stParam.nJpgQuality = 70;						  //JPEGͼƬ��������  

			//Ŀ������
			stParam.enImageType = MV_Image_Jpeg;            //��Ҫ�����ͼ�����ͣ�ת����JPEG��ʽ
			stParam.nBufferSize = nBufSize;                 //�洢�ڵ�Ĵ�С
			unsigned char* pImage = (unsigned char*)malloc(nBufSize);
			stParam.pImageBuffer = pImage;                   //������ݻ����������ת��֮���ͼƬ����  
	

			nRet = MV_CC_SaveImageEx2(m_handle, &stParam);
			if (MV_OK != nRet)
			{
				return 0;
			}
			//��ת��֮��ͼƬ���ݱ�����ļ�
			FILE* fp = fopen(imagepath, "wb");
			if (NULL == fp)
			{
				cout << "5555555" << endl;	
			}
			fwrite(pImage, 1, stParam.nImageLen, fp);
			fclose(fp);
			free(pImage);
			cout << "save down" << endl;
		}
		else
		{
			printf("No data[0x%x]\n", nRet);
		}

		free(pFrameBuf);

		return 0;
	}

	int captureImage(unsigned char* pdata, int height, int width, int nchannels) {
		MV_FRAME_OUT_INFO_EX stInfo;
		memset(&stInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
		int nBufSize = height * width*nchannels;
		unsigned char*  pFrameBuf = NULL;
		pFrameBuf = (unsigned char*)malloc(nBufSize);
		int nRet = MV_CC_GetImageForBGR(m_handle, pFrameBuf, nBufSize, &stInfo, 1000);
		if (nRet != 0)
		{
			cout << "error:GetImageForRGB: "   << nRet << endl;
			delete pFrameBuf;
			return -1;
		}
		else
		{
			//...ͼ�����ݴ���
			if (stInfo.enPixelType == PixelType_Gvsp_BGR8_Packed)
			{
				memcpy(pdata, pFrameBuf, nBufSize);
			}
			else {
				delete pFrameBuf;
				return -2;
			}
		}
		delete pFrameBuf;
		return 1;
	}

	int captureImageToFile(const char* imagePath) {
		// ch:��ʼȡ�� | en:Start grab image
		int nRet = MV_CC_StartGrabbing(m_handle);
		if (MV_OK != nRet)
		{
			return 0;
			printf("Start Grabbing fail! nRet [0x%x]\n", nRet);
		}

		unsigned int nThreadID = 0;
		std::thread grabThread = std::thread(&Camera::WorkThread, this, imagePath);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		// ch:ֹͣȡ�� | en:Stop grab image
		nRet = MV_CC_StopGrabbing(m_handle);
		if (MV_OK != nRet)
		{
			printf("Stop Grabbing fail! nRet [0x%x]\n", nRet);
			return -2;
		}

		return 1;
	};

	int closeCamera() {
		// ch:ֹͣȡ�� | en:Stop grab image
		int nRet = MV_CC_StopGrabbing(m_handle);
		if (MV_OK != nRet)
		{
			printf("Stop Grabbing fail! nRet [0x%x]\n", nRet);
			return -2;
		}

		// ch:�ر��豸 | Close device
		nRet = MV_CC_CloseDevice(m_handle);
		if (MV_OK != nRet)
		{
			printf("ClosDevice fail! nRet [0x%x]\n", nRet);
			return -1;
		}

		// ch:���پ�� | Destroy handle
		nRet = MV_CC_DestroyHandle(m_handle);
		if (MV_OK != nRet)
		{
			printf("Destroy Handle fail! nRet [0x%x]\n", nRet);
			return -2;
		}
		return 1;
	}

	void* m_handle;
	int m_nPayloadSize;
};

Camera* g_cam = nullptr;


extern "C"  IMAGECAPTURE_API int initCamera() {
	g_cam = new Camera();
	if (g_cam) {
		return g_cam->initCamera();
	}
	else {
		return 0;
	}
}

extern "C"  IMAGECAPTURE_API int captureImageToFile(const char* imagePath) {
	if (g_cam) {
		return g_cam->captureImageToFile(imagePath);
	}
	else {
		return 0;
	}
	
}

extern "C"  IMAGECAPTURE_API int captureImage(unsigned char* pdata, int height, int width, int nchannels) {
	if (g_cam) {
		return g_cam->captureImage(pdata,height,width,nchannels);
	}
	else {
		return 0;
	}
}

extern "C"  IMAGECAPTURE_API int closeCamera() {
	if (g_cam) {
		cout << "close camera" << endl;
		g_cam->closeCamera();
		delete g_cam;
	}

	return 1;
}


//extern "C"  IMAGECAPTURE_API int getImg(unsigned char* pdata, int height, int width, int nchannels) {
//	cout << "dddddddddd" << height << width << nchannels << endl;
//	for (int i = 0; i < 10; i++) {
//		cout << " i:" << i << endl;
//		for (int j = 0; j < width / 2; j++) {
//			*(pdata + i * width*nchannels + j) = 255;
//			*(pdata + i * width*nchannels + j+1) = 255;
//			*(pdata + i * width*nchannels + j + 2) = 255;
//		}
//	}
//	cout << "**********" << endl;
//	return 1;
//}