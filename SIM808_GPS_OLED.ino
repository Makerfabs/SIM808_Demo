#include<stdio.h>
#include<string.h>
#define DEBUG true

//www.makerfabs.com
//Fuction: Maduino Zero SIMI808 test: Show Location via OLED, 
//version:v1.0
//author:Charlin
//date:2019/10/31

int PWR_KEY = 9; //Power Key of SIM808
int LOW_PWR = 5;  //Low power control

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void(* resetFunc) (void) = 0; //declare reset function at address 0

void setup()
{
  delay(5000);
  pinMode(PWR_KEY, OUTPUT);
  pinMode(LOW_PWR, OUTPUT);
  SerialUSB.begin(115200);//USB Serial
  //while (!SerialUSB)
  {
    ; // wait for Arduino serial Monitor port to connect
  }
  
  digitalWrite(LOW_PWR, LOW);
  digitalWrite(PWR_KEY, LOW);
  delay(1100);
  digitalWrite(PWR_KEY, HIGH);//A pulse to turn on or turn off SIM808

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
    {
        SerialUSB.println("Now reset the maduino zero");
        delay(1000);
        resetFunc();//restart 
       delay(20);
    }
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("www.makerfabs.com!");
  display.println("Maduino Zero SIM808  GPS Tracker!");
  display.display();
  delay(2000);
  delay(5000);

  Serial1.begin(115200);// serial connect to SIM808
  SerialUSB.println("Maduino Zero SIM808 Test Start!");

  Serial1.println("AT+CGNSPWR=1");
  delay(200);
  Serial1.println("AT+CGNSSEQ=RMC");
  delay(200);
  Serial1.println("AT+CGNSURC=1");
  delay(200);
  display.clearDisplay();
  
}

void loop()
{
  String response = "";
  //get what form serial send to USB serial
  while (Serial1.available() > 0) {
    display.clearDisplay();
    String cstring = Serial1.readString();//Serial1.read();//
    SerialUSB.print(cstring);//SerialUSB.write(cstring);
    //response += cstring;
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);

    display.println(cstring);
    display.display();
    //Serial1.flush();
    yield();
  }
  //get what form USB serial send to serial
  while (SerialUSB.available() > 0) {
    Serial1.write(SerialUSB.read());
    yield();
  }
}
