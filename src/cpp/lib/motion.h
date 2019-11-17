#ifndef MOTION
#define MOTION

void MOTION_init();
void MOTION_setForward();
void MOTION_goForward();

class Motion{
  public:
  Motion();
  void setForward();
  void setBackward();
  void goStraight();
  void stop();
  void turnLeft(int grade);
  void turnRight(int grade);
};

#endif
