import os
import ctypes
import numpy as np 
import cv2 as cv




class Camera:
    def __init__(self,dllpath):
        self.img = np.array()
        self.camera_lib = ctypes.cdll.LoadLibrary(dllpath)

    def initCamera(self):
        ret = self.camera_lib.initCamera()
        return ret

    def captureImageToFile(self,imagePath):
        ret = self.camera_lib.captureImageToFile(imagePath)
        return ret
    
    def captureImage(self,image,width,height,channels):
        tmp = np.asarray(image)
        dataptr = tmp.ctypes.data_as(ctypes.c_char_p)
        ret = self.camera_lib.captureImage(dataptr,width,height,channels);
        return ret,image;


    def closeCamera(self):
        ret = self.camera_lib.closeCamera()
        return ret


if __name__ == "__main__":
    img = np.array((2448,2048),dtype='int8')
    cv.imshow("new image", img)
    cv.waitKey(0)
    # cam = Camera("ImageCapture.dll")
    # ret = cam.initCamera()
    # if ret != 1:
    #     print("fail to init camera!")

    # ret = cam.captureImageToFile("pytest.jpg")
    # if ret != 1:
    #     print("fait to capture image")
    
    # cam.closeCamera()

    
