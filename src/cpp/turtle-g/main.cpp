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

// Dependacies motion control
#include "motion.h"

// Dependacies manual control
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>


// Namespaces
using namespace std;
using namespace cv;
using namespace raspicam;


/****************************************************
* IMAGE PROCESSING CONFIGURATION
*****************************************************/

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

/****************************************************
* PRIMARRY OPERATING FUNCTIONS
*****************************************************/

//PARAMS -convergeToLane
#define DEV_MAX 102 //max deviation ±140px from current bird's eye settings (±30deg)
#define CONV_1 30   //deviation which triggers the first level of convergence
#define CONV_2 80   //deviation which triggers the second level of convergence


/****************************************************
* GLOBAL VARIABLES
*****************************************************/

//GLOBAL image processing
Mat frame, frameRGB, matrix, framePerspective, frameThreshold, frameCanny, frameFinal, frameFinalDuplicate;
Mat ROIlane;
vector<int> histogramLane;
int lanePosition, frameCenter, deviation;
RaspiCam_Cv Camera;
stringstream deviationStream;

//Talk to the h-bridge, controlling the movement of the turtle.
Motion motion;

//ROI SHAPE
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

/****************************************************
* METHODS
*****************************************************/

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

	// The camera is placed upside-down, thus flip it.
	flip(frame, frame,-1);

  // Adjust offset
  frame = rotateImage(frame, 3);
}

// Define region of interest
void applyBirdsEye(){
	// Transform image to RGB.
	cvtColor(frame, frameRGB, COLOR_BGR2RGB);

	// Define ROI
	line(frameRGB, Source[0], Source[1], Scalar(0,255,0), 2);
	line(frameRGB, Source[1], Source[3], Scalar(0,255,0), 2);
	line(frameRGB, Source[3], Source[2], Scalar(0,255,0), 2);
	line(frameRGB, Source[2], Source[0], Scalar(0,255,0), 2);

	// Copy to the origin
	line(frame, Source[0], Source[1], Scalar(0,255,0), 2);
	line(frame, Source[1], Source[3], Scalar(0,255,0), 2);
	line(frame, Source[3], Source[2], Scalar(0,255,0), 2);
	line(frame, Source[2], Source[0], Scalar(0,255,0), 2);

	matrix = getPerspectiveTransform(Source, Destination);
	warpPerspective(frameRGB, framePerspective, matrix, Size(360,240));
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

// Defines the deviation between center of vehicles and a black lane on the floor
void laneFinder(){
	vector<int>::iterator leftPtr;
	leftPtr = max_element(histogramLane.begin(), histogramLane.begin() + 360);
	lanePosition = distance(histogramLane.begin(), leftPtr) + 16; //Calibrated

	frameCenter = 180;

	line(frameFinal, Point2f(lanePosition, 0), Point2f(lanePosition, 240), Scalar(0,255,0), 2);
	line(frameFinal, Point2f(frameCenter, 0), Point2f(frameCenter, 240), Scalar(255,255,0), 2);

	deviation = lanePosition - frameCenter;

}

// Make the rover following a black lane
void convergeToLane(){

	motion.setForward();

	if(deviation == 0){
		motion.goStraight();
    cout<<"Converged"<<endl;
	}
	//if deviation negative turn left
	else if(deviation < 0 && deviation >= -CONV_1){
		motion.turnLeft(1);
		cout<<"Left converge I"<<endl;
	}
	//if deviation negative go left 2
	else if(deviation < -CONV_1 && deviation >=-CONV_2){
		motion.turnLeft(2);
		cout<<"Left converge II"<<endl;
	}
	//if deviation negative go left 3
	else if(deviation < -CONV_2 && abs(deviation) < DEV_MAX){
		motion.turnLeft(3);
		cout<<"Left converge III"<<endl;
	}
	//if deviation positive turn right
	else if(deviation > 0 & deviation <= CONV_1){
		motion.turnRight(1);
		cout<<"Right converge I"<<endl;
	}
	//if deviation positive turn right 2
	else if(deviation > CONV_1 && deviation <= CONV_2){
		motion.turnRight(2);
		cout<<"Right converge II"<<endl;
	}
	//if deviation positive turn right 3
	else if(deviation > CONV_2 && abs(deviation) < DEV_MAX){
		motion.turnRight(3);
		cout<<"Right converge III"<<endl;
	}
	// In case of DEV_MAX, stop the car
	else{
		motion.stop();
		cout<<"Lane lost"<<endl;
	}
}

// Display what the rover sees
void displayResults(){

	// Display deviation from center line
	deviationStream.str("");
	deviationStream.clear();
	deviationStream<<"Deviation: "<<deviation<<"px";
	putText(frame, deviationStream.str(), Point2f(1,50), 0, 1, Scalar(255,255,0), 2);

	// Present results
	namedWindow("Live", WINDOW_KEEPRATIO);
	moveWindow("Live", 50, 100);
	resizeWindow("Live",360, 240);
	imshow("Live", frame);

	namedWindow("BirdsEyeB&W", WINDOW_KEEPRATIO);
	moveWindow("BirdsEyeB&W", 410, 100);
	resizeWindow("BirdsEyeB&W",360, 240);
	imshow("BirdsEyeB&W", framePerspective);

	namedWindow("ThresholdFilter", WINDOW_KEEPRATIO);
	moveWindow("ThresholdFilter", 410, 360);
	resizeWindow("ThresholdFilter",360, 240);
	imshow("ThresholdFilter", frameFinal);

}

// Manual control
int kbhit(){
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    //DO whatever you like to do with this charecter ..

		/*
		* a=97, w=119,d=100,s=115,enter=10,space=32
		*/
		if(ch==97){
			cout<<"  left"<<endl;
			motion.turnLeft(4);
		} else if (ch==119){
			cout<<"  forward"<<endl;
			motion.setForward();
			motion.goStraight();
		} else if (ch==100){
			cout<<"  right"<<endl;
			motion.turnRight(4);
		} else if (ch==115){
			cout<<"  back"<<endl;
			motion.setBackward();
			motion.goStraight();
		} else if (ch==10 || ch==32){
			cout<<"  CLICK!"<<endl;
		} else {
			cout<<"  Stop"<<endl;
			motion.stop();
		}
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}


/****************************************************
* MAIN
*****************************************************/

int main(int argc, char **argv){

	//Setup camera
	setupCamera(argc, argv, Camera);
	cout<<"Connecting to camera"<<endl;
	if (!Camera.open()){
		cout<<"Cannot connect to the camera"<<endl;
		return -1;
	}
	cout<<"Camera ID"<<Camera.getId()<<endl;

	while(1) {

		//Control manually
		if(kbhit()){
			//clearing the buffer
			char ch = getchar();
			// printf("you hit keyboard and key = %c\n", ch);
		}


		// Image processing
		captureImage();
		applyBirdsEye();
		applyThresholdFilter();
		histogram();
		laneFinder();
		displayResults();

		//Tasks
		//convergeToLane();

		waitKey(1);
	}

	return 0;

}
