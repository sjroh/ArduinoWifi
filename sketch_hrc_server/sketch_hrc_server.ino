#include "ESP8266.h"
#include <SoftwareSerial.h>

#define SSID        "HELLO"
#define PASSWORD    "helloworld"
#define PORT        8090
#define DEBUG true

SoftwareSerial esp8266(2, 3); // RX:2, TX:3
ESP8266 wifi(esp8266);

// Minimal class to replace std::vector
template<typename Data>
class Vector {
  size_t d_size; // Stores no. of actually stored objects
  size_t d_capacity; // Stores allocated capacity
  Data *d_data; // Stores data
  public:
    Vector() : d_size(0), d_capacity(0), d_data(0) {}; // Default constructor
    Vector(Vector const &other) : d_size(other.d_size), d_capacity(other.d_capacity), d_data(0) { d_data = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(d_data, other.d_data, d_size*sizeof(Data)); }; // Copy constuctor
    ~Vector() { free(d_data); }; // Destructor
    Vector &operator=(Vector const &other) { free(d_data); d_size = other.d_size; d_capacity = other.d_capacity; d_data = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(d_data, other.d_data, d_size*sizeof(Data)); return *this; }; // Needed for memory management
    void push_back(Data const &x) { if (d_capacity == d_size) resize(); d_data[d_size++] = x; }; // Adds new value. If needed, allocates more space
    size_t size() const { return d_size; }; // Size getter
    Data const &operator[](size_t idx) const { return d_data[idx]; }; // Const getter
    Data &operator[](size_t idx) { return d_data[idx]; }; // Changeable getter
  private:
    void resize() { d_capacity = d_capacity ? d_capacity*2 : 1; Data *newdata = (Data *)malloc(d_capacity*sizeof(Data)); memcpy(newdata, d_data, d_size * sizeof(Data)); free(d_data); d_data = newdata; };// Allocates double the old space
};

class ClientSet {
  String ip = "";
  int power = 0;
  public:
    ClientSet() : ip(""), power(0) {};
    ClientSet(String s, int i) { ip = s; power = i; };
    String getIP() { return ip; };
    int getPower() { return power; };
    boolean checkIP(String s) { return s.equals(ip); };
    void setPower(int i) { power = i; };
    void setIP(String s) { ip = s; }; 
};

Vector<ClientSet> clientList;

void setup() {
  Serial.println("[SETUP BEGIN] ---------------------------------------------");
  Serial.begin(9600);
  esp8266.begin(9600);

  Serial.print("[SETUP] FW Version: ");
  Serial.println(wifi.getVersion().c_str());

  if (!wifi.restart()) {
    Serial.print("[FAIL] ");
  } else {
    Serial.print("[SETUP] ");
  } Serial.println("ESP8266 reseted");

  if (!wifi.enableMUX()) {
    Serial.print("[FAIL] ");
  } else {
    Serial.print("[SETUP] ");
  } Serial.println("Enabling MUX");

  if (!wifi.setOprToSoftAP()) {
    Serial.print("[FAIL] ");
  } else {
    Serial.print("[SETUP] ");
  } Serial.println("Setted up as an AP");

  if (!wifi.setSoftAPParam(SSID, PASSWORD)) {
    Serial.print("[FAIL] ");
  } else {
    Serial.print("[SETUP] ");
  } Serial.println("Setted up the parameters for AP");

  if (!wifi.startTCPServer(PORT)) {
    Serial.print("[FAIL] ");
  } else {
    Serial.print("[SETUP] ");
  } Serial.println("Starting TCP Server");

  if (!wifi.setTCPServerTimeout(15)) {
    Serial.print("[FAIL] ");
  } else {
    Serial.print("[SETUP] ");
  } Serial.println("Setting up TCP server timeout");

  Serial.print("[SETUP] Local IP: ");
  Serial.println(wifi.getLocalIP().c_str());

  Serial.print("[SETUP] Get Soft AP Parameters: ");
  Serial.println(wifi.getSoftAPParam().c_str());

  pinMode(13, OUTPUT);
  oneSec();

  Serial.println("[SETUP END] ---------------------------------------------");

  // 192.168.4.1:8090/?mode=1
  // 192.168.4.1:8090/?mode=2&ip=192.168.4.100&power=1
}

void oneSec() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
}

void loop() {
  if (esp8266.available()) {
    if (esp8266.find("+IPD,")) {
      oneSec();

      delay(100);
      int connectionId = esp8266.read() - 48;
      delay(100);
      Serial.print("[DEBUG] connection id : ");
      Serial.print(connectionId);
      Serial.print("\r\n");

      delay(100);
      esp8266.find("mode=");
      delay(100);
      int mode = esp8266.read() - 48;
      Serial.print("[DEBUG] mode : " + mode);
      Serial.print(mode);
      Serial.print("\r\n");

      delay(100);
      switch (mode) {
        case 1: // get all joined ips
          delay(100);
          getJoinedIP(connectionId);
          break;
        case 2: // change client status
          delay(100);
          changeStatus(connectionId);
          String reply = "MODE 2 OK";
          showData(connectionId, reply);
          break;
        case 3: { // from client
          delay(100);
          returnStatus(connectionId);
          String reply = "MODE 3";
          showData(connectionId, reply);
          break;
        }
        default:
          delay(100);
          printError("Wrong mode");
          String err = "ERROR";
          showData(connectionId, err);
      }
      closeConnection(connectionId);
    }

    oneSec();
  }
}

void returnStatus(int connectionId) {
  delay(100);
  Serial.println("[MODE 3] request from client");

  // getting an ip address
  String target = "";
  esp8266.find("ip=");
  // 192.168.4.nnn pattern, so length is 13.
  for (int i = 0; i < 13; i ++) {
    int read_temp = esp8266.read();
    if (read_temp == 46) {
      target += ".";
    } else {
      target += String(read_temp - 48);
    }
  }
  Serial.println("target ip : " + target);

  int power = 0;
  for (int i = 0; i < clientList.size(); i ++) {
    if (clientList[i].checkIP(target)) {
      power = clientList[i].getPower();
      Serial.println("I found one!");
    }
  }

  showData(connectionId, String(power));

  Serial.println("[MODE 3] END");
}

void closeConnection(int connectionId) {
  delay(100);
  String cipClose = "AT+CIPCLOSE=" + String(connectionId) + "\r\n";
  sendData(cipClose, 1000, DEBUG);
}

void showData(int connectionId, String result) {
  String cipSend = "AT+CIPSEND=";
  cipSend += connectionId;
  cipSend += ",";
  cipSend += result.length();
  cipSend += "\r\n";
  sendData(cipSend, 2000, DEBUG);
  sendData(result, 2000, DEBUG);
}

void getJoinedIP(int connectionId) {
  delay(100);
  Serial.println("[MODE 1] get joined IPs");
  Serial.println(wifi.getJoinedDeviceIP().c_str());
  String result = wifi.getJoinedDeviceIP().c_str();
  showData(connectionId, result);
  Serial.println("[MODE 1] END");
}

void changeStatus(int connectionId) {
  delay(100);
  Serial.println("[MODE 2] change status of one client");

  // getting an ip address
  String target = "";
  esp8266.find("ip=");
  // 192.168.4.nnn pattern, so length is 13.
  for (int i = 0; i < 13; i ++) {
    int read_temp = esp8266.read();
    if (read_temp == 46) {
      target += ".";
    } else {
      target += String(read_temp - 48);
    }
  }
  Serial.println("target ip : " + target);

  // getting a changed value; 0: Off, 1: On
  esp8266.find("power=");
  int power = esp8266.read() - 48;
  Serial.print("power : ");
  Serial.print(power);
  Serial.print("\r\n");

  addToClientList(target, power);

  Serial.println("[MODE 2] END");
}

void addToClientList(String target, int power) {
  Serial.println("Add To Client List START");
  boolean notexist = true;
  for (int i = 0; i < clientList.size(); i ++) {
    if (clientList[i].checkIP(target)) {
      clientList[i].setPower(power);
      notexist = false;
    }
  }
  if (notexist) {
    Serial.print("[MODE 2] Vector ");
    Serial.print(clientList.size());
    ClientSet temp(target, power);
    clientList.push_back(temp);
    Serial.print(" Added ");
    Serial.println(clientList.size());
  }
  Serial.println("Add To Client List END");
}

void printError(String s) {
  Serial.println("[ERROR] " + s);
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

