// Adafruit GFX Library - Version: Latest
#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

// Adafruit SSD1306 - Version: Latest
#include <Adafruit_SSD1306.h>
#include <splash.h>

// DHT sensor library - Version: Latest
#include <DHT.h>
#include <DHT_U.h>

// Wire Library for I2C
#include <Wire.h>

// Set OLED size in pixels
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Set OLED parameters
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define DHT11 Parameters
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

float humid;
float temp;


const int DryValue = 2940;
const int WetValue = 1110;

int moistureval;
int moisturepercent;

#define SENSOR_IN 34
#define RELAY_OUT 26

String pump = "OFF";
int trigger = 30;

#include "thingProperties.h"

void setup()
{
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();


  // Initialize I2C display using 3.3 volts
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();

  // Initialize DHT22
  dht.begin();

  // Set ADC to use 12 bits
  analogReadResolution(12);

  // Set Relay as Output
  pinMode(RELAY_OUT, OUTPUT);

  // Turn off relay
  digitalWrite(RELAY_OUT, LOW);

  // Set Pump Status to Off
  pump_Status = false;

}

void loop() {
  ArduinoCloud.update();
  
  temp = dht.readTemperature();
  humid = dht.readHumidity();
  
  temperature = temp;
  humidity = humid;
  
  moistureval = analogRead(SENSOR_IN); 
  moisturepercent = map(moistureval, DryValue, WetValue, 0, 100);
  
  moisturepercent = constrain(moisturepercent, 0, 100); // Ranging between 0 to 100
  Serial.println(moisturepercent);
  
  moisture = moisturepercent;
  
  if(moisturepercent <= trigger)
  {
    pumpON();
  }
  else
  {
    pumpOFF();
  }
  
  //Printing in OLED Display using Functions
  printOLED(35, "PUMP", 40, pump, 2000);
  printOLED(35, "TEMP", 10, String(temp) + "C", 2000);
  printOLED(30, "HUMID", 10, String(humid) + "%", 2000);
  printOLED(35, "MOIST", 30, String(moisturepercent) + "%", 2000);
  
}

void printOLED(int top_cursor, String top_text, int main_cursor, String main_text, int delay_time)
{
  // Prints to OLED and holds display for delay_time
  display.setCursor(top_cursor, 0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(top_text);

  display.setCursor(main_cursor, 40);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.print(main_text);
  display.display();

  delay(delay_time);
  display.clearDisplay();
 
}

void pumpON()
{
  digitalWrite(RELAY_OUT, HIGH);
  pump = "ON";
  pump_Status = true;
}

void pumpOFF()
{
  digitalWrite(RELAY_OUT, LOW);
  pump = "OFF";
  pump_Status = false;
}


void onTriggerLevelChange()  
{
  trigger = trigger_Level;
}
