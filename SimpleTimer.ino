#include <RTClib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>



struct switchTime {
  int dow;
  int shour;
  int smin;
  int duration;
};

typedef switchTime SwitchTime;

ESP8266WebServer server(80);
RTC_DS1307 rtc;
DateTime now;
bool lineState = false;
bool switchState = false;
bool testMode = false;

SwitchTime timings[20];

void setup() {
  timings[0].dow = 0;
  timings[0].shour = 13;
  timings[0].smin = 00;
  timings[0].duration = 20;

  timings[1].dow = 1;
  timings[1].shour = 13;
  timings[1].smin = 0;
  timings[1].duration = 20;
  
  timings[2].dow = 2;
  timings[2].shour = 13;
  timings[2].smin = 0;
  timings[2].duration = 20;
  
  timings[3].dow = 3;
  timings[3].shour = 13;
  timings[3].smin = 0;
  timings[3].duration = 20;
 
  timings[4].dow = 4;
  timings[4].shour = 13;
  timings[4].smin = 0;
  timings[4].duration = 20;
  
  timings[5].dow = 5;
  timings[5].shour = 13;
  timings[5].smin = 0;
  timings[5].duration = 20;
  
  timings[6].dow = 6;
  timings[6].shour = 13;
  timings[6].smin = 0;
  timings[6].duration = 20;

//TEST SLOT
  timings[6].dow = 6;
  timings[6].shour = 14;
  timings[6].smin = 40;
  timings[6].duration = 20;

  pinMode(LED_BUILTIN, OUTPUT); 
  pinMode(D6, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(D6, HIGH);

  Serial.begin(115200);
  WiFi.softAP("GWATERONE", "wind"); //begin WiFi access point 
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  
  server.begin();
  Wire.begin(D2,D1);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  } 
  else {
    Serial.println("RTC OK!!");
  }
  //rtc.adjust(DateTime(2018,5,12,14,33,0));
}

void handleOn() {
  testMode=true;
  server.send(200, "text/plain", "Water start");
}

void handleOff() {
  testMode = false;
  server.send(200, "text/plain", "Water stop");
}


void handleRoot() {
  now = rtc.now();
  int h = now.hour();
  int m = now.minute();
  char message[1024];
  char strBuffer[50];
  

  Serial.println(h);
  Serial.println(m);
  
  sprintf(message, " Zeit auf System: %0.2d:%0.2d  Tag: %0.2d\n\n",h,m, now.dayOfTheWeek());

  for (int i=0; i<20; i++) {
    sprintf (strBuffer, "    Slot: %0.2d Tag; %0.2d Zeit: %.02d:%0.2d Dauer: %0.2d min.\n", i, timings[i].dow, timings[i].shour, timings[i].smin, timings[i].duration);
    strcat (message, strBuffer); 
  }
  
  server.send(200, "text/plain", message);
  
  //digitalWrite(LED_BUILTIN, HIGH);
}

bool timeToWater() {
  now = rtc.now();
  
  long sec = now.secondstime();

  lineState = false;
  for (int i=0; i<20; i++){
    DateTime switchTiming = DateTime(now.year(), now.month(), now.day(), timings[i].shour, timings[i].smin, 0);
    long y = switchTiming.secondstime();

    if (y<sec && y+(timings[i].duration*60) > sec && now.dayOfTheWeek() == timings[i].dow) {
      lineState=true;
    }
  }
  if (lineState) {
    digitalWrite(LED_BUILTIN,LOW);
  }
  else {
    digitalWrite(LED_BUILTIN,HIGH);
  }
  return lineState;
}

void switchOnDevice() {
  if (!switchState) {
    digitalWrite(D6, LOW);
    switchState=true;
  }
}

void switchOffDevice() {
  if (switchState) {
    digitalWrite(D6, HIGH);
    switchState = false;
  }
}

void loop() {
//  now = rtc.now();
//  long sec = now.secondstime();
//  DateTime switchTiming = DateTime(now.year(), now.month(), now.day(), 12, 53,0);
//  long y = switchTiming.secondstime();
  
  server.handleClient();  
  if (timeToWater() || testMode){
    switchOnDevice();
  }
  else {
    switchOffDevice();
  }
  delay(200);
}






