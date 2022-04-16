/*
  HelloServerBearSSL - Simple HTTPS server example

  This example demonstrates a basic ESP8266WebServerSecure HTTPS server
  that can serve "/" and "/inline" and generate detailed 404 (not found)
  HTTP respoinses.  Be sure to update the SSID and PASSWORD before running
  to allow connection to your WiFi network.

  Adapted by Earle F. Philhower, III, from the HelloServer.ino example.
  This example is released into the public domain.
*/
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include <umm_malloc/umm_malloc.h>
#include <umm_malloc/umm_heap_select.h>
#include <RtcDateTime.h>
#include "clock.h"

#ifndef STASSID
#define STASSID "ChoriNet 2.4Ghz"
#define STAPSK  "00434081431"
#endif

//closest NTP Server
#define NTP_SERVER "0.us.pool.ntp.org"

//GMT Time Zone with sign
#define GMT_TIME_ZONE -3

const char* ssid = STASSID;
const char* password = STAPSK;

BearSSL::ESP8266WebServerSecure server(443);
BearSSL::ServerSessions serverCache(5);

static const char serverCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDSzCCAjMCCQD2ahcfZAwXxDANBgkqhkiG9w0BAQsFADCBiTELMAkGA1UEBhMC
VVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNVBAcMDU9yYW5nZSBDb3VudHkx
EDAOBgNVBAoMB1ByaXZhZG8xGjAYBgNVBAMMEXNlcnZlci56bGFiZWwuY29tMR8w
HQYJKoZIhvcNAQkBFhBlYXJsZUB6bGFiZWwuY29tMB4XDTE4MDMwNjA1NDg0NFoX
DTE5MDMwNjA1NDg0NFowRTELMAkGA1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3Rh
dGUxITAfBgNVBAoMGEludGVybmV0IFdpZGdpdHMgUHR5IEx0ZDCCASIwDQYJKoZI
hvcNAQEBBQADggEPADCCAQoCggEBAPVKBwbZ+KDSl40YCDkP6y8Sv4iNGvEOZg8Y
X7sGvf/xZH7UiCBWPFIRpNmDSaZ3yjsmFqm6sLiYSGSdrBCFqdt9NTp2r7hga6Sj
oASSZY4B9pf+GblDy5m10KDx90BFKXdPMCLT+o76Nx9PpCvw13A848wHNG3bpBgI
t+w/vJCX3bkRn8yEYAU6GdMbYe7v446hX3kY5UmgeJFr9xz1kq6AzYrMt/UHhNzO
S+QckJaY0OGWvmTNspY3xCbbFtIDkCdBS8CZAw+itnofvnWWKQEXlt6otPh5njwy
+O1t/Q+Z7OMDYQaH02IQx3188/kW3FzOY32knER1uzjmRO+jhA8CAwEAATANBgkq
hkiG9w0BAQsFAAOCAQEAnDrROGRETB0woIcI1+acY1yRq4yAcH2/hdq2MoM+DCyM
E8CJaOznGR9ND0ImWpTZqomHOUkOBpvu7u315blQZcLbL1LfHJGRTCHVhvVrcyEb
fWTnRtAQdlirUm/obwXIitoz64VSbIVzcqqfg9C6ZREB9JbEX98/9Wp2gVY+31oC
JfUvYadSYxh3nblvA4OL+iEZiW8NE3hbW6WPXxvS7Euge0uWMPc4uEcnsE0ZVG3m
+TGimzSdeWDvGBRWZHXczC2zD4aoE5vrl+GD2i++c6yjL/otHfYyUpzUfbI2hMAA
5tAF1D5vAAwA8nfPysumlLsIjohJZo4lgnhB++AlOg==
-----END CERTIFICATE-----
)EOF";

static const char serverKey[] PROGMEM =  R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA9UoHBtn4oNKXjRgIOQ/rLxK/iI0a8Q5mDxhfuwa9//FkftSI
IFY8UhGk2YNJpnfKOyYWqbqwuJhIZJ2sEIWp2301OnavuGBrpKOgBJJljgH2l/4Z
uUPLmbXQoPH3QEUpd08wItP6jvo3H0+kK/DXcDzjzAc0bdukGAi37D+8kJfduRGf
zIRgBToZ0xth7u/jjqFfeRjlSaB4kWv3HPWSroDNisy39QeE3M5L5ByQlpjQ4Za+
ZM2yljfEJtsW0gOQJ0FLwJkDD6K2eh++dZYpAReW3qi0+HmePDL47W39D5ns4wNh
BofTYhDHfXzz+RbcXM5jfaScRHW7OOZE76OEDwIDAQABAoIBAQDKov5NFbNFQNR8
djcM1O7Is6dRaqiwLeH4ZH1pZ3d9QnFwKanPdQ5eCj9yhfhJMrr5xEyCqT0nMn7T
yEIGYDXjontfsf8WxWkH2TjvrfWBrHOIOx4LJEvFzyLsYxiMmtZXvy6YByD+Dw2M
q2GH/24rRdI2klkozIOyazluTXU8yOsSGxHr/aOa9/sZISgLmaGOOuKI/3Zqjdhr
eHeSqoQFt3xXa8jw01YubQUDw/4cv9rk2ytTdAoQUimiKtgtjsggpP1LTq4xcuqN
d4jWhTcnorWpbD2cVLxrEbnSR3VuBCJEZv5axg5ZPxLEnlcId8vMtvTRb5nzzszn
geYUWDPhAoGBAPyKVNqqwQl44oIeiuRM2FYenMt4voVaz3ExJX2JysrG0jtCPv+Y
84R6Cv3nfITz3EZDWp5sW3OwoGr77lF7Tv9tD6BptEmgBeuca3SHIdhG2MR+tLyx
/tkIAarxQcTGsZaSqra3gXOJCMz9h2P5dxpdU+0yeMmOEnAqgQ8qtNBfAoGBAPim
RAtnrd0WSlCgqVGYFCvDh1kD5QTNbZc+1PcBHbVV45EmJ2fLXnlDeplIZJdYxmzu
DMOxZBYgfeLY9exje00eZJNSj/csjJQqiRftrbvYY7m5njX1kM5K8x4HlynQTDkg
rtKO0YZJxxmjRTbFGMegh1SLlFLRIMtehNhOgipRAoGBAPnEEpJGCS9GGLfaX0HW
YqwiEK8Il12q57mqgsq7ag7NPwWOymHesxHV5mMh/Dw+NyBi4xAGWRh9mtrUmeqK
iyICik773Gxo0RIqnPgd4jJWN3N3YWeynzulOIkJnSNx5BforOCTc3uCD2s2YB5X
jx1LKoNQxLeLRN8cmpIWicf/AoGBANjRSsZTKwV9WWIDJoHyxav/vPb+8WYFp8lZ
zaRxQbGM6nn4NiZI7OF62N3uhWB/1c7IqTK/bVHqFTuJCrCNcsgld3gLZ2QWYaMV
kCPgaj1BjHw4AmB0+EcajfKilcqtSroJ6MfMJ6IclVOizkjbByeTsE4lxDmPCDSt
/9MKanBxAoGAY9xo741Pn9WUxDyRplww606ccdNf/ksHWNc/Y2B5SPwxxSnIq8nO
j01SmsCUYVFAgZVOTiiycakjYLzxlc6p8BxSVqy6LlJqn95N8OXoQ+bkwUux/ekg
gz5JWYhbD6c38khSzJb0pNXCo3EuYAVa36kDM96k1BtWuhRS10Q1VXk=
-----END RSA PRIVATE KEY-----
)EOF";

WiFiUDP ntpUDP;

// You can specify the time server pool and the offset, (in seconds)
// additionaly you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, NTP_SERVER, GMT_TIME_ZONE * 3600 , 60000);

const int CH1 = 12;
const int CH2 = 13;
const int CH3 = 15;
const int CH4 = 2;
//const int CH5 = 1;
//const int CH6 = 10;
const int CH7 = 16;
const int CH8 = 5;

void handleRoot() {
  digitalWrite(CH1, 1);
  digitalWrite(CH2, 1);
  digitalWrite(CH3, 1);
  digitalWrite(CH4, 1);
  digitalWrite(CH7, 1);
  digitalWrite(CH8, 1);
  timeClient.update();
  RtcDateTime rtcNow = getCurrentRtcDateTime();
  long diff = (long)rtcNow.Epoch32Time()-(long)timeClient.getEpochTime();
  server.send(200, "text/plain", "Hello from esp8266 over HTTPS! - RTC Date: "+getCurrentDate()+" - Internet Epoch Time: "+timeClient.getFormattedTime()+" || RTC Date: "+rtcNow.Epoch32Time()+" - Internet Epoch Time: "+timeClient.getEpochTime()+" - DIFF: "+diff);
  digitalWrite(CH1, 0);
  digitalWrite(CH2, 0);
  digitalWrite(CH3, 0);
  digitalWrite(CH4, 0);
  digitalWrite(CH7, 0);
  digitalWrite(CH8, 0);
}

void handleNotFound(){
  String message = "404 Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
  timeClient.begin();
  setupClock();
  pinMode(CH1, OUTPUT);
  pinMode(CH2, OUTPUT);
  pinMode(CH3, OUTPUT);
  pinMode(CH4, OUTPUT);
  pinMode(CH7, OUTPUT);
  pinMode(CH8, OUTPUT);
  
  digitalWrite(CH1, 0);
  digitalWrite(CH2, 0);
  digitalWrite(CH3, 0);
  digitalWrite(CH4, 0);
  digitalWrite(CH7, 0);
  digitalWrite(CH8, 0);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));

  // Cache SSL sessions to accelerate the TLS handshake.
  server.getServer().setCache(&serverCache);

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/new", [](){
    bool validHour = false;
    bool validMinutes = false;
    bool validName = false;
    int hour = NULL;
    int minutes = NULL;
    String name = "";
    for (uint8_t i=0; i<server.args(); i++){
      if(server.argName(i).equalsIgnoreCase("hour")){
        if(isValidNumber(server.arg(i))){
          hour = server.arg(i).toInt();
          if(hour>=0&&hour<=23){
            validHour = true;
          }
        }
      }
      if(server.argName(i).equalsIgnoreCase("minutes")){
        Serial.println(server.arg(i));
        if(isValidNumber(server.arg(i))){
          minutes = server.arg(i).toInt();
          if(minutes>=0&&minutes<=59){
            validMinutes = true;
          }
        }
      }
      if(server.argName(i).equalsIgnoreCase("name")){
        String nameParam = server.arg(i);
        nameParam.trim();
        if(!nameParam.equals("")){
          validName = true;
          name = nameParam;
        }
      }
    }
    Serial.print("hour param ");
    Serial.println(hour);
    Serial.print("Minute param ");
    Serial.println(minutes);
    Serial.print("name param ");
    Serial.println(name);
    if(validHour && validMinutes && validName){
      int alarmCount = addAlarm(name, hour, minutes, &setAll);
      server.send(200, "text/plain", "Alarm added - "+String(alarmCount));
    }else{
      server.send(200, "text/plain", "Params required...");
    }
  });

  server.on("/list", [](){
    String alarmsList = showAlarms();
    Serial.print("Alarms: ");
    Serial.println(alarmsList);
    server.send(200, "text/json", alarmsList);
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTPS server started");
}

boolean isValidNumber(String str){
  boolean isNum=false;
  for(byte i=0;i<str.length();i++)
  {
    isNum = isDigit(str.charAt(i));
    if(!isNum) return false;
  }
  return isNum;
}

void setAll(int value){
  Serial.println("Alarms!!!!!!!!!!!!!!");
  digitalWrite(CH1, value);
  digitalWrite(CH2, value);
  digitalWrite(CH3, value);
  digitalWrite(CH4, value);
  digitalWrite(CH7, value);
  digitalWrite(CH8, value);
}

void loop(void){
  server.handleClient();
  MDNS.update();
  alarmLoop();
}
