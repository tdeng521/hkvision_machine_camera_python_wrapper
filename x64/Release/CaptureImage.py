import os
import sys
import ctypes
import numpy as np 
import cv2 as cv

class Camera:
    def __init__(self,dllpath):
        #self.camera_lib = np.ctypeslib.load_library(dllpath,'.')
        self.camera_lib = ctypes.cdll.LoadLibrary(dllpath)
    '''
    initialize the camera
    Returns:
        1 success, other failure.
    '''
    def initCamera(self):
        ret = self.camera_lib.initCamera()
        return ret
    
    '''
    capture image and save to file.
    Parameters:
        imagePath - image storage path.
    Returns:
        1 success, other failure.
    '''
    def captureImageToFile(self,imagePath):
        ret = self.camera_lib.captureImageToFile(imagePath)
        return ret
    
    '''
    capture image
    Parameters:
        image - image,<Numpy type>
        width - image width
        height- image height
        channels- image channels
    Returns:
        1 success, other failure.
    '''
    def captureImage(self,image,width,height,channels):
        print("capture image")
        self.camera_lib.captureImage.argtypes = [ctypes.c_char_p, ctypes.c_int,ctypes.c_int,ctypes.c_int]
        self.camera_lib.captureImage.restype = ctypes.c_int
        ret = self.camera_lib.captureImage(ctypes.c_char_p(image.ctypes.data) ,width,height,channels)
        return ret;
    
    '''
    close the camera and release resources.
    '''
    def closeCamera(self):
        ret = self.camera_lib.closeCamera()
        return ret


if __name__ == "__main__":
    PROC_PATH = os.path.abspath(os.path.realpath(__file__))  #得到执行文件绝对路径
    ROOT = os.path.dirname(PROC_PATH)                        # 得到执行文件目录
    #1.创建camera实例
    cam = Camera(os.path.join(ROOT,"ImageCapture.dll"))
	#2.初始化camera
	ret = cam.initCamera()
	if ret != 1:
		print("xx:failed to init camera")
        sys.exit()
	#3.申请图像空间
    img=np.zeros((2048,2448,3), np.uint8)
	#4.采集图像
    ret = cam.captureImage(img,2048,2448,3)
	if ret != 1:
		print("xx:failed to capture image")
        sys.exit()
    print(img.shape)
	#
    cv.imshow("new image", img)
    cv.waitKey(0)

    
