#include "ESP8266.h"
#include <SoftwareSerial.h>

#define SSID        "HELLO"
#define PASSWORD    "helloworld"
#define DEBUG true
#define HOST_NAME   "192.168.4.1"
#define HOST_PORT   (8090)

SoftwareSerial esp8266(2, 3); // RX:2, TX:3
ESP8266 wifi(esp8266);


String localIP = "";

void setup(void)
{
  Serial.begin(9600);
  Serial.print("setup begin\r\n");

  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());
  sendData("AT+RST\r\n", 2000, DEBUG); // reset module
  sendData("AT+CWMODE=1\r\n", 2000, DEBUG);
  sendData("AT+CIPMUX=0\r\n", 2000, DEBUG);
  sendData("AT+CWJAP=\"HELLO\",\"helloworld\"\r\n", 5000, DEBUG);
  sendData("AT+CIFSR\r\n", 5000, DEBUG);

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  localIP = wifi.getLocalIP().c_str();
  Serial.print("###" + localIP + "###");

  Serial.print("setup end\r\n");
}

void loop(void) {
  sendData("AT+CIFSR\r\n", 5000, DEBUG);

  uint8_t buffer[128] = {0};

  if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
    Serial.print("create tcp ok\r\n");
  } else {
    Serial.print("create tcp err\r\n");
  }

  String requestString = "mode=3&ip=" + localIP;
  char *request = (char*) requestString.c_str();

  wifi.send((const uint8_t*)request, strlen(request));

  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  if (len > 0) {
    Serial.print("Received:[");
    for (uint32_t i = 0; i < len; i++) {
      Serial.print((char)buffer[i]);
    }
    Serial.print("]\r\n");
  }

  if (buffer[0] == '0') {
    digitalWrite(13, LOW);
  }
  
  if (buffer[0] == '1') {
    digitalWrite(13, HIGH);
  }

  if (wifi.releaseTCP()) {
    Serial.print("release tcp ok\r\n");
  } else {
    Serial.print("release tcp err\r\n");
  }
  delay(5000);
}

String sendData(String command, const int timeout, boolean debug)
{
  String response = "";

  esp8266.print(command); // send the read character to the esp8266

  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {

      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }

  if (debug)
  {
    Serial.print(response);
  }

  return response;
}
