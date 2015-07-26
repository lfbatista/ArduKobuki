/*
  ArduKobuki.cpp - Library for controlling the Kobuki Robot trough an Arduino board.
  Created by Paulo Fernandes, Autonumous Systems, IPG MCM Master, July 14, 2015.
  Released into the public domain.
*/

#include "Arduino.h"
#include "ArduKobuki.h"

//Class constructor: 
ArduKobuki::ArduKobuki() {

  Serial.begin(9600);
  
  pinMode(_pinBumpLeft, INPUT);           // set the left bumper pin as reader
  pinMode(_pinBumpCentre, INPUT);         // set the middle bumper pin as reader
  pinMode(_pinBumpRight, INPUT);          // set the right bumper pin as reader
  pinMode(_pinMotor_L, OUTPUT);           // set the Left Engine pin as writer
  pinMode(_pinMotor_R, OUTPUT);           // set the Right Engine pin as writer
  pinMode(_pinPanicButton, INPUT_PULLUP); // Set the Panic Button pin as PULLUP
  
  stopRobot();
  _setGear(0);
  
}

// Sets the state for the State Machine according to the changes in the flow.
// Keeps the value in a local variable for checking in other Methods.
int ArduKobuki::setState(int thisState){
  _state = thisState;
  return thisState;
}

// Detects the event in the Panic Button. In case so, determines the state to STOP_OFF.
int ArduKobuki::getPanicButtonState(int thisState) {
  int panicButton = digitalRead(_pinPanicButton);

  switch (panicButton){
    case 0: //_BT_PRESSED:
      _setGear(0);
      stopRobot();
	  _state = _STOP_OFF;
	  return _STOP_OFF; 
	  break;
    case 1: //_BT_RELEASED:
      return thisState;
	  break;
  }
}

// Checks the time lapse since the las command. Sets the state do STAND_BY if longer than 10sec
int ArduKobuki::checkTimerOut(int thisState, int thisTimerCount){
  if (thisTimerCount > _timerOut*100 && _state == _VOICE_COMMAND){
    _setGear(0);
    stopRobot();
    _state = _STAND_BY;
	return _STAND_BY;
  } else if (thisTimerCount == _turnTimerOut*100) {
	_turnMode = _MODE_STRAIGHT;
    angleTurnReset();
	return thisState;
  } else {
    return thisState;
  }
}

//Reading bumper pins
int ArduKobuki::getBumperHit() {

  _bumperLeft =   digitalRead(_pinBumpLeft);
  _bumperCentre = digitalRead(_pinBumpCentre);
  _bumperRight =  digitalRead(_pinBumpRight);

  //VOICE_COMMAND: (VC), EXPLORE_MODE: (EX)

  if (_bumperLeft == LOW && _bumperCentre==LOW && _bumperRight==LOW) {
    return _NO_OBSTACLE;   //No obstacles. Wait/execute command. (VC e EX)
  } else if (_bumperLeft == HIGH && _bumperCentre==LOW && _bumperRight==LOW){
    return _TURN_RIGHT_30; //Obstacle at the left hand side. Moves back and (VC) stops/(EX) turns to right hand side at least 30º and keeps moving.
  } else if (_bumperLeft == HIGH && _bumperCentre==HIGH && _bumperRight==LOW){
    return _TURN_RIGHT_60; //Obstacle at the left hand side. Moves back and (VC) stops/(EX) turns to right hand side at least 60º and keeps moving.
  } else if (_bumperLeft == HIGH && _bumperCentre==LOW && _bumperRight==HIGH){
    return _BACK_A;        //No space to move forward. Moves back and (VC) stops/(EX) rotates 180º, moves forward, rotates randomly and keeps moving.
  } else if (_bumperLeft == HIGH && _bumperCentre==HIGH && _bumperRight==HIGH){
    return _BACK_B;        //No space to move forward. Moves back and (VC) stops/(EX) rotates 180º, moves forward, rotates randomly and keeps moving.
  } else if (_bumperLeft == LOW && _bumperCentre==HIGH && _bumperRight==LOW){
    return _BACK_C;        //Obstacle ahead. Moves back and (VC) stops/(EX) rotates randomly and keeps moving.
  } else if (_bumperLeft == LOW && _bumperCentre==HIGH && _bumperRight==HIGH){
    return _TURN_LEFT_60;  //Obstacle at the right hand side. Moves back and (VC) stops/(EX) turns to left hand side at least 60º and keeps moving.
  } else if (_bumperLeft == LOW && _bumperCentre==LOW && _bumperRight==HIGH){
    return _TURN_LEFT_30;  //Obstacle at the right hand side. Moves back and (VC) stops/(EX) turns to left hand side at least 30º and keeps moving.
  }
}

// Set the Robot for Stand-by state.
void ArduKobuki::setStandby(){
  stopRobot();
  _setGear(0);
}

// Set definitions for starting the state of VOICE_COMMAND
void ArduKobuki::setVoiceCommand(){
  _setGear(2);
  angleTurnReset();
  _actionMemory = _STOP_BREAK;
}

// Sets the robot to move Forward and saves the state in memory.
int ArduKobuki::setFront(){
  moveForward();
  _actionMemory = _FRONT;
  return _FRONT;
}

// Sets the robot to move forward. Moves back for 0,5 sec, turns around and moves again.
int ArduKobuki::setBack(){
  moveBack();
  delay(500);
  rotateRight(180);
  moveForward();
  angleTurnReset();
  return _FRONT;
}

// Coordinates the action of turning the robot to the left hand side. After this action the robot returns to the previous state.
int ArduKobuki::setTurnLeft(){
  rotateLeft(_angleTurn);
  if (_turnMode != _MODE_STRAIGHT) DecreaseTurnAngle();
  _turnMode = _MODE_LEFT;
  return _actionMemory;
}

// Coordinates the action of turning the robot to the right hand side. After this action the robot returns to the previous state.
int ArduKobuki::setTurnRight(){
  rotateRight(_angleTurn);
  if (_turnMode != _MODE_STRAIGHT) DecreaseTurnAngle();
  _turnMode = _MODE_RIGHT;
  return _actionMemory;
}

// Sets every aspect of the robot for standing still.
int ArduKobuki::setStopBreak(){
    stopRobot();  
    angleTurnReset();
    _actionMemory = _STOP_BREAK;
	return _STOP_BREAK;
}

// Function to operate the low level function moving the robot to the front.
// 127 to 135 engines won´t run
void ArduKobuki::moveForward() { // min 0, max 120
  _moveRobot(135 + abs(_robotSpeed));
}
// Function to operate the low level function moving the robot to the back.
void ArduKobuki::moveBack() { // min 0, max 127
  _moveRobot(127 - abs(_robotSpeed));
}

// Function to operate the low level function turning it to the left.
void ArduKobuki::rotateLeft(int leftRotation) { // in degrees
  _rotateRobot(abs(leftRotation)*-1);
}

// Function to operate the low level function turning it to the right.
void ArduKobuki::rotateRight(int rightRotation) { // in degrees
  _rotateRobot(abs(rightRotation));
}

// Function to operate the low level function, setting the robot for standing still.
void ArduKobuki::stopRobot(){
  _moveRobot(127);
}

// Moving the Robot in with the designated speed
void ArduKobuki::_moveRobot(int speedRobot) {
  analogWrite(_pinMotor_L, speedRobot);
  analogWrite(_pinMotor_R, speedRobot);
}

// Rotating the Robot with the designated rotation.
// Calibration based on the 90 degrees turn.
void ArduKobuki::_rotateRobot(float angleTurn) {
  if (_angleTurn != 0 && _state != _STOP_OFF) {
    int rotation = 0;
    while (rotation < _angleTurn || _getCommand() != _STOP_BREAK) {
      rotation += 5;
      int side = angleTurn/abs(angleTurn);
      float timeFactor = 5/90;
	  //920 and 140 are calibration origin factors
      float turnTime = 920*timeFactor+140*_frictionCorr;
    
	  //The engine speed is 160 for either sides.
      analogWrite(_pinMotor_L, 160 * side);
      analogWrite(_pinMotor_R, 160 * (side * -1) );
      delay(turnTime);
    }
    stopRobot();
  }
}

// Sets the flow for the explore state, managing the movements according to the bumpers events.
void ArduKobuki::setExploreMode(int thisBumper) {
  _setGear(2);
  switch (thisBumper){
    case 0: //_NO_OBSTACLE:
      setFront();
      break;
    case 1: //_TURN_RIGHT_30:
      _setTurnRight30();
      break;
    case 2: //_TURN_RIGHT_60:
      _setTurnRight60();
      break;
    case 3: //_BACK_A:
      _setBackA();
      break;
    case 4: //_BACK_B:
      _setBackB();
      break;
    case 5: //_BACK_C:
      _setBackC();
      break;
    case 6: //_TURN_LEFT_60:
      _setTurnLeft60();
      break;
    case 7: //_TURN_LEFT_30:
      _setTurnLeft30();
      break;
  }
}

// Sequence defined to avoid the obstacle detected. Rotation of thirty degrees.
void ArduKobuki::_setTurnRight30() {
    moveBack();
    delay(300);
    rotateRight(30);
}

// Sequence defined to avoid the obstacle detected. Rotation of sixty degrees.
void ArduKobuki::_setTurnRight60() {
    moveBack();
    delay(300);
    rotateRight(60);
}

// Sequence defined to avoid the obstacle detected. Rotation of hundred and eighty degrees before a random turn.
void ArduKobuki::_setBackA(){
    moveBack();
    delay(500);
    rotateRight(180);
    moveForward();
    delay(1500);
    _angleTurn=random(-30,30);
    _rotateRobot(_angleTurn);
}

// Sequence defined to avoid the obstacle detected. Rotation of hundred and eighty degrees before a random turn.
void ArduKobuki::_setBackB () {
    moveBack();
    delay(500);
    rotateRight(180);
    moveForward();
    delay(1500);
    _angleTurn=random(-60,60);
    _rotateRobot(_angleTurn);
}

// Sequence defined to avoid the obstacle detected. Rotation in random turn.
void ArduKobuki::_setBackC() {
    moveBack();
    delay(500);
    _angleTurn=random(-90,90)+180;
    if (_angleTurn > 180) _angleTurn -= 360;
    _rotateRobot(_angleTurn);
}

// Sequence defined to avoid the obstacle detected. Rotation of sixty degrees.
void ArduKobuki::_setTurnLeft60() {
    moveBack();
    delay(300);
    rotateLeft(60);
}

// Sequence defined to avoid the obstacle detected. Rotation of thirty degrees.
void ArduKobuki::_setTurnLeft30() {
    moveBack();
    delay(300);
    rotateLeft(30);
}

// sets the specific speed for the robot movement.
void ArduKobuki::_setGear(int thisGear){
  _gear = thisGear;
  _robotSpeed = _speedGear[_gear];  
}

// Operates the low level function and sets the next level of speed for the robot movement.
// Sets back the previous state.
int ArduKobuki::gearUp(int up){
  _changeGear(up);
  return _actionMemory;
}

// Operates the low level function and sets the level beneath of speed for the robot movement.
// Sets back the previous state.
int ArduKobuki::gearDown(int down){
  _changeGear(down * -1);
  return _actionMemory;
}

// Low level function to define the robot Speed.
void ArduKobuki::_changeGear(int dif) {
  if (_actionMemory != _STOP_BREAK) {
    _gear += dif;
    if (_gear < 1) {
      _gear = 1;
    } else if (_gear > 6) {
      _gear = 6;
    }
    _robotSpeed = _speedGear[_gear];  
  }
}

// Operates the low level function to set the turn angle to the default value of 90 degrees.
// If no new turning command arrives in less than 3 seconds, the default value is set.
void ArduKobuki::angleTurnReset() {
  _angleTurnChange(-6);  
}

// Operates the low level function to decrease the turn angle to half of the previous value defined.
// If no new turning command arrives in less than 3 seconds, the default value will be determined
void ArduKobuki::DecreaseTurnAngle() {
  _angleTurnChange(1);
}

// Low level function to set the turning angle.
void ArduKobuki::_angleTurnChange(int change){
  _angleLevel += change;
  if (_angleLevel > 6) {
    _angleLevel = 6;
  } else if (_angleLevel < 0){
    _angleLevel = 0;
  } 
  _angleTurn = _angleChange[_angleLevel];
}

// Reads the serial port to detect the new command.
int ArduKobuki::_getCommand(){
  String stringSerial;
  _androidValue = analogRead(_analogPin);
  
  while (Serial.available() > 0) {
    char chars = Serial.read();
    
    if (chars > 0) {
      stringSerial += String(chars);

      if (stringSerial == " stop" || stringSerial == " break") {
        return _STOP_BREAK;
      } else if (stringSerial.length() > 6) {
        break;
      }
    }
  }
  return _NULL_COMMAND;
}

// Function to send a message back to the Android device. Future work.
 void sendMessToAndroid(String message) {
  //Insert the code in here. 	 
}