#include <Bridge.h>
#include <Console.h>


#include <math.h>

 
int DHpin = 8;          // Pin for humidity and temperature digital sensor
byte dat [5];           // array for store humidity and temperature value

int lumPin  = A5;       // pin for luminosity sensor
int swPin   = 10;        // pin for switch

#include <dht11.h>

dht11 DHT;


// Arduino setup
 
void setup() {
  Bridge.begin();
  pinMode (DHpin, OUTPUT); // set the pin of humidity and temperature sensor in output mode.
  pinMode (swPin, INPUT);  // set the switch pin in input mode.
}
 
// Main loop
void loop() {
 
  int temp;
  int hum;
  int s;
  int lum;
  int chk;

  chk = DHT.read(DHpin);    // READ DATA
  hum = DHT.humidity;
  temp = DHT.temperature;
  
  lum = getLuminosity();    // get luminosity
  s = getSwitch();          // get switch position
 
 // send data to YUCCA
 sdpSendHTTP(buildMessage(temp, hum, lum, s), "user", "password");
 
 delay(5000);
}

// this function get the timestamp in ISO mode from Linino
String getTimeStamp() {
  String timestamp;
  Process time;
  time.runShellCommand("date -u +\"%Y-%m-%dT%H:%M:%SZ\"");
  while (time.available() > 0) {
    char c = time.read();
    if (c != '\n')
      timestamp += c;
  }
  return timestamp;
}


// This function get luminosity value (0 - 1023) from photoresistor
int getLuminosity() {
  int readVal = analogRead(lumPin);
  return readVal;
}

// This function get switch Value: 1 = pressed, 0 = released
int getSwitch() {
  int readVal = 1 - digitalRead(swPin);
  return readVal;
}

// Build JSON Message
String buildMessage(int temp, int humidity, int luminosity, int sw) {
  String msg = "";
  
  // Build JAS
  msg += "{\"stream\": \"environment\", \"sensor\": \"922c0438-9dfd-4ce2-fd3c-b17960b189cb\",\"values\": [{\"time\": \"";
  msg += getTimeStamp();
  msg += "\", \"components\": {\"temperatura\":";
  msg += String(temp, DEC);
  msg += ", \"umidita\":";
  msg += String(humidity, DEC);
  msg += ", \"switch\":";
  msg += String(sw, DEC);
  msg += ", \"luminosita\":";
  msg += String(luminosity, DEC);
  msg += "},\"validity\": \"valid\"}]}";
  return msg;

}

void sdpSendHTTP(String msg, String user, String pwd){
  Process p;
  String curlCMD;
  String credenziali;
  
  credenziali = user + ":" + pwd;
 
  // Build CURL Command
  curlCMD = "curl -H \"Content-Type: application/json\"";
  curlCMD += " -u " + credenziali;
  curlCMD += " -X POST -d '";
  curlCMD += msg;
  curlCMD += "' http://stream.smartdatanet.it/api/input/smartlab";
 p.runShellCommand(curlCMD);
}

