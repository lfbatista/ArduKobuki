/** LIBRARIES **/
  #include <ArduKobuki.h>
  #include <TimerOne.h>
  
/** CREATION OF ROBOT OBJECT **/
  ArduKobuki robot;

/** PINS **/
  const int analogPin = A0;      // Serial port

/** ROBOT STATES **/
  const int STAND_BY = 0;
  const int VOICE_COMMAND = 1;
  const int EXPLORE_MODE = 2;
  const int STOP_OFF = 3;

/** VOICE COMMANDS **/
  const int NULL_COMMAND = 0;
  const int HELLO_ROBOT = 1;
  const int STOP_BREAK = 2;
  const int FRONT = 3;
  const int BACK = 4;
  const int TURN_LEFT = 5;
  const int TURN_RIGHT = 6;
  const int FASTER = 7;
  const int SLOWER = 8;
  const int EXPLORE = 9;
  const int EXIT = 10;
  const int SWITCH_OFF = 11;
  
/** VARIABLES **/
  int state; 
  int bumper;
  int command;
  int timerCount;
  int androidValue = 0; // Bluetooth variable

void setup() {
  state = robot.setState(STAND_BY);
  command = NULL_COMMAND;
  timerCount = 0;
  
  Timer1.initialize(10000);
  Timer1.attachInterrupt(timerKB);
}

void timerKB() {
  state = robot.getPanicButtonState(state);
 
  if (state == VOICE_COMMAND) timerCount += 1;
  state = robot.checkTimerOut(state, timerCount);
  if (state == STAND_BY) {
    timerCount = 0;
    command = NULL_COMMAND;
  }   
}

void loop() {
  bumper = robot.getBumperHit();
  command = getCommand(command); 
 
  switch(state) {
    case STAND_BY: //Robot stands in Stand by mode and waits for "Hello Robot".
      robot.setStandby();
      if (command == HELLO_ROBOT) {
        state = robot.setState(VOICE_COMMAND);
        robot.setVoiceCommand();
      }
      break;

    case VOICE_COMMAND: //Robot executes the order coming from Mobile Device
      switch (command){
        case FRONT:
          command = robot.setFront();
          break;
        case BACK:
          command = robot.setBack();
          break;
        case TURN_LEFT:
          command = robot.setTurnLeft();
          break;
        case TURN_RIGHT:
          command = robot.setTurnRight();
          break;
        case STOP_BREAK:
          command = robot.setStopBreak();
          break;
        case FASTER:
          command = robot.gearUp(1);
          break;
        case SLOWER:
          command = robot.gearDown(1);
          break;
      }
      if (bumper > 0){
        robot.moveBack();
        delay(300);
        state = robot.setState(STAND_BY);
      } else {
        switch (command){
          case EXPLORE:
            state = robot.setState(EXPLORE_MODE);
            break;
          case SWITCH_OFF:
            state = robot.setState(STOP_OFF);
            break;
        }
      }
      break;

    case EXPLORE_MODE: //Robot explores the room
      robot.setExploreMode(bumper);
      switch (command){
        case EXIT:
          state = robot.setState(STAND_BY);
          break;
        case SWITCH_OFF:
          state = robot.setState(STOP_OFF);
          break;
      }
      break;    

    case STOP_OFF: //No action possible other than Switch Off
      robot.stopRobot();    
      break;
  }
  delay (50);

} //Loop end

int getCommand(int thisCommand){
  String stringSerial;
  androidValue = analogRead(analogPin);
  
  while (Serial.available() > 0) {
    char chars = Serial.read();
    
    if (chars > 0) {
      stringSerial += String(chars);

      if (stringSerial == " hello robot") {
        timerCount = 0;
        return HELLO_ROBOT;
      } else if (stringSerial == " stop" || stringSerial == " break") {
        timerCount = 0;
        return STOP_BREAK;
      } else if (stringSerial == " go front" || stringSerial == " go forward") {
        timerCount = 0;
        return FRONT;
      } else if (stringSerial == " go back" || stringSerial == " backwards") {
        timerCount = 0;
        return BACK;
      } else if (stringSerial == " go left" || stringSerial == " turn left") {
        timerCount = 0;
        return TURN_LEFT;
      } else if (stringSerial == " go right" || stringSerial == " turn right") {
        timerCount = 0;
        return TURN_RIGHT;
      } else if (stringSerial == " go faster" || stringSerial == " fast") {
        timerCount = 0;
        return FASTER;
      } else if (stringSerial == " go slower" || stringSerial == " slow") {
        timerCount = 0;
        return SLOWER;
      } else if (stringSerial == " go explore" || stringSerial == " explore") {
        timerCount = 0;
        return EXPLORE;
      } else if (stringSerial == " exit" || stringSerial == " stand by") {
        timerCount = 0;
        return EXIT;
      } else if (stringSerial == " switch off" || stringSerial == " go off") {
        timerCount = 0;
        return SWITCH_OFF;
      } else if (stringSerial.length() > 11) {
        break;
      }
    }
  }
  return thisCommand;
}