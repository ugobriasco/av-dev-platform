#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "raspicam_cv.h"
#include <iostream>
#include <chrono>
#include <ctime>

#include "wiringPi.h"

using namespace std;
using namespace cv;
using namespace raspicam;

Mat frame;
RaspiCam_Cv Camera;

Point2f Source[] = {
	Point2f(70,160),
	Point2f(300, 160),
	Point2f(0,230),
	Point2f(360, 230)
	};


void Setup (int argc, char **argv, RaspiCam_Cv &Camera){
	Camera.set(CAP_PROP_FRAME_WIDTH, ("-w", argc, argv,360));
	Camera.set(CAP_PROP_FRAME_HEIGHT, ("-h", argc, argv, 240));
	Camera.set(CAP_PROP_BRIGHTNESS, ("-br", argc, argv, 50));
	Camera.set(CAP_PROP_CONTRAST, ("-co", argc, argv, 50));
	Camera.set(CAP_PROP_SATURATION, ("-sa", argc, argv, 50 ));
	Camera.set(CAP_PROP_GAIN, ("-g",argc,argv,50));
	Camera.set(CAP_PROP_FPS, ("-fps",argc, argv, 0));
}

// Define area of interest
void Perspective(){
	line(frame, Source[0], Source[1], Scalar(0,0,255), 2);
	line(frame, Source[1], Source[3], Scalar(0,0,255), 2);
	line(frame, Source[3], Source[2], Scalar(0,0,255), 2);
	line(frame, Source[2], Source[0], Scalar(0,0,255), 2);

}

// Rotate image by a given angle
Mat rotate(Mat src, double angle)
{
    Mat dst;
    Point2f pt(src.cols/2., src.rows/2.);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(src, dst, r, Size(src.cols, src.rows));
    return dst;
}

// Get image from raspicam
void Capture(){
	Camera.grab();
	Camera.retrieve(frame);

	// Transform image to RGB.
	cvtColor(frame, frame, COLOR_BGR2RGB);

	// The camera is placed upside-down, thus flip it.
	flip(frame, frame,-1);

  // Adjust offset
  frame = rotate(frame, 3);
}

int main(int argc, char **argv)
{

	Setup(argc, argv, Camera);
	cout<<"Connecting to camera"<<endl;
	if (!Camera.open()){
		cout<<"Cannot connect to the camera"<<endl;
		return -1;
	}
	cout<<"Camera ID"<<Camera.getId()<<endl;

	/* PICTURE
	Camera.grab();
	Camera.retrieve(frame);
	imshow("frame", frame);
	waitKey();*/


	// VIDEO
	while(1) {
		auto start = std::chrono::system_clock::now();

		Capture();
		Perspective();

		auto end = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed_seconds = end-start;

		float t = elapsed_seconds.count();
		int FPS = 1/t;
		cout<<"FPS= "<<FPS<<endl;


		namedWindow("RGB", WINDOW_KEEPRATIO);
		moveWindow("RGB", 50, 100);
		resizeWindow("RGB",720, 480);
		imshow("RGB", frame);

		waitKey(1);
	}

	return 0;

}
