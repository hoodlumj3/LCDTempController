/*
 Name:		LCDTempController.ino
 Created:	11/16/2022 10:57:15 PM
 Author:	hoodlumj3
*/
#define REQUIRESALARMS false

#include "modules\functionEnable\functionEnable.h"
#include "modules\ButtonDebounce.h"
#include <OneWire.h>                       // library to access DS18B20
#include <DallasTemperature.h>             // library to support DS18B20
#include <Wire.h>                          // library for communicating to I2C devices
#include <hd44780.h>                       // main hd44780 library
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class library
#include <EEPROM.h>


typedef enum _enum_ioPin_Type {
	pinHWSerialRX = 0,
	pinHWSerialTX = 1,

	pinOut_DS18B20 = 2,
	pinOut_D3 = 3, // PWM
	pinIn_Button1 = 4,
	pinOut_D5 = 5, // PWM
	pinOut_D6 = 6, // PWM
	pinOut_D7 = 7,
	pinOut_D8 = 8,
	pinOut_D9 = 9, //PWM
	pinOut_D10 = 10, //PWM
	pinOut_D11 = 11,
	pinOut_D12 = 12,

	pinOut_StatusLED = 13,

	pinOut_A0 = A0,
	pinOut_A1 = A1,
	pinOut_A2 = A2,
	pinOut_A3 = A3,
	pinOut_A4 = A4,
	pinOut_A5 = A5,
	pinOut_A6 = A6,
	pinOut_A7 = A7,

} ioPin_Type;

#define SETUPINFOPAUSE		50

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto configure expander chip

OneWire oneWire(pinOut_DS18B20); // tell the library to use the digital pin the DS18B20 is connected to

DallasTemperature sensors(&oneWire); // tell the library to compute temp on the oneWire pin

//int IOL; // initialize the LDR pin name as a 16-bit value

unsigned long currentMillis = 0;
unsigned long lastMillis = 0;
unsigned long loopcount = 0;
bool iCFlagMakeCall = true;
int iCTemp = 0;


functionEnable scheduler;




byte lcdchar_Vertical[8] = {
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
	lcdVertical = 1, 
	lcdHorizontal,
	lcdCornerTL,
	lcdCornerTR,
	lcdCornerBL,
	lcdCornerBR
} lcdCustomCharacters_Type;

enum {
	eeprom_fan1_speed = 0,
	eeprom_fan2_speed,
	eeprom_fan3_speed,
	eeprom_pump1_speed,
	eeprom_count
} eepromByteIndex_Type;

//EEPROMDATA abc;

class _settings {

	struct xv {
		byte _fanspeed1;
		byte _fanspeed2;
		byte _fanspeed3;
		byte _pumpspeed1;
	};

private:
	
	xv bc;

public:
	const byte fanspeed1() const  { return bc._fanspeed1; }
	void fanspeed1(const byte _bar) { bc._fanspeed1 =  _bar; }


	void readAll() {
		EEPROM.get(0, bc);
		//for (int i = 0; i < eeprom_count; i++) {
		//	EEPROM.read(i);
		//}

	}


	void writeAll() {
		EEPROM.put(0, bc);
//		for (int i = 0; i < eeprom_count; i++) {
//			EEPROM.write(i, ((byte*)&bc)[i]);
//		}
	};

};

_settings settings;

//void saveToEEPROM(byte byteIndex, byte value) {
//	EEPROM.write(byteIndex, value);
//}



void lcdCreateCustomCharacters()
{

	lcd.createChar(lcdVertical, lcdchar_Vertical);

	lcd.createChar(lcdHorizontal, lcdchar_Horizontal);

	lcd.createChar(lcdCornerTL, lcdchar_CornerTL);

	lcd.createChar(lcdCornerTR, lcdchar_CornerTR);

	lcd.createChar(lcdCornerBL, lcdchar_CornerBL);

	lcd.createChar(lcdCornerBR, lcdchar_CornerBR);

}


void lcdDisplayInitialMessage()
{
	const char abc[20] = { lcdCornerTL, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdCornerTR };
	const char abd[20] = { lcdVertical, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', lcdVertical };
	const char abe[20] = { lcdCornerBL, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdHorizontal, lcdCornerBR };

	lcd.setCursor(0, 0);

	lcd.write(abc, sizeof(abc));


	lcd.setCursor(0, 1);

	lcd.write(abd, sizeof(abd));


	lcd.setCursor(0, 2);

	lcd.write(abd, sizeof(abd));


	lcd.setCursor(0, 3);

	lcd.write(abe, sizeof(abe));

	lcd.setCursor(5, 1);

	lcd.print("Hello");

	lcd.setCursor(10, 2);

	lcd.print("There");


}

void lcdDisplayMainInfo() {

	lcd.clear();

	lcd.setCursor(5, 2);

	lcd.print(F("Hello"));

	lcd.setCursor(10, 1);

	lcd.print(F("There"));

	scheduler.remFunc(lcdDisplayCMInfo);

	scheduler.addFunc(lcdDisplayCMInfo, 125, 0);


}

void lcdDisplayCMInfo() {


	lcd.setCursor(0, 0);
	String abc = String(iCTemp);
	abc += char(0xDF);
	abc += "C";
	lcd.print(abc);


	lcd.setCursor(1, 4);

	lcd.print(currentMillis);

}

void displayManager(int display) {
	switch (display) {
	case 0:
		lcdCreateCustomCharacters();
		lcdDisplayInitialMessage();
		break;

	case 1:
		//lcdCreateCustomCharacters();
		lcdDisplayMainInfo();
		break;
	}
}


void checkTemperature() {

	//
	// this should happen every 500ms 
	//

	if (iCFlagMakeCall) {

		//
		// send data asking for temperature to DS18B20
		//
		sensors.requestTemperatures(); 
		
		//
		// set flag to say dont make any more calls to requestTemperatures
		//

		iCFlagMakeCall = false;

		digitalWrite(pinOut_StatusLED, HIGH);

		//Serial.println(
		//	"requestTemperatures"
		//);
	}

	//
	// check if conversion is complete
	//

	if (sensors.isConversionComplete()) {
		
		//
		// pull temp from sensor
		//
		iCTemp = sensors.getTempCByIndex(0);

		//
		// set flag say we can now make another call to requestTemperatures
		//
		iCFlagMakeCall = true;

		digitalWrite(pinOut_StatusLED, LOW);

		//Serial.println(
		//	"checkTemperature"
		//);

	}

}

void setup() {

	lcd.clear();

	Serial.begin(9600); // setup Serial for debug
	//while (!Serial);
	//delay(500);


	Serial.print("Sketch:   ");   Serial.println(__FILE__);
	Serial.print("Uploaded: ");   Serial.println(__DATE__);
	Serial.println(" ");

	Serial.println(F("\nLCD Temperature Controller v1.0a 20221116\n\n"));
	Serial.println(F("Initializing "));






	/////////////////////////////////////////////////////////////////////////////////////////////
	Serial.print(F(" - I/O Pins (Buttons)"));

	pinMode(pinOut_StatusLED, OUTPUT);


	Serial.print(F("."));

	pinMode(pinIn_Button1, INPUT_PULLUP);
//	pinMode(pinIn_Button2, INPUT_PULLUP);
//	pinMode(pinIn_Button3, INPUT_PULLUP);

	Serial.print(F("."));

	buttonsInit();


	Serial.println(F(" Done"));

	delay(SETUPINFOPAUSE);






	/////////////////////////////////////////////////////////////////////////////////////////////
	Serial.print(F(" - LCD (20x4) "));


	lcd.begin(20, 4); // initialize lcd as 20 columns, 4 rows

	Serial.print(F("."));

	lcd.clear(); // clear any old data on the lcd

	Serial.print(F("."));

	displayManager(0);


	Serial.println(F(" Done"));

	delay(SETUPINFOPAUSE);






	/////////////////////////////////////////////////////////////////////////////////////////////
	Serial.print(F(" - Sensors (DS18B20) "));


	Serial.print(F("."));

	sensors.begin(); // start DS18B20

	Serial.print(F("."));

	sensors.setResolution(12); // start DS18B20

	Serial.print(F("."));

	sensors.setWaitForConversion(false);

	Serial.println(F(" Done"));

	delay(SETUPINFOPAUSE);






	/////////////////////////////////////////////////////////////////////////////////////////////
	Serial.print(" - Scheduler ");


	Serial.print(".");

	scheduler.addFunc(lcdDisplayMainInfo, 8500, 1);

	Serial.print(".");

	scheduler.addFunc(checkTemperature, 500, 0);

	Serial.print(".");

	scheduler.addFunc(buttonDecoder, 50, 0);


	Serial.println(" Done");

	delay(SETUPINFOPAUSE);






	/////////////////////////////////////////////////////////////////////////////////////////////
	Serial.print(" - Load Settings ");


	Serial.print(".");

	settings.readAll();


	Serial.println(" Done");

	delay(SETUPINFOPAUSE);


}



void loop() {
	
	currentMillis = millis();
	loopcount++;

	//lcd.setCursor(0, 0); // set the lcd cursor for celsius temperature
	//lcd.print("Temp C: "); // print explanation of following data
  //  lcd.print(sensors.getTempCByIndex(0)); // print the degrees in celsius
	//lcd.print("       "); // print spaces to blank out any remanants of data
	//lcd.setCursor(0, 1); // set the lcd for farenheit temperature
	//lcd.print("Temp F: "); // print explanation of following data
  //  lcd.print(DallasTemperature::toFahrenheit(sensors.getTempCByIndex(0))); // Convert tempC to Fahrenheit and print it
	//lcd.print("       "); // print spaces to blank out any remanants of data

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
	scheduler.execute(currentMillis);

//	long amillis = 0;
//	long bmillis = 0;
//	long cmillis = 0;
	
	
//	amillis = millis();

	if (currentMillis - lastMillis > 1000 /*&& 1 == 0*/) {


		Serial.println(
			" CM:" + String(currentMillis)
			//+ " CC:" + String(controllerConnected)
			+ " LC:" + String(loopcount)
			+ " TMP0:" + String(iCTemp)

			//+ " b-a:" + String(bmillis-amillis)
			//+ " c-b:" + String(cmillis-bmillis)
			+ " SBR:" + String(sensors.getResolution())
			+ " DPE:" + String(iCFlagMakeCall)
			//+ " DHE:" + String(bHolosOutputEnabled)
			//+ " SS:" + String(sequence_started)
			//+ " STO:" + String(seq_timeout)
			//+ " SST:" + String(sequence_step)
			//+ " SSL:" + String(sequence_length)
			//+ " STG:" + String(stageDisplay)
			//+" LED:" + String(onFlag)
			//+ " TS:" + String(mesh.scheduler.size())
			//+ " FM:" + String( feetMotorStateEnabled )
			//+ " DP:" + String( domePowerStateEnabled )
			//+ " DPV:" + String( currentDomePowerValue )
			//+ " DPV:" + String( targetDomePowerValue )
			//      + " LN:" + String( colourNextLED.val )
			//+ " MFS:" + String(_motorFailSafe)

			//+ " J0H:" + String(dataIn._joy0HorzValue )
			//+ " J0V:" + String(dataIn._joy0VertValue )
			//+ " J0V:" + String(_joy0VertPos )


		);

		lastMillis = currentMillis;
		loopcount = 0;
	}


}
