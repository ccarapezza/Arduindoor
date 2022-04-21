#include <Arduino.h>
#include "Mux.h"

using namespace admux;

const int muxSIG = D0;
const int muxS0 = D7;
const int muxS1 = D6;
const int muxS2 = D4;
const int muxS3 = D1;

Mux mux(Pin(muxSIG, OUTPUT, PinType::Digital), Pinset(muxS0, muxS1, muxS2, muxS3));

void setMpxChannel(byte channel)
{
   digitalWrite(muxS0, bitRead(channel, 0));
   digitalWrite(muxS1, bitRead(channel, 1));
   digitalWrite(muxS2, bitRead(channel, 2));
   digitalWrite(muxS3, bitRead(channel, 3));
}

void digitalOutputMode(){
  pinMode(muxSIG, OUTPUT);
  pinMode(muxS0, OUTPUT);
  pinMode(muxS1, OUTPUT);
  pinMode(muxS2, OUTPUT);
  pinMode(muxS3, OUTPUT);
}

void digitalInputMode(){
  pinMode(muxSIG, INPUT);
  pinMode(muxS0, OUTPUT);
  pinMode(muxS1, OUTPUT);
  pinMode(muxS2, OUTPUT);
  pinMode(muxS3, OUTPUT);
}

void setupMpx(){
  //digitalOutputMode();
}

void muxDigitalWrite(byte channel, boolean value){
  /*setMpxChannel(channel);
  digitalWrite(muxSIG, value);*/
  mux.channel(channel);
  mux.write(value);
  mux.channel(7);
  mux.write(value);
}