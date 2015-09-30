# ArduKobuki - Kobuki controled by voice commands

## Summary
This project aims the implementation of hardware and software that lets you control the Kobuki Turtlebot robot navigation by simple voice commands such as "forward", "turn right", "stop", etc.
The robot is controlled by an Arduino, which receives voice commands from an Android client, via Bluetooth.

## What's done
  - Levantamento de requisitos e design da arquitetura do sistema
  - Controlar o robô Turtlebot via Arduino (ver secção Recursos)
  - Implementação de cliente móvel com comunicação Android <-> Arduino via Bluetooth
  - Adição de reconhecimento de voz à App 
  - Adição de envio de comandos para o Arduino
  - Desenvolvimento da biblioteca Arduino para controlar o robô com base nos comandos recebidos

## Used tech
  * Kobuki Turtlebot
  * Arduino + HC06 bluetooth module + breadboard
  * Wires/clabes
  * Smartphone/tablet

## Architecture Diagrams
**Solution architecture**

![Solution architecture](http://i.imgur.com/v780PXY.png?500)


**Block diagram**

![Block diagram](http://i.imgur.com/OX2TqgE.png?500)

## Project memo


**Project paper**

[Voice Interface for Robot Instructions](http://www.slideshare.net/batistaluisfilipe/voice-interface-for-robot-instruction)

**Video demo**

[Kobuki controlled by voice commands](https://www.youtube.com/watch?v=MuiPCSegu9s)

## Resources
[Android client (direct download)](http://ge.tt/api/1/files/556RAOL2/0/blob?download)

**Libs used**

[Pocket Sphinx, for speech recognition](http://cmusphinx.sourceforge.net)

[Amarino, for bluetooth communication](http://amarino-toolkit.net)

**Arduino - Kobuki connection**

![Arduino - Kobuki connection](http://i.imgur.com/SjBTjUf.jpg?500)

## Related projects
[Arduino and Android Voice control Robot](http://learnhowtomakerobot.blogspot.in/2014/11/arduino-and-android-voice-control-robot.html)
