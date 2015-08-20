# Arduino Wifi
## Description
This project was part of my team project which named "Home Remote Control".

Basic concept of this project was like below.

<img src="https://github.com/sjroh/ArduinoWifi/blob/master/images/diagram.png" alt="Simple diagram of this project"/>

This git is have the code for inside of blue box from the diagram.

At first, I was planning to send a TCP packet from Server Arduino to Client Arduino.

However, I couldn't make it. I'm still looking for the solution.

In this program, Client Arduino will connect to Server Arduino to regularly check changed value in the Server.
This value is to turn on and off the connected electronic equipment. In this code, it will turn on and off connected LED on pin 13.

Client Arduino will connect to the electronic equipment through relay in the future or at the end.

## Server
<code>
setup(): setting up AP, TCP server (8090 port), pin mode.
oneSec(): turn on LED one second and turn off.
loop(): will recieve packets from other devices including Client Arduino.
returnStatus(int connectionId):
</code>
