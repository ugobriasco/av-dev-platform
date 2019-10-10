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
int lMotorParams [2] = {130, 255};
int rMotorParams [2] = {130, 255};


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
  lMotorParams[0] = min;
  lMotorParams[1] = max;
}

// Calibrate right motor
void calibrateLeftMotor(int min, int max){
  lMotorParams[0] = min;
  lMotorParams[1] = max;
}

// Apply linear working curve
int applyLinearCurve(int min, int max, int value){
  if(value = 0){
    return 0
  }
  else if(value > 99){
    return 255
  }
  else {
      int d = max - min;
      return round(min + ((value * (max - min))/100));
  }
}

// Apply linear calibration to the left motor
int leftMotion(int value){
  return applyLinearCurve(lMotorParams[0], lMotorParams[1], value)
}

// Apply linear calibration to right motor
int righMotion(int value){
 return applyLinearCurve(rMotorParams[0], eMotorParams[1], value)
}



/*
* MOTION CONTROLLERS
*/


// Stop motion
void Stop(){
  digitalWrite(lHigh, LOW);
  digitalWrite(lLow, HIGH);
  analogWrite(lEnable,0);

  digitalWrite(rHigh, LOW);
  digitalWrite(rLow, HIGH);
  analogWrite(rEnable,0);
}

// Forward motion
void GoFw(int r, int l){
  digitalWrite(lHigh, LOW);
  digitalWrite(lLow, HIGH);
  analogWrite(lEnable, leftMotion(l));

  digitalWrite(rHigh, LOW);
  digitalWrite(rLow, HIGH);
  analogWrite(rEnable, rightMotion(r));
}

// Backwards motion
void GoBk(int r, int l){
  digitalWrite(lHigh, HIGH);
  digitalWrite(lLow, LOW);
  analogWrite(lEnable,leftMotion(l));

  digitalWrite(rHigh, HIGH);
  digitalWrite(rLow, LOW);
  analogWrite(rEnable,rightMotion(r));
}

void loop() {
  GoFw(0, 0); //min 130 mx 255
}
