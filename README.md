# hkvision_machine_camera_python_wrapper
  海康工业相机采集图像，封装简易的python接口便于项目使用。

  目录结构：
  ImageCapture:对HK sdk进行封装，导出为动态库。
  --ImageCapture.h
  --ImageCapture.cpp
  TestCamera:
  --Test.cpp c++版本的测试程序，用来验证生成的ImageCapture是否可用。

  x64/Release:
  --ImageCapture.dll 图像采集的动态库。
  --CaptureImage.py  将ImageCapture.dll的接口封装为python接口。



