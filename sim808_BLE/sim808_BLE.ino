#include <stdio.h>
#include <string.h>

#define DEBUG true
int POWER_KEY = 9;
int LED_PIN = 8;
bool SIM808_ON = false;
bool led_status = false;

void setup()
{
    pinMode(POWER_KEY, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    //********Turn on SIM808 when power up*********
    digitalWrite(POWER_KEY, HIGH);
    delay(1000);
    digitalWrite(POWER_KEY, LOW);

    SerialUSB.begin(115200);
    delay(2000);

    while (!SerialUSB)
    {
        delay(50); // wait for Arduino Serial Monitor to connect
    }

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
        delay(16000);
    }
    sendData("AT", 1000, DEBUG); //GNSS POWER CONTROL ON
    delay(1000);
    sendData("at+btpower=1", 1000, DEBUG); //BLE turn on
    delay(3000);
}

void loop()
{
    while (Serial1.available() > 0)
    {
        String rec_text = Serial1.readString();
        if (rec_text.indexOf("BTPAIRING") != -1)
        {
            sendData("at+btpair=1,1", 1000, DEBUG);
            SerialUSB.println("[CMD]PAIR!");
        }
        if (rec_text.indexOf("BTCONNECTING") != -1)
        {
            sendData("AT+BTACPT=1", 1000, DEBUG);
            SerialUSB.println("[CMD]CONNECTING!");
        }
        if (rec_text.indexOf("LEDON") != -1)
        {
            SerialUSB.println("[CMD]LEDON");
            led_status = true;
        }
        if (rec_text.indexOf("LEDOFF") != -1)
        {
            SerialUSB.println("[CMD]LEDOFF");
            led_status = false;
        }

        if (led_status == true)
        {
            digitalWrite(LED_PIN, HIGH);
        }
        else
        {
            digitalWrite(LED_PIN, LOW);
        }

        SerialUSB.println(rec_text);

        //SerialUSB.write(Serial1.read());
        yield();
    }
    while (SerialUSB.available() > 0)
    {
        Serial1.write(SerialUSB.read());
        yield();
    }
}

String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    Serial1.println(command);

    String commandpre = getcommand_pref(command);
    //SerialUSB.println(commandpre);

    long int time = millis();
    while ((time + timeout) > millis())
    {
        if (commandpre == "AT")
        {
            if (Serial1.find("OK"))
            {
                SIM808_ON = true;
                SerialUSB.println("SIM808 opened");
            }
        }

        while (Serial1.available())
        {
            String c = Serial1.readString();
            response += c;
        }
    }

    if (debug)
    {
        SerialUSB.println(response);
    }
    return response;
}

String getcommand_pref(String command)
{

    String command_pref = "";
    char *cstr = new char[command.length() + 1];
    strcpy(cstr, command.c_str());
    char *token = strtok(cstr, "=");
    int i = 0;
    while (token != NULL)
    {
        //SerialUSB.print(token);
        //SerialUSB.println("  line" + (String)i);

        switch (i)
        {
        case 0:
            command_pref = token;
            break;

        default:
            break;
        }

        token = strtok(NULL, ",");
        i = i + 1;
    }

    if (command_pref == "")
        command_pref = command;

    return command_pref;
}
