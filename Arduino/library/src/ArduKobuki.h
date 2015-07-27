/*
  ArduKobuki.h - Library for controlling the Kobuki Robot trough an Arduino board.
  Created by Paulo Fernandes, Autonumous Systems, IPG MCM Master, July 14, 2015.
  Released into the public domain.
*/
#ifndef ArduKobuki_h
#define ArduKobuki_h

#include "Arduino.h"

class ArduKobuki
{
  public:

  /** FUNCTIONS / METHODS **/	
    ArduKobuki();
	int  setState(int thisState);
    int  getPanicButtonState(int thisState);
	int  checkTimerOut(int thisState, int thisTimerCount);
    int  getBumperHit();
    void setStandby();
	void setVoiceCommand();
    int  setFront();
    int  setBack();
    int  setTurnLeft();
    int  setTurnRight();
    int  setStopBreak();
    void setExploreMode(int thisCommand);
    void moveForward();
    void moveBack();
    void rotateLeft(int rotationLeft);
    void rotateRight(int rotationRight);
    void stopRobot();
    int  gearUp(int up);
    int  gearDown(int down);
    void angleTurnReset();
    void DecreaseTurnAngle();
    void sendMessToAndroid(String message);


  private:

  /** PINS **/
  const int _analogPin = A0;      // Serial port
  const int _pinBumpLeft = 4;     // the pin for the left bumper - bumperLeft DO0
  const int _pinBumpCentre = 3;   // the pin for the middle bumper - bumperCenter DO1
  const int _pinBumpRight = 2;    // the pin for the right bumper - bumperRight DO2
  const int _pinMotor_L = 5;      // the pin for the left motor power - MotorRightWheel AI0?? - A porta AI0 está a controlar o motor esquerdo
  const int _pinMotor_R = 6;      // the pin for the right motor power - MotorLeftWheel AI1?? - A porta AI1 está a controlar o motor direito
  const int _pinPanicButton = 7;  // the pin for the Panic Button

/** CALIBRATION **/
  const float _frictionCorr = 1.08; //Friction Correction due to pavement finishing
  const int _timerOut = 10;         //[Seconds] Timelapse until the next voice command without switching to Stand-by.
  const int _turnTimerOut = 3;      //[Seconds] Timelapse for refining the Change Direction Command. If overdue, the next turning is 90 degrees.

/** ROBOT STATES **/
  const int _STAND_BY = 0;
  const int _VOICE_COMMAND = 1;
  const int _EXPLORE_MODE = 2;
  const int _STOP_OFF = 3;

/** VOICE COMMANDS **/
  const int _NULL_COMMAND = 0;
  const int _HELLO_ROBOT = 1;
  const int _STOP_BREAK = 2;
  const int _FRONT = 3;
  const int _BACK = 4;
  const int _TURN_LEFT = 5;
  const int _TURN_RIGHT = 6;
  const int _FASTER = 7;
  const int _SLOWER = 8;
  const int _EXPLORE = 9;
  const int _EXIT = 10;
  const int _SWITCH_OFF = 11;
  
/** PANIC BUTTON STATES **/
  const int _BT_PRESSED = 0;
  const int _BT_RELEASED = 1;
  
/** EXPLORE STATES **/
  const int _NO_OBSTACLE = 0;
  const int _TURN_RIGHT_30 = 1;
  const int _TURN_RIGHT_60 = 2;
  const int _BACK_A = 3;
  const int _BACK_B = 4;
  const int _BACK_C = 5;
  const int _TURN_LEFT_60 = 6;
  const int _TURN_LEFT_30 = 7;

/** SPEED GEAR **/
  const int _speedGear[7]={0,15,25,40,65,95,120};
  int _gear;
  int _robotSpeed; //de 0 a 120
  
/** DIRECTION CHANGE **/
  const double _angleChange[7]={90,45,22.5,11.25,5.625,2.81,1.41};
  int _angleLevel = 0;
  int _angleTurn;
  
/** TURN MODE **/  
  const int _MODE_STRAIGHT = 0;
  const int _MODE_LEFT = 1;
  const int _MODE_RIGHT = 2;
  int _turnMode;
  
/** VARIABLES **/
  int _state = _STAND_BY; 
  int _bumperLeft;
  int _bumperCentre;
  int _bumperRight;
  int _androidValue = 0; // Bluetooth variable
  int _actionMemory;
  
/** FUNCTIONS / METHODS **/	
  void _moveRobot(int speedRobot);
  void _rotateRobot(float angleTurn);
  void _setTurnRight30();
  void _setTurnRight60();
  void _setBackA();
  void _setBackB();
  void _setBackC();
  void _setTurnLeft60();
  void _setTurnLeft30();
  void _setGear(int thisGear);
  void _changeGear(int dif);
  void _angleTurnChange(int change);
  int  _getCommand();
  
};

#endif