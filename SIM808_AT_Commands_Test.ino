#include<stdio.h>
#include<string.h>
#define DEBUG true

//www.makerfabs.com
//Fuction: Maduino Zero SIMI808 test: you can send AT Command via serial tool to know how to use SIM800L, 
//you can program to control it after you know how to use it.
//version:v1.2
//author:Charlin
//date:2019/10/30

int PWR_KEY = 9; //Power Key of SIM808
int LOW_PWR = 5;  //Low power control

void setup()
{

  delay(5000);
  pinMode(PWR_KEY, OUTPUT);
  pinMode(LOW_PWR, OUTPUT);
  digitalWrite(LOW_PWR, LOW);
  digitalWrite(PWR_KEY, LOW);
  delay(1100);
  digitalWrite(PWR_KEY, HIGH);//A pulse to turn on or turn off SIM808 
  delay(10000);
  
  SerialUSB.begin(115200);//USB Serial
  //while (!SerialUSB)
  {
    ; // wait for Arduino serial Monitor port to connect
  }
  Serial1.begin(115200);// serial connect to SIM808
  SerialUSB.println("Maduino Zero SIM808 Test Start!");
}

void loop()
{
  //get what form serial send to USB serial
  while (Serial1.available() > 0) {
    SerialUSB.write(Serial1.read());
    yield();
  }
  //get what form USB serial send to serial
  while (SerialUSB.available() > 0) {
    Serial1.write(SerialUSB.read());
    yield();
  }
}
