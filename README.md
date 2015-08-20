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

void setup(): setting up AP, TCP server (8090 port), pin mode.

void oneSec(): turn on LED one second and turn off.

void loop(): will recieve packets from other devices including Client Arduino.

void returnStatus(int connectionId): it will run when the Client Arduino requested the on/off value.
(ServerIP:ServerPort/?mode=3&ip=ClientIP)

void closeConnection(int connectionId): close connection

void showData(int connectionId, String result): it will respond String variable to connected device.

void changeStatus(int connectionId): to change the status of client. 
(ServerIP:ServerPort/?mode=2&ip=ClientIP&power=[0/1])

void addToClientList(String target, int power): target is a given client IP. It will add client IP and power status into the vector variable.

String sendData(String command, const int timeout, boolean debug): It will send AT command to esp8266 chipset.

## Client

void setup(): Setting up client and join Server AP.

void loop(): Keep checking Server Arduino to turn on and off the LED (or else).

String sendData(String command, const int timeout, boolean debug): It will send AT command to esp8266 chipset.
