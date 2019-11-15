/*
***************************************************************************
*
* Author: Ugo Briasco
*
* Copyright (C) 2019 - 2020 Ugo Briasco
*
* Email: hi@ugobriasco.me
*
***************************************************************************
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************
*/

// Dependancies for Image processing
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "raspicam_cv.h"
#include <iostream>
#include <chrono>
#include <ctime>

// Dependancies for motion control
/******
* NOTE: To compile programs with wiringPi, you need to add:
*    -lwiringPi
*  to your compile line(s) To use the Gertboard, MaxDetect, etc.
*  code (the devLib), you need to also add:
*    -lwiringPiDev
*  to your compile line(s).
*
*******/
#include <wiringPi.h>
#include <softPwm.h>


// Namespaces
using namespace std;
using namespace cv;
using namespace raspicam;

//PARAMS - Camera
#define BRIGHTNESS 50
#define CONTRAST 50
#define SATURATION 50

//PARAMS - Threshold Filter
#define THRESHOLD_S 5 //it can be a scalar as well
#define THRESHOLD_H 50

//PARAMS - Canny Filter
#define CANNY_LOW_THRESHOLD 100
#define CANNY_LOW_THRESHOLD_RATIO 500
#define CANNY_KERNEL_SIZE 3

//PARAMS -ConvergeToLane
#define DEV_MAX 102 //max deviation ±140px from current bird's eye settings (±30deg)
#define CONV_1 30//deviation which triggers the first level of convergence
#define CONV_2 80//deviation which triggers the second level of convergence

//PARAMS -MOTION
#define THRUST_1 30
#define THRUST_2 50
#define THRUST_3 60
#define THRUST_4 70


//PARAMS WiringPI pins
#define L_ENABLE 30
#define L_HIGH 21
#define L_LOW 22
#define R_LOW 23
#define R_HIGH 24
#define R_ENABLE 25


//GLOBAL image processing
Mat frame, matrix, framePerspective, frameThreshold, frameCanny, frameFinal, frameFinalDuplicate;
Mat ROIlane;
vector<int> histogramLane;
int lanePosition, frameCenter, deviation;
RaspiCam_Cv Camera;
stringstream deviationStream;
Point2f Source[] = {
	Point2f(70,160),
	Point2f(300, 160),
	Point2f(0,230),
	Point2f(360, 230)
	};
Point2f Destination[] = {
	Point2f(60,0),
	Point2f(300, 0),
	Point2f(60,240),
	Point2f(300, 240)
	};

// Setup camera
void setupCamera (int argc, char **argv, RaspiCam_Cv &Camera){
	Camera.set(CAP_PROP_FRAME_WIDTH, ("-w", argc, argv, 360));
	Camera.set(CAP_PROP_FRAME_HEIGHT, ("-h", argc, argv, 240));
	Camera.set(CAP_PROP_BRIGHTNESS, ("-br", argc, argv, BRIGHTNESS));
	Camera.set(CAP_PROP_CONTRAST, ("-co", argc, argv, CONTRAST));
	Camera.set(CAP_PROP_SATURATION, ("-sa", argc, argv, SATURATION ));
	Camera.set(CAP_PROP_GAIN, ("-g",argc,argv,50));
	Camera.set(CAP_PROP_FPS, ("-fps",argc, argv, 0));
}

// Rotate image by a given angle
Mat rotateImage(Mat src, double angle){
    Mat dst;
    Point2f pt(src.cols/2., src.rows/2.);
    Mat r = getRotationMatrix2D(pt, angle, 1.0);
    warpAffine(src, dst, r, Size(src.cols, src.rows));
    return dst;
}

// Get image from raspicam
void captureImage(){
	Camera.grab();
	Camera.retrieve(frame);

	// Transform image to RGB.
	cvtColor(frame, frame, COLOR_BGR2RGB);

	// The camera is placed upside-down, thus flip it.
	flip(frame, frame,-1);

  // Adjust offset
  frame = rotateImage(frame, 3);
}

// Define region of interest
void applyBirdsEye(){
	line(frame, Source[0], Source[1], Scalar(255,0,0), 2);
	line(frame, Source[1], Source[3], Scalar(255,0,0), 2);
	line(frame, Source[3], Source[2], Scalar(255,0,0), 2);
	line(frame, Source[2], Source[0], Scalar(255,0,0), 2);

	matrix = getPerspectiveTransform(Source, Destination);
	warpPerspective(frame, framePerspective, matrix, Size(360,240));
	cvtColor(framePerspective, framePerspective, COLOR_RGB2GRAY);
}

// Image processing
void applyThresholdFilter(){
	//Apply Threshold filter
	inRange(framePerspective, THRESHOLD_S, THRESHOLD_H, frameThreshold);

	//Apply Canny filter
	Canny(framePerspective, frameCanny, CANNY_LOW_THRESHOLD,CANNY_LOW_THRESHOLD_RATIO, CANNY_KERNEL_SIZE, false);

	//Convolute both images
	add(frameThreshold, frameCanny, frameFinal);
	cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);
	cvtColor(frameFinal, frameFinalDuplicate, COLOR_RGB2GRAY); //duplicate frame for histogram only
}

//Histogram
void histogram(){

	//frame.size().width == 360;

	histogramLane.resize(360);
	histogramLane.clear();

	for(int i=0; i<360; i++){ // frame.size().width
		ROIlane = frameFinalDuplicate(Rect(i,140,1,100));
		divide(255,ROIlane, ROIlane);
		histogramLane.push_back((int)(sum(ROIlane)[0]));
	}
}

// Defines the deviiation between center of vehicles and a black lane on the floor
void laneFinder(){
	vector<int>::iterator leftPtr;
	leftPtr = max_element(histogramLane.begin(), histogramLane.begin() + 360);
	lanePosition = distance(histogramLane.begin(), leftPtr) + 16; //Calibrated

	frameCenter = 180;

	line(frameFinal, Point2f(lanePosition, 0), Point2f(lanePosition, 240), Scalar(0,255,0), 2);
	line(frameFinal, Point2f(frameCenter, 0), Point2f(frameCenter, 240), Scalar(255,255,0), 2);

	deviation = lanePosition - frameCenter;

}

void initWiringPi(){
	//WiringPI setup
	wiringPiSetup();
	printf("wiringPi is working!\n");

	// Map PinMode
	pinMode(L_HIGH, OUTPUT);
	pinMode(L_LOW, OUTPUT);
	pinMode(R_LOW, OUTPUT);
	pinMode(R_HIGH, OUTPUT);
	softPwmCreate(L_ENABLE, 100, 100);
	softPwmCreate(R_ENABLE, 100, 100);

	// Reset pins
	digitalWrite(L_HIGH, LOW);
	digitalWrite(L_LOW, LOW);
	digitalWrite(R_HIGH, LOW);
	digitalWrite(R_LOW, LOW);
}

void convergeToLane(){

	digitalWrite(L_HIGH, HIGH);
	digitalWrite(L_LOW, LOW);
	digitalWrite(R_HIGH, HIGH);
	digitalWrite(R_LOW, LOW);

	if(deviation == 0){
		softPwmWrite(L_ENABLE, THRUST_4);
		softPwmWrite(R_ENABLE, THRUST_4);
    cout<<"Converged"<<endl;
	}
	//if deviation negative turn left
	else if(deviation < 0 && deviation >= -CONV_1){
		softPwmWrite(L_ENABLE, THRUST_3);
		softPwmWrite(R_ENABLE, THRUST_4);
		cout<<"Left converge I"<<endl;
	}
	//if deviation negative go left 2
	else if(deviation < -CONV_1 && deviation >=-CONV_2){
		softPwmWrite(L_ENABLE, THRUST_2);
		softPwmWrite(R_ENABLE, THRUST_4);
		cout<<"Left converge II"<<endl;
	}
	//if deviation negative go left 3
	else if(deviation < -CONV_2 && abs(deviation) < DEV_MAX){
		softPwmWrite(L_ENABLE, THRUST_1);
		softPwmWrite(R_ENABLE, THRUST_4);
		cout<<"Left converge III"<<endl;
	}
	//if deviation positive turn right
	else if(deviation > 0 & deviation <= CONV_1){
		softPwmWrite(L_ENABLE, THRUST_4);
		softPwmWrite(R_ENABLE, THRUST_3);
		cout<<"Right converge I"<<endl;
	}
	//if deviation positive turn right 2
	else if(deviation > CONV_1 && deviation <= CONV_2){
		softPwmWrite(L_ENABLE, THRUST_4);
		softPwmWrite(R_ENABLE, THRUST_2);
			cout<<"Right converge II"<<endl;
	}
	//if deviation positive turn right 3
	else if(deviation > CONV_2 && abs(deviation) < DEV_MAX){
		softPwmWrite(L_ENABLE, THRUST_4);
		softPwmWrite(R_ENABLE, THRUST_1);
			cout<<"Right converge III"<<endl;
	}
	// In case of DEV_MAX, stop the car
	else{
		softPwmWrite(L_ENABLE, 0);
		softPwmWrite(R_ENABLE, 0);
		cout<<"Lane lost"<<endl;
	}
}

// display what the av sees
void displayResults(){

	// Display deviation from center line
	deviationStream.str("");
	deviationStream.clear();
	deviationStream<<"Deviation: "<<deviation<<"px";
	putText(frame, deviationStream.str(), Point2f(1,50), 0, 1, Scalar(255,255,0), 2);

	// Present results
	namedWindow("RGB", WINDOW_KEEPRATIO);
	moveWindow("RGB", 50, 100);
	resizeWindow("RGB",360, 240);
	imshow("RGB", frame);

	namedWindow("BirdsEyeB&W", WINDOW_KEEPRATIO);
	moveWindow("BirdsEyeB&W", 410, 100);
	resizeWindow("BirdsEyeB&W",360, 240);
	imshow("BirdsEyeB&W", framePerspective);

	namedWindow("ThresholdFilter", WINDOW_KEEPRATIO);
	moveWindow("ThresholdFilter", 410, 360);
	resizeWindow("ThresholdFilter",360, 240);
	imshow("ThresholdFilter", frameFinal);

}

int main(int argc, char **argv){

	//Setup camera
	setupCamera(argc, argv, Camera);
	cout<<"Connecting to camera"<<endl;
	if (!Camera.open()){
		cout<<"Cannot connect to the camera"<<endl;
		return -1;
	}
	cout<<"Camera ID"<<Camera.getId()<<endl;

	// Setup communication with H-Bridge
	initWiringPi();

	while(1) {
		//auto start = std::chrono::system_clock::now();

		// Image processing
		captureImage();
		applyBirdsEye();
		applyThresholdFilter();
		histogram();
		laneFinder();
		displayResults();

		//Motion controller
    convergeToLane();

		//Log FPS
		// auto end = std::chrono::system_clock::now();
		// std::chrono::duration<double> elapsed_seconds = end-start;
		// float t = elapsed_seconds.count();
		// int FPS = 1/t;
		// cout<<"cam-i "<<FPS<<" fps"<<endl;
		waitKey(1);
	}

	return 0;

}
