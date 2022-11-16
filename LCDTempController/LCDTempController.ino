/*
 Name:		LCDTempController.ino
 Created:	11/16/2022 10:57:15 PM
 Author:	hoodlumj3
*/

#include <OneWire.h>                       // library to access DS18B20
#include <DallasTemperature.h>             // library to support DS18B20
#include <Wire.h>                          // library for communicating to I2C devices
#include <hd44780.h>                       // main hd44780 library
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class library

#define SETUPINFOPAUSE		50

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto configure expander chip

//OneWire oneWire(2); // tell the library to use the digital pin the DS18B20 is connected to

//DallasTemperature sensors(&oneWire); // tell the library to compute temp on the oneWire pin

//int IOL; // initialize the LDR pin name as a 16-bit value


byte lcdchar_Verticl[8] = {
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100
};


byte lcdchar_Horizontal[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b11111,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};


byte lcdchar_CornerTL[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b00111,
	0b00100,
	0b00100,
	0b00100,
	0b00100
};


byte lcdchar_CornerTR[8] = {
	0b00000,
	0b00000,
	0b00000,
	0b11100,
	0b00100,
	0b00100,
	0b00100,
	0b00100
};


byte lcdchar_CornerBL[8] = {
	0b00100,
	0b00100,
	0b00100,
	0b00111,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};


byte lcdchar_CornerBR[8] = {
	0b00100,
	0b00100,
	0b00100,
	0b11100,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};


typedef enum {
	lcdVertical, 
	lcdHorizontal,
	lcdCornerTL,
	lcdCornerTR,
	lcdCornerBL,
	lcdCornerBR
} lcdCustomCharacters_Type;

void createCustomCharacters()
{

	lcd.createChar(lcdVertical, lcdchar_Verticl);

	lcd.createChar(lcdHorizontal, lcdchar_Horizontal);

	lcd.createChar(lcdCornerTL, lcdchar_CornerTL);

	lcd.createChar(lcdCornerTR, lcdchar_CornerTR);

	lcd.createChar(lcdCornerBL, lcdchar_CornerBL);

	lcd.createChar(lcdCornerBR, lcdchar_CornerBR);



}

void setup() {
	Serial.begin(9600); // setup Serial for debug
	while (!Serial);
	delay(500);


	Serial.print("Sketch:   ");   Serial.println(__FILE__);
	Serial.print("Uploaded: ");   Serial.println(__DATE__);
	Serial.println(" ");

	Serial.println(F("\nLCD Temperature Controller v1.0a 20221116\n\n"));
	Serial.println(F("Initializing "));




	Serial.print(F(" - I/O Pins (Buttons)"));


	Serial.print(F("."));


	Serial.println(F(" Done"));

	delay(SETUPINFOPAUSE);




	Serial.print(F(" - LCD (20x4) "));


	lcd.begin(20, 4); // initialize lcd as 20 columns, 4 rows

	Serial.print(F("."));

	createCustomCharacters();

	Serial.print(F("."));

	lcd.clear(); // clear any old data on the lcd


	Serial.println(F(" Done"));

	delay(SETUPINFOPAUSE);




	Serial.print(F(" - Sensors (DS18B20) "));


	Serial.print(F("."));

	//sensors.begin(); // start DS18B20


	Serial.println(F(" Done"));

	delay(SETUPINFOPAUSE);


}

void loop() {
	

	//sensors.requestTemperatures(); // send data asking for temperature to DS18B20
	lcd.setCursor(0, 0); // set the lcd cursor for celsius temperature
	lcd.print("Temp C: "); // print explanation of following data
  //  lcd.print(sensors.getTempCByIndex(0)); // print the degrees in celsius
	lcd.print("       "); // print spaces to blank out any remanants of data
	lcd.setCursor(0, 1); // set the lcd for farenheit temperature
	lcd.print("Temp F: "); // print explanation of following data
  //  lcd.print(DallasTemperature::toFahrenheit(sensors.getTempCByIndex(0))); // Convert tempC to Fahrenheit and print it
	lcd.print("       "); // print spaces to blank out any remanants of data

  /*
	sensors.requestTemperatures(); // send data asking for temperature to DS18B20
	lcd.setCursor(0, 0); // set the lcd cursor for celsius temperature
	lcd.print(F("Temp: ")); // print explanation of following data from program storage
	lcd.print(sensors.getTempCByIndex(0)); // print the degrees in celsius
	lcd.print(F(" ")); // print space from program storage
	lcd.print((char)223); // print degrees symbol
	lcd.print(F("C")); // print celsius abbreviation
	lcd.print("       "); // print spaces to blank out any remanants of data
	lcd.setCursor(0, 1); // set the lcd for light levels
	lcd.print(F("Light Level: ")); // print explanation of following data
	lcd.print(IOL); // print light levels
	lcd.print("       "); // print spaces to blank out any remanants of data
  */
}
