/*
* DUO MOTORs CONTROLLER
* Controller for two dc motors brushed.
*/

// Mapping PINs
const int lEnable = 10;
const int lHigh = 9;
const int lLow = 8;

const int rEnable = 7;
const int rHigh = 6;
const int rLow = 5;

// Default parameters
int leftMotorParams [2] = {130, 255};
int rightMotorParams [2] = {130, 255};

char rx_byte = 0;


void setup() {
  Serial.begin(9600);
  pinMode(lEnable, OUTPUT);
  pinMode(lHigh, OUTPUT);
  pinMode(lLow, OUTPUT);
  pinMode(rEnable, OUTPUT);
  pinMode(rHigh, OUTPUT);
  pinMode(rLow, OUTPUT);
}

/*
* CALIBRATION
*/

// Calibrate left motor
void calibrateLeftMotor(int min, int max){
  leftMotorParams[0] = min;
  leftMotorParams[1] = max;
}

// Calibrate right motor
void calibrateRighttMotor(int min, int max){
  rightMotorParams[0] = min;
  rightMotorParams[1] = max;
}

/*
* APPLY CALIBRATION
*/

// Apply linear working curve
int applyLinearCurve(int min, int max, int value){
  if(value == 0){
    return 0;
  }
  else if(value > 99){
    return 255;
  }
  else {
      int d = max - min;
      return round(min + ((value * d)/100));
  }
}

// Apply linear calibration to the left motor
int leftMotion(int value){
  return applyLinearCurve(leftMotorParams[0], leftMotorParams[1], value);
}

// Apply linear calibration to right motor
int rightMotion(int value){
 return applyLinearCurve(rightMotorParams[0], rightMotorParams[1], value);
}

/*
* MOTION CONTROLLERS
* set goFw(0,0) || goBk(0,0) to stop motion
* set goFw(100, 100) to have max thrust
* set goFw(50, 100) to curve left
* set goFw(100, 50) to curve right
*/

// Forward motion
void GoFw(int l, int r){
  digitalWrite(lHigh, LOW);
  digitalWrite(lLow, HIGH);
  analogWrite(lEnable, leftMotion(l));

  digitalWrite(rHigh, LOW);
  digitalWrite(rLow, HIGH);
  analogWrite(rEnable, rightMotion(r));

  //logMotion(l, r, leftMotion(l), rightMotion(r));
}

// Backwards motion
void GoBk(int l, int r){
  digitalWrite(lHigh, HIGH);
  digitalWrite(lLow, LOW);
  analogWrite(lEnable,leftMotion(l));

  digitalWrite(rHigh, HIGH);
  digitalWrite(rLow, LOW);
  analogWrite(rEnable,rightMotion(r));
}

/**
* LOGGING
*/

void logMotion (int l, int r, int lSent, int rSent) {
  Serial.print("Motor");
  Serial.print("\t");
  Serial.print("Left: ");
  Serial.print(l);
  Serial.print(" --> ");
  Serial.print(lSent);
  Serial.print("\t");
  Serial.print("Right: ");
  Serial.print(r);
  Serial.print(" --> ");
  Serial.print(rSent);
  Serial.println();
}

void listenToSerial(){
  // if (Serial.available() > 0) {    // is a character available?
  //   rx_byte = Serial.read();       // get the character
  //
  //   // check if a number was received
  //   if ((rx_byte >= '0') && (rx_byte <= '9')) {
  //     Serial.print("Number received: ");
  //     Serial.println(rx_byte);
  //   }
  //   else {
  //     Serial.println("Not a number.");
  //   }
  // } // end: if (Serial.available() > 0
  while (Serial.available() > 0) { // if any data available
   char incomingByte = Serial.read(); // read byte
   Serial.write(incomingByte); // send it back
 }
}


/**
* MAIN
*/

void loop() {

  int motion_left = 0;
  int motion_right = 0;

  listenToSerial();

  GoFw(motion_left, motion_right); //min 0 max 100
}
