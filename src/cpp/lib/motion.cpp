#include <iostream>
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
#include "motion.h"

using namespace std;

/****************************************************
* H-BRIDGE / MOTORS CONFIGURATION
*****************************************************/

//PARAMS -MOTION
#define THRUST_0 0
#define THRUST_1 30 //MIN THRUST in %
#define THRUST_2 45
#define THRUST_3 60
#define THRUST_4 80 //MAX THRUST in %

//PARAMS WiringPI pins
#define L_ENABLE 30
#define L_HIGH 22
#define L_LOW 21
#define R_LOW 24
#define R_HIGH 23
#define R_ENABLE 25


/*******************
* Public
*******************/
Motion::Motion(){
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

void Motion::setForward(){
  digitalWrite(L_HIGH, HIGH);
	digitalWrite(L_LOW, LOW);
	digitalWrite(R_HIGH, HIGH);
	digitalWrite(R_LOW, LOW);
}

void Motion::setBackward(){
  digitalWrite(L_HIGH, LOW);
	digitalWrite(L_LOW, HIGH);
	digitalWrite(R_HIGH, LOW);
	digitalWrite(R_LOW, HIGH);
}

void Motion::goStraight(){
  softPwmWrite(L_ENABLE, THRUST_4);
  softPwmWrite(R_ENABLE, THRUST_4);
}

void Motion::stop(){
  softPwmWrite(L_ENABLE, THRUST_0);
  softPwmWrite(R_ENABLE, THRUST_0);
}

void Motion::turnLeft(int grade){
    softPwmWrite(R_ENABLE, THRUST_4);
  if(grade == 1){
    softPwmWrite(L_ENABLE, THRUST_3);
  } else if(grade == 2){
    softPwmWrite(L_ENABLE, THRUST_2);
  } else if(grade == 3){
    softPwmWrite(L_ENABLE, THRUST_1);
  } else if(grade == 4){
    softPwmWrite(L_ENABLE, THRUST_0);
  } else {
    softPwmWrite(L_ENABLE, THRUST_2);
  }
}

void Motion::turnRight(int grade){
    softPwmWrite(L_ENABLE, THRUST_4);
  if(grade == 1){
    softPwmWrite(R_ENABLE, THRUST_3);
  } else if(grade == 2){
    softPwmWrite(R_ENABLE, THRUST_2);
  } else if(grade == 3){
    softPwmWrite(R_ENABLE, THRUST_1);
  } else if(grade == 4){
    softPwmWrite(R_ENABLE, THRUST_0);
  } else {
    softPwmWrite(R_ENABLE, THRUST_2);
  }
}
