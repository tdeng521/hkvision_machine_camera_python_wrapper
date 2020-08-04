#include "../ImageCapture/ImageCapture.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


int main() {
	const char* cam = "169.254.60.25";
	int ret = initCamera(cam);

	Mat img(2048, 2448, CV_8UC3);
	Mat showImg;
	while (1) {
		double t = (double)getTickCount();
		captureImage(cam,img.data, 2048, 2448, 3);
		t = ((double)getTickCount() - t) / getTickFrequency();
		cout << "cost time is :" << t << endl;
		resize(img, showImg, Size(), 0.3, 0.3);
		imshow("image",showImg);
		char c = waitKey(10);
		if (c == 'b') {
			break;
		}
	}

	ret = closeCamera(cam);

	return ret;
}