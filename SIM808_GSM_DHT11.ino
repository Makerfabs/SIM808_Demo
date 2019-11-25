#include<stdio.h>
#include<string.h>
#include <DHT.h>
#include <DHT_U.h>
/**
 * GPRS/GSM test send dht11 data to internet
 * Author:Charlin
 * Date:2019/11/21
 * Version:V1.3
 * www.makerfabs.com
 * 
 */

 /**
  * 
  * https://thingspeak.com/channels/916902
  */

#define DEBUG true
int POWER_KEY=9;
bool SIM808_ON = false;


//*******for senser **********
float humi;//humidity
float temp;//temperature

//for location
float latitude=113.83;//latitude
float longitude=22.61;//longitude


#define DHTPIN 6    //D6
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

//**********api write key******************
const String apiKey="HLIOKKWF7ADQBQ5K";


void setup()
{
  pinMode(POWER_KEY, OUTPUT);
  //********Turn on SIM808 when power up*********
  digitalWrite(POWER_KEY, HIGH);
  delay(1000);
  digitalWrite(POWER_KEY, LOW);
  
  SerialUSB.begin(115200);
  delay(2000);
  
//  while (!SerialUSB) {
//    delay(50); // wait for Arduino Serial Monitor to connect the port
//  }
  
  
  SerialUSB.println("Start to run!");
  Serial1.begin(115200);

  //********check open************
  sendData("AT", 1000, DEBUG);
  delay(1000);
  //******************************

  //********if SIM808 off, turn on it*********
  if (!SIM808_ON)
  {
    digitalWrite(POWER_KEY, LOW);    
    delay(3000); 
    digitalWrite(POWER_KEY, HIGH);
    SerialUSB.println("Turn on SIM808");
    delay(10000);
  }

  dht.begin();
  
  //GPS test
  /**
  sendData("AT+CGNSPWR=1", 1000, DEBUG);//GNSS POWER CONTROL ON
  delay(1000);
  sendData("AT+CGNSSEQ=\"RMC\"", 1000, DEBUG);//
  delay(1000);
  sendData("AT+CGNSINF", 1000, DEBUG);//
  delay(1000);
  sendData("AT+CGNSURC=1", 1000, DEBUG);// printf to serial every 1 second
  delay(1000);
  **/
  //GSM/GPRS test
  
  sendData("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"", 1000, DEBUG);//Set bearer parameters
  delay(1000);
  sendData("AT+SAPBR=3,1,\"APN\",\"CMNET\"", 1000, DEBUG);//CMNET CMWAP 3GNET
  delay(1000);
  sendData("AT+SAPBR=1,1", 1000, DEBUG);//
  delay(1000);
  sendData("AT+CGATT=1", 1000, DEBUG);// connected to the GPRS network 
  delay(1000);
  sendData("AT+HTTPINIT", 1000, DEBUG);//
  delay(1000);
  sendData("AT+HTTPPARA=\"URL\",\"http://api.thingspeak.com/update?api_key=HLIOKKWF7ADQBQ5K&field1=22.612008&field2=113.835192&field3=29.4&field4=22.6\"", 1000, DEBUG);//
  delay(1000);
  sendData("AT+HTTPACTION=1", 1000, DEBUG);//HTTP POST   //response: +HTTPACTION: 1,200,1
  delay(1000);
  sendData("AT+HTTPREAD", 1000, DEBUG);//
  delay(1000);
  //sendData("AT+HTTPTERM", 1000, DEBUG);//Terminate HTTP Server
  //delay(1000);
  

  
}

void loop()
{
  /**
    while (Serial1.available() > 0) {
      SerialUSB.write(Serial1.read());
      yield();
    }
    while (SerialUSB.available() > 0) {
        //if(Serial1.find("SEND")){
        //}
        Serial1.write(SerialUSB.read());
        yield();
    }
    **/

  humi = dht.readHumidity();
                       
  temp = dht.readTemperature();
  
  SerialUSB.println("humi is " + (String)humi);
  SerialUSB.println("temp is " + (String)temp);
  
    //sendData("AT+HTTPPARA=\"URL\",\"http://api.thingspeak.com/update?api_key=HLIOKKWF7ADQBQ5K&field1=22.612008&field2=113.835192&field3=29.4&field4=76.6\"", 1000, DEBUG);//
    String command = "AT+HTTPPARA=\"URL\",\"http://api.thingspeak.com/update.json?api_key=" + apiKey + "&field1=" + (String)latitude +"&field2=" + (String)longitude + "&field3=" + (String)temp +"&field4=" + (String)humi + "\"";
    sendData(command, 3000, DEBUG);//Set HTTP Parameters Value 
    delay(1000);
    sendData("AT+HTTPACTION=1", 1000, DEBUG);//HTTP POST   //response: +HTTPACTION: 1,200,1
    delay(1000);
    sendData("AT+HTTPREAD", 1000, DEBUG);//
    delay(30000);
}


String sendData(String command, const int timeout, boolean debug)
{
    String response = "";    
    Serial1.println(command);

    String commandpre = getcommand_pref(command);
    //SerialUSB.println(commandpre);

    long int time = millis();   
    while( (time+timeout) > millis())
    {
       if(commandpre == "AT"){  
        if(Serial1.find("OK")){
          SIM808_ON = true;
          SerialUSB.println("SIM808 opened");
        }
      }

      while(Serial1.available())
      {       
        String c = Serial1.readString(); 
        response+=c;
      } 
   }   

    if(debug)
    {
      SerialUSB.println(response);
    }
    return response;
}


String getcommand_pref(String command){

    String command_pref = "";
    char *cstr = new char[command.length() + 1];
    strcpy(cstr, command.c_str());
    char * token = strtok(cstr,"=");
    int i = 0;
    while(token != NULL){
      //SerialUSB.print(token);
      //SerialUSB.println("  line" + (String)i);

      switch(i) {
        case 0:
          command_pref = token;
        break;
        
        default:
        break;
      }

      token = strtok(NULL,",");
      i = i+1;
    }

    if(command_pref == "")
    command_pref = command;
    
    return command_pref;
}
