#include <Arduino.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <LinkedList.h>

ThreeWire myWire(0, 14, 4);  // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

#define countof(a) (sizeof(a) / sizeof(a[0]))

#define ALARM_COUNT 1

struct Alarm {
  String name;
  boolean executed;
  int hour;
  int minute;
  void (*execute)(int);
  Alarm *next;
};

Alarm *firstAlarm = NULL;
Alarm *lastAlarm = NULL;

int addAlarm(String name, int hour, int minute, void (*execute)(int)){
  Alarm *newAlarm = new Alarm;
  newAlarm->name = name;
  newAlarm->hour = hour;
  newAlarm->minute = minute;
  newAlarm->execute = execute;
  newAlarm->executed = false;
  newAlarm->next = NULL;

  if(lastAlarm==NULL){
    firstAlarm = newAlarm;
  }else{
    lastAlarm->next = newAlarm;
  }

  lastAlarm = newAlarm;

  Alarm *currentAlarm = firstAlarm;
  RtcDateTime now = Rtc.GetDateTime();


  int count = 0;
  while(currentAlarm!=NULL){
    count++;
    currentAlarm = currentAlarm->next;
  }

  return count;
}

RtcDateTime getCurrentRtcDateTime() {
  return Rtc.GetDateTime();
}

String getCurrentDate() {
  String currentTime;
  RtcDateTime now = Rtc.GetDateTime();
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             now.Month(),
             now.Day(),
             now.Year(),
             now.Hour(),
             now.Minute(),
             now.Second());

  currentTime = datestring;
  return currentTime;
}

void alarmLoop(){
  RtcDateTime now = Rtc.GetDateTime();
  Alarm *currentAlarm = firstAlarm;
  int count = 0;
  while(currentAlarm!=NULL){

    if(!currentAlarm->executed &&
    currentAlarm->hour <= (int)now.Hour() &&
    (currentAlarm->hour < (int)now.Hour() || currentAlarm->minute <= (int)now.Minute())){
      currentAlarm->executed = true;
      Serial.println(currentAlarm->name+" - Executed at: "+getCurrentDate());
      currentAlarm->execute(0);
    }
    
    currentAlarm = currentAlarm->next;
  }
}

int alarmsCount(){
  Serial.println("showAlarms");
  Alarm *currentAlarm = firstAlarm;
  int count = 0;
  while(currentAlarm){
    count++;
    currentAlarm = currentAlarm->next;
  }

  return count;
}

String showAlarms(){
  Serial.println("showAlarms");
  String rtnValue = String("[");
  Alarm *currentAlarm = firstAlarm;

  while(currentAlarm){
    Serial.print("name: ");
    Serial.println(currentAlarm->name);
    
    rtnValue+=currentAlarm->name;
    if(currentAlarm->next){
      rtnValue+=",";
    }
    currentAlarm = currentAlarm->next;
  }

  rtnValue+="]";
  return rtnValue;
}

void setupClock() {
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  Serial.println();

  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC lost confidence in the DateTime!");
    Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected()) {
    Serial.println("RTC was write protected, enabling writing now");
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    Rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
}