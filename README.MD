# Autonomous Vehicles Development Platform
This is a collection of applications for autonomous vehicles targeting budget devices like RaspberryPi3 and Arduino uno. It aims to provide a baseline of R&D of autonomous ground (AGV), surface (ASV) and flying (AFV) vehicles.

## Turtle-S
Modular AGV prototype including a RaspberryPI (RPI) as main computation unit and a Arduino (INO) as motion controller. The INO controls 2DC motors via H-Bridge, while the RPI takes care of image recognition and definition of trajectory. The communication between INO and RPI uses the USB serial port. It provides:
* Linear convergence to lane using image recognition.
* Modular architecture and decoupling between motion controller and computational unit.

### Installation
* Install OpenCV (TBD)
* Load the following script to the Arduino controller:
```
  ./src/ino/duo-motors
```
* Compile the turtle-s bundle in your RPI:
```
./sbin/turtle-s
```
* Run the program in the RPI


## Turtle-g
RPI-Only AGV prototype. This model does not require any usage of microcontrollers for controlling the 2 DC motors via H bridge, as it uses the GPIO interface proper of the RaspberryPI.
* Linear convergence to lane using image recognition.
* Fast feedback response between sensor and actuators.

### Installation
* Install opencv (TBD)
* Load the following script to the Arduino controller:
```
  ./src/ino/duo-motors
```
* Compile the turtle-s bundle in your RPI:
```
./sbin/turtle-s
```
* Run the program in the RPI
